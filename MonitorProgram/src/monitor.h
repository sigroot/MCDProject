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

#define monitor_h

int parse_buf(char* buffer, uint8_t buff_tail);
int buff_clear(char* buffer, uint8_t* buff_tail);
int run_comm(uint8_t index, char* buffer, uint8_t buff_tail);

int read_memory(char* buffer, uint8_t buff_tail);
int write_memory(char* buffer, uint8_t buff_tail);
int read_register(char* buffer, uint8_t buff_tail);
int write_register(char* buffer, uint8_t buff_tail);
int parse_srecord();
int run_srecord(char* buffer, uint8_t buff_tail);

int interface() {
    char input;
    uint8_t buffer_len = 50;
    // Initialize buffer
    char buffer[buffer_len];
    for(int i = 0; i < buffer_len; i++) {
        buffer[i] = '\0';
    }
    // Next blank byte
    uint8_t buff_tail = 0;
    print_str(main_menu);
    print_str(buffer);
    while(1){
        // Print menu and saved buffer

        // Get user input
        input = b_poll();

        // Permit only line feed, new line, and backspace special chars
        if (input < 32 && input != '\r' && input != '\n' && input != 0x7F) continue;
        switch (input) {
            case '\n': // Push command
                p_nl();
                parse_buf(buffer, buff_tail);
                buff_clear(buffer, &buff_tail);
                // Reprint Screen
                print_str(main_menu);
                print_str(buffer);
                break;
            case '\r': // Push command
                p_nl();
                parse_buf(buffer, buff_tail);
                buff_clear(buffer, &buff_tail);
                // Reprint Screen
                print_str(main_menu);
                print_str(buffer);
                break;
            case 0x7F: // Delete last character
                if (buff_tail <= 0) break;
                buff_tail -= 1;
                buffer[buff_tail] = '\0';
                // Remove last character
                print_str("\b \b");
                break;
            default: // Add character
                if (buff_tail > buffer_len) break;
                buffer[buff_tail] = input; 
                // Print new character
                print_char(buffer[buff_tail]);
                buff_tail += 1;
                break;
        }
    }
    
    return 0;
}

int buff_clear(char* buffer, uint8_t* buff_tail){
    for (int i = 0; i < *buff_tail; i++) {
        buffer[i] = '\0';
    } 
    *buff_tail = 0;
    return 0;
}

int parse_buf(char* buffer, uint8_t buffer_tail) {
    // Save Registers
    __asm("movel %SP, storage");
    __asm("moveml %a0-%a7/%d0-%d7, -(%SP)");

    // Parse Buffer
    uint8_t comm_len = 5;
    uint8_t total_comm = 6;
    char* commands[6] = {"ReadM", "WritM", "ReadR", "WritR", "LSRec", "RunSR"};

    if (buffer_tail < comm_len) {
        print_nl("Command too short\n\rPress any key...");
        b_poll();
        // undo saving
        __asm("moveml (%SP)+, %a0-%a7/%d0-%d7");
        return 1;
    }

    for (int i = 0; i < total_comm; i++) {
        for (int j = 0; j < comm_len; j++) {
            if (buffer[j] != commands[i][j]) {
                break;
            }
            if (j == comm_len - 1) {
                run_comm(i, buffer, buffer_tail);
                print_nl("Press any key...");
                b_poll();
                // undo saving
                __asm("moveml (%SP)+, %a0-%a7/%d0-%d7");
                return 0;
            }
        }
    }
    print_nl("Command not recognized");
    print_nl("Press any key...");
    b_poll();
    // undo saving
    __asm("moveml (%SP)+, %a0-%a7/%d0-%d7");
    return 1;
}

int run_comm(uint8_t index, char* buffer, uint8_t buff_tail) {
    switch (index) {
        // Read memory
        case 0:
            read_memory(buffer, buff_tail);
            break;
        // Write memory
        case 1:
            write_memory(buffer, buff_tail);        
            break;
        // Read register
        case 2:
            read_register(buffer, buff_tail);
            break;
        // Write register
        case 3:
            write_register(buffer, buff_tail);
            break;
        // Load S-Record
        case 4:
            parse_srecord();            
            break;
        // Run S-Record
        case 5:
            run_srecord(buffer, buff_tail);
            break;
    }
    return 0;
}


// Just learned: addr is not allowed as a parameter name... probably because that is the name of a header file
int read_memory(char* buffer, uint8_t buff_tail){
    char parameter_buff[9];
    for(int i = 0; (buffer+6)[i] != 0x20 && i < 8; i++) {
        parameter_buff[i] = (buffer+6)[i];
    }
    parameter_buff[8] = 0;
    uint32_t* address = (uint32_t*) shtod(parameter_buff);
    uint32_t count = 1;
    if (buff_tail > 15) {
        count = shtod(buffer+15);
    }
    for(int i = 0; i < count; i++) {
        print_str("Address ");
        print_long((uint32_t) address + i*4);
        print_str(" : ");
        print_long(*(address+i));
        p_nl();
    }
    return 0;
}

int write_memory(char* buffer, uint8_t buff_tail){
    char parameter_buff[9];
    for(int i = 0; (buffer+6)[i] != 0x20 && i < 8; i++) {
        parameter_buff[i] = (buffer+6)[i];
    }
    parameter_buff[8] = 0;
    uint32_t* address = (uint32_t*) shtod(parameter_buff);
    if ((uint32_t) address < 0x00080000) {
        print_str("Cannot Write to 0x");
        print_long((uint32_t) address);
        print_nl("!");
        return 1;
    }
    uint32_t value = shtod(buffer+15);
    *address = value;
    print_str("Address 0x");
    print_long((uint32_t) address);
    print_str(" : ");
    print_long(*address);
    p_nl();
    return 0;
}

int read_register(char* buffer, uint8_t buff_tail) {
    static char* names[16] = {"D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7"}; 

    if (buff_tail < 8) {
        for(uint32_t i = 0; i < 16; i++) {
            print_str(names[i]);
            print_str(": 0x");
            print_long(((uint32_t*) (storage-64))[i]);
            p_nl();
        }
        return 0;
    }

    uint8_t index = 0;
    if (buffer[6] == 'A' || buffer[6] == 'a') {
        index += 8;
    } else {
        if (buffer[6] != 'D' && buffer[6] != 'd') {
            print_nl("Register letter not recognized.");
            return 1;
        }
    }

    if (buffer[7] >= '0' && buffer[7] <= '7') {
        index += buffer[7] & 0xF;
    } else {
        print_nl("Register number not recognized.");
        return 1;
    }
    print_str(names[index]);
    print_str(": 0x");
    print_long(((uint32_t*) (storage-64))[index]);
    p_nl();
    return 0;
}

int write_register(char* buffer, uint8_t buff_tail) {
    static char names[16][3] = {"D0", "D1", "D2", "D3", "D4", "D5", "D6", "D7", "A0", "A1", "A2", "A3", "A4", "A5", "A6", "A7"}; 

    uint8_t index = 0;
    if (buffer[6] == 'A' || buffer[6] == 'a') {
        index += 8;
    } else {
        if (buffer[6] != 'D' && buffer[6] != 'd') {
            print_nl("Register letter not recognized.");
            return 1;
        }
    }

    if (buffer[7] >= '0' && buffer[7] <= '7') {
        index += buffer[7] & 0xF;
    } else {
        print_nl("Register number not recognized.");
        return 1;
    }
    print_nl(buffer+8);
    ((uint32_t*) (storage-64))[index] = shtod(buffer+8);

    print_str(names[index]);
    print_str(": 0x");
    print_long(((uint32_t*) (storage-64))[index]);
    p_nl();
    return 0;
}

int parse_srecord() {
    print_nl("Please enter 1 S-Record per line");
    uint32_t total_lines = 0;
    uint8_t type = 0;
    uint8_t count = 0;
    uint8_t* address = 0;
    uint8_t data = 0;
    uint8_t checksum = 0;
    char buffer[9] = {0,0,0,0,0,0,0,0,0};
    while (1) {
        // Start SRec
        while(b_poll() != 'S');
        print_char('S'); // Echo
        checksum = 0;
        total_lines += 1;
        // Get type
        type = b_poll(); 
        if(type < '0' || type > '9') {
            print_nl("Type error!");
            return 1;
        }
        print_char(type); // Echo
        type &= 0xF;
        // Ignore S0
        if (type == 0) {
            while (b_poll() > 0x1F);
            p_nl();
            continue;
        }
        // Get count
        buffer[0] = b_poll();
        buffer[1] = b_poll();
        buffer[2] = 0;
        count = (uint8_t) shtod(buffer);
        if (count < 3) {
            print_nl("Count error!");
            return 1;
        }
        print_str(buffer); // Echo
        checksum += count;
        // Get address
        if (type < 4){
            for (int i = 0; i < (type + 1) * 2; i++) {
                buffer[i] = b_poll();
            }
            count -= type + 1;
        } else if (type > 6) {
            for (int i = 0; i < (11 - type) * 2; i++) {
                buffer[i] = b_poll();
            }
            count -= 11 - type;
        }
        buffer[8] = 0;
        print_str(buffer); // Echo
        address = (uint8_t*) shtod(buffer);
        if ((uint32_t) address < 0x00080010 || (uint32_t) address > 0x000A0000) {
            print_nl("Address Error");
            return 1;
        }
        checksum += (uint32_t) address;
        checksum += ((uint32_t) address) >> 8;
        checksum += ((uint32_t) address) >> 16;
        checksum += ((uint32_t) address) >> 24;
        // Write data
        buffer[2] = 0;
        for ( ; count > 1; count--) {
            buffer[0] = b_poll();
            buffer[1] = b_poll();
            print_str(buffer); // Echo
            data = (uint8_t) shtod(buffer); 
            checksum += data;
            *address = data;
            address += 1;
        }
        // Check Checksum
        buffer[0] = b_poll();
        buffer[1] = b_poll();
        print_nl(buffer); // Echo
        if (((~checksum) & 0xFF) != ((uint8_t) shtod(buffer))) {
            print_nl("Checksum Error!");
            print_hex(~checksum);
            print_str(" ");
            print_hex((uint8_t) shtod (buffer));
            return 1;
        }
        // End loading
        if (type > 6) {
            print_nl("Recieved S-Record");
            print_str("Recieved 0x");
            print_long(total_lines);
            print_nl(" lines.");
            print_str("Start at address: 0x");
            print_long((uint32_t)address);
            print_nl(".");
            break;
        }
    } 
    return 0; 
}

int run_srecord(char* buffer, uint8_t buff_tail) {
    uint8_t* address = (uint8_t*) shtod(buffer + 6);
    ((void (*)(void))address)();
    return 0;
}
