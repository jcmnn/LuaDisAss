#include "WriteBuffer.h"


size_t WriteBuffer::write(const std::string &buffer) {
	return writeBytes(buffer.c_str(), buffer.size());
}


std::pair<bool, std::string> WriteBuffer::write(int number) {
	if (writeBytes(reinterpret_cast<char*>(&number), sizeof(int)) != sizeof(int)) {
		return std::make_pair(false, "write failed");
	}
	return std::make_pair(true, "");
}


std::pair<bool, std::string> WriteBuffer::write(long double number) {
	if (writeBytes(reinterpret_cast<char*>(&number), sizeof(long double)) != sizeof(long double)) {
		return std::make_pair(false, "write failed");
	}
	return std::make_pair(true, "");
}

std::pair<bool, std::string> WriteBuffer::write(double number) {
	if (writeBytes(reinterpret_cast<char*>(&number), sizeof(double)) != sizeof(double)) {
		return std::make_pair(false, "write failed");
	}
	return std::make_pair(true, "");
}

std::pair<bool, std::string> WriteBuffer::write(long number) {
	if (writeBytes(reinterpret_cast<char*>(&number), sizeof(long)) != sizeof(long)) {
		return std::make_pair(false, "write failed");
	}
	return std::make_pair(true, "");
}

std::pair<bool, std::string> WriteBuffer::write(unsigned char byte) {
	if (writeBytes(reinterpret_cast<char*>(&byte), sizeof(unsigned char)) != sizeof(unsigned char)) {
		return std::make_pair(false, "write failed");
	}
	return std::make_pair(true, "");
}