cmake_minimum_required(VERSION 3.1)
project(luadisass)


set(CMAKE_CXX_STANDARD 11)


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

add_executable(luadisass ${SOURCES})
