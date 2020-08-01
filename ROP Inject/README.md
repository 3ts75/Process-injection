# Backup windows

## ROP Inject
Windowsの引数の扱い
```
rcx = arg1
rdx = arg2
r8 = arg3
r9 = arg4

[stack]
| NULL |
+------+
| NULL |
+------+
| NULL |
+------+
| NULL |
+------+
| arg5 |
+------+
| arg6 |
+------+
| ...  |
```

Windows 10で使うROP Gadgets
```asm
[ pop gadgets ]
pop rax  // 0x58
pop rdx  // 0x5a
pop rcx  // 0x59
pop r8  // 0x41 0x58
pop r9  // 0x41 0x59
pop r10  // 0x41 0x5a
pop r11  // 0x41 0x5b
ret  // 0xc3

[ push rax ]  // kernel32.dll
push rax  // 0x50
ret  // 0xc3
```

今回は**ZwAllocateVirtualMemory**->**memcpy**の順に呼び出す
- ZwAllocateVirtualMemory
  - RWXでメモリをallocateする
- memcpy
  - RWに書き込んだshellcodeをZwAllocateVirutalMemoryで確保したメモリにコピーする

```cpp
NTSTATUS ZwAllocateVirtualMemory(
  _In_    HANDLE    ProcessHandle,
  _Inout_ PVOID     *BaseAddress,
  _In_    ULONG_PTR ZeroBits,
  _Inout_ PSIZE_T   RegionSize,
  _In_    ULONG     AllocationType,
  _In_    ULONG     Protect
);

rcx = ProcessHandle
rdx = *BaseAddress
r8 = ZeroBits
r9 = RegionSize

[stack]
| NULL         |
+--------------+
| NULL         |
+--------------+
| NULL         |
+--------------+
| NULL         |
+--------------+
| AllocateType |
+--------------+
| Protect      |
```

```cpp
void *memcpy(
   void *dest,
   const void *src,
   size_t count
);

rcx = *dest
rdx = *src
r8 = count
```

これらより作成したスタック構成
```
| *BaseAddress                      |  // rdx
+-----------------------------------+
| ProcessHandle                     |  // rcx
+-----------------------------------+
| ZeroBits                          |  // r8
+-----------------------------------+
| RegionSize                        |  // r9
+-----------------------------------+
| NULL                              |  // r10
+-----------------------------------+
| NULL                              |  // r11
+-----------------------------------+
| <ZwAllocateVirtualMemory Address> |
+-----------------------------------+
| <PopGadgets Address>              |
+-----------------------------------+
| NULL                              |  // rdx
+-----------------------------------+
| NULL                              |  // rcx
+-----------------------------------+
| NULL                              |  // r8
+-----------------------------------+
| NULL                              |  // r9
+-----------------------------------+
| AllocationType                    |  // r10
+-----------------------------------+
| Protect                           |  // r11
+-----------------------------------+
| <PopGadgets Address>              |
+-----------------------------------+
| *src                              |  // rdx
+-----------------------------------+
| *dest                             |  // rcx
+-----------------------------------+
| count                             |  // r8
+-----------------------------------+
| NULL                              |  // r9
+-----------------------------------+
| NULL                              |  // r10
+-----------------------------------+
| NULL                              |  // r11
+-----------------------------------+
| <memcpy Address>                  |
+-----------------------------------+
| <PushRaxGadget Address>           |
```
