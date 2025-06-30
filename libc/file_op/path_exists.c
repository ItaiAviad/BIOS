#include "file_op.h"

#include <vfs.h>
#include <file_op.h>
#include <sys/syscall.h>

int path_exists(char *path){
    #if defined(__is_libk)
        return vfs_path_exists(path);
    #else
        return syscall(sys_path_exists, path);
    #endif
}