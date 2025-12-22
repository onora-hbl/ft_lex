#pragma once

#include <string>
#include <variant>

class RegexParser {
	public:
		RegexParser(const std::string &pattern);
		~RegexParser();

		bool parse();

		enum AtomType {
			WILDCARD,
			CHARACTER,
			CHARACTER_CLASS,
			STRING,
			SUBSTITUTION
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
	
		void printNode(const RegexNode *node, int indent = 0) const;
		void printTree() const;

	private:
		std::string _pattern;
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