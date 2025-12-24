#include "CliArguments.hpp"
#include "LexFileParser.hpp"
#include "RegexParser.hpp"

#include <iostream>

int main(int argc, char **argv) {
	CliArguments cliArgs(argc, argv);
	if (!cliArgs.parse()) {
		cliArgs.printUsage();
		return 1;
	}

	LexFileParser parser(cliArgs.getInputFile());
	if (!parser.parse()) {
		return 1;
	}
	
	for (const auto &rules : parser.getContent().rules) {
		RegexParser regexParser(rules.pattern, parser.getContent().substitutions);
		if (!regexParser.parse()) {
			return 1;
		}
		regexParser.printTree();
		std::cout << std::endl;
	}

	return 0;
}