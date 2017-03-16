#ifndef LCONFIG_H
#define LCONFIG_H

#define ERIS

#include <memory>

#define LUA_VERSION_MAJOR 5
#define LUA_VERSION_MINOR 3
#define LUA_VERSION_NUM		503
#define LUA_VERSION_RELEASE	"4"

#define LUA_SIGNATURE "\x1bLua"
#define LUAC_VERSION (LUA_VERSION_MAJOR * 16) + LUA_VERSION_MINOR
#define LUA_VERSION	"Lua " LUA_VERSION_MAJOR "." LUA_VERSION_MINOR
#define LUA_RELEASE	LUA_VERSION "." LUA_VERSION_RELEASE

#define LUAC_FORMAT 0

#define LUAC_DATA "\x19\x93\r\n\x1a\n"


#define LUA_COPYRIGHT	LUA_RELEASE "  Copyright (C) 1994-2017 Lua.org, PUC-Rio"
#define LUA_AUTHORS	"R. Ierusalimschy, L. H. de Figueiredo, W. Celes"


typedef double lua_Number;
typedef long long lua_Integer;
typedef unsigned int Instruction;
#define LUAC_INT 0x5678
#define LUAC_NUM (lua_Number)(370.5)


#define LUA_TNONE		(-1)

#define LUA_TNIL		0
#define LUA_TBOOLEAN		1
#define LUA_TLIGHTUSERDATA	2
#define LUA_TNUMBER		3
#define LUA_TSTRING		4
#define LUA_TTABLE		5
#define LUA_TFUNCTION		6
#define LUA_TUSERDATA		7
#define LUA_TTHREAD		8

#define LUA_NUMTAGS		9

#define LUA_TSHRSTR	(LUA_TSTRING | (0 << 4))  /* short strings */
#define LUA_TLNGSTR	(LUA_TSTRING | (1 << 4))  /* long strings */

#define LUA_TNUMFLT   (LUA_TNUMBER | (0 << 4))  /* float numbers */
#define LUA_TNUMINT   (LUA_TNUMBER | (1 << 4))  /* integer numbers */

#include "util.h"


class TValue {
public:
	TValue() : type_(LUA_TNIL) {};
	TValue(int type) : type_(type) {};
	inline int type() {return type_; };

	virtual std::string str() {return "nil"; };
	inline virtual bool operator==(TValue &v) {return v.type() == LUA_TNIL;};
private:
	int type_;
};

typedef std::shared_ptr<TValue> TValuePtr;

class TString : public TValue {
public:
	TString(const std::string &string) : TValue(LUA_TSTRING), string_(string) {};
	TString(std::string &&string) : TValue(LUA_TSTRING), string_(std::forward<std::string>(string)) {};

	inline std::string string() {return string_; };
	inline std::string str() override {return std::string("\"") + Util::escape(string_) + std::string("\""); };

	inline bool operator==(TValue &v) override {
		return v.type() == LUA_TSTRING && reinterpret_cast<TString*>(&v)->string() == string_;
	};
private:
	std::string string_;
};

class TBool : public TValue {
public:
	TBool(bool value) : TValue(LUA_TBOOLEAN), value_(value) {};

	inline bool value() {return value_; };
	inline std::string str() override {return (value_ ? "true" : "false"); };

	inline bool operator==(TValue &v) override {
		return v.type() == LUA_TBOOLEAN && reinterpret_cast<TBool*>(&v)->value() == value_;
	};
private:
	bool value_;
};

class TNumber : public TValue {
public:
	TNumber(lua_Number number) : TValue(LUA_TNUMBER), value_(number) {};

	inline lua_Number number() {return value_; };
	inline std::string str() override {return std::to_string(value_); };

	inline bool operator==(TValue &v) override {
		return v.type() == LUA_TNUMBER && reinterpret_cast<TNumber*>(&v)->number() == value_;
	};
private:
	lua_Number value_;
};



#endif