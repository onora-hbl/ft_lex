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
			SUB_EXPRESSION,
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
			RegexNode *subExpression; // use for SUB_EXPRESSION
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

	private:
		std::string _pattern;
		RegexNode *_root = nullptr;
};