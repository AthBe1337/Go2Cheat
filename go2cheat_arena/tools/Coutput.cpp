#include "Coutput.h"

void SetConsoleColor(WORD color)
{
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
	SetConsoleTextAttribute(hConsole, color);
}

void RedirectToNewConsole()
{
	// 分配新控制台
	AllocConsole();

	// 重定向标准输出(stdout)和标准错误(stderr)
	FILE* newStdout = nullptr;
	FILE* newStderr = nullptr;

	// 使用 freopen_s 重定向 stdout
	if (freopen_s(&newStdout, "CONOUT$", "w", stdout) != 0)
	{
		MessageBoxA(NULL, "无法重定向 stdout", "错误", MB_ICONERROR);
	}

	// 使用 freopen_s 重定向 stderr
	if (freopen_s(&newStderr, "CONOUT$", "w", stderr) != 0)
	{
		MessageBoxA(NULL, "无法重定向 stderr", "错误", MB_ICONERROR);
	}

	// 确保缓冲区立即刷新（避免输出延迟）
	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stderr, NULL, _IONBF, 0);

	// 设置控制台标题
	SetConsoleTitle(L"Go2Cheat Arena");

	// utf-8
	SetConsoleCP(65001); 
	SetConsoleOutputCP(65001); 

	SetConsoleColor(FOREGROUND_GREEN | FOREGROUND_INTENSITY);
	std::cout << "   ___     ____    ___ _                _   " << std::endl;
	std::cout << "  / _ \\___|___ \\  / __\\ |__   ___  __ _| |_ " << std::endl;
	std::cout << " / /_\\/ _ \\ __) |/ /  | '_ \\ / _ \\/ _` | __|" << std::endl;
	std::cout << "/ /_\\\\ (_) / __// /___| | | |  __/ (_| | |_ " << std::endl;
	std::cout << "\\____/\\___/_____\\____/|_| |_|\\___|\\__,_|\\__|\n" << std::endl;
	SetConsoleColor(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

	SetConsoleColor(FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	std::cout << "Go2Cheat Arena" << std::endl;
	std::cout << "Version: 1.0" << std::endl;
	std::cout << "\n";
	SetConsoleColor(FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_RED);

}
