ENTRY(main)
OUTPUT_ARCH(m68k)
OUTPUT_FORMAT(srec)

MEMORY {
    ROM (rx)    : o = 0x00000000, l = 524288
    RAM (rwx)   : o = 0x00080000, l = 131072
}

SECTIONS
{
    stack_base = 0x000A0000;
    start_text = 0x00002000;
    
    .start : 
    {
        LONG(stack_base);
        LONG(main);
        . = ALIGN(4);
    } > ROM

    .text start_text :
    {
        . = ALIGN(4);
        *(.text); 
        . = ALIGN(4);
        end_text = .;
    } > ROM

    .rodata : 
    {
       . = ALIGN(4);
       start_rodata = .;
       *(.rodata);
       . = ALIGN(4);
       end_rodata = .;
    } > ROM

    .data : 
    {
        . = ALIGN(4);
        start_data = .;
        *(.data);
        end_data = .;
    } > RAM AT > ROM

    .bss : 
    {
        . = ALIGN(4);
        start_bss = .;
        *(.bss)
        end_bss = .;
    } > RAM
}
