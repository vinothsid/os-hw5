#define MAX_FILE_NAME_SIZE 100
#define MAX_PATH_NAME_SIZE 100
#define LOG_FS_H 1

typedef struct lfs_meta_data {
	char file_name[MAX_FILE_NAME_SIZE];
	int num_blocks;
	char path[MAX_PATH_NAME_SIZE];
} MDATA;

	

