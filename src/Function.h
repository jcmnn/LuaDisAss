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

	Util::BoolRes loadFunction();

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
	Util::BoolRes loadString(std::string &out);
	Util::BoolRes loadCode();
	Util::BoolRes loadConstants();
	Util::BoolRes loadUpvalues();
	Util::BoolRes loadProtos();
	Util::BoolRes loadDebug();

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