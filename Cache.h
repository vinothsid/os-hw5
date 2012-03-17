#ifndef LOG_FS_H
#include "log_fs.h"
#endif
#include <stdbool.h>
#define METADATA_CACHE 1
#define WRITE_BUFFER 2

/*
1) Cache for storing the write buffer
2) Cache for storing the in-memory meta data
*/

int page_size;
typedef struct cache_block {
	MDATA * mdata;
	bool free_flag;
	char *buf;
	int offset;
	int lru_counter;
} *CBLK ;

typedef struct cache {
	CBLK cblocks;
	int cache_size;
	int cache_block_size;
	char type;
} *CACHE;


/*
Allocate memory and return CACHE .
type : meta_data cache or write_buffer

write_buffer:
block_size must be multiple of PAGE_SIZE
size must be multiple of block_size

meta_data_cache:
block_size doesnt matter . But something should be passed
size - number of elements
*/
CACHE  create_cache(int size,int block_size,char type); 


int  write_cache_block(CACHE c,MDATA *meta_data,char *in_buf,int buf_len  );

CBLK get_free_cache_block(CACHE c);

int read_cache_block(CACHE c,MDATA *meta_data,char *out_buf,int buf_len);

static int update_lru(CACHE c,CBLK cache_cblk );

int update_cache_block(CACHE c,MDATA *meta_data,char *add_buf,int buf_len);

static int find_cache_block(CACHE c,MDATA *meta_data);

static int evict_cache_block(CACHE c,CBLK cache_blk);


static int init_cache_block(CBLK cache_blk,int block_size,char type);




