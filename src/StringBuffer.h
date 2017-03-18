#ifndef STRINGBUFFER_H
#define STRINGBUFFER_H

#include "Buffer.h"

class StringBuffer : public Buffer {
public:
	StringBuffer(const std::string &buffer);
	StringBuffer(std::string &&buffer);

	size_t readBytes(char *buffer, size_t amount) override;
	std::pair<bool, std::string> readLine(std::string &buffer) override;

private:
	std::string buffer_;
};

#endif