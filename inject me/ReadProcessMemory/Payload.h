#pragma once
#include <Windows.h>

#include "create_type.h"

typedef struct _FunctionAddress {
	ULONGLONG MessageBoxAddress;
	ULONGLONG RtlExitUserThreadAddress;
} FunctionAddress, * PFunctionAddress;

class Payload{
public:
	unsigned char payload[76] = {
	   0x48, 0x83, 0xec, 0x28,
	   0x48, 0x83, 0xe4, 0xf0,
	   0x48, 0xc7, 0xc1, 0x00, 0x00, 0x00,0x00,
	   0x49, 0xb8, 0x12, 0x12, 0x12, 0x12, 0x12,
	   0x12, 0x12, 0x12,
	   0x48, 0xba, 0x23, 0x23, 0x23, 0x23, 0x23,
	   0x23, 0x23, 0x23,
	   0x45, 0x33, 0xc9,
	   0x48, 0xb8, 0x34, 0x34, 0x34, 0x34, 0x34,
	   0x34, 0x34, 0x34,
	   0xff, 0xd0,
	   0x48, 0xc7, 0xc1, 0x00, 0x00, 0x00, 0x00,
	   0x48, 0xb8, 0x45, 0x45, 0x45, 0x45, 0x45,
	   0x45, 0x45, 0x45,
	   0xff, 0xd0,
	   'I', 'n', 'j', 'e', 'c', 't', 0x00,
	};
	FunctionAddress FuncAddress;
public:
	bool FindFunctionAddress();
};