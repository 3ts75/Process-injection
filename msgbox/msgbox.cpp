#include <Windows.h>
#pragma comment(lib, "user32.lib")

BOOL WINAPI
DllMain(HMODULE, DWORD call, LPVOID) {
    MessageBoxA(NULL, "Inject!!", "Hack", MB_OK);
}