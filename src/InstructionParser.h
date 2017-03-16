#ifndef INSTRUCTIONPARSER_H
#define INSTRUCTIONPARSER_H

#include "lconfig.h"
#include <vector>
#include <string>
#include <utility>
#include <sstream>

class Function;

class InstructionParser {
public:
	InstructionParser(Function *function, const std::vector<Instruction> &code);
	InstructionParser(Function *function, std::vector<Instruction> &&code);

	Util::BoolRes parse();

	inline std::string disas() {
		return decomp_.str();
	}
private:
	std::vector<Instruction> code_;
	Function *function_;

	std::stringstream decomp_;
};

#endif