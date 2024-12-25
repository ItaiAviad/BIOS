#ifndef VFS_H 

#define VFS_H

#define VFS_NODE_NAME_NUM_OF_CHARS 256// 255 chars not including null like in linux

#include <linkedList.h>

enum file_system_type{FILESYSTEM_TYPE_EXT2, FILESYSTEM_TYPE_UNKNOWN};

typedef struct filesystem_data{
    uint64_t disk_number;
    uint64_t start_offset;
    enum file_system_type type;
} filesystem_data;

#include <ext2.h>


#define ROOT_DIR_NAME "/"
#define PATH_SEPERATOR ROOT_DIR_NAME

enum vfs_node_type{UNWNOWN_TYPE, VFS_NODE_TYPE_REGULAR_FILE, VFS_NODE_TYPE_DIR, VFS_NODE_TYPE_CHAR_DEVICE, VFS_NODE_TYPE_BLOCK_DEVICE, VFS_NODE_TYPE_FIFO, VFS_NODE_TYPE_SOCKET, VFS_NODE_TYPE_SYMBOLINK, VFS_NODE_TYPE_FILE_SYSTEM};

typedef struct vfs_node{
    enum vfs_node_type type;
    char name[VFS_NODE_NAME_NUM_OF_CHARS];// Max name size 
    void* data;
} vfs_node;



extern vfs_node *vsf_root;

void create_dir(char* dir_path);
void init_vfs();
vfs_node* vfs_get_create_node_in_path(char* path);
vfs_node* mount_file_system(char* path, uint64_t disk_number, uint64_t disk_offset, enum file_system_type type);

#endif