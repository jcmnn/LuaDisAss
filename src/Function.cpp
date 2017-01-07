#include "Function.h"
#include "InstructionParser.h"
#include "Parser.h"

std::pair<bool, std::string> Function::loadString(std::string &out) {
	size_t size = 0;
	auto res = buffer_->read((unsigned char&)size);
	if (!res.first) {
		return res;
	}
	if (size == 0xFF) {
		res = buffer_->read(size);
		if (!res.first) {
			return res;
		}
	}

	if (size == 0) {
		return std::make_pair(true, "");
	}

	buffer_->read(out, size - 1);
	return std::make_pair(true, "");
}

std::pair<bool, std::string> Function::loadCode() {
	int n;
	auto res = buffer_->read(n);
	if (!res.first) {
		return res;
	}

	code_.resize(n);

	if (buffer_->read((char*)code_.data(), n * sizeof(Instruction)) != n * sizeof(Instruction)) {
		return std::make_pair(false, "failed to read code");
	}
	return std::make_pair(true, "");
}

std::pair<bool, std::string> Function::loadProtos() {
	int n;
	auto res = buffer_->read(n);
	if (!res.first) {
		return res;
	}

	for (int i = 0; i < n; i++) {
		FunctionPtr function = FunctionPtr(new Function(parser_, buffer_));
		if (!(res = function->loadFunction()).first) {
			return res;
		}
		protos_.push_back(function);
	}
}

std::pair<bool, std::string> Function::loadDebug() {
	int n;
	auto res = buffer_->read(n); // lineinfo
	if (!res.first) {
		return res;
	}

	lineInfo_.resize(n);
	if (buffer_->read((char*)lineInfo_.data(), n * sizeof(int)) != n * sizeof(int)) {
		return std::make_pair(false, "failed to read lineinfo (eof?)");
	}

	if (!(res = buffer_->read(n)).first) { // locvars
		return res;
	}

	locVars_.resize(n);

	for (int i = 0; i < n; i++) {
		if (!(res = loadString(locVars_[i].varName)).first) {
			return res;
		}
		if (!(res = buffer_->read(locVars_[i].startpc)).first) {
			return res;
		}
		if (!(res = buffer_->read(locVars_[i].endpc)).first) {
			return res;
		}
	}

	if (!(res = buffer_->read(n)).first) { // upvalue names
		return res;
	}

	if (n > upvalues_.size()) {
		return std::make_pair(false, "the upvalue debug names array size is larger than the upvalues");
	}

	for (int i = 0; i < n; i++) {
		if (!(res = loadString(upvalues_[i].name)).first) {
			return res;
		}
	}


	return std::make_pair(true, "");
}

std::pair<bool, std::string> Function::loadUpvalues() {
	int n;
	auto res = buffer_->read(n);
	if (!res.first) {
		return res;
	}

	upvalues_.resize(n);
	for (int i = 0; i < n; i++) {
		if (!(res = buffer_->read(upvalues_[i].instack)).first) {
			return res;
		}
		if (!(res = buffer_->read(upvalues_[i].idx)).first) {
			return res;
		}
	}

	return std::make_pair(true, "");
}

std::pair<bool, std::string> Function::loadConstants() {
	int n;
	auto res = buffer_->read(n);
	if (!res.first) {
		return res;
	}

	for (int i = 0; i < n; i++) {
		unsigned char t;
		if (!(res = buffer_->read(t)).first) {
			return res;
		}
		switch (t) {
		case LUA_TNIL:
			constants_.push_back(TValuePtr(new TValue()));
			break;
		case LUA_TBOOLEAN:
			unsigned char b;
			if (!(res = buffer_->read(b)).first) {
				return res;
			}
			constants_.push_back(TValuePtr(new TBool(b != 0)));
			break;
		case LUA_TNUMFLT:
			lua_Number num;
			if (!(res = buffer_->read(num)).first) {
				return res;
			}
			constants_.push_back(TValuePtr(new TNumber(num)));
			break;
		case LUA_TNUMINT:
			lua_Integer in;
			if (!(res = buffer_->read(in)).first) {
				return res;
			}
			constants_.push_back(TValuePtr(new TNumber(in)));
			break;
		case LUA_TSHRSTR:
		case LUA_TLNGSTR: {
			std::string string;
			if (!(res = loadString(string)).first) {
				return res;
			}
			constants_.push_back(TValuePtr(new TString(std::move(string))));
			break;
		}
		default: {
			return std::make_pair(false, "invalid constant type");
		}
		}
	}

	return std::make_pair(true, "");
}

std::pair<bool, std::string> Function::loadFunction() {
	std::string source;
	auto res = loadString(source);
	if (!res.first) {
		return res;
	}

	if (!(res = buffer_->read(lineDefined_)).first) {
		return res;
	}
	if (!(res = buffer_->read(lastLineDefined_)).first) {
		return res;
	}
	if (!(res = buffer_->read(numParams_)).first) {
		return res;
	}
	if (!(res = buffer_->read(isVarArg_)).first) {
		return res;
	}
	if (!(res = buffer_->read(maxStackSize_)).first) {
		return res;
	}

	if (!(res = loadCode()).first) {
		return res;
	}
	if (!(res = loadConstants()).first) {
		return res;
	}
	if (!(res = loadUpvalues()).first) {
		return res;
	}
	if (!(res = loadProtos()).first) {
		return res;
	}
	if (!(res = loadDebug()).first) {
		return res;
	}

	InstructionParser parser(this, std::move(code_));
	if (!(res = parser.parse()).first) {
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

	disas_ << ".begin_code\n";
	disas_ << parser.disas();
	disas_ << ".end_code\n\n\n";

	for (auto it = protos_.begin(); it != protos_.end(); it++) {
		disas_ << (*it)->disas() << "\n";
	}

	return std::make_pair(true, "");
}

Function::Function(Parser *parser, const BufferPtr &buffer) : parser_(parser), buffer_(buffer) {
	label_ = parser_->label();
}