#include <iostream>
using namespace std;

#include "process_and_threads.h"
#include "inject_and_resume.h"

int main() {
	DWORD dw_process_id = NameToPid(TEXT("notepad.exe"));
	if (!dw_process_id)
		return 1;
	cout << dw_process_id << endl;

	vector<DWORD> vecdw_threads_id = PidToTids(dw_process_id);
	if (vecdw_threads_id.size() == 0)
		return 1;
	for (DWORD dw_thread_id : vecdw_threads_id)
		cout << dw_thread_id << " ";
	cout << endl;

	for (DWORD dw_thread_id : vecdw_threads_id) {
		inject(dw_thread_id);
		Sleep(300);
	}

	return 0;
}