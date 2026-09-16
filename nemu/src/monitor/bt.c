#include "nemu.h"
#include "monitor/elf.h"
#include <stdio.h>

typedef struct {
    swaddr_t prev_ebp;
    swaddr_t ret_addr;
    uint32_t args[4];
} StackFrame;

static StackFrame get_frame(swaddr_t ebp) {
    StackFrame sf;
    sf.prev_ebp = swaddr_read(ebp, 4);
    sf.ret_addr = swaddr_read(ebp + 4, 4);
    int i;
    for(i = 0; i < 4; i ++)
        sf.args[i] = swaddr_read(ebp + 8 + 4 * i, 4);
    return sf;
}

void print_stackframe() {
    swaddr_t ebp = cpu.ebp;
    swaddr_t pc = cpu.eip;      
    int frame = 0;

    while(ebp != 0 && frame < 32) {    
        StackFrame sf = get_frame(ebp);

        printf("#%-2d 0x%08x in %s(", frame, pc, lookup_addr(pc));
        int i;
        for(i = 0; i < 4; i ++)
            printf("0x%08x%s", sf.args[i], i == 3 ? "" : ", ");
        printf(")\n");

        pc = sf.ret_addr;       
        ebp = sf.prev_ebp;     
        frame ++;
    }
}