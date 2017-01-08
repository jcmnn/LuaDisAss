#include "StringWriteBuffer.h"

#include <iostream>

StringWriteBuffer::StringWriteBuffer(std::string &buffer) : buffer_(buffer) {

}

size_t StringWriteBuffer::writeBytes(const char *buffer, size_t amount) {
	buffer_.append(buffer, amount);
	return amount;
}