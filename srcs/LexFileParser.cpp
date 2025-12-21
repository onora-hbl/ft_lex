#include "LexFileParser.hpp"

#include <fstream>
#include <iostream>

LexFileParser::LexFileParser(const std::string& filename)
	: _filename(filename), _state(DEFINITIONS) {}

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
	std::cout << "Handling definition line: " << line << std::endl;
}

void LexFileParser::handleRuleLine(const std::string& line) {
	std::cout << "Handling rule line: " << line << std::endl;
}

void LexFileParser::handleUserSubroutineLine(const std::string& line) {
	std::cout << "Handling user subroutine line: " << line << std::endl;
}