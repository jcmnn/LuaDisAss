#include "Function.h"
#include "InstructionParser.h"
#include "Parser.h"
#include "util.h"

Util::BoolRes Function::loadString(std::string &out) {
	size_t size = 0;
	auto res = buffer_->read((unsigned char&)size);
	if (!res.success()) {
		return res;
	}
	if (size == 0xFF) {
		res = buffer_->read(size);
		if (!res.success()) {
			return res;
		}
	}

	if (size == 0) {
		return Util::BoolRes(true, "");
	}

	buffer_->read(out, size - 1);
	return Util::BoolRes(true, "");
}

Util::BoolRes Function::loadCode() {
	int n;
	auto res = buffer_->read(n);
	if (!res.success()) {
		return res;
	}

	code_.resize(n);

	if (buffer_->read((char*)code_.data(), n * sizeof(Instruction)) != n * sizeof(Instruction)) {
		return Util::BoolRes(false, "failed to read code");
	}
	return Util::BoolRes(true, "");
}

Util::BoolRes Function::loadProtos() {
	int n;
	auto res = buffer_->read(n);
	if (!res.success()) {
		return res;
	}

	for (int i = 0; i < n; i++) {
		FunctionPtr function = FunctionPtr(new Function(parser_, buffer_));
		if (!(res = function->loadFunction()).success()) {
			return res;
		}
		protos_.push_back(function);
	}
    throw std::exception();
}

Util::BoolRes Function::loadDebug() {
	int n;
	auto res = buffer_->read(n); // lineinfo
	if (!res.success()) {
		return res;
	}

	lineInfo_.resize(n);
	if (buffer_->read((char*)lineInfo_.data(), n * sizeof(int)) != n * sizeof(int)) {
		return Util::BoolRes(false, "failed to read lineinfo (eof?)");
	}

	if (!(res = buffer_->read(n)).success()) { // locvars
		return res;
	}

	locVars_.resize(n);

	for (int i = 0; i < n; i++) {
		if (!(res = loadString(locVars_[i].varName)).success()) {
			return res;
		}
		if (!(res = buffer_->read(locVars_[i].startpc)).success()) {
			return res;
		}
		if (!(res = buffer_->read(locVars_[i].endpc)).success()) {
			return res;
		}
	}

	if (!(res = buffer_->read(n)).success()) { // upvalue names
		return res;
	}

	if (n > upvalues_.size()) {
		return Util::BoolRes(false, "the upvalue debug names array size is larger than the upvalues");
	}

	for (int i = 0; i < n; i++) {
		if (!(res = loadString(upvalues_[i].name)).success()) {
			return res;
		}
	}


	return Util::BoolRes(true, "");
}

Util::BoolRes Function::loadUpvalues() {
	int n;
	auto res = buffer_->read(n);
	if (!res.success()) {
		return res;
	}

	upvalues_.resize(n);
	for (int i = 0; i < n; i++) {
		if (!(res = buffer_->read(upvalues_[i].instack)).success()) {
			return res;
		}
		if (!(res = buffer_->read(upvalues_[i].idx)).success()) {
			return res;
		}
	}

	return Util::BoolRes(true, "");
}

Util::BoolRes Function::loadConstants() {
	int n;
	auto res = buffer_->read(n);
	if (!res.success()) {
		return res;
	}

	for (int i = 0; i < n; i++) {
		unsigned char t;
		if (!(res = buffer_->read(t)).success()) {
			return res;
		}
		switch (t) {
		case LUA_TNIL:
			constants_.push_back(TValuePtr(new TValue()));
			break;
		case LUA_TBOOLEAN:
			unsigned char b;
			if (!(res = buffer_->read(b)).success()) {
				return res;
			}
			constants_.push_back(TValuePtr(new TBool(b != 0)));
			break;
		case LUA_TNUMFLT:
			lua_Number num;
			if (!(res = buffer_->read(num)).success()) {
				return res;
			}
			constants_.push_back(TValuePtr(new TNumber(num)));
			break;
		case LUA_TNUMINT:
			lua_Integer in;
			if (!(res = buffer_->read(in)).success()) {
				return res;
			}
			constants_.push_back(TValuePtr(new TNumber(in)));
			break;
		case LUA_TSHRSTR:
		case LUA_TLNGSTR: {
			std::string string;
			if (!(res = loadString(string)).success()) {
				return res;
			}
			constants_.push_back(TValuePtr(new TString(std::move(string))));
			break;
		}
		default: {
			return Util::BoolRes(false, "invalid constant type");
		}
		}
	}

	return Util::BoolRes(true, "");
}

Util::BoolRes Function::loadFunction() {
	std::string source;
	auto res = loadString(source);
	if (!res.success()) {
		return res;
	}

	if (!(res = buffer_->read(lineDefined_)).success()) {
		return res;
	}
	if (!(res = buffer_->read(lastLineDefined_)).success()) {
		return res;
	}
	if (!(res = buffer_->read(numParams_)).success()) {
		return res;
	}
	if (!(res = buffer_->read(isVarArg_)).success()) {
		return res;
	}
	if (!(res = buffer_->read(maxStackSize_)).success()) {
		return res;
	}

	if (!(res = loadCode()).success()) {
		return res;
	}
	if (!(res = loadConstants()).success()) {
		return res;
	}
	if (!(res = loadUpvalues()).success()) {
		return res;
	}
	if (!(res = loadProtos()).success()) {
		return res;
	}
	if (!(res = loadDebug()).success()) {
		return res;
	}

	InstructionParser parser(this, std::move(code_));
	if (!(res = parser.parse()).success()) {
		return res;
	}

	disas_ << ".func " << label_ << " " << (int)maxStackSize_ << " " << (int)numParams_ << " " << (int)isVarArg_;
	if (!source.empty()) {
		disas_ << " ; source: " << source;
	}

	disas_ << " ; maxstacksize: " << (int)maxStackSize_ << ", params: " << (int)numParams_ << ", vararg: " << (int)isVarArg_ << " (" << (isVarArg_ == 0 ? "does not use varag" : (isVarArg_ == 1 ? "uses vararg" : "declared vararg")) << ")\n";

	disas_ << ".begin_const\n";
	for (auto it = constants_.begin(); it != constants_.end(); it++) {
		disas_ << "   " << (*it)->str() << "\n";
	}
	disas_ << ".end_const\n\n\n";

	disas_ << ".begin_upvalue\n";
	for (auto it = upvalues_.begin(); it != upvalues_.end(); it++) {
		disas_ << "   " << (int)it->instack << " " << (int)it->idx << "\n";
	}
	disas_ << ".end_upvalue\n\n\n";

	disas_ << ".begin_code\n";
	disas_ << parser.disas();
	disas_ << ".end_code\n\n\n";

	for (auto it = protos_.begin(); it != protos_.end(); it++) {
		disas_ << (*it)->disas() << "\n";
	}

	return Util::BoolRes(true, "");
}

Function::Function(Parser *parser, const BufferPtr &buffer) : parser_(parser), buffer_(buffer) {
	label_ = parser_->label();
}