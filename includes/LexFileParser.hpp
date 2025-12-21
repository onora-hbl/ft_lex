#pragma once

#include <string>
#include <vector>

class LexFileParser {
	public:
		LexFileParser(const std::string& filename);
		~LexFileParser();

		bool parse();

	private:
		enum State {
			DEFINITIONS,
			RULES,
			USER_SUBROUTINES
		};
		
		std::string _filename;
		State _state;

		void handleDefinitionLine(const std::string& line);
		void handleRuleLine(const std::string& line);
		void handleUserSubroutineLine(const std::string& line);
};