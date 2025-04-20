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
 *              Text:       0x00002000 
 *      RAM:    0x00080000 - 0x0009ffff
 *              
 */
uint8_t* utp = (uint8_t*) 0x00100001;
uint8_t* poll = (uint8_t*) 0x00180001;
uint32_t storage = 0x00000000;
uint32_t random;
