#include "CliArguments.hpp"
#include "LexFileParser.hpp"

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
	// parser.show();

	return 0;
}