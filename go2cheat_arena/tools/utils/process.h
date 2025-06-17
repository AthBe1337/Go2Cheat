#pragma once
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

namespace utils {

	namespace process {
		DWORD get_process_id(const wchar_t* process_name);

		bool is_process_running(DWORD pid);

		std::uintptr_t get_module_base(const DWORD pid, const wchar_t* module_name);
	}
}