# Disk Memory Layout

|Name|Size (bytes)|Base Address|Loaded Base Address|
|--|--|--|--|
|Bootloader Real Mode Sector|512|0x0|0x7C00|
|Bootloader Protected Mode Sector|512|0x200|0x7E00|
|Bootloader Long Mode Sector|512|0x400|0x8000|
|Kernel||0x600|0x8C00|
|Page Frame Allocator|#_PAGES * 8||After Kernel (Page Aligned)|
|PML4T|0x1000||0x1000|
|PDPT|0x1000||0x2000|
|PDT|0x1000||0x3000|
|PT|0x1000||0x4000|
