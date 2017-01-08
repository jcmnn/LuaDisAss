#ifndef PARSER_H
#define PARSER_H

#include "Buffer.h"
#include "lconfig.h"
#include <utility>
#include <cstring>

class Parser {
public:
	Parser(Buffer *buffer);

	std::pair<bool, std::string> parse(std::string &out);
	inline std::string label() {
		if (labels_++ == 0) {
			return "main";
		}
		return std::string("subroutine_") + std::to_string(labels_);
	}

private:
	std::pair<bool, std::string> parseHeader();

	bool checkLiteral(const char *literal) {
		size_t len = std::strlen(literal);
		char bytes[len];
		if (buffer_->read(bytes, len) != len) {
			return false;
		}

		return std::memcmp(bytes, literal, len) == 0;
	}

	inline bool checkByte(unsigned char byte) {
		unsigned char b;
		return buffer_->read(b).first && byte == b;
	}

	inline bool checkInteger(lua_Integer n) {
		lua_Integer num;
		buffer_->read(num);
		return num == n;
	}

	inline bool checkNumber(lua_Number n) {
		lua_Number num;
		buffer_->read(num);
		return num == n;
	}

	unsigned int labels_;

	std::pair<bool, std::string> loadString(std::string &out);

	BufferPtr buffer_;
};

#endif