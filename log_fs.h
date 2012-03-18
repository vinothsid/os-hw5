#define MAX_FILE_NAME_SIZE 100
#define MAX_PATH_NAME_SIZE 200
#define MAX_PATHS 1000
#define MAX_MDATA_FILE_SIZE 200*1000+300
#define MAX_LINE_SIZE 200
#define LOG_FS_H 1

typedef struct lfs_meta_data {
	char file_name[MAX_FILE_NAME_SIZE];
	int num_paths;
	char path[MAX_PATHS][MAX_PATH_NAME_SIZE];
} MDATA;

	

