#pragma once
#include <Windows.h>

class CreateHandle {
	HANDLE _handle;
public:
	CreateHandle(HANDLE handle) :_handle{ handle } {};
	~CreateHandle() { CloseHandle(_handle); };

	CreateHandle(const CreateHandle&) = default;  // コピーコンストラクタ
	CreateHandle& operator=(const CreateHandle&) = delete;  // ムーブコンストラクタ
	CreateHandle(CreateHandle&&) = delete;  // コピー代入演算子
	CreateHandle& operator=(CreateHandle&&) = delete;  // ムーブ代入演算子

	const PVOID GetHandle() { return this->_handle; }
	void SetHandle(HANDLE handle) { _handle = handle; }
};

typedef CreateHandle * PCreateHandle;

template<typename T>
class Heap {
	T _address;

public:
	Heap(T address) : _address{ address } {}
	~Heap() { delete[] _address; }

	Heap(const Heap&) = delete;
	Heap& operator=(const Heap&) = delete;
	Heap(Heap&&) = delete;
	Heap& operator=(Heap&&) = delete;

	T Get() { return this->_address; }
};