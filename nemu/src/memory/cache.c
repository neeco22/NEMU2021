#include "common.h"
#include <stdlib.h>
#include "memory/memory.h"
#include "memory/cache.h"

CacheSet cache[CACHE_SET_NUM];

void init_cache(){
    int i,j;
    for(i=0;i<CACHE_SET_NUM;i++){
        for(j=0;j<CACHE_WAY;j++){
            cache[i].line[j].valid=0;
        }
    }
}

static uint32_t get_tag(hwaddr_t addr){
    return addr >> (CACHE_SET_BITS + CACHE_BLOCK_BITS);
}
static uint32_t get_set(hwaddr_t addr){
    return (addr >> CACHE_BLOCK_BITS) & (CACHE_SET_NUM - 1);
}
static uint32_t get_offset(hwaddr_t addr){
    return addr & (CACHE_BLOCK_SIZE - 1);
}

static int find_line(uint32_t set,uint32_t tag){
    int i;
    for(i=0;i<CACHE_WAY;i++)
        if(cache[set].line[i].valid && cache[set].line[i].tag == tag)
            return i;
    return -1;
}

static int load_block(uint32_t set,uint32_t tag,hwaddr_t addr){
    int way =-1,i;
    for(i=0;i<CACHE_WAY;i++)
        if(!cache[set].line[i].valid) {way=i;break;}
    if(way==-1) way=rand() % CACHE_WAY;

    CacheLine *l=&cache[set].line[way];
    hwaddr_t block_base=addr & ~(hwaddr_t)(CACHE_BLOCK_SIZE - 1);

    l->tag=tag;
    l->valid=1;

    for(i=0;i<CACHE_BLOCK_SIZE;i++){
        l->data[i]=dram_read(block_base + i,1);
    }
    return way;
}

uint32_t cache_read(hwaddr_t addr,size_t len){
    uint32_t offset=get_offset(addr);
    if(offset + len > CACHE_BLOCK_SIZE ){
        size_t first=CACHE_BLOCK_SIZE - offset;
        uint32_t lo=cache_read(addr,first);
        uint32_t hi=cache_read(addr+first,len-first);
        return lo | (hi << (first*8));
    }

    uint32_t set=get_set(addr);
    uint32_t tag=get_tag(addr);
    int way=find_line(set,tag);
    if(way==-1) way=load_block(set,tag,addr);

    uint32_t result=0;
    size_t i;
    for(i=0;i<len;i++){
        result |= (uint32_t)cache[set].line[way].data[offset+i] << (i*8);
    }
    return result;
}

void cache_write(hwaddr_t addr,size_t len,uint32_t data){
    uint32_t offset=get_offset(addr);

    if(offset + len > CACHE_BLOCK_SIZE){
        size_t first=CACHE_BLOCK_SIZE - offset;
        cache_write(addr,first,data & (-0u >> ((4-first) << 3)));
        cache_write(addr+first,len-first,data >> (first * 8));
        return;
    }

    uint32_t set=get_set(addr);
    uint32_t tag=get_tag(addr);
    int way=find_line(set,tag);

    if(way!=-1){
        size_t i;
        for(i=0;i<len;i++){
            cache[set].line[way].data[offset+i]=(data >> (i*8)) & 0xff;
        }
    }

    dram_write(addr,len,data);
}