#include "memmem.h"

// https://stackoverflow.com/questions/52988769/writing-own-memmem-for-windows
void* memmem(const void* haystack, size_t haystack_len, const void* const needle, const size_t needle_len)
{
	if (haystack == NULL) return NULL; // or assert(haystack != NULL);
	if (haystack_len == 0) return NULL;
	if (needle == NULL) return NULL; // or assert(needle != NULL);
	if (needle_len == 0) return NULL;

	for (const char* h = (char*)haystack;
		haystack_len >= needle_len;
		++h, --haystack_len) {
		if (!memcmp(h, needle, needle_len)) {
			return (void*)h;
		}
	}
	return NULL;
}