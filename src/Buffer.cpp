#include "Buffer.h"

size_t Buffer::read(std::string &buffer, size_t amount) {
	char tbuf[amount];
	size_t ret = readBytes(tbuf, amount);
	if (ret != amount) {
		return ret;
	}

	buffer.assign(tbuf, amount);
	return amount;
}


std::pair<bool, std::string> Buffer::read(int &number) {
	if (readBytes(reinterpret_cast<char*>(&number), sizeof(int)) != sizeof(int)) {
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