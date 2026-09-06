#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;
	char *expression;
	uint32_t val;
	uint32_t address;
	/* TODO: Add more members if necessary */

} WP;

WP* new_wp();
void free_wp(WP* wp);
int wp_is_changed();
void display_watchpoint();
WP* find_wp(int NO);

#endif
