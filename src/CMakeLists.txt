cmake_minimum_required(VERSION 2.4)
project(LuaDec)

set(SOURCES
	main.cpp
	Buffer.cpp
	WriteBuffer.cpp
	StringBuffer.cpp
	StringWriteBuffer.cpp
	Parser.cpp
	Function.cpp
	InstructionParser.cpp
	Assembler.cpp
	opcodes.c)

add_executable(LuaDec ${SOURCES})