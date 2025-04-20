ENTRY(main)
OUTPUT_ARCH(m68k)
OUTPUT_FORMAT(srec)

MEMORY {
    RAM (rwx)   : o = 0x00080010, l = 131056
}

SECTIONS
{
    start_text = 0x00080100;
    .text start_text :
    {
        . = ALIGN(4);
        *(.text); 
        . = ALIGN(4);
        end_text = .;
    } > RAM

    .rodata : 
    {
       . = ALIGN(4);
       start_rodata = .;
       *(.rodata);
       . = ALIGN(4);
       end_rodata = .;
    } > RAM

    .data : 
    {
        . = ALIGN(4);
        start_data = .;
        *(.data);
        end_data = .;
    } > RAM

    .bss : 
    {
        . = ALIGN(4);
        start_bss = .;
        *(.bss)
        end_bss = .;
    } > RAM
}
