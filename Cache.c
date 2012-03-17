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
	newCache->num_blocks = numBlocks;
	newCache->cache_block_size = block_size;

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
	CBLK cblk = find_cache_block(c,meta_data);
	if(cblk->offset+buf_len >= c->cache_block_size) {
		return -1; // write the cache block to disk
	} 

	memcpy(cblk->buf + cblk->offset , in_buf,buf_len);
	cblk->offset += buf_len;
	update_lru(c,cblk);
	return 0; 
}

CBLK get_free_cache_block(CACHE c , int *result) {

	int max=-1;
	int index=-1;
	int i=0;
	for(i=0;i<c->num_blocks;i++) {
		if( max < c->cblocks[i].lru_counter) {
			max = c->cblocks[i].lru_counter;
			index = i;
		}
	}

/*
	When no free blocks are present
*/
	if(c->cblocks[index].free_flag == false ) {
		*result = WRITE_BACK;
	} else {
		*result = NORMAL;
	}

	c->cblocks[index].free_flag = false; // allocated
#ifdef DEBUG_CACHE
	printf("Returning block : %d\n",index);
#endif
	return c->cblocks+index; // if result == WRITE_BACK , write to disk then use the cache_block
	
}


int read_cache_block(CACHE c,MDATA *meta_data,char *out_buf,int *buf_len ) {
	CBLK cblk = find_cache_block(c,meta_data);
	
	if(cblk == NULL) {
		return -1; // cache block not present in memory
	}

	strcpy(out_buf, cblk->buf);
#ifdef DEBUG_CACHE
	printf("read_cache_block() cache read:%s\n",cblk->buf);
#endif
	*buf_len = cblk->offset;
	update_lru(c,cblk);
	return 0;
	

}

CBLK find_meta_data_block(CACHE c,char *file_name) {
	if(c->type == METADATA_CACHE) {
		int i=0;
		for(i=0;i<c->num_blocks;i++) {
			if(c->cblocks[i].free_flag == false && strcmp(c->cblocks[i].mdata->file_name,file_name) ==0 ) {
				return c->cblocks+i;
			}
		}

	} else {
		fprintf(stderr,"ERROR:Wrong type of cache\n");
		return NULL;
	}

	return NULL; // if NULL allocate new block

}

static int update_lru(CACHE c,CBLK cache_blk ) {
	int i=0;

	for(i=0;i<c->num_blocks;i++) {
		if( c->cblocks+i != cache_blk && c->cblocks[i].lru_counter <= cache_blk->lru_counter) {
//			printf("<<<< %d : %d >>>>\n",c->cblocks[i].lru_counter , cache_blk->lru_counter);	
			c->cblocks[i].lru_counter = (c->cblocks[i].lru_counter+1)%c->num_blocks;
		}
	}

	cache_blk->lru_counter = 0; 
}

/*
int update_cache_block(CACHE c,MDATA *meta_data,char *add_buf,int buf_len) {

}
*/
static CBLK find_cache_block(CACHE c,MDATA *meta_data) {
	int i=0;
	for(i=0;i<c->num_blocks;i++) {
		if(c->cblocks[i].mdata == meta_data) {
			return c->cblocks+i;
		}
	}

	return NULL;

}

/*
static int evict_cache_block(CACHE c,CBLK cache_blk) {

}
*/
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

void print_cache(CACHE c) {
	int i=0;
	for(i=0;i<c->num_blocks;i++) {
		if(c->cblocks[i].free_flag == false) {
			printf("Cache Block:%d\n",i);
			print_cache_block(c->cblocks+i);
			printf("===============\n");
		}
	}

}


int main() {

	int result;
	CACHE buffer_cache = create_cache(4096*10,4096,WRITE_BUFFER);
	
	CACHE meta_data_cache = create_cache(100,1,METADATA_CACHE);

	CBLK new_block = get_free_cache_block(meta_data_cache,&result);

	strcpy(new_block->mdata->file_name,"file1.txt");

	new_block->mdata->num_paths = 2;

	strcpy(new_block->mdata->path[0],"/home/vino/lfs-store/16-03-2012/file1.txt-12:03-12:30");
	strcpy(new_block->mdata->path[1],"/home/vino/lfs-store/16-03-2012/file1.txt-12:31-12:45");

	update_lru(meta_data_cache,new_block);
	CBLK new_block2 = get_free_cache_block(meta_data_cache,&result);

	strcpy(new_block2->mdata->file_name,"file2.txt");

	new_block2->mdata->num_paths = 2;

	strcpy(new_block2->mdata->path[0],"/home/vino/lfs-store/16-03-2012/file2.txt-12:03-12:30");
	strcpy(new_block2->mdata->path[1],"/home/vino/lfs-store/16-03-2012/file2.txt-12:31-12:45");


//	print_cache_block(new_block);

	CBLK new_wb_block = get_free_cache_block(buffer_cache,&result);
	new_wb_block->mdata = new_block->mdata;

	char string[20] = "Content 1";
	write_cache_block(buffer_cache,new_block->mdata,string,strlen(string));
	//strcpy(new_wb_block->buf,"Content 1");
	//new_wb_block->offset = 8;
	//new_wb_block->free_flag = false;

//	update_lru(buffer_cache,new_wb_block);

//	print_cache_block(new_wb_block);
	CBLK new_wb_block2 = get_free_cache_block(buffer_cache,&result);
	new_wb_block2->mdata = new_block2->mdata;
	strcpy(string,"Content 2");
	write_cache_block(buffer_cache,new_block2->mdata,string,strlen(string));
	write_cache_block(buffer_cache,new_block2->mdata,string,strlen(string));
	

	strcpy(string,"Content X");
	write_cache_block(buffer_cache,new_block->mdata,string,strlen(string));
//	strcpy(new_wb_block2->buf,"Content 2");
//	new_wb_block2->offset = 8;
//	new_wb_block2->free_flag = false;


//	update_lru(buffer_cache,new_wb_block2);
//	update_lru(buffer_cache,new_wb_block);
//	print_cache(buffer_cache);
	


	printf("----------------\n");
  //      update_lru(buffer_cache,new_wb_block);
        print_cache(buffer_cache);
	printf("------Meta data cache--------\n");
        print_cache(meta_data_cache);

	char out[2000];
	int len;
	printf("Result : %d\n",read_cache_block(buffer_cache,new_block->mdata,out,&len));

	printf("Read content : %s\n",out);

	if (find_meta_data_block(meta_data_cache,"file2.txt") == NULL ) {
		printf("Find returned NULL\n");
	} else { 
		print_cache_block(find_meta_data_block(meta_data_cache,"file2.txt"));
	}

	char *buf = "FileName:file1\nNumPaths:2\nPaths:path1;path2;";
	char fileName[100];
	int num_path;
	char paths[200*1000];
	sscanf(buf,"FileName:%s\nNumPaths:%d\nPaths:%s\n",fileName,&num_path,paths);

	printf("File name : %s num_path : %d paths: %s\n",fileName,num_path,paths);

	char all_paths[1000][200];
	int i=0;
	int j=0;
	int offset=0;
	for(i=0;i<num_path;i++) {
		offset=0;
		while(paths[j]!= ';') {
			all_paths[i][offset] = paths[j];
			offset++;
			j++;
		}
		j++;
		all_paths[i][offset]='\0';
	}	

	for(i=0;i<num_path;i++) {
		printf("Path %d : %s\n",i,all_paths[i]);
	}
} 


