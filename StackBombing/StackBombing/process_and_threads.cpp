#include <vector>

#include <Windows.h>
#include <TlHelp32.h>

DWORD NameToPid(const WCHAR* kw_process_name) {
	DWORD dw_process_id = 0;
	PROCESSENTRY32* p_process_entry = new PROCESSENTRY32();

	HANDLE h_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if (h_snapshot) {
		p_process_entry->dwSize = sizeof(*p_process_entry);
		if (Process32First(h_snapshot, p_process_entry)) {
			do {
				if (lstrcmp(kw_process_name, p_process_entry->szExeFile) == 0) {
					dw_process_id = p_process_entry->th32ProcessID;
					break;
				}
			} while (Process32Next(h_snapshot, p_process_entry));
		}

		CloseHandle(h_snapshot);
	}

	return dw_process_id;
}

std::vector<DWORD> PidToTids(const DWORD kdw_process_id) {
	std::vector<DWORD> vecdw_threads_id;
	THREADENTRY32* p_thread_entry = new THREADENTRY32();

	HANDLE h_snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPTHREAD, 0);
	if (h_snapshot) {
		p_thread_entry->dwSize = sizeof(*p_thread_entry);
		if (Thread32First(h_snapshot, p_thread_entry)) {
			do {
				if (kdw_process_id == p_thread_entry->th32OwnerProcessID)
					vecdw_threads_id.push_back(p_thread_entry->th32ThreadID);
			} while (Thread32Next(h_snapshot, p_thread_entry));
		}

		CloseHandle(h_snapshot);
	}

	return vecdw_threads_id;
}