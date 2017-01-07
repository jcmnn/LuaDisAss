#include "Assembler.h"

#include <iostream>
#include "util.h"


Assembler::Assembler(Buffer *rbuffer, WriteBufferPtr wbuffer) : rbuffer_(rbuffer), wbuffer_(wbuffer), parseStatus_(PARSE_NONE) {

}

inline const char *parseLabel(std::string &out, const char *start, const char *end) {
	start = std::find_if_not(start, end, std::ptr_fun<int, int>(std::isblank));
	if (start == end) {
		return end;
	}

	const char *lend = std::find_if_not(start, end, [](char c) {return std::isalnum(c) || c == '_'; });
	if (lend == end || lend == start) {
		return end;
	}

	out.assign(start, lend);
	return lend;
}

template<typename T>
inline const char *parseInt(T &out, const char *start, const char *end) {
	start = std::find_if_not(start, end, std::ptr_fun<int, int>(std::isblank));
	if (start == end) {
		return end;
	}

	const char *iend = std::find_if_not(start, end, std::ptr_fun<int, int>(std::isdigit));
	if (iend == end) {
		return end;
	}

	out = 0;
	
	const char *c;
	for (c = iend-1; c >= start; c--) {
		out = out * 10 + (*c - '0');
	}

	return iend;
}

inline std::pair<bool, std::string> Assembler::parseDirective(const char *line, size_t len) {
	std::string name;
	const char *c = line + 1;
	const char *end = line + len;
	for (; c != end; c++) {
		if (!(std::isalnum(*c) || *c == '_')) {
			if (std::isblank(*c) || *c == ';') {
				if (*c == ';') {
					end = --c;
				}
				break;
			}
			return std::make_pair(false, std::string("could not parse directive: illegal character '") + *c + "'");
		}
	}
	name.assign(&line[1], c);
	if (name.empty()) {
		return std::make_pair(false, "could not parse directive");
	}

	Util::lower(name);
	if (name == "func") {
		if (parseStatus_ != PARSE_FUNC && parseStatus_ != PARSE_NONE) {
			return std::make_pair(false, "func declaration cannot be inside a code or const segment");
		}
		if ((c = parseLabel(funcname_, c, end)) == end) {
			return std::make_pair(false, "invalid args for directive .func");
		}

		if ((c = parseInt(f_maxstacksize_, c, end)) == end) {
			return std::make_pair(false, "invalid args for directive .func");
		}
		if ((c = parseInt(f_params_, c, end)) == end) {
			return std::make_pair(false, "invalid args for directive .func");
		}
		if ((c = parseInt(f_vararg_, c, end)) == end) {
			return std::make_pair(false, "invalid args for directive .func");
		}
		if (f_vararg_ > 2) {
			return std::make_pair(false, "vararg cannot be greater than 2");
		}

		parseStatus_ = PARSE_FUNC;
	} else if (name == "begin_const") {
		if (parseStatus_ != PARSE_FUNC) {
			return std::make_pair(false, "const declaration must be inside function");
		}

		parseStatus_ = PARSE_CONST;
	} else if (name == "end_const") {
		if (parseStatus_ != PARSE_CONST) {
			return std::make_pair(false, "end_const must be inside const segment");
		}

		parseStatus_ = PARSE_FUNC;
	} else if (name == "begin_code") {
		if (parseStatus_ != PARSE_FUNC) {
			return std::make_pair(false, "code declaration must be inside function");
		}

		parseStatus_ = PARSE_CODE;
	} else if (name == "end_code") {
		if (parseStatus_ != PARSE_CODE) {
			return std::make_pair(false, "end_code must be inside code segment");
		}

		parseStatus_ = PARSE_FUNC;
	}

	return std::make_pair(true, "");
}

const char *Assembler::parseConstant(const char *line, size_t len, size_t *id) {
	const char *c = line;
	const char *end = line + len;
	const char *bend;

	char cf = std::tolower(line[0]);

	TValuePtr tval;

	if (cf == '\'' || cf == '"') { // parse string
		std::string string;

		c++;
		for (; c != end; ++c) {
			if (*c == '\\') {
				if (c + 1 == end) {
					return nullptr;
				}
				switch (*(++c)) {
					case 'a':
						string += '\a';
						break;
					case 'b':
						string += '\b';
						break;
					case 'f':
						string += '\f';
						break;
					case 'n':
						string += '\n';
						break;
					case 'r':
						string += '\r';
						break;
					case 't':
						string += '\t';
						break;
					case 'v':
						string += '\v';
						break;
					case '\\':
						string += '\\';
						break;
					case '"':
						string += '"';
						break;
					case '\'':
						string += '\'';
						break;
					case '[':
						string += '[';
						break;
					case ']':
						string += ']';
						break;
				}
			} else if (*c == cf) {
				tval.reset(new TString(string));
				bend = c + 1;
				break;
			} else {
				string += *c;
			}
		}
	}
	else if (std::isdigit(cf) || (cf == '-' || cf == '+')) { // parse number
		bool negative = false;
		lua_Number num = 0;

		if (cf == '-' || cf == '+') {
			if (cf == '-') {
				negative = true;
			}
			if (++c == end) {
				return nullptr;
			}
		}

		bool hex = false;
		if (cf == '0' && c + 1 != end && *(c + 1) == 'x') {
			// hexadecimal
			hex = true;
			c+=2;
		}

		if (hex) {
			for (; c != end; ++c) {
				if (std::isxdigit(*c)) {
					cf = *c;
					if (cf <= '9' && cf >= '0') {
						num = num * 16 + (cf - '0');
					} else {
						cf = std::tolower(cf);
						num = num * 16 + (cf - 'a' + 10);
					}
				} else {
					break;
				}
			}
		} else {
			lua_Number frac = 0;
			bool dpart = false;
			for (; c != end; ++c) {
				if (std::isdigit(*c)) {
					if (dpart) {
						num += frac * (*c - '0');
						frac *= 0.1;
					} else {
						num = num * 10 + (*c - '0');
					}
				} else if (*c == '.' && !dpart) {
					dpart = true;
					frac = 0.1;
				} else {
					break;
				}
			}
		}

		if (negative)
			num = -num;

		tval.reset(new TNumber(num));
		bend = c;

	} else if (cf == 't' || cf == 'f') { // possibly true or false
		bend = std::find_if_not(c, end, std::ptr_fun<int, int>(std::isalpha));
		if (bend == c) {
			return nullptr;
		}

		std::string cval(c, bend);
		Util::lower(cval);
		if (cval == "true") {
			tval.reset(new TBool(true));
		} else if (cval == "false") {
			tval.reset(new TBool(false));
		} else {
			return nullptr;
		}
	}

	if (!tval) {
		return nullptr;
	}

	if (parseStatus_ == PARSE_CONST) {
		if (bend != end && *bend != ';') {
			bend = std::find_if_not(bend, end, std::ptr_fun<int, int>(std::isblank));
			if (bend != end && *bend != ';') {
				return nullptr;
			}
		}
	}

	bool found = false;
	for (int i = 0; i < constants_.size(); i++) {
		if (*constants_[i] == *tval) {
			// use this constant instead of creating a duplicate
			found = true;
			if (id != nullptr) {
				*id = constants_.size() - 1;
			}
		}
	}

	if (!found) {
		constants_.push_back(tval);
		if (id != nullptr) {
			*id = constants_.size() - 1;
		}
	}

	return end;
}

inline std::pair<bool, std::string> Assembler::parseCode(const char *line, size_t len) {
	return std::make_pair(true, "");
}

std::pair<bool, std::string> Assembler::parseLine(const char *line, size_t len) {
	// std::cout << "line: " << line << std::endl;
	if (line[0] == '.') { // directive
		return parseDirective(line, len);
	}

	switch(parseStatus_) {
		case PARSE_CONST:
			if (parseConstant(line, len, nullptr) == nullptr) {
				return std::make_pair(false, "could not parse constant");
			}
			return std::make_pair(true, "");
		case PARSE_CODE:
			return parseCode(line, len);
		default:
			return std::make_pair(false, "unimplemented");
	}
}

std::pair<bool, std::string> Assembler::assemble() {
	if (!rbuffer_) {
		return std::make_pair(false, "invalid read buffer");
	}
	if (!wbuffer_) {
		return std::make_pair(false, "invalid write buffer");
	}

	std::string line;
	unsigned int linen = 0;
	while (rbuffer_->readLine(line).first) {
		linen++;

		Util::trim(line);
		if (line.empty() || line.front() == ';') {
			continue;
		}
		auto res = parseLine(line.c_str(), line.length());
		if (!res.first) {
			return std::make_pair(false, std::string("error parsing line ") + std::to_string(linen) + ": " + res.second);
		}
	}

	return std::make_pair(true, "");
}