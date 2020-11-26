#pragma once
#include <Windows.h>
#include <TlHelp32.h>

#include "create_type.h"

typedef struct _ProcessThread {
	LPCWSTR ProcessName;
	DWORD dwProcessId{ 0 };
	CreateHandle hProcess{ 0 };
	DWORD dwThreadId{ 0 };
	CreateHandle hThread{ 0 };
} ProcessThread, * PProcessThread;

void NameToProcessId(PProcessThread process);