#include "cpu/exec/helper.h"

#define make_cmovcc(name, cond) \
	make_helper(name) { \
		int len = decode_rm2r_l(eip + 1); \
		if(cond) { \
			write_operand_l(op_dest, op_src->val); \
		} \
		print_asm("%s %s,%s", #name, op_src->str, op_dest->str); \
		return len + 1; \
	}

make_cmovcc(cmovo,  cpu.eflags.OF)
make_cmovcc(cmovno, !cpu.eflags.OF)
make_cmovcc(cmovb,  cpu.eflags.CF)
make_cmovcc(cmovae, !cpu.eflags.CF)
make_cmovcc(cmove,  cpu.eflags.ZF)
make_cmovcc(cmovne, !cpu.eflags.ZF)
make_cmovcc(cmovbe, cpu.eflags.CF || cpu.eflags.ZF)
make_cmovcc(cmova,  !cpu.eflags.CF && !cpu.eflags.ZF)
make_cmovcc(cmovs,  cpu.eflags.SF)                          
make_cmovcc(cmovns, !cpu.eflags.SF)
make_cmovcc(cmovp,  cpu.eflags.PF)
make_cmovcc(cmovnp, !cpu.eflags.PF)
make_cmovcc(cmovl,  cpu.eflags.SF != cpu.eflags.OF)
make_cmovcc(cmovge, cpu.eflags.SF == cpu.eflags.OF)
make_cmovcc(cmovle, cpu.eflags.ZF || (cpu.eflags.SF != cpu.eflags.OF))
make_cmovcc(cmovg,  !cpu.eflags.ZF && (cpu.eflags.SF == cpu.eflags.OF))