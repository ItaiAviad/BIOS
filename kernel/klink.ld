ENTRY(_kentry)
OUTPUT_FORMAT("binary")
phys = $(KERNEL_LOAD_ADDR);
SECTIONS
{
    . = phys;

    .entry       : { __entry_start = .;     *(.entry)   }
    .text        : { __text_start = .;      *(.text)    }
    .data        : { __data_start = .;      *(.data)    }
    .rodata      : { __rodata_start = .;    *(.rodata)  }
    .bss         : { __bss_start = .;       *(.bss)     }

    __end = .;
}
