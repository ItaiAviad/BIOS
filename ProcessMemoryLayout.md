# Process Memory Layout

### size: 8MB

|Name|Size (bytes)|Virtual Address|
|--|--|--|
|**binary**|0x20000 (128KB)|0x0|
|**page frame allocator (pfa)**|0x1000 (4KB)|after **binary**|
|**pml4t**|0x1000 (4KB)|after **pfa**|
|**stack**|128KB|0x0-0x400000 (random)|
|**heap**|128KB|0x0-0x400000 (random)|
|**kernel** (higher half)|0x400000 (4MB)|0x400000 (4MB)|

||**bin**|**pfa**|**pml4t**|**stack**|**heap**|**kernel**|
|--|--|--|--|--|--|--|
|size|128KB|128KB||128KB|128KB|4MB|
