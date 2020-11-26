#pragma once
#include <Windows.h>

struct ProcessThread {
	LPCWSTR ProcessName;
	DWORD dwProcessId;
	HANDLE hProcess;
	
	DWORD dwThreadId;
	HANDLE hThread;
};

typedef ProcessThread* PProcessThread;