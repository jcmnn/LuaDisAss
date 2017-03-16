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


Util::BoolRes Buffer::read(__int32 &number) {
	if (readBytes(reinterpret_cast<char*>(&number), sizeof(__int32)) != sizeof(__int32)) {
		return Util::BoolRes(false, "read failed; end of stream?");
	}
	return Util::BoolRes(true, "");
}

Util::BoolRes Buffer::read(__int64 &number) {
  if (readBytes(reinterpret_cast<char*>(&number), sizeof(__int64)) != sizeof(__int64)) {
    return Util::BoolRes(false, "read failed; end of stream?");
  }
  return Util::BoolRes(true, "");
}


Util::BoolRes Buffer::read(long double &number) {
	if (readBytes(reinterpret_cast<char*>(&number), sizeof(long double)) != sizeof(long double)) {
		return Util::BoolRes(false, "read failed; end of stream?");
	}
	return Util::BoolRes(true, "");
}

Util::BoolRes Buffer::read(double &number) {
	if (readBytes(reinterpret_cast<char*>(&number), sizeof(double)) != sizeof(double)) {
		return Util::BoolRes(false, "read failed; end of stream?");
	}
	return Util::BoolRes(true, "");
}

Util::BoolRes Buffer::read(long &number) {
	if (readBytes(reinterpret_cast<char*>(&number), sizeof(long)) != sizeof(long)) {
		return Util::BoolRes(false, "read failed; end of stream?");
	}
	return Util::BoolRes(true, "");
}

Util::BoolRes Buffer::read(unsigned char &byte) {
	if (readBytes(reinterpret_cast<char*>(&byte), sizeof(unsigned char)) != sizeof(unsigned char)) {
		return Util::BoolRes(false, "read failed; end of stream?");
	}
	return Util::BoolRes(true, "");
}