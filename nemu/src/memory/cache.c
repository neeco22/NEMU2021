#include "common.h"
#include <stdlib.h>
#include "memory/memory.h"
#include "memory/cache.h"

L1Set l1_cache[L1_SET_NUM];
L2Set l2_cache[L2_SET_NUM];

void init_cache() {
	int i, j;
	for(i = 0; i < L1_SET_NUM; i ++)
		for(j = 0; j < L1_WAY; j ++)
			l1_cache[i].line[j].valid = 0;
	for(i = 0; i < L2_SET_NUM; i ++)
		for(j = 0; j < L2_WAY; j ++)
			l2_cache[i].line[j].valid = 0;
}



static uint32_t l2_tag(hwaddr_t a)    { return a >> (L2_BLOCK_BITS + L2_SET_BITS); }
static uint32_t l2_set(hwaddr_t a)    { return (a >> L2_BLOCK_BITS) & (L2_SET_NUM - 1); }
static uint32_t l2_offset(hwaddr_t a) { return a & (L2_BLOCK_SIZE - 1); }

static int l2_find(uint32_t set, uint32_t tag) {
	int i;
	for(i = 0; i < L2_WAY; i ++)
		if(l2_cache[set].line[i].valid && l2_cache[set].line[i].tag == tag)
			return i;
	return -1;
}


static void l2_writeback(uint32_t set, int way) {
	L2Line *v = &l2_cache[set].line[way];
	int i;
	if(!v->dirty) return;
	hwaddr_t base = (v->tag << (L2_BLOCK_BITS + L2_SET_BITS)) | (set << L2_BLOCK_BITS);
	for(i = 0; i < L2_BLOCK_SIZE; i ++)
		dram_write(base + i, 1, v->data[i]);
	v->dirty = 0;
}

static int l2_load(uint32_t set, uint32_t tag, hwaddr_t addr) {
	int way = -1, i;
	for(i = 0; i < L2_WAY; i ++)
		if(!l2_cache[set].line[i].valid) { way = i; break; }
	if(way == -1) {
		way = rand() % L2_WAY;         
		l2_writeback(set, way);        
	}
	L2Line *l = &l2_cache[set].line[way];
	hwaddr_t base = addr & ~(hwaddr_t)(L2_BLOCK_SIZE - 1);
	l->tag = tag;
	l->valid = 1;
	l->dirty = 0;
	for(i = 0; i < L2_BLOCK_SIZE; i ++)
		l->data[i] = dram_read(base + i, 1);   
	return way;
}

uint32_t l2_read(hwaddr_t addr, size_t len) {
	uint32_t off = l2_offset(addr);
	if(off + len > L2_BLOCK_SIZE) {
		size_t first = L2_BLOCK_SIZE - off;
		uint32_t lo = l2_read(addr, first);
		uint32_t hi = l2_read(addr + first, len - first);
		return lo | (hi << (first * 8));
	}
	uint32_t set = l2_set(addr), tag = l2_tag(addr);
	int way = l2_find(set, tag);
	if(way == -1) way = l2_load(set, tag, addr);
	uint32_t r = 0; size_t i;
	for(i = 0; i < len; i ++)
		r |= (uint32_t)l2_cache[set].line[way].data[off + i] << (i * 8);
	return r;
}

void l2_write(hwaddr_t addr, size_t len, uint32_t data) {
	uint32_t off = l2_offset(addr);
	if(off + len > L2_BLOCK_SIZE) {
		size_t first = L2_BLOCK_SIZE - off;
		l2_write(addr, first, data & (~0u >> ((4 - first) << 3)));
		l2_write(addr + first, len - first, data >> (first * 8));
		return;
	}
	uint32_t set = l2_set(addr), tag = l2_tag(addr);
	int way = l2_find(set, tag);
	if(way == -1) way = l2_load(set, tag, addr);   
	size_t i;
	for(i = 0; i < len; i ++)
		l2_cache[set].line[way].data[off + i] = (data >> (i * 8)) & 0xff;
	l2_cache[set].line[way].dirty = 1;             
}



static uint32_t l1_tag(hwaddr_t a)    { return a >> (L1_BLOCK_BITS + L1_SET_BITS); }
static uint32_t l1_set(hwaddr_t a)    { return (a >> L1_BLOCK_BITS) & (L1_SET_NUM - 1); }
static uint32_t l1_offset(hwaddr_t a) { return a & (L1_BLOCK_SIZE - 1); }

static int l1_find(uint32_t set, uint32_t tag) {
	int i;
	for(i = 0; i < L1_WAY; i ++)
		if(l1_cache[set].line[i].valid && l1_cache[set].line[i].tag == tag)
			return i;
	return -1;
}

static int l1_load(uint32_t set, uint32_t tag, hwaddr_t addr) {
	int way = -1, i;
	for(i = 0; i < L1_WAY; i ++)
		if(!l1_cache[set].line[i].valid) { way = i; break; }
	if(way == -1) way = rand() % L1_WAY;   
	L1Line *l = &l1_cache[set].line[way];
	hwaddr_t base = addr & ~(hwaddr_t)(L1_BLOCK_SIZE - 1);
	l->tag = tag;
	l->valid = 1;
	for(i = 0; i < L1_BLOCK_SIZE; i ++)
		l->data[i] = l2_read(base + i, 1);   
	return way;
}

uint32_t l1_read(hwaddr_t addr, size_t len) {
	uint32_t off = l1_offset(addr);
	if(off + len > L1_BLOCK_SIZE) {
		size_t first = L1_BLOCK_SIZE - off;
		uint32_t lo = l1_read(addr, first);
		uint32_t hi = l1_read(addr + first, len - first);
		return lo | (hi << (first * 8));
	}
	uint32_t set = l1_set(addr), tag = l1_tag(addr);
	int way = l1_find(set, tag);
	if(way == -1) way = l1_load(set, tag, addr);
	uint32_t r = 0; size_t i;
	for(i = 0; i < len; i ++)
		r |= (uint32_t)l1_cache[set].line[way].data[off + i] << (i * 8);
	return r;
}

void l1_write(hwaddr_t addr, size_t len, uint32_t data) {
	uint32_t off = l1_offset(addr);
	if(off + len > L1_BLOCK_SIZE) {
		size_t first = L1_BLOCK_SIZE - off;
		l1_write(addr, first, data & (~0u >> ((4 - first) << 3)));
		l1_write(addr + first, len - first, data >> (first * 8));
		return;
	}
	uint32_t set = l1_set(addr), tag = l1_tag(addr);
	int way = l1_find(set, tag);
	if(way != -1) {
		size_t i;
		for(i = 0; i < len; i ++)
			l1_cache[set].line[way].data[off + i] = (data >> (i * 8)) & 0xff;
	}
	
	l2_write(addr, len, data);        
}