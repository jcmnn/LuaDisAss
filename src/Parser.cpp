#include "Parser.h"
#include "lconfig.h"
#include "Function.h"
#include <iostream>

#define CHK_ASSERT(f, msg) if (!f) return std::make_pair(false, msg);

Parser::Parser(Buffer *buffer) : buffer_(buffer), labels_(0) {

}

std::pair<bool, std::string> Parser::parseHeader() {
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

	return std::make_pair(true, "");
}

std::pair<bool, std::string> Parser::parse(std::string &out) {
	if (!buffer_) {
		return std::make_pair(false, "invalid buffer");
	}
	auto res = parseHeader();
	if (!res.first) {
		return res;
	}

	unsigned char numUpvalues;
	res = buffer_->read(numUpvalues);
	if (!res.first) {
		return res;
	}

	Function func(this, buffer_);
	res = func.loadFunction();
	if (!res.first) {
		return res;
	}

	out = std::string(".upvalues ") + std::to_string(numUpvalues) + "\n\n" + func.disas();

	return res;
}