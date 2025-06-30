#include <linkedList.h>
#include <vfs.h>
#include <file_op.h>
#include <sys/syscall.h>

linkedListNode *list_dir(char *path){
    #if defined(__is_libk)
        return vfs_list_dir(path);
    #else
        return (linkedListNode *)syscall(sys_list_dir, path);
    #endif
}