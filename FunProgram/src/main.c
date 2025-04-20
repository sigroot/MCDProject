// Writen by sigroot

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
extern uint32_t start_rodata;
extern uint32_t end_rodata;
extern uint32_t start_data;
extern uint32_t end_data;
extern uint32_t start_bss;
extern uint32_t end_bss;
extern uint32_t stack_base;

uint8_t mod (uint8_t base, uint8_t divisor);

char* randomCharacter = "abcdefghijklmnop0123456789ABCDEF";

int main() {
    uint32_t matrix[80];
    for(int i = 0; i < 80; i++) matrix[i] = 0;
    while(1) {
    if (*poll == 0) {
        break;
    }
    for (uint8_t i = 0; i < 80; i++) {
        matrix[i] = randomCharacter[mod(matrix[mod(i-1, 80)]*3+i, 32)];
    }
    print_nl("");
    print_str((char*) matrix);    
    }
    print_nl("");
    return 0;
}

uint8_t mod (uint8_t base, uint8_t divisor) {
    while (base >= divisor) base -= divisor;
    return base;
}
