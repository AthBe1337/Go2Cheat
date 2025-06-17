#pragma once
#include <Windows.h>
#include <cstdint>
#include <iostream>
#include <vector>
#include <thread>
#include <mutex>

#include "../driver.h"
#include "offsets.h"

namespace utils {
	namespace memory {

		inline std::mutex resultMutex;
		inline uintptr_t foundAddress = 0;

		inline void SearchInRange(uintptr_t start, uintptr_t end, std::vector<int> bytes, bool isOffset) {

			int patternSize = bytes.size();
			for (uintptr_t i = start; i <= end; i++)
			{
				if (isOffset && !((i & 0xFFFF) == client_offset))
					continue;
				bool found = true;
				for (int j = 0; j < patternSize; j++)
				{
					if (bytes[j] != -1 && driver::read_memory<unsigned char>(driver_handle, i + j) != bytes[j])
					{
						found = false;
						break;
					}
				}
				if (found)
				{
					std::lock_guard<std::mutex> lock(resultMutex);
					foundAddress = i;
					return;
				}
			}
		}


		inline std::vector<int> PatternToBytes(const char* pattern) {
			std::vector<int> bytes;
			const char* current = pattern;
			while (*current) {
				if (*current == '?') {
					bytes.push_back(-1);  // Wildcard
					current++;
					if (*current == '?') current++;  // Skip second '?'
				}
				else {
					bytes.push_back(static_cast<int>(std::strtoul(current, nullptr, 16)));
					current += 2;
				}
				if (*current == ' ') current++;  // Skip spaces
			}
			return bytes;
		}


		inline uintptr_t FindPattern(const char* pattern, int threadNum)
		{
			uintptr_t minAddress = 0x7FF000000000;
			uintptr_t maxAddress = 0x7FFFFFFFFFFF;

			std::vector<int> bytes = PatternToBytes(pattern);
			uintptr_t rangeSize = (maxAddress - minAddress) / threadNum;

			std::vector<std::thread> threads;
			for (int i = 0; i < threadNum; i++)
			{
				uintptr_t start = minAddress + rangeSize * i;
				uintptr_t end = ((i == threadNum - 1) ? maxAddress : (start + rangeSize));
				threads.push_back(std::thread(SearchInRange, start, end, bytes, true));
			}

			for (auto& t : threads) {
				if (t.joinable())
					t.join();
			}

			return foundAddress;
		}
	}
}