#ifndef VFS_H 

#define VFS_H

#define VFS_NODE_NAME_NUM_OF_CHARS 256// 255 chars not including null like in linux

#include <linkedList.h>

#define ROOT_DIR_NAME "\\"

enum vfs_node_type{UNWNOWN_TYPE, VFS_NODE_TYPE_REGULAR_FILE, VFS_NODE_TYPE_DIR, VFS_NODE_TYPE_CHAR_DEVICE, VFS_NODE_TYPE_BLOCK_DEVICE, VFS_NODE_TYPE_FIFO, VFS_NODE_TYPE_SOCKET, VFS_NODE_TYPE_SYMBOLINK, VFS_NODE_TYPE_FILE_SYSTEM};

typedef struct vfs_node{
    enum vfs_node_type type;
    char name[VFS_NODE_NAME_NUM_OF_CHARS];// Max name size 
    void* data;
} vfs_node;

extern vfs_node *vsf_root;

void create_dir(char* dir_path);
void init_vfs();

#endif