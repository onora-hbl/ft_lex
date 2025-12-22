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

void LexFileParser::handleRuleLine(const std::string& line) {
	std::cout << "Handling rule line: " << line << std::endl;
}

void LexFileParser::handleUserSubroutineLine(const std::string& line) {
	std::cout << "Handling user subroutine line: " << line << std::endl;
}