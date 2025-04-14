// Test main
// Written by sigroot
#include <stdint.h>

#include "main.h"

#ifndef addr
#include "addr.h"
#define addr
#endif

extern uint32_t end_text;
extern uint32_t start_data;
extern uint32_t end_data;
extern uint32_t start_bss;
extern uint32_t end_bss;
extern uint32_t stack_base;

__attribute__((section(".main")))
int main();

int main() {
    uint32_t *src = &end_text;
    uint32_t *dst = &start_data;

    while (dst < &end_data) {
        *dst++ = *src++;
    }

    dst = &start_bss;
    while (dst < &end_bss) {
        *dst++ = 0;
    }
    // Now stored variables work (eg. addr.h, strings, etc.)

    print_str("Test: \0");

    // Test that determined the stack pointer was at address 0x00080008
    __asm("move.l %sp, end_bss");
    print_long((uint32_t) &end_bss); 

    print_str("\n\r\0");
    return 0;
}

