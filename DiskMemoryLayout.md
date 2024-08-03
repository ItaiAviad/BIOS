# Disk Memory Layout

|Name|Size (bytes)|Base Address (on Disk)|Loaded Base Address (Physical Memory)|
|--|--|--|--|
|PML4T|0x1000||0x1000|
|PDPT|0x1000||0x2000|
|PDT|0x1000||0x3000|
|PT_0 (2MB)|0x1000||0x4000|
|PT_1 (2MB)|0x1000||0x5000|
|**Bootloader** Real Mode Sector|512|0x0|0x7C00|
|**Bootloader** Protected Mode Sector|512|0x200|0x7E00|
|**Bootloader** Long Mode Sector|512|0x400|0x8000|
|**Stack** (Kernel)|||0xF000|
|**Kernel**||0x600|0xF000|
|**Page Frame Allocator**|||After Kernel (Page Aligned)|
