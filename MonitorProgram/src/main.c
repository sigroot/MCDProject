// Board main
// Written by sigroot

#ifndef stdint
#include <stdint.h>
#define stdint
#endif

#ifndef main_h
#include "main.h"
#endif

#ifndef addr
#include "addr.h"
#endif

#ifndef monitor_h
#include "monitor.h"
#endif

extern uint32_t start_text;
extern uint32_t end_text;
extern uint32_t start_rodata;
extern uint32_t end_rodata;
extern uint32_t start_data;
extern uint32_t end_data;
extern uint32_t start_bss;
extern uint32_t end_bss;
extern uint32_t stack_base;

int main() {
    uint32_t *src = (&end_rodata);
    uint32_t *dst = &start_data;

    while (dst < &end_data) {
        *dst++ = *src++;
    }

    dst = &start_bss;
    while (dst < &end_bss) {
        *dst++ = 0;
    }
    // Now stored variables work (eg. addr.h, strings, etc.)

    interface();

    return 0;
}

