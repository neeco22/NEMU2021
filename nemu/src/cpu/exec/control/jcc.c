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


#define make_jcc_rel32(name, mnem, cond) \
	make_helper(concat(name, _l)) { \
		int32_t rel = (int32_t)instr_fetch(eip + 1, 4); \
		if(cond) { \
			cpu.eip += rel; \
		} \
		print_asm("%s 0x%x", mnem, eip + 5 + rel); \
		return 5; \
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

make_jcc_rel32(jo,  "jo",  cpu.eflags.OF)
make_jcc_rel32(jno, "jno", !cpu.eflags.OF)
make_jcc_rel32(jb,  "jb",  cpu.eflags.CF)
make_jcc_rel32(jae, "jae", !cpu.eflags.CF)
make_jcc_rel32(je,  "je",  cpu.eflags.ZF)
make_jcc_rel32(jne, "jne", !cpu.eflags.ZF)
make_jcc_rel32(jbe, "jbe", cpu.eflags.CF || cpu.eflags.ZF)
make_jcc_rel32(ja,  "ja",  !cpu.eflags.CF && !cpu.eflags.ZF)
make_jcc_rel32(js,  "js",  cpu.eflags.SF)
make_jcc_rel32(jns, "jns", !cpu.eflags.SF)
make_jcc_rel32(jp,  "jp",  cpu.eflags.PF)
make_jcc_rel32(jnp, "jnp", !cpu.eflags.PF)
make_jcc_rel32(jl,  "jl",  cpu.eflags.SF != cpu.eflags.OF)
make_jcc_rel32(jge, "jge", cpu.eflags.SF == cpu.eflags.OF)
make_jcc_rel32(jle, "jle", cpu.eflags.ZF || (cpu.eflags.SF != cpu.eflags.OF))
make_jcc_rel32(jg,  "jg",  !cpu.eflags.ZF && (cpu.eflags.SF == cpu.eflags.OF))