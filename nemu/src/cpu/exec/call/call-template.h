#include "cpu/exec/template-start.h"

#define instr call

static void do_execute(){
    uint32_t ret_addr=cpu.eip + 1 + DATA_BYTE;
    cpu.esp-=4;
    swaddr_write(cpu.esp,4,ret_addr);
    cpu.eip+=op_src->val;
    print_asm(str(instr) " %x",ret_addr);
}

make_instr_helper(si);

#include "cpu/exec/template-end.h"