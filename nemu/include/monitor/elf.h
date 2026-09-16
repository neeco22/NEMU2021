#ifndef __MONITOR_ELF_H__
#define __MONITOR_ELF_H__

#include "common.h"

swaddr_t lookup_symbol(const char *name);
const char *lookup_addr(swaddr_t addr);

#endif