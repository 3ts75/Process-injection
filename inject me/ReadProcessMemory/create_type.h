#pragma once
#include <Windows.h>

class CreateHModule {
	HMODULE _hmodule;
public:
	CreateHModule(LPCWSTR str) { _hmodule = LoadLibraryW(str); }
	~CreateHModule() { FreeLibrary(_hmodule); }

	HMODULE GetHModule() { return this->_hmodule; }
};

class CreateHandle {
	HANDLE _handle;
public:
	CreateHandle(HANDLE handle) : _handle{ handle } {}
	~CreateHandle() { CloseHandle(_handle); }

	void SetHandle(HANDLE handle) { this->_handle = handle; }
	HANDLE GetHandle() { return this->_handle; }
};