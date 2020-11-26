#include <Windows.h>
#include <TlHelp32.h>

#include <iostream>
using namespace std;

#include "RopBuffer.h"
#include "RopHelper.h"

DWORD NameToProcessId(PProcessThread process) {
	LPPROCESSENTRY32 lppe32 = new PROCESSENTRY32();
	ZeroMemory(lppe32, sizeof(*lppe32));
	lppe32->dwSize = sizeof(*lppe32);

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, FALSE);

	if (Process32First(hSnapshot, lppe32)) {
		do {
			if (lstrcmp(lppe32->szExeFile, process->ProcessName) == 0) {
				process->dwProcessId = lppe32->th32ProcessID;
				break;
			}
		} while (Process32Next(hSnapshot, lppe32));
	}

	if (hSnapshot)
		CloseHandle(hSnapshot);
	if (lppe32)
		delete[] lppe32;
	return process->dwProcessId;
}

int main() {
	const char hello[] = "helloworld";
	DWORD size = sizeof(hello);
	LPVOID address = VirtualAlloc(0, size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy_s(address, size, hello, size);

	PProcessThread target = new ProcessThread();
	ZeroMemory(target, sizeof(*target));
	target->ProcessName = L"notepad.exe";


}