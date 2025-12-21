#pragma once

#include <string>
#include <vector>

class CliArguments {
	public:
		CliArguments(int argc, char **argv);
		~CliArguments();

		bool parse();
		std::string getInputFile() const;

		void printUsage() const;

	private:
		int	_argc;
		std::vector<std::string>	_argv;
		std::string	_inputFile;
};