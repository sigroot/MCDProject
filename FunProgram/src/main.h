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

//          ***
//      Read functions
//          ***

// Block until character recieved from usb
char b_poll() {
    while ((*poll) != 0);
    return *utp;
}
