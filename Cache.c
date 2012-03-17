#include "Cache.h"
#include <assert.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>

CACHE  create_cache(int size,int block_size,char type) {
	
	page_size = sysconf(_SC_PAGESIZE);

	int numBlocks;

	switch(type) {
		case METADATA_CACHE:
			numBlocks = size;
			break;
		case WRITE_BUFFER:
			if(block_size%page_size != 0 ) {
				fprintf(stderr,"ERROR:block_size must be multiple of page_size: %d\n",page_size);
				exit(2);
			}

			if( size%block_size != 0 ) {
				fprintf(stderr,"ERROR:size must be multiple of block_size: %d\n",block_size);
				exit(3);
			}

			numBlocks = size/block_size;


			break;
		default:

			break;
	
	}
	
	
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

CBLK get_free_cache_block(CACHE c) {

	return c->cblocks+0;
	
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

void print_cache_block(CBLK cblock) {
	
	if(cblock->mdata != NULL) {
		printf("File Name: %s\n",cblock->mdata->file_name);
		printf("Number of paths: %d\n",cblock->mdata->num_paths);
		int i=0;
		for(i=0;i < cblock->mdata->num_paths;i++) {
			printf("%s\n",cblock->mdata->path[i]);
		} 
	
	} else {
		fprintf(stderr,"ERROR:mdata is NULL\n");
	}

	printf("Free flag: %d\n",cblock->free_flag);
	printf("lru_counter:%d\n",cblock->lru_counter);
	if( cblock->buf!=NULL ) {
		printf("Offset : %d\n",cblock->offset);
		printf("Buffer content:%s\n",cblock->buf);
	}
}

/*
int main() {
	CACHE buffer_cache = create_cache(4096*10,4096,WRITE_BUFFER);
	
	CACHE meta_data_cache = create_cache(100,1,METADATA_CACHE);

	CBLK new_block = get_free_cache_block(meta_data_cache);

	strcpy(new_block->mdata->file_name,"file1.txt");

	new_block->mdata->num_paths = 2;

	strcpy(new_block->mdata->path[0],"/home/vino/lfs-store/16-03-2012/file1.txt-12:03-12:30");
	strcpy(new_block->mdata->path[1],"/home/vino/lfs-store/16-03-2012/file1.txt-12:31-12:45");

	print_cache_block(new_block);

	CBLK new_wb_block = get_free_cache_block(buffer_cache);
	new_wb_block->mdata = new_block->mdata;
	strcpy(new_wb_block->buf,"Content 1");
	new_wb_block->offset = 8;
	new_wb_block->free_flag = false;
	print_cache_block(new_wb_block);

} */


