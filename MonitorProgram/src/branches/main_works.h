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
    print_str("\n\r\0");
    return 0;
}

// Print a newline
int p_nl() {
    print_str("\n\r\0");
    return 0;
}

// Print a byte in hex format
int print_hex(uint8_t phex) {
    uint8_t first = (phex & 0xF0) >> 4;
    uint8_t second = (phex & 0x0F);

    if (first > 9) {
        first -= 9;
        first |= 0x40;
    } else first |= 0x30;

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
    while ((*poll & 0x01) != 0);
    return *utp;
}

//          ***
//      Constants
//          ***

// Menu output for user input (Terminal 80w x 24h)
char* main_menu = "\
                                                                                \r\n\
                                                                                \r\n\
                                                                                \r\n\
                                                                                \r\n\
                          Welcome to the...                                     \r\n\
        ,--------.                       ,--.                ,--.               \r\n\
        '--.  .--',---. ,--.--.,--,--,--.`--',--,--,  ,--,--.|  |               \r\n\
           |  |  | .-. :|  .--'|        |,--.|      \\' ,-.  ||  |               \r\n\
           |  |  \\   --.|  |   |  |  |  ||  ||  ||  |\\ '-'  ||  |               \r\n\
           `--'   `----'`--'   `--`--`--'`--'`--''--' `--`--'`--'               \r\n\
                                                                                \r\n\
                                                                                \r\n\
                             Available Commmands:                               \r\n\
                                                                                \r\n\
         * ReadM [address] [count]  - Read [count] (def: 1) bytes from [address]\r\n\
         * WritM [address] [value]  - Write [value] to the byte at [address]    \r\n\
         * ReadR [register]         - Read the value in [register]              \r\n\
         * WritR [register] [value] - Write [value] to [register]               \r\n\
         * LSRec                    - Load input S-Record until stop code       \r\n\
         * Run   [address]          - Begin running the code at [address]       \r\n\
                                                                                \r\n\
                                                                                \r\n\
                                                                                \r\n\
 :> \0";
