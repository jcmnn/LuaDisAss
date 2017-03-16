#ifndef WRITEBUFFER_H
#define WRITEBUFFER_H

#include <utility>
#include <string>
#include <memory>

#include <iostream>

#include "util.h"

class WriteBuffer;
typedef std::shared_ptr<WriteBuffer> WriteBufferPtr;

class WriteBuffer {
public:
	size_t writeString(const std::string &buffer);

	template<typename T>
	inline Util::BoolRes write(T n) {
		if (writeBytes(reinterpret_cast<char*>(&n), sizeof(T)) != sizeof(T)) {
			return Util::BoolRes(false, "write failed");
		}
		return Util::BoolRes(true, "");
	};

	virtual ~WriteBuffer() {};

	virtual size_t writeBytes(const char *buffer, size_t amount) =0;

};

#endif