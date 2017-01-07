#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <utility>
#include <string>
#include <vector>

#include "WriteBuffer.h"
#include "Buffer.h"
#include "lconfig.h"

class Assembler {
public:
	Assembler(Buffer *rbuffer, WriteBufferPtr wbuffer);
	std::pair<bool, std::string> assemble();

private:
	std::pair<bool, std::string> parseLine(const char *line, size_t len);
	std::pair<bool, std::string> parseDirective(const char *line, size_t len);
	const char *parseConstant(const char *line, size_t len, size_t *id);
	std::pair<bool, std::string> parseCode(const char *line, size_t len);

	WriteBufferPtr wbuffer_;
	BufferPtr rbuffer_;

	

	union {
		struct {
			unsigned int f_maxstacksize_, f_params_, f_vararg_;
		};
	};
	enum ParseStatus {
		PARSE_FUNC,
		PARSE_CODE,
		PARSE_CONST,
		PARSE_NONE
	};

	ParseStatus parseStatus_;
	std::string funcname_;

	std::vector<TValuePtr> constants_;
};

#endif