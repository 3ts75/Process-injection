#include <Windows.h>
#include <stdio.h>

int main(int argc, char* argv[]) {
    if (argc != 2) {
        printf("[-] Please \".\a.exe <target process id>\"");
        return 1;
    }
    
    const char* dll_path = "C:\\tmp\\msgbox.dll";
    DWORD dll_path_size = strlen(dll_path);

    DWORD dwProcessId = atoi(argv[1]);
    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, dwProcessId);
    LPVOID lpBaseAddress = VirtualAllocEx(hProcess, NULL, dll_path_size, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    WriteProcessMemory(hProcess, lpBaseAddress, dll_path, dll_path_size, NULL);
    CreateRemoteThread(hProcess, NULL, 0, (LPTHREAD_START_ROUTINE)LoadLibrary, lpBaseAddress, 0, NULL);
    return 0;
}