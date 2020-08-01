#ifndef StackBombing_process_and_threads_H_
#define StackBombing_process_and_threads_H_

#include <vector>

#include <Windows.h>

DWORD NameToPid(const WCHAR* kw_process_name);

std::vector<DWORD> PidToTids(const DWORD kdw_process_id);

#endif // !StackBombing_process_and_threads_H_
