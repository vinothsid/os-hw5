#ifndef LOG_FS_H
#include "log_fs.h"
#endif

int cache_size;
int cache_block_size;

typedef struct cache_block {
	struct lfs_meta_data * mdata;
	int free_flag;
	char *buf;
	int offset;
	int lru_counter;
} *CBLK ;

struct cache {
	struct cache_block *cblocks;
};


void init_cache(int size,int block_size);

CBLK  alloc_cache_block(struct lfs_meta_data  );

CBLK get_free_block();




