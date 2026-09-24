#ifndef __CACHE_H__
#define __CACHE_H__

#include "common.h"

#define CACHE_BLOCK_SIZE 64
#define CACHE_WAY 8
#define CACHE_SET_NUM 128

#define CACHE_BLOCK_BITS 6
#define CACHE_SET_BITS 7

typedef struct{
    uint8_t valid;
    uint32_t tag;
    uint8_t data[CACHE_BLOCK_SIZE];
} CacheLine;

typedef struct {
    CacheLine line[CACHE_WAY];
} CacheSet;

extern CacheSet cache[CACHE_SET_NUM];

void init_cache();
uint32_t cache_read(hwaddr_t addr,size_t len);
void cache_write(hwaddr_t addr,size_t len,uint32_t data);

#endif
