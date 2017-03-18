#ifndef BUFFER_H
#define BUFFER_H

#include <utility>
#include <string>
#include <memory>

class Buffer;
typedef std::shared_ptr<Buffer> BufferPtr;

class Buffer {
public:
	inline size_t read(char *buffer, size_t amount) {
		return readBytes(buffer, amount);
	}

	size_t read(std::string &buffer, size_t amount);
	std::pair<bool, std::string> read(long &number);
	inline std::pair<bool, std::string> read(size_t &number) {
		return read((long&)number);
	};
	std::pair<bool, std::string> read(int &number);
	std::pair<bool, std::string> read(long double &number);
	std::pair<bool, std::string> read(double &number);
	std::pair<bool, std::string> read(unsigned char &byte);

	virtual std::pair<bool, std::string> readLine(std::string &buffer) =0;

	virtual ~Buffer() {};

protected:
	virtual size_t readBytes(char* buffer, size_t amount) =0;
};

#endif