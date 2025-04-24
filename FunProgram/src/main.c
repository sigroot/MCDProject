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
uint8_t random();
void delay(uint32_t time);

char brightness[32] = " ..-=:;*^<>/\\()|[]{}94OX8#$0W%&@";
uint8_t* randomByte = (uint8_t*) 0x0009F0FF;
uint8_t row_sample = 10;
uint8_t bright_reduce = 20;

struct firework {
    uint8_t x;
    uint8_t y;
    uint8_t Vx;
    uint8_t Vy;
    uint8_t Ax;
    uint8_t Ay;
    uint8_t fuse;
};

int main() {
    uint8_t matrix[80][24];
    print_str("\x1b[38;5;202m");
    print_str("\x1b[48;5;234m");
    print_str("\x1b[?25l");

    for(int i = 0; i < 24; i++) {
        for(int j = 0; j < 80; j++) {
            matrix[j][i] = 0;
        }
    }
    uint8_t current_row = 0;
    while (1) {
        print_str("\x1b[38;5;202m");
        if (*poll == 0) {
            uint8_t input = *utp;
            if (input == 'q') {
                row_sample = 5;
                bright_reduce = 28;
            } else if (input == 'w') {
                row_sample = 10;
                bright_reduce = 20;
            } else if (input == 'e') {
                row_sample = 15;
                bright_reduce = 9;
            } else if (input == 'r') {
                row_sample = 20;
                bright_reduce = 3;
            } else {
                print_str("\x1b[0m");
                print_str("\x1b[38;5;82m");
                print_str("\x1b[?25h");
                break;
            }
        }
        for (uint8_t i = 0; i < 80; i++) {
            uint8_t value = random();

            matrix[i][current_row] = ((value&0xF) + (value >> 4));
            if (i < 40) {
                if (matrix[i][current_row] > (40 - i)) {
                    matrix[i][current_row] -= (40 - i);
                } else {
                    matrix[i][current_row] = 0;
                }
            } else {
                if (matrix[i][current_row] > (i - 40)) {
                    matrix[i][current_row] -= (i - 40);
                } else {
                    matrix[i][current_row] = 0;
                }
            }
            if (matrix[i][current_row] > 31) {
                matrix[i][current_row] = 31;
            }
        }
        print_nl("");
        for (int i = 0;i < 80; i++) {
            print_char(brightness[matrix[i][current_row]]);
        }

        print_str("\x1b[38;5;196m");
        uint8_t row = 1;
        for (uint8_t j = current_row+1; j != current_row; j++) {
            if (j >= 24) j = 0;
            if (j == current_row) break;
            // Send Cursor up
            print_str("\x1b[");
            print_char('A');
            for (uint8_t l = 0; l < row_sample - (row/2); l++) {
                uint8_t column = mod(random(), (6*row+row_sample)-l)+row;
                if (column > 79) column = 79;
                if (matrix[column][j] >= bright_reduce) {
                    matrix[column][j] -= bright_reduce;
                } else {
                    matrix[column][j] = 0;
                }
                print_str("\x1b[");
                print_char(0x30 + (column/10));
                print_char(0x30 + (column - (column/10)*10));
                print_char('G');
                print_char(brightness[matrix[column][j]]); 

                column = 79 - mod(random(), (6*row+row_sample)-l) - row;
                if (column > 79) column = 0;
                if (matrix[column][j] >= bright_reduce) {
                    matrix[column][j] -= bright_reduce;
                } else {
                    matrix[column][j] = 0;
                }
                print_str("\x1b[");
                print_char(0x30 + (column/10));
                print_char(0x30 + (column - (column/10)*10));
                print_char('G');
                print_char(brightness[matrix[column][j]]); 
            }
            row++;
        }
        print_str("\x1b[24B");
        if (current_row == 0) {
            current_row = 24;
        } else {
            current_row -= 1; 
        }
    }
    print_nl("");
    return 0;
}

void delay(uint32_t time) {
    for(uint32_t i = 0; i < time; i++) {
    }
}

uint8_t mod (uint8_t base, uint8_t divisor) {
    while (base >= divisor) base -= divisor;
    return base;
}

uint8_t random() {
    *randomByte = (uint8_t)(93*(*randomByte)+67);
    return *randomByte;
}

