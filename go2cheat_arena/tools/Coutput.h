#pragma once
#include <Windows.h>
#include <iostream>
#include <vector>
#include <string>

void SetConsoleColor(WORD color);

inline void debug_print(const char* format, ...)
{
#ifndef NDEBUG
	constexpr int BUFFER_SIZE = 4096;
	std::vector<char> buf(BUFFER_SIZE);

	va_list args;
	va_start(args, format);
	vsnprintf(buf.data(), BUFFER_SIZE, format, args);
	va_end(args);

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleColor(FOREGROUND_GREEN);
	std::cout << "[DEBUG] " << buf.data() << std::endl;
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
#endif
}

inline void normal_print(const char* format, ...)
{
	constexpr int BUFFER_SIZE = 4096;
	std::vector<char> buf(BUFFER_SIZE);

	va_list args;
	va_start(args, format);
	vsnprintf(buf.data(), BUFFER_SIZE, format, args);
	va_end(args);

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleColor(FOREGROUND_BLUE);
	std::cout << "[+] " << buf.data() << std::endl;
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

inline void normal_print(WORD color,const char* format, ...)
{
	constexpr int BUFFER_SIZE = 4096;
	std::vector<char> buf(BUFFER_SIZE);

	va_list args;
	va_start(args, format);
	vsnprintf(buf.data(), BUFFER_SIZE, format, args);
	va_end(args);

	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleColor(color);
	std::cout << "[+] " << buf.data() << std::endl;
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

inline void error_print(const char* format, ...)
{
	constexpr int BUFFER_SIZE = 4096;
	std::vector<char> buf(BUFFER_SIZE);
	va_list args;
	va_start(args, format);
	vsnprintf(buf.data(), BUFFER_SIZE, format, args);
	va_end(args);
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleColor(FOREGROUND_RED);
	std::cout << "[-] " << buf.data() << std::endl;
	SetConsoleColor(FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
}

void RedirectToNewConsole();