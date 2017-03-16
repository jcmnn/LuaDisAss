#ifndef BUFFER_H
#define BUFFER_H

#include <utility>
#include <string>
#include <memory>
#include <stdint.h>
#include "util.h"

class Buffer;
typedef std::shared_ptr<Buffer> BufferPtr;

class Buffer {
public:
	inline size_t read(char *buffer, size_t amount) {
		return readBytes(buffer, amount);
	}

	size_t read(std::string &buffer, size_t amount);
	Util::BoolRes read(long &number);
	inline Util::BoolRes read(size_t &number) {
		return read((long&)number);
	};
	Util::BoolRes read(int32_t &number);
	Util::BoolRes read(long double &number);
	Util::BoolRes read(double &number);
	Util::BoolRes read(unsigned char &byte);
    Util::BoolRes read(int64_t &number);

	virtual Util::BoolRes readLine(std::string &buffer) =0;

	virtual ~Buffer() {};

protected:
	virtual size_t readBytes(char* buffer, size_t amount) =0;
};

#endif