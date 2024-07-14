# Disk Memory Layout

|Name|Size (bytes)|Base Address|Loaded Base Address|
|--|--|--|--|
|Bootloader Real Mode Sector|512|0x0|0x7C00|
|Bootloader Protected Mode Sector|512|0x200|0x7E00|
|Bootloader Long Mode Sector|512|0x400|0x8000|
|Kernel|||

