#include "cpu/exec/template-start.h"

#define instr push

static void do_execute(){
    cpu.esp-=DATA_BYTE;
    swaddr_write(cpu.esp,DATA_BYTE,op_src->val);
    print_asm(str(instr) str(SUFFIX) " %s",op_src->str);
}

make_instr_helper(r);

#include "cpu/exec/template-end.h"