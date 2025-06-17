#pragma once
#include "process.h"

DWORD utils::process::get_process_id(const wchar_t* process_name) {
		DWORD process_id = 0;

		HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);
		if (snapshot == INVALID_HANDLE_VALUE)
			return process_id;

		PROCESSENTRY32 entry = {};
		entry.dwSize = sizeof(decltype(entry));

		if (Process32FirstW(snapshot, &entry) == TRUE) {

			if (_wcsicmp(process_name, entry.szExeFile) == 0)
				process_id = entry.th32ProcessID;
			else {
				while (Process32NextW(snapshot, &entry) == TRUE) {
					if (_wcsicmp(process_name, entry.szExeFile) == 0) {
						process_id = entry.th32ProcessID;
						break;
					}
				}
			}
		}

		CloseHandle(snapshot);

		return process_id;
}

std::uintptr_t utils::process::get_module_base(const DWORD pid, const wchar_t* module_name) {
	std::uintptr_t module_base = 0;

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, pid);
	if (snapshot == INVALID_HANDLE_VALUE)
		return module_base;

	MODULEENTRY32W entry = {};
	entry.dwSize = sizeof(decltype(entry));

	if (Module32FirstW(snapshot, &entry) == TRUE) {
		if (wcsstr(module_name, entry.szModule) != nullptr)
			module_base = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
		else {
			while (Module32NextW(snapshot, &entry) == TRUE) {
				if (wcsstr(module_name, entry.szModule) != nullptr) {
					module_base = reinterpret_cast<std::uintptr_t>(entry.modBaseAddr);
				}
			}
		}
	}

	CloseHandle(snapshot);

	return module_base;
}

bool utils::process::is_process_running(DWORD pid) {
	HANDLE process = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
	if (process == NULL)
		return false;
	DWORD exit_code = 0;
	GetExitCodeProcess(process, &exit_code);
	CloseHandle(process);
	return exit_code == STILL_ACTIVE;
}