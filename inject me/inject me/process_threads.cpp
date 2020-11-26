#include "process_threads.h"

ProcessThreads::ProcessThreads(std::wstring str) : Threads{}, hProcess{ 0 }, dwProcessId{ 0 }, wstrProcessName{str} {}

const std::wstring ProcessThreads::GetProcessName() {
	return this->wstrProcessName;
}

void ProcessThreads::SetProcessId(DWORD id) {
	this->dwProcessId = id;
}

const DWORD ProcessThreads::GetProcessId() {
	return this->dwProcessId;
}

void ProcessThreads::SetProcessHandle() {
	this->hProcess.SetHandle(OpenProcess(PROCESS_ALL_ACCESS, FALSE, this->dwProcessId));
}

const HANDLE ProcessThreads::GetProcessHandle() {
	return this->hProcess.GetHandle();
}

void ProcessThreads::SetThreadId(DWORD id) {
	this->Threads.push_back({ id, 0 });
}

void ProcessThreads::SetThreadHandle() {
	for (int i = 0; i < this->Threads.size(); ++i)
		this->Threads[i].hThread.SetHandle(OpenThread(THREAD_ALL_ACCESS, FALSE, this->Threads[i].dwThreadId));
}

const std::vector<ThreadInfo> ProcessThreads::GetThreads() {
	return this->Threads;
}

void NameToProcessId(ProcessThreads& pt) {
	Heap<LPPROCESSENTRY32W> lppe32{ new PROCESSENTRY32W() };
	ZeroMemory(lppe32.Get(), sizeof(*lppe32.Get()));
	lppe32.Get()->dwSize = sizeof(*lppe32.Get());

	CreateHandle hSnapshot{ CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, FALSE) };

	if (Process32FirstW(hSnapshot.GetHandle(), lppe32.Get())) {
		do {
			if (wcscmp(pt.GetProcessName().c_str(), lppe32.Get()->szExeFile) == 0) {
				pt.SetProcessId(lppe32.Get()->th32ProcessID);
				break;
			}
		} while (Process32NextW(hSnapshot.GetHandle(), lppe32.Get()));
	}
}

void ProcessIdToThreadIds(ProcessThreads& pt) {
	Heap<LPTHREADENTRY32> lpte32{ new THREADENTRY32() };
	ZeroMemory(lpte32.Get(), sizeof(*lpte32.Get()));
	lpte32.Get()->dwSize = sizeof(*lpte32.Get());

	CreateHandle hSnapshot{ CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, FALSE) };

	if (Thread32First(hSnapshot.GetHandle(), lpte32.Get())) {
		do {
			if (lpte32.Get()->th32OwnerProcessID == pt.GetProcessId())
				pt.SetThreadId(lpte32.Get()->th32ThreadID);
		} while (Thread32Next(hSnapshot.GetHandle(), lpte32.Get()));
	}
}