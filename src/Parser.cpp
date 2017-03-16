#include "Parser.h"
#include "lconfig.h"
#include "Function.h"

#define CHK_ASSERT(f, msg) if (!f) return Util::BoolRes(false, msg);

Parser::Parser(Buffer *buffer) : buffer_(buffer), labels_(0) {

}

Util::BoolRes Parser::parseHeader() {
	CHK_ASSERT(checkLiteral(LUA_SIGNATURE), "signature check failed");
	CHK_ASSERT(checkByte(LUAC_VERSION), "version check failed");
	CHK_ASSERT(checkByte(LUAC_FORMAT), "format check failed");
	CHK_ASSERT(checkLiteral(LUAC_DATA), "corrupted");
	CHK_ASSERT(checkByte(sizeof(int)), "int size check failed");
	CHK_ASSERT(checkByte(sizeof(size_t)), "size_t size check failed");
	CHK_ASSERT(checkByte(sizeof(Instruction)), "Instruction size check failed");
	CHK_ASSERT(checkByte(sizeof(lua_Integer)), "lua_Integer size check failed");
	CHK_ASSERT(checkByte(sizeof(lua_Number)), "lua_Number size check failed");
	CHK_ASSERT(checkInteger(LUAC_INT), "endianness mismatch");
	CHK_ASSERT(checkNumber(LUAC_NUM), "float format mismatch");

	return Util::BoolRes(true, "");
}

Util::BoolRes Parser::parse(std::string &out) {
	if (!buffer_) {
		return Util::BoolRes(false, "invalid buffer");
	}
	auto res = parseHeader();
	if (!res.success()) {
		return res;
	}

	unsigned char numUpvalues;
	res = buffer_->read(numUpvalues);
	if (!res.success()) {
		return res;
	}

	Function func(this, buffer_);
	res = func.loadFunction();
	if (!res.success()) {
		return res;
	}

	out = std::string(".upvalues ") + std::to_string(numUpvalues) + "\n\n" + func.disas();

	return res;
}