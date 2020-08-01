#ifndef StackBombing_set_remote_memory_H_
#define StackBombing_set_remote_memory_H_

#include <Windows.h>

#include "pinject_packet.h"

typedef struct {
	DWORD dwThreadId;
	HANDLE hThread;
} TARGET_THREAD;

int WritePayload(TARGET_THREAD* target, TStrDWORD64Map* params);

#endif // !StackBombing_set_remote_memory_H_