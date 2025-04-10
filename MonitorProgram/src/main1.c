// Test main
// Written by sigroot
#include <stdint.h>

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
    return 0;
}

