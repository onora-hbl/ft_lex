#include "LexFileParser.hpp"

#include <fstream>
#include <iostream>

LexFileParser::LexFileParser(const std::string& filename)
	: _filename(filename), _state(DEFINITIONS), _content(), _isValid(true) {}

LexFileParser::~LexFileParser() {}

bool LexFileParser::parse() {
	std::ifstream file(_filename);
	if (!file.is_open()) {
		std::cerr << "Error: Could not open file " << _filename << std::endl;
		return false;
	}

	std::string line;
	while (std::getline(file, line)) {
		if (line == "%%") {
			if (_state == DEFINITIONS) {
				_state = RULES;
			} else if (_state == RULES) {
				_state = USER_SUBROUTINES;
			}
			continue;
		}
		switch (_state) {
			case DEFINITIONS:
				handleDefinitionLine(line);
				break;
			case RULES:
				handleRuleLine(line);
				break;
			case USER_SUBROUTINES:
				handleUserSubroutineLine(line);
				break;
		}
	}

	return true;
}

void LexFileParser::handleDefinitionLine(const std::string& line) {
	static bool insideCodeBlock = false;
	if (line.find("%{") != std::string::npos) {
		insideCodeBlock = true;
		return;
	}
	if (line.find("%}") != std::string::npos) {
		insideCodeBlock = false;
		return;
	}
	if (insideCodeBlock) {
		_content.definitionCode.push_back(line);
		return;
	}
	if (!line.empty() && line[0] == ' ') {
		_content.definitionCode.push_back(line);
		return;
	}
	if (!line.empty() && line[0] == '%') {
		if (line.find("%array") != std::string::npos) {
			_content.yytextType = Content::ARRAY;
		} else if (line.find("%pointer") != std::string::npos) {
			_content.yytextType = Content::POINTER;
		} else {
			switch (line[1]) {
				case 'p': {
					size_t size = std::stoul(line.substr(2));
					_content.positionsSize = std::max(size, MIN_POSITIONS_SIZE);
					break;
				}
				case 'n': {
					size_t size = std::stoul(line.substr(2));
					_content.statesSize = std::max(size, MIN_STATES_SIZE);
					break;
				}
				case 'a': {
					size_t size = std::stoul(line.substr(2));
					_content.transitionsSize = std::max(size, MIN_TRANSITIONS_SIZE);
					break;
				}
				case 'e': {
					size_t size = std::stoul(line.substr(2));
					_content.parseTreeSize = std::max(size, MIN_PARSE_TREE_SIZE);
					break;
				}
				case 'k': {
					size_t size = std::stoul(line.substr(2));
					_content.packedCharacterClassesSize = std::max(size, MIN_PACKED_CHARACTER_CLASSES_SIZE);
					break;
				}
				case 'o': {
					size_t size = std::stoul(line.substr(2));
					_content.outputArraySize = std::max(size, MIN_OUTPUT_ARRAY_SIZE);
					break;
				}
				case 's':
				case 'S':
				case 'x':
				case 'X': {
					bool inclusive = (line[1] == 's' || line[1] == 'S');
					size_t pos = line.find_first_of(" \t");
					if (pos == std::string::npos) {
						std::cerr << "Invalid start condition line: " << line << std::endl;
						_isValid = false;
						return;
					}
					while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t')) {
						++pos;
					}
					std::string name = line.substr(pos);
					if (name.empty()) {
						std::cerr << "Invalid start condition name in line: " << line << std::endl;
						_isValid = false;
						return;
					}
					_content.startConditions.push_back({ name, inclusive });
					break;
				}
				default:
					std::cerr << "Unknown definition directive: " << line << std::endl;
					_isValid = false;
			}
		}
		return;
	}
	if (!line.empty()) {
		size_t pos = line.find_first_of(" \t");
		if (pos != std::string::npos) {
			std::string key = line.substr(0, pos);
			while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t')) {
				++pos;
			}
			std::string value = line.substr(pos);
			if (key.empty() || value.empty()) {
				std::cerr << "Invalid substitution line: " << line << std::endl;
				_isValid = false;
				return;
			}
			_content.substitutions[key] = value;
		}
	}
}

static bool isActionFinished(const std::string& action) {
	bool inSimpleQuote = false;
	bool inDoubleQuote = false;
	bool inComment = false;
	int depth = 0;
	size_t pos = 0;
	while (pos < action.size()) {
		if (action[pos] == '\'' && !inDoubleQuote && !inComment) {
			inSimpleQuote = !inSimpleQuote;
		} else if (action[pos] == '\"' && !inSimpleQuote && !inComment) {
			inDoubleQuote = !inDoubleQuote;
		} else if (!inSimpleQuote && !inDoubleQuote) {
			if (!inComment && action.substr(pos, 2) == "/*") {
				inComment = true;
			} else if (inComment && action.substr(pos, 2) == "*/") {
				inComment = false;
			}
		}
		if (!inSimpleQuote && !inDoubleQuote && !inComment) {
			if (action[pos] == '{') {
				++depth;
			} else if (action[pos] == '}') {
				--depth;
			}
		}
		++pos;
	}
	if (!inSimpleQuote && !inDoubleQuote && !inComment && depth == 0) {
		return true;
	}
	return false;
}

void LexFileParser::handleRuleLine(const std::string& line) {
	static Content::Rule currentRule = { "", "", {} };
	if (line.empty()) {
		return;
	}
	if (currentRule.pattern.empty()) {
		size_t pos = 0;
		std::vector<std::string> conditions;
		if (line[pos] == '<') {
			pos = line.find('>');
			if (pos == std::string::npos) {
				std::cerr << "Invalid rule line (missing '>'): " << line << std::endl;
				_isValid = false;
				return;
			}
			std::string conditionsStr = line.substr(1, pos - 1);
			size_t start = 0;
			while (start < conditionsStr.size()) {
				size_t commaPos = conditionsStr.find(',', start);
				if (commaPos == std::string::npos) {
					commaPos = conditionsStr.size();
				}
				std::string condition = conditionsStr.substr(start, commaPos - start);
				bool found = false;
				for (auto& sc : _content.startConditions) {
					if (sc.name == condition) {
						if (std::find(conditions.begin(), conditions.end(), condition) == conditions.end()) {
							conditions.push_back(condition);
						} else {
							std::cerr << "Duplicate start condition in rule: " << condition << std::endl;
							_isValid = false;
						}
						found = true;
						break;
					}
				}
				if (!found) {
					std::cerr << "Unknown start condition: " << condition << std::endl;
					_isValid = false;
				}
				start = commaPos + 1;
			}
			++pos;
		}
		size_t patternStart = pos;;
		while (pos < line.size() && line[pos] != ' ' && line[pos] != '\t') {
			++pos;
		}
		std::string pattern = line.substr(patternStart, pos - patternStart);
		while (pos < line.size() && (line[pos] == ' ' || line[pos] == '\t')) {
			++pos;
		}
		std::string action = line.substr(pos);
		currentRule = { pattern, action, conditions };
	} else {
		currentRule.action += "\n" + line;
	}
	if (isActionFinished(currentRule.action)) {
		std::cout << "Finished rule: pattern='" << currentRule.pattern << "', action='" << currentRule.action << "'" << std::endl;
		_content.rules.push_back(currentRule);
		currentRule = { "", "", {} };
	}
}

void LexFileParser::handleUserSubroutineLine(const std::string& line) {
	_content.userSubroutinesCode.push_back(line);
}