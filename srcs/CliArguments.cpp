#include "CliArguments.hpp"

#include <iostream>

CliArguments::CliArguments(int argc, char **argv) {
	_argc = argc;
	for (int i = 0; i < argc; ++i) {
		_argv.push_back(std::string(argv[i]));
	}
}

CliArguments::~CliArguments() {
}

bool CliArguments::parse() {
	if (_argc < 2) {
		return false;
	}
	_inputFile = _argv[1];
	if (_inputFile.length() < 3 || _inputFile.substr(_inputFile.length() - 2) != ".l") {
		return false;
	}
	return true;
}

std::string CliArguments::getInputFile() const {
	return _inputFile;
}

void CliArguments::printUsage() const {
	std::cout << "Usage: " << _argv[0] << " <input_file.l>" << std::endl;
}