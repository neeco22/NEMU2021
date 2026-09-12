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

make_jcc(jo,   cpu.eflags.OF)
make_jcc(jno,  !cpu.eflags.OF)
make_jcc(jb,   cpu.eflags.CF)
make_jcc(jae,  !cpu.eflags.CF)
make_jcc(je,   cpu.eflags.ZF)                          
make_jcc(jne,  !cpu.eflags.ZF)
make_jcc(jbe,  cpu.eflags.CF || cpu.eflags.ZF)
make_jcc(ja,   !cpu.eflags.CF && !cpu.eflags.ZF)
make_jcc(js,   cpu.eflags.SF)
make_jcc(jns,  !cpu.eflags.SF)
make_jcc(jp,   cpu.eflags.PF)
make_jcc(jnp,  !cpu.eflags.PF)
make_jcc(jl,   cpu.eflags.SF != cpu.eflags.OF)
make_jcc(jge,  cpu.eflags.SF == cpu.eflags.OF)
make_jcc(jle,  cpu.eflags.ZF || (cpu.eflags.SF != cpu.eflags.OF))
make_jcc(jg,   !cpu.eflags.ZF && (cpu.eflags.SF == cpu.eflags.OF))