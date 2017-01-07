#ifndef STRINGWRITEBUFFER_H
#define STRINGWRITEBUFFER_H

#include "WriteBuffer.h"
#include <string>

class StringWriteBuffer : public WriteBuffer {
public:
	StringWriteBuffer(std::string &buffer);

	size_t writeBytes(const char *buffer, size_t amount) override;
private:
	std::string &buffer_;
};

#endif