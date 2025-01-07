#ifndef VFS_H 

#define VFS_H

#define VFS_NODE_NAME_NUM_OF_CHARS 256// 255 chars not including null like in linux
// #define VFS_DEBUG
#define VFS_INFO
#define VFS_ERR

#ifdef VFS_DEBUG
    #define VFS_DEBUG_PRINT(fmt, ...) printf("%s VFS: " fmt, LOG_SYM_DEBUG, ##__VA_ARGS__)
#else
    #define VFS_DEBUG_PRINT(fmt, ...)
#endif

#ifdef VFS_INFO
    #define VFS_INFO_PRINT(fmt, ...) printf("%s VFS: " fmt, LOG_SYM_INF, ##__VA_ARGS__)
#else
    #define VFS_INFO_PRINT(fmt, ...)
#endif

#ifdef VFS_ERR
    #define VFS_ERR_PRINT(fmt, ...) printf("%s VFS: " fmt, LOG_SYM_ERR, ##__VA_ARGS__)
#else
    #define VFS_ERR_PRINT(fmt, ...)
#endif


#include <linkedList.h>

enum file_system_type{FILESYSTEM_TYPE_EXT2, FILESYSTEM_TYPE_UNKNOWN};
enum vfs_status{OK ,DIR_ALREADY_EXISTS, UNSUPPORTED, INVALID_PATH, NO_DIR, MEMORY_ERR};

typedef struct filesystem_data{
    uint64_t disk_number;
    uint64_t start_offset;
    enum file_system_type type;
} filesystem_data;

#include <ext2.h>


#define ROOT_DIR_NAME "/"
#define PATH_SEPERATOR '/'

enum vfs_node_type{UNWNOWN_TYPE, VFS_NODE_TYPE_REGULAR_FILE, VFS_NODE_TYPE_DIR, VFS_NODE_TYPE_CHAR_DEVICE, VFS_NODE_TYPE_BLOCK_DEVICE, VFS_NODE_TYPE_FIFO, VFS_NODE_TYPE_SOCKET, VFS_NODE_TYPE_SYMBOLINK, VFS_NODE_TYPE_FILE_SYSTEM};

typedef struct vfs_node{
    enum vfs_node_type type;
    char name[VFS_NODE_NAME_NUM_OF_CHARS];// Max name size 
    void* data;
} vfs_node;


typedef struct vfs_get_node_return_t{
    vfs_node* found_node;
    char* remaining_path;
    enum vfs_status status;
    char* path_searched;
} vfs_get_node_return_t;


extern vfs_node *vsf_root;

void create_dir(char* dir_path);
void init_vfs();
vfs_get_node_return_t vfs_get_node(char *path);
vfs_node* vfs_mkdir(char* path);
vfs_node *vfs_mknode(char *path);
vfs_node* mount_file_system(char* path, uint64_t disk_number, uint64_t disk_offset, enum file_system_type type);

#endif