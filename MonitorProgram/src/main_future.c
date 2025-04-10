// Test main
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


extern uint32_t start_text;
extern uint32_t end_text;
//extern uint32_t start_main;
//extern uint32_t end_main;
extern uint32_t start_data;
extern uint32_t end_data;
extern uint32_t start_bss;
extern uint32_t end_bss;
extern uint32_t stack_base;

/*__attribute__((section(".main")))
int main();*/

int main() {
    uint32_t *src = (&end_text) + 1;
    uint32_t *dst = &start_data;

    while (dst < &end_data) {
        *dst++ = *src++;
    }

    dst = &start_bss;
    while (dst < &end_bss) {
        *dst++ = 0;
    }
    // Now stored variables work (eg. addr.h, strings, etc.)

    print_nl("Test 0\0");
    char input;
    // Intended length = 20
    char* buffer = "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0";
    uint8_t buff_tail = 0;
    while(1){
        print_nl("Test 1\0");
        print_str(main_menu);
        print_str(buffer);
        input = b_poll();
        buffer[buff_tail] = input; 
        buff_tail += 1;
        if (buff_tail >= 20) {
            buff_tail -= 1;
        }
    }

    return 0;
}

