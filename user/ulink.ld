ENTRY(_uentry)
phys = $(PROC_BIN_ADDR);
SECTIONS
{
    . = phys;
    __entry = .;
    .entry       : { __entry_start = .;     *(.entry)   }
    .text        : { __text_start = .;      *(.text)    }
    .data        : { __data_start = .;      *(.data)      __end = .; }
    .rodata      : { __rodata_start = .;    *(.rodata)  }
    .bss         : { __bss_start = .;       *(.bss)     }
}
