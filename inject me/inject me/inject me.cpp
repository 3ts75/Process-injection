#include "process_threads.h"

#include <iostream>
using namespace std;

unsigned char payload[] = "hello world";

bool inject(ProcessThreads& pt) {
	// 自分のプロセスハンドルを複製する
	pt.SetProcessHandle();
	Heap<LPHANDLE> hCurrent{ new HANDLE() };
	DuplicateHandle(GetCurrentProcess(), GetCurrentProcess(), pt.GetProcessHandle(), hCurrent.Get(), NULL, FALSE, DUPLICATE_SAME_ACCESS);

	// VirutalAllocExを用いてRWXを割り当てる(payload)
	PVOID payload_address = VirtualAllocEx(pt.GetProcessHandle(), 0, 1, MEM_COMMIT, PAGE_EXECUTE_READWRITE);

	// VirutalAllocExを用いてRWを割り当てる(dummy stack)
	PVOID dummy_stack = VirtualAllocEx(pt.GetProcessHandle(), 0, 1, MEM_COMMIT, PAGE_READWRITE);

	// payloadをメモリに格納する

	// payloadのサイズを確認する
	DWORD payload_size = sizeof(payload);

	// GetProcAddressでkernel32!RtlExitThreadを見つける
	LPVOID _RtlExitThread{ GetProcAddress(GetModuleHandle(L"kernel32"), "RtlExitThread") };

	// CreateRemoteThreadでSuspend状態のスレッドを作成する
	CreateRemoteThread(
		pt.GetProcessHandle(),

		);

	// GetProcAddressでntdll!RtlCopyMemoryを見つける

	// NtQueueApcThreadを呼び、RtlCopyMemory(src=RtlExitThread,dst=dummy stack)をセットする

	// ResumeThreadを実行する

	// WaitForSingleObjectで待機する

	// ntdll or kernel32からjmp rbx(0xffe3)を見つける

	// GetProcAddressでntdll!ReadProcessMemoryを見つける

	// CreateRemoteThreadでSuspend状態のスレッドを作成する(jmp rbxを指すようにする)

	// SetThreadContextでrbxをjmp rbxを指すようにする

	// ResumeThreadを実行する

	// 使用しているスレッドを中断し、GetThreadContextでripがjmp rbxであるかを確認する

	// SetThreadContextでrcx=hProcess,rdx=lpBaseAddress,r8=lpBuffer,r9=nSize,rsp=zero stack,rbx=ReadProcessMemoryをセットする

	return false;
}

int main() {
	ProcessThreads target_process{ L"notepad.exe" };
	wcout << "[*] Target process name is " << target_process.GetProcessName() << endl;

	NameToProcessId(target_process);
	wcout << "[+] Process id is " << target_process.GetProcessId() << endl;

	ProcessIdToThreadIds(target_process);
	for (auto thread : target_process.GetThreads())
		wcout << "[+] Thread id is " << thread.dwThreadId << endl;

	if (inject(target_process)) {
		cout << "Didn't succeed" << endl;
	}
	else {
		cout << "Successful" << endl;
	}
}