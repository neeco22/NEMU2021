#include "cpu/exec/helper.h"

#define DATA_BYTE 4
#include "call-template.h"
#undef DATA_BYTE

make_helper(call_rm_l){
    int len=decode_rm_l(eip + 1);
    cpu.esp -= 4;
    swaddr_write(cpu.esp,4,eip + 1 + len);
    cpu.eip = op_src->val;
    print_asm("call *%s",op_src->str);
    return 0;
}