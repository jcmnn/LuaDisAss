#include "Buffer.h"
#include <vector>

size_t Buffer::read(std::string &buffer, size_t amount) {
    std::vector<char> tbuf;
    tbuf.resize(amount);
	size_t ret = readBytes(tbuf.data(), amount);
	if (ret != amount) {
		return ret;
	}

	buffer.assign(tbuf.data(), amount);
	return amount;
}


std::pair<bool, std::string> Buffer::read(__int32 &number) {
	if (readBytes(reinterpret_cast<char*>(&number), sizeof(__int32)) != sizeof(__int32)) {
		return std::make_pair(false, "read failed; end of stream?");
	}
	return std::make_pair(true, "");
}

std::pair<bool, std::string> Buffer::read(__int64 &number) {
  if (readBytes(reinterpret_cast<char*>(&number), sizeof(__int64)) != sizeof(__int64)) {
    return std::make_pair(false, "read failed; end of stream?");
  }
  return std::make_pair(true, "");
}


std::pair<bool, std::string> Buffer::read(long double &number) {
	if (readBytes(reinterpret_cast<char*>(&number), sizeof(long double)) != sizeof(long double)) {
		return std::make_pair(false, "read failed; end of stream?");
	}
	return std::make_pair(true, "");
}

std::pair<bool, std::string> Buffer::read(double &number) {
	if (readBytes(reinterpret_cast<char*>(&number), sizeof(double)) != sizeof(double)) {
		return std::make_pair(false, "read failed; end of stream?");
	}
	return std::make_pair(true, "");
}

std::pair<bool, std::string> Buffer::read(long &number) {
	if (readBytes(reinterpret_cast<char*>(&number), sizeof(long)) != sizeof(long)) {
		return std::make_pair(false, "read failed; end of stream?");
	}
	return std::make_pair(true, "");
}

std::pair<bool, std::string> Buffer::read(unsigned char &byte) {
	if (readBytes(reinterpret_cast<char*>(&byte), sizeof(unsigned char)) != sizeof(unsigned char)) {
		return std::make_pair(false, "read failed; end of stream?");
	}
	return std::make_pair(true, "");
}