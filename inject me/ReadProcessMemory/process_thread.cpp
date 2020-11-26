#include "process_thread.h"

void NameToProcessId(PProcessThread process) {
	LPPROCESSENTRY32 lppe32 = new PROCESSENTRY32();
	ZeroMemory(lppe32, sizeof(*lppe32));
	lppe32->dwSize = sizeof(*lppe32);

	CreateHandle hSnapshot{ CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, FALSE) };

	if (Process32First(hSnapshot.GetHandle(), lppe32)) {
		do {
			if (lstrcmp(lppe32->szExeFile, process->ProcessName) == 0) {
				process->dwProcessId = lppe32->th32ProcessID;
				break;
			}
		} while (Process32Next(hSnapshot.GetHandle(), lppe32));
	}

	delete[] lppe32;
}