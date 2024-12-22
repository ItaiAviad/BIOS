#include <vfs.h>
#include <memory.h>
#include <string.h>

vfs_node *vfs_root = (vfs_node*)NULL;

void init_vfs(){
    vfs_root = (vfs_node*)malloc(sizeof(vfs_node));
    memcpy(vfs_root->name, ROOT_DIR_NAME, strlen(ROOT_DIR_NAME));
}

