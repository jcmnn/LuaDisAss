#ifndef STRINGBUFFER_H
#define STRINGBUFFER_H

#include "Buffer.h"
#include "util.h"

class StringBuffer : public Buffer {
public:
	StringBuffer(const std::string &buffer);
	StringBuffer(std::string &&buffer);

	size_t readBytes(char *buffer, size_t amount) override;
	Util::BoolRes readLine(std::string &buffer) override;

private:
	std::string buffer_;
};

#endif