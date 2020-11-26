#pragma once
#include <Windows.h>
#include <TlHelp32.h>

#include <vector>
#include <string>

#include "create_type.h"

class ThreadInfo {
public:
	DWORD dwThreadId;  // スレッドID
	CreateHandle hThread;  // スレッドハンドル
};

class ProcessThreads {
	std::wstring wstrProcessName;  // プロセス名
	DWORD dwProcessId;  // プロセスID
	CreateHandle hProcess;  // プロセスハンドル
	std::vector<ThreadInfo> Threads;

public:
	ProcessThreads(std::wstring str);

	const std::wstring GetProcessName();

	void SetProcessId(DWORD id);
	const DWORD GetProcessId();

	void SetProcessHandle();
	const HANDLE GetProcessHandle();

	void SetThreadId(DWORD id);

	void SetThreadHandle();

	const std::vector<ThreadInfo> GetThreads();
};

typedef ProcessThreads* PProcessThreads;

// ターゲットのプロセス名からプロセスIDを取得
void NameToProcessId(ProcessThreads& pt);

// ターゲットのプロセスIDからスレッドIDを取得
void ProcessIdToThreadIds(ProcessThreads& pt);