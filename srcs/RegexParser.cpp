#include "RegexParser.hpp"

#include <iostream>

RegexParser::RegexParser(const std::string &pattern) : _pattern(pattern) {}

void freeNode(RegexParser::RegexNode *node) {
	if (node == nullptr) {
		return;
	}
	switch (node->type) {
		case RegexParser::ATOM: {
			RegexParser::AtomNode &atom = std::get<RegexParser::AtomNode>(node->data);
			if (atom.type == RegexParser::SUB_EXPRESSION && atom.subExpression != nullptr) {
				freeNode(atom.subExpression);
			}
			break;
		}
		case RegexParser::CONCATENATION: {
			RegexParser::ConcatenationNode &concat = std::get<RegexParser::ConcatenationNode>(node->data);
			freeNode(concat.left);
			freeNode(concat.right);
			break;
		}
		case RegexParser::ALTERNATION: {
			RegexParser::AlternationNode &alt = std::get<RegexParser::AlternationNode>(node->data);
			freeNode(alt.left);
			freeNode(alt.right);
			break;
		}
		case RegexParser::QUANTIFIER: {
			RegexParser::QuantifierNode &quant = std::get<RegexParser::QuantifierNode>(node->data);
			freeNode(quant.node);
			break;
		}
	}
	delete node;
}

RegexParser::~RegexParser() {
	freeNode(_root);
}

bool RegexParser::parse() {
	std::cout << "Parsing regex pattern: " << _pattern << std::endl;
	return true;
}