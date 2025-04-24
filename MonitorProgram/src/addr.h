// Written by sigroot

#ifndef stdint
#include <stdint.h>
#define stdint
#endif

#define addr

/*
 * Memory Map:
 *      ROM:    0x00000000 - 0x0007ffff
 *              Init SP:    0x00000000 - 0x00000003 
 *              Init PC:    0x00000004 - 0x00000007
 *              .text:      0x00002000 - 0x00002ea0
 *              .rodata:    0x00002ea4 - 0x000037b8
 *      RAM:    0x00080000 - 0x0009ffff
 *              .data:      0x00080000 - 0x00080078
 *              .bss:       0x0008007c - 0x00080080
 *      USB:    0x00100001 (or any odd number until poll)
 *      POLL:   0x00180001 (or any higher odd number with the '8' bit set)
 */
uint8_t* utp = (uint8_t*) 0x00100001;
uint8_t* poll = (uint8_t*) 0x00180001;
uint32_t storage = 0x00000000;
