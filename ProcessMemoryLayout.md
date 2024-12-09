# Process Memory Layout

### size: 8MB

|Name|Size (bytes)|Virtual Address|
|--|--|--|
|**binary**|0x20000 (128KB)|0x0|
|**page frame allocator (pfa)**|128KB|after **binary**|
|**pml4t**|0x1000 (4KB)|after **pfa**|
|**stack**|128KB|random*|
|**heap**|128KB|random*|
|**kernel** (higher half)|(2MB)|0x400000 (4MB)|
|**kernel pfa**|1MB|after **kernel**|
|**boot** (bootloader memory (gdt, tss))||after kernel pfa|

random*: ASLR like random address of stack and heap. Address is between process maps and kernel maps (after process **pml4t**, before **kernel**)

Note: **kernel** includes **IDT** but not **GDT** and **TSS**! **GDT** and **TSS** are mapped separately (during boot).

||**bin**|**pfa**|**pml4t**|**stack**|**heap**|**kernel**|**kernel pfa**|**boot**|
|--|--|--|--|--|--|--|--|--|
|size|128KB|128KB|4KB|128KB|128KB|2MB|1MB||
