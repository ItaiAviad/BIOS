# Disk Memory Layout

|Name|Size (bytes)|Base Address (on Disk)|Loaded Base Address (Physical Memory)|
|--|--|--|--|
|**Bootloader** PML4T|0x1000||0x1000|
|**Bootloader** PDPT|0x1000||0x2000|
|**Bootloader** PDT|0x1000||0x3000|
|**Bootloader** PT_0 (2MB)|0x1000||0x4000|
|**Bootloader** PT_1 (2MB)|0x1000||0x5000|
|**Bootloader** PT_2 (2MB)|0x1000||0x6000|
|**Bootloader** Real Mode Sector|512|0x0|0x7C00|
|**Bootloader** Protected Mode Sector|512|0x200|0x7E00|
|**Bootloader** Long Mode Sector|512|0x400|0x8000|
|**Kernel** Stack|||0xF000|
|**Kernel**||0x600|0x4F000|
|**Kernel** Page Frame Allocator|||0x200000 (2MB)|
|**Kernel** PML4T|||0x400000 (4MB)|
|**Userspace** Stack|||0x4000F000 (1GB + 0xF000)|
|**Userspace**||0x600|0x4000F000 (1GB + 0xF000)|
|**Userspace** Page Frame Allocator|||0x40200000 (1GB + 2MB)|
|**Userspace** PML4T|||0x40400000 (1GB + 4MB)|
