#include "Payload.h"

bool Payload::FindFunctionAddress() {
	CreateHModule hUser32{ L"user32" };
	this->FuncAddress.MessageBoxAddress = reinterpret_cast<ULONGLONG>(GetProcAddress(hUser32.GetHModule(), "MessageBoxA"));
	CreateHModule hNtdll{ L"ntdll" };
	this->FuncAddress.RtlExitUserThreadAddress = reinterpret_cast<ULONGLONG>(GetProcAddress(hNtdll.GetHModule(), "RtlExitUserThread"));

	return 0;
}