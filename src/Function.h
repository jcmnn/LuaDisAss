#ifndef FUNCTION_H
#define FUNCTION_H

#include "Buffer.h"
#include "lconfig.h"
#include <utility>
#include <vector>
#include <memory>
#include <sstream>

struct Upvalue {
	unsigned char instack, idx;
	std::string name;
};

struct LocVar {
	std::string varName;
	int startpc, endpc;
};

class Function;
typedef std::shared_ptr<Function> FunctionPtr;

class Parser;

class Function {
public:
	Function(Parser *parser, const BufferPtr &buffer);

	std::pair<bool, std::string> loadFunction();

	inline Upvalue *upvalue(size_t i) {
		if (i < upvalues_.size())
			return &upvalues_[i];
		else
			return nullptr;
	}

	inline TValuePtr constant(size_t i) {
		if (i < constants_.size()) {
			return constants_[i];
		}
		return TValuePtr(nullptr);
	}

	inline std::string disas() {
		return disas_.str();
	}

	inline std::string label() {
		return label_;
	}

	FunctionPtr proto(int i) {
		if (i < protos_.size()) {
			return protos_[i];
		}
		return FunctionPtr(nullptr);
	}
private:
	std::pair<bool, std::string> loadString(std::string &out);
	std::pair<bool, std::string> loadCode();
	std::pair<bool, std::string> loadConstants();
	std::pair<bool, std::string> loadUpvalues();
	std::pair<bool, std::string> loadProtos();
	std::pair<bool, std::string> loadDebug();

	BufferPtr buffer_;
	std::vector<Instruction> code_;

	std::string label_;
	Parser *parser_;

	int lineDefined_, lastLineDefined_;
	// unsigned char numUpvalues_;
	unsigned char numParams_, isVarArg_, maxStackSize_;

	std::vector<TValuePtr> constants_;
	std::vector<Upvalue> upvalues_;
	std::vector<FunctionPtr> protos_;
	std::vector<LocVar> locVars_;
	std::vector<int> lineInfo_;

	std::stringstream disas_;
};

#endif