ENTRY(_kentry)
phys = $(KERNEL_VBASE);
SECTIONS
{
    . = phys;
    __entry = .;
    .entry       : { __entry_start = .;     *(.entry)   }
    .text        : { __text_start = .;      *(.text)    }
    .rodata      : { __rodata_start = .;    *(.rodata)  }
    .data        : { __data_start = .;      *(.data)    }
    .bss         : { __bss_start = .;       *(.bss)     }
}
