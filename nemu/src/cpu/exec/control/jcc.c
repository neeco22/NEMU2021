#include "cpu/exec/helper.h"

#define make_jcc(name,cond)\
    make_helper(name){\
        int8_t rel = instr_fetch(eip + 1, 1);\
        if(cond){\
            cpu.eip += rel;\
        }\
        print_asm("%s 0x%x", #name, cpu.eip + rel + 2);\
        return 2;\
    }

make_jcc(je, cpu.eflags.ZF == 1)