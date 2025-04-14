// Test main
// Written by sigroot
#include <stdint.h>

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
    *((uint8_t*) 0x00100001) = 't';

    uint32_t *src = &end_text;
    uint32_t *dst = &start_data;

    while (dst < &end_data) {
        *dst++ = *src++;
    }

    *((uint8_t*) 0x00100001) = 'e';

    dst = &start_bss;
    while (dst < &end_bss) {
        *dst++ = 0;
    }

    *((uint8_t*) 0x00100001) = 's';
    *utp = 'r';
    return 0;
}

