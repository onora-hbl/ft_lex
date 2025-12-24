#pragma once

#include <string>
#include <variant>
#include <map>

class RegexParser {
	public:
		RegexParser(const std::string &pattern, const std::map<std::string, std::string> &substitutions);
		~RegexParser();

		bool parse();

		enum AtomType {
			WILDCARD,
			CHARACTER,
			CHARACTER_CLASS,
			STRING
		};
		enum QuantifierType {
			NONE,
			STAR,
			PLUS,
			OPTIONAL,
			RANGE
		};
		enum NodeType {
			ATOM,
			CONCATENATION,
			ALTERNATION,
			QUANTIFIER
		};
		struct RegexNode;
		struct AtomNode {
			AtomType type;
			std::string value;
		};
		struct ConcatenationNode {
			RegexNode *left;
			RegexNode *right;
		};
		struct AlternationNode {
			RegexNode *left;
			RegexNode *right;
		};
		struct QuantifierNode {
			RegexNode *node;
			QuantifierType quantifierType;
			int min; // use for RANGE
			int max; // use for RANGE
		};
		struct RegexNode {
			NodeType type;
			std::variant<AtomNode, ConcatenationNode, AlternationNode, QuantifierNode> data;
		};

		RegexNode* getRoot() const;
	
		void printNode(const RegexNode *node, int indent = 0) const;
		void printTree() const;

	private:
		std::string _pattern;
		std::map<std::string, std::string> _substitutions;
		RegexNode *_root = nullptr;
		size_t _position = 0;

		char peek() const;
		void consume(char expected);

		bool canStartAtom(char c) const;

		RegexNode *parseAlternation();
		RegexNode *parseConcatenation();
		RegexNode *parseQuantifier();
		RegexNode *parseAtom();
};