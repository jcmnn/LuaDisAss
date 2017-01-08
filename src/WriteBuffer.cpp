#include "WriteBuffer.h"


size_t WriteBuffer::writeString(const std::string &buffer) {
	return writeBytes(buffer.c_str(), buffer.size());
}