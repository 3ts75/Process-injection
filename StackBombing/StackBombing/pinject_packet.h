#ifndef StackBombing_pinject_packet_H_
#define StackBombing_pinject_packet_H_

#include <string>
#include <map>

#include <Windows.h>

typedef std::map<std::string, DWORD64> TStrDWORD64Map;

typedef struct {
	VOID* buffer;
	DWORD buffer_size;
	TStrDWORD64Map* metadata;
} PINJECT_PACKET;

#endif // !StackBombing_pinject_packet_H_
