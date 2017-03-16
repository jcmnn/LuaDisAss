#include "StringBuffer.h"

StringBuffer::StringBuffer(const std::string &buffer) : buffer_(buffer) {

}

StringBuffer::StringBuffer(std::string &&buffer) : buffer_(std::move(buffer)) {

}

size_t StringBuffer::readBytes(char *buffer, size_t amount) {
	if (buffer_.size() < amount) {
		amount = buffer_.size();
	}

	std::copy(buffer_.begin(), buffer_.begin() + amount, buffer);
	buffer_.erase(0, amount);
	return amount;
}

Util::BoolRes StringBuffer::readLine(std::string &buffer) {
	if (buffer_.empty()) {
		return Util::BoolRes(false, "end of stream");
	}

	auto pos = buffer_.find("\n");
	if (pos == std::string::npos) {
		buffer = std::move(buffer_);
		buffer_.clear();
		if (buffer.back() == '\r') {
			buffer.pop_back();
		}
		return Util::BoolRes(true, "");
	}

	buffer.assign(buffer_.begin(), buffer_.begin() + pos);
	buffer_.erase(0, pos + 1);

	if (buffer.back() == '\r') {
		buffer.pop_back();
	}
	return Util::BoolRes(true, "");
}