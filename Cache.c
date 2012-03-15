#include "Cache.h"
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

CACHE  create_cache(int size,int block_size,char type) {
	
	page_size = sysconf(_SC_PAGESIZE);

	if(block_size%page_size != 0 ) {
		fprintf(stderr,"ERROR:block_size must be multiple of page_size: %d\n",page_size);
		exit(2);
	}

	if( size%block_size != 0 ) {
		fprintf(stderr,"ERROR:size must be multiple of block_size: %d\n",block_size);
		exit(3);
	}

	int numBlocks = size/block_size;

	CACHE newCache;

	newCache = malloc(sizeof(struct cache));
	assert(newCache!=NULL);

	newCache->type = type;
	newCache->cblocks = malloc(sizeof(struct cache_block) * numBlocks );
	assert(newCache->cblocks!=NULL);
	int i=0;
	for(i=0;i<numBlocks;i++) {
		init_cache_block((newCache->cblocks)+i,block_size,type);	
	}	

	
	return newCache;
	
}

static int init_cache_block(CBLK cache_blk,int block_size,char type) {
	
	switch(type) {
		case METADATA_CACHE:
			cache_blk->mdata = malloc(sizeof(MDATA));
			assert(cache_blk->mdata != NULL);
			break;
		case WRITE_BUFFER:
			cache_blk->mdata= NULL;
			int res = posix_memalign((void **)&(cache_blk->buf),page_size,block_size);
			assert(res == 0);
			break;
		default:
			fprintf(stderr,"ERROR:Invalide cache type .Should be METADATA_CACHE or WRITE_BUFFER\n");
			break;
	}

	cache_blk->free_flag = true;
	cache_blk->offset  = 0;
	cache_blk->lru_counter = 0;
		

}
int  write_cache_block(CACHE c,MDATA *meta_data,char *in_buf,int buf_len  ) {

}

static CBLK get_free_cache_block(CACHE c) {

}

int read_cache_block(CACHE c,MDATA *meta_data,char *out_buf,int buf_len) {

}

static int update_lru(CACHE c,CBLK cache_cblk ) {

}

int update_cache_block(CACHE c,MDATA *meta_data,char *add_buf,int buf_len) {

}

static int find_cache_block(CACHE c,MDATA *meta_data) {

}

static int evict_cache_block(CACHE c,CBLK cache_blk) {

}

int main() {
	CACHE buffer_cache = create_cache(4096*10,4096,WRITE_BUFFER);
	

}
