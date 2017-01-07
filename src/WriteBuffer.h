#ifndef WRITEBUFFER_H
#define WRITEBUFFER_H

#include <utility>
#include <string>
#include <memory>

class WriteBuffer;
typedef std::shared_ptr<WriteBuffer> WriteBufferPtr;

class WriteBuffer {
public:
	inline size_t write(const char *buffer, size_t amount) {
		return writeBytes(buffer, amount);
	}

	size_t write(const std::string &buffer);
	std::pair<bool, std::string> write(long number);
	inline std::pair<bool, std::string> write(size_t number) {
		return write((long)number);
	};
	std::pair<bool, std::string> write(int number);
	std::pair<bool, std::string> write(long double number);
	std::pair<bool, std::string> write(double number);
	std::pair<bool, std::string> write(unsigned char byte);

	virtual ~WriteBuffer() {};

protected:
	virtual size_t writeBytes(const char *buffer, size_t amount) =0;
};

#endif