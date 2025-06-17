#pragma once
#include "driver.h"

HANDLE driver_handle = 0;

bool driver::attach_to_process(HANDLE driver_handle, const DWORD pid) {
	Request r;
	r.process_id = reinterpret_cast<HANDLE>(pid);

	return DeviceIoControl(driver_handle, codes::attach, &r, sizeof(r), &r, sizeof(r), nullptr, nullptr);
}

HANDLE driver::get_driver_handle() {
	return CreateFile(L"\\\\.\\G2CDriver", GENERIC_READ, 0, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
}

bool driver::initialize() {
	driver_handle = driver::get_driver_handle();

	if (driver_handle == INVALID_HANDLE_VALUE) {
		std::cout << "Failed to create driver handle." << std::endl;
		std::cin.get();
		return false;
	}
	
	return true;
}

void driver::close() {
	CloseHandle(driver_handle);
}