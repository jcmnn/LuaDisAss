#ifndef WRITEBUFFER_H
#define WRITEBUFFER_H

#include <utility>
#include <string>
#include <memory>

#include <iostream>

class WriteBuffer;
typedef std::shared_ptr<WriteBuffer> WriteBufferPtr;

class WriteBuffer {
public:
	size_t writeString(const std::string &buffer);

	template<typename T>
	inline std::pair<bool, std::string> write(T n) {
		if (writeBytes(reinterpret_cast<char*>(&n), sizeof(T)) != sizeof(T)) {
			return std::make_pair(false, "write failed");
		}
		return std::make_pair(true, "");
	};

	virtual ~WriteBuffer() {};

	virtual size_t writeBytes(const char *buffer, size_t amount) =0;

};

#endif