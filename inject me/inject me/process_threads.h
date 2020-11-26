#pragma once
#include <Windows.h>
#include <TlHelp32.h>

#include <vector>
#include <string>

#include "create_type.h"

class ThreadInfo {
public:
	DWORD dwThreadId;  // �X���b�hID
	CreateHandle hThread;  // �X���b�h�n���h��
};

class ProcessThreads {
	std::wstring wstrProcessName;  // �v���Z�X��
	DWORD dwProcessId;  // �v���Z�XID
	CreateHandle hProcess;  // �v���Z�X�n���h��
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

// �^�[�Q�b�g�̃v���Z�X������v���Z�XID���擾
void NameToProcessId(ProcessThreads& pt);

// �^�[�Q�b�g�̃v���Z�XID����X���b�hID���擾
void ProcessIdToThreadIds(ProcessThreads& pt);