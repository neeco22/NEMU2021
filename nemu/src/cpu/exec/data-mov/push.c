#include "cpu/exec/helper.h"

#define DATA_BYTE 4
#include "push-template.h"
#undef DATA_BYTE

make_helper(push_i_b) {
    int8_t imm = instr_fetch(eip + 1, 1);
    cpu.esp -= 4;
    swaddr_write(cpu.esp, 4, (uint32_t)imm);
    print_asm("push $0x%x", imm);
    return 2;
}

make_helper(push_i_l){
    int32_t imm = instr_fetch(eip + 1, 4);
    cpu.esp -= 4;
    swaddr_write(cpu.esp, 4, imm);
    print_asm("push $0x%x", imm);
    return 5;
}