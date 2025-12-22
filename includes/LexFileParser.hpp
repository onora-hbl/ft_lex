#pragma once

#include <string>
#include <vector>
#include <map>

class LexFileParser {
	public:
		struct Content {
			enum YytextType {
				ARRAY,
				POINTER
			};
			struct StartCondition {
				std::string name;
				bool inclusive;
			};

			std::vector<std::string> definitionCode;
			YytextType yytextType = POINTER;
			size_t positionsSize = 5000;
			size_t statesSize = 1000;
			size_t transitionsSize = 4000;
			size_t parseTreeSize = 2000;
			size_t packedCharacterClassesSize = 2000;
			size_t outputArraySize = 6000;
			std::map<std::string, std::string> substitutions;
			std::vector<StartCondition> startConditions = { {"INITIAL", true}  };
		};
	
		LexFileParser(const std::string& filename);
		~LexFileParser();

		bool parse();

	private:
		enum State {
			DEFINITIONS,
			RULES,
			USER_SUBROUTINES
		};

		constexpr static size_t MIN_POSITIONS_SIZE = 2500;
		constexpr static size_t MIN_STATES_SIZE = 500;
		constexpr static size_t MIN_TRANSITIONS_SIZE = 2000;
		constexpr static size_t MIN_PARSE_TREE_SIZE = 1000;
		constexpr static size_t MIN_PACKED_CHARACTER_CLASSES_SIZE = 1000;
		constexpr static size_t MIN_OUTPUT_ARRAY_SIZE = 3000;

		std::string _filename;
		State _state;
		Content _content;
		bool _isValid;

		void handleDefinitionLine(const std::string& line);
		void handleRuleLine(const std::string& line);
		void handleUserSubroutineLine(const std::string& line);
};