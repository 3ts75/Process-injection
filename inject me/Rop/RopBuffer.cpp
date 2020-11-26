#include "RopBuffer.h"

RopBuffer::RopBuffer(ULONGLONG Rsp, SIZE_T RopBufferEntries) :
	TargetRsp{ Rsp }, CurrentPosition{ 0 }, DataPosition{ (ULONG)RopBufferEntries - 1 }, Buffer{ nullptr } {
	Buffer = new ULONGLONG[RopBufferEntries];
	BufferSize = RopBufferEntries * sizeof(ULONGLONG);
	ZeroMemory(Buffer, BufferSize);
}

RopBuffer::~RopBuffer() {
	if (Buffer != NULL)
		delete[] Buffer;

	Buffer = NULL;
}

ULONGLONG RopBuffer::SetRip(ULONGLONG Address) {
	TargetRip = Address;
	return TargetRip;
}

ULONGLONG RopBuffer::InsertRopValue(ULONGLONG Value) {
#ifdef _DEBUG
	if (CurrentPosition >= DataPosition)
		throw; // ROP Buffer too small
#endif

	Buffer[CurrentPosition] = Value;
	++CurrentPosition;

	return Value;
}

ULONGLONG RopBuffer::InsertRopDataPointer(ULONGLONG Data) {
	ULONGLONG Value;

#ifdef _DEBUG
	if (DataPosition <= CurrentPosition)
		throw; // ROP Buffer too small
#endif

	Buffer[DataPosition] = Data;

	Value = TargetRsp + sizeof(ULONGLONG) * DataPosition;

	--DataPosition;

	return InsertRopValue(Value);
}

ULONGLONG* RopBuffer::GetBuffer() {
	return Buffer;
}

SIZE_T RopBuffer::GetBufferSize() {
	return BufferSize;
}

ULONGLONG RopBuffer::GetRip() {
	return TargetRip;
}

ULONGLONG RopBuffer::GetRsp() {
	return TargetRsp;
}