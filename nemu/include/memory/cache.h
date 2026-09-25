#ifndef __CACHE_H__
#define __CACHE_H__

#include "common.h"

#define L1_BLOCK_SIZE  64
#define L1_WAY         8
#define L1_SET_NUM     128
#define L1_BLOCK_BITS  6
#define L1_SET_BITS    7

typedef struct {
	uint8_t  valid;
	uint32_t tag;
	uint8_t  data[L1_BLOCK_SIZE];
} L1Line;
typedef struct { L1Line line[L1_WAY]; } L1Set;

#define L2_BLOCK_SIZE  64
#define L2_WAY         16
#define L2_SET_NUM     4096
#define L2_BLOCK_BITS  6
#define L2_SET_BITS    12

typedef struct {
	uint8_t  valid;
	uint8_t  dirty;                 
	uint32_t tag;
	uint8_t  data[L2_BLOCK_SIZE];
} L2Line;
typedef struct { L2Line line[L2_WAY]; } L2Set;

extern L1Set l1_cache[L1_SET_NUM];
extern L2Set l2_cache[L2_SET_NUM];

void init_cache();                        
uint32_t l1_read(hwaddr_t, size_t);
void     l1_write(hwaddr_t, size_t, uint32_t);
uint32_t l2_read(hwaddr_t, size_t);
void     l2_write(hwaddr_t, size_t, uint32_t);

#endif