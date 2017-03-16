#include <string>
#include <fstream>
#include <iterator>
#include "StringBuffer.h"
#include "Parser.h"
#include "Assembler.h"
#include "StringWriteBuffer.h"

#include <iostream>

void printUsage(const char *name) {
	std::cout << "usage: " << name << " <-d <luac dump> ; -a <luas assembly> > <output>" << std::endl;
}

int main(int argc, char *argv[]) {
	if (argc < 4) {
		printUsage(argv[0]);
		return 0;
	}

	if (std::string("-d") == argv[1]) {
		std::ifstream file(argv[2], std::ifstream::binary);
		if (!file.is_open()) {
			std::cerr << "could not open file " << argv[2] << std::endl;
			return 1;
		}
		file >> std::noskipws;

		std::string dump(std::istream_iterator<char>(file), (std::istream_iterator<char>()));
		file.close();

		Parser parser(new StringBuffer(std::move(dump)));

		std::string out;
		auto res = parser.parse(out);
		std::cout << "success: " << res.success() << " (" << res.error_msg() << ")" << std::endl;

		if (res.success()) {
			std::ofstream of(argv[3], std::ifstream::binary);
			if (!of.is_open()) {
				std::cerr << "could not open file " << argv[3] << std::endl;
				return 1;
			}
			of << out;
			of.close();
		}

	} else if (std::string("-a") == argv[1]) {
		std::ifstream file(argv[2], std::ifstream::binary);
		if (!file.is_open()) {
			std::cerr << "could not open file " << argv[2] << std::endl;
			return 1;
		}
		file >> std::noskipws;

		std::string dump(std::istream_iterator<char>(file), (std::istream_iterator<char>()));
		file.close();


		std::string str;
		WriteBufferPtr wbuffer(new StringWriteBuffer(str));

		Assembler ass(new StringBuffer(std::move(dump)), wbuffer);
		auto res = ass.assemble();
		std::cerr << "success: " << res.success() << " (" << res.error_msg() << ")" << std::endl;

		if (res.success()) {
			std::ofstream of(argv[3], std::ifstream::binary);
			if (!of.is_open()) {
				std::cerr << "could not open file " << argv[3] << std::endl;
				return 1;
			}
			of << str;
			of.close();
		}
	} else {
		printUsage(argv[0]);
	}

	return 0;
}