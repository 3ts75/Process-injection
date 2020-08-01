#ifndef StackBombing_memmem_H_
#define StackBombing_memmem_H_

#include <windows.h>

void* memmem(const void* haystack, size_t haystack_len, const void* const needle, const size_t needle_len);

#endif // !StackBombing_memmem_H_
