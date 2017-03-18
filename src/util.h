#ifndef UTIL_H
#define UTIL_H

#include <algorithm>
#include <string>
#include <cctype>
#include <functional>

namespace Util {
	static inline void trim(std::string &s) {
		s.erase(s.begin(), std::find_if(s.begin(), s.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));
		s.erase(std::find_if(s.rbegin(), s.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), s.end());
	}

	static inline void lower(std::string &s) {
		std::transform(s.begin(), s.end(), s.begin(), std::ptr_fun<int, int>(std::tolower));
	}

	static inline std::string escape(const std::string &string) {
		std::string s;
		for (char c : string) {
			switch (c) {
				case '\a':
					s += "\\a";
					break;
				case '\b':
					s += "\\b";
					break;
				case '\f':
					s += "\\f";
					break;
				case '\n':
					s += "\\n";
					break;
				case '\r':
					s += "\\r";
					break;
				case '\t':
					s += "\\t";
					break;
				case '\v':
					s += "\\v";
					break;
				case '\\':
					s += "\\\\";
					break;
				case '"':
					s += "\\\"";
					break;
				case '\'':
					s += "\\'";
					break;
				default:
					s += c;
					break;
			}
		}
		return s;
	}
}

#endif