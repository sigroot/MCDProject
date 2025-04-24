// Written by sigroot

#ifndef stdint
#include <stdint.h>
#define stdint
#endif

#ifndef addr
#include "addr.h"
#endif

#define main_h

//          ***
//      Print functions
//          ***

// Print a character to USB
int print_char(char pchar) {
    *utp = pchar;
    return 0;
}

// Print a null-terminated string to USB
int print_str(char* pstr) {
    for(int i = 0; pstr[i] != 0; i++) {
        print_char(pstr[i]);
    } 
    return 0;
}

// Print a string followed by a newline
int print_nl(char* pstr) {
    print_str(pstr);
    print_str("\n\r");
    return 0;
}

// Print a byte in hex format
int print_hex(uint8_t phex) {
    uint8_t first = (phex & 0xF0) >> 4;
    uint8_t second = (phex & 0x0F);

    // Convert high nibble to hex
    if (first > 9) {
        first -= 9;
        first |= 0x40;
    } else first |= 0x30;

    // Convert low nibble to hex
    if (second > 9) {
        second -= 9;
        second |= 0x40;
    } else second |= 0x30;

    print_char(first);
    print_char(second);
    return 0;
}

// Print a long word in hex format
int print_long (uint32_t plong) {
    print_hex((uint8_t)(plong >> 24));
    print_hex((uint8_t)(plong >> 16));
    print_hex((uint8_t)(plong >> 8));
    print_hex((uint8_t)(plong));
    return 0;
}

//          ***
//      Read functions
//          ***

// Block until character recieved from usb
char b_poll() {
    while ((*poll) != 0);
    return *utp;
}
