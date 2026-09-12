#include "cpu/exec/helper.h"

#define make_setcc(name, cond) \
	make_helper(name) { \
		int len = decode_rm_b(eip + 1); \
		write_operand_b(op_src, (cond) ? 1 : 0); \
		print_asm("%s %s", #name, op_src->str); \
		return len + 1; \
	}

make_setcc(seto,  cpu.eflags.OF)
make_setcc(setno, !cpu.eflags.OF)
make_setcc(setb,  cpu.eflags.CF)
make_setcc(setae, !cpu.eflags.CF)
make_setcc(sete,  cpu.eflags.ZF)
make_setcc(setne, !cpu.eflags.ZF)          
make_setcc(setbe, cpu.eflags.CF || cpu.eflags.ZF)
make_setcc(seta,  !cpu.eflags.CF && !cpu.eflags.ZF)
make_setcc(sets,  cpu.eflags.SF)
make_setcc(setns, !cpu.eflags.SF)
make_setcc(setp,  cpu.eflags.PF)
make_setcc(setnp, !cpu.eflags.PF)
make_setcc(setl,  cpu.eflags.SF != cpu.eflags.OF)
make_setcc(setge, cpu.eflags.SF == cpu.eflags.OF)
make_setcc(setle, cpu.eflags.ZF || (cpu.eflags.SF != cpu.eflags.OF))
make_setcc(setg,  !cpu.eflags.ZF && (cpu.eflags.SF == cpu.eflags.OF))