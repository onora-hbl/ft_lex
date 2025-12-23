#include "RegexParser.hpp"

#include <iostream>
#include <sstream>

RegexParser::RegexParser(const std::string &pattern) : _pattern(pattern) {}

void freeNode(RegexParser::RegexNode *node) {
	if (node == nullptr) {
		return;
	}
	switch (node->type) {
		case RegexParser::ATOM: {
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
	_position = 0;
	_root = parseAlternation();
	return true;
}

char RegexParser::peek() const {
	if (_position < _pattern.size()) {
		return _pattern[_position];
	}
	return '\0';
}

void RegexParser::consume(char expected) {
	if (peek() != expected) {
		throw std::runtime_error(std::string("Expected '") + expected + "', but found '" + peek() + "'");
	}
	_position++;
}

bool RegexParser::canStartAtom(char c) const {
	return (isalnum(c) || c == '.' || c == '[' || c == '(' || c == '{' || c == '\"');
}

RegexParser::RegexNode* RegexParser::parseAlternation() {
	RegexParser::RegexNode* left = this->parseConcatenation();

	while (this->peek() == '|') {
		this->consume('|');
		RegexParser::RegexNode* right = this->parseConcatenation();

		left = new RegexParser::RegexNode{RegexParser::ALTERNATION, RegexParser::AlternationNode{left, right}};
	}

	return left;
}

RegexParser::RegexNode* RegexParser::parseConcatenation() {
	RegexParser::RegexNode* left = this->parseQuantifier();

	while (canStartAtom(this->peek())) {
		RegexParser::RegexNode* right = this->parseQuantifier();

		left = new RegexParser::RegexNode{RegexParser::CONCATENATION, RegexParser::ConcatenationNode{left, right}};
	}

	return left;
}

RegexParser::RegexNode* RegexParser::parseQuantifier() {
	RegexParser::RegexNode* atom = this->parseAtom();

	if (this->peek() == '*') {
		this->consume('*');
		return new RegexParser::RegexNode{RegexParser::QUANTIFIER, RegexParser::QuantifierNode{atom, RegexParser::STAR, -1, -1}};
	} else if (this->peek() == '+') {
		this->consume('+');
		return new RegexParser::RegexNode{RegexParser::QUANTIFIER, RegexParser::QuantifierNode{atom, RegexParser::PLUS, -1, -1}};
	} else if (this->peek() == '?') {
		this->consume('?');
		return new RegexParser::RegexNode{RegexParser::QUANTIFIER, RegexParser::QuantifierNode{atom, RegexParser::OPTIONAL, -1, -1}};
	} else if (this->peek() == '{') {
		this->consume('{');
		std::string rangeContent;
		while (true) {
			char c = this->peek();
			if (c == '}') {
				this->consume('}');
				break;
			}
			rangeContent += c;
			this->consume(c);
		}
		std::istringstream iss(rangeContent);
		std::string token;
		int min = 0, max = 0;
		if (std::getline(iss, token, ',')) {
			min = std::stoi(token);
		}
		if (std::getline(iss, token)) {
			max = std::stoi(token);
		}
		return new RegexParser::RegexNode{RegexParser::QUANTIFIER, RegexParser::QuantifierNode{atom, RegexParser::RANGE, min, max}};
	}

	return atom;
}

RegexParser::RegexNode* RegexParser::parseAtom() {
	if (this->peek() == '(') {
		this->consume('(');
		RegexParser::RegexNode* subExpr = this->parseAlternation();
		this->consume(')');
		return subExpr;
	}

	if (this->peek() == '.') {
		this->consume('.');
		return new RegexParser::RegexNode{RegexParser::ATOM, RegexParser::AtomNode{RegexParser::WILDCARD, "."}};
	}

	if (this->peek() == '[') {
		this->consume('[');
		std::string classContent;
		while (true) {
			char c = this->peek();
			if (c == ']') {
				this->consume(']');
				break;
			}
			classContent += c;
			this->consume(c);
		}
		return new RegexParser::RegexNode{RegexParser::ATOM, RegexParser::AtomNode{RegexParser::CHARACTER_CLASS, classContent}};
	}

	if (this->peek() == '{') {
		this->consume('{');
		std::string substitutionContent;
		while (true) {
			char c = this->peek();
			if (c == '}') {
				this->consume('}');
				break;
			}
			substitutionContent += c;
			this->consume(c);
		}
		return new RegexParser::RegexNode{RegexParser::ATOM, RegexParser::AtomNode{RegexParser::SUBSTITUTION, substitutionContent}};
	}

	if (this->peek() == '\"') {
		this->consume('\"');
		std::string stringContent;
		while (true) {
			char c = this->peek();
			if (c == '\"') {
				this->consume('\"');
				break;
			}
			stringContent += c;
			this->consume(c);
		}
		return new RegexParser::RegexNode{RegexParser::ATOM, RegexParser::AtomNode{RegexParser::STRING, stringContent}};
	}

	char c = this->peek();
	this->consume(c);
	return new RegexParser::RegexNode{RegexParser::ATOM, RegexParser::AtomNode{RegexParser::CHARACTER, std::string(1, c)}};
}

static void printPrefix(int indent) {
    for (int i = 0; i < indent; ++i)
        std::cout << "|  ";
    std::cout << "|- ";
}

void RegexParser::printNode(const RegexNode* node, int indent) const {
    if (!node) {
        printPrefix(indent);
        std::cout << "(null)\n";
        return;
    }

    switch (node->type) {

		case ATOM: {
			const AtomNode& atom = std::get<AtomNode>(node->data);
			printPrefix(indent);
			std::cout << "ATOM ";

			switch (atom.type) {
			case WILDCARD:
				std::cout << ".";
				break;
			case CHARACTER:
				std::cout << "'" << atom.value << "'";
				break;
			case CHARACTER_CLASS:
				std::cout << "[" << atom.value << "]";
				break;
			case STRING:
				std::cout << "\"" << atom.value << "\"";
				break;
			case SUBSTITUTION:
				std::cout << "{" << atom.value << "}";
				break;
			}
			std::cout << "\n";
			break;
		}

		case CONCATENATION: {
			const auto& c = std::get<ConcatenationNode>(node->data);
			printPrefix(indent);
			std::cout << "CONCAT\n";
			printNode(c.left, indent + 1);
			printNode(c.right, indent + 1);
			break;
		}

		case ALTERNATION: {
			const auto& a = std::get<AlternationNode>(node->data);
			printPrefix(indent);
			std::cout << "ALT\n";
			printNode(a.left, indent + 1);
			printNode(a.right, indent + 1);
			break;
		}

		case QUANTIFIER: {
			const auto& q = std::get<QuantifierNode>(node->data);
			printPrefix(indent);

			switch (q.quantifierType) {
			case STAR:
				std::cout << "STAR\n";
				break;
			case PLUS:
				std::cout << "PLUS\n";
				break;
			case OPTIONAL:
				std::cout << "OPTIONAL\n";
				break;
			case RANGE:
				std::cout << "RANGE {" << q.min << "," << q.max << "}\n";
				break;
			case NONE:
				std::cout << "NONE\n";
				break;
			}

			printNode(q.node, indent + 1);
			break;
		}

		default:
			printPrefix(indent);
			std::cout << "UNKNOWN NODE\n";
    }
}

void RegexParser::printTree() const {
	RegexNode* root = _root;
	if (root == nullptr) {
		std::cout << "Empty regex tree." << std::endl;
		return;
	}
	printNode(root);
}