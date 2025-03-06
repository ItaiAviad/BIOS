#include <file_op.h>
#include <sys/syscall.h>

fdp fdp_arr[256];

void init_fdp_arr(){
    memset(fdp_arr, 0, sizeof(fdp) * MAX_NUM_OF_FILE_DESCRIPTORS);
}

int open(const char* path, int flags){
    #if defined(__is_libk)
        for(int i = 0; i < MAX_NUM_OF_FILE_DESCRIPTORS; i++){
            if(fdp_arr[i].file_path == NULL){
                size_t path_size = strlen(path);
                char* path_copy = malloc(strlen(path));
                memcpy(path_copy, path, path_size);
                fdp_arr[i].file_path = path_copy;
                fdp_arr[i].current_offset = 0;
                fdp_arr[i].flags = flags;
                return i;
            }
        }
        return -1; // Too many file descriptors were used
    #else
        return syscall(sys_open, path, flags);
    #endif


}

int close(int fd){
    #if defined(__is_libk)
        if(fdp_arr[fd].file_path == NULL){
            return -1;
        }

        free(fdp_arr[fd].file_path);
        fdp_arr[fd].file_path = NULL;
    #else

        return syscall(sys_close, fd);
    #endif
}

int read(int fd, void* buf, size_t cnt){
    #if defined(__is_libk)
        if(fdp_arr[fd].file_path == NULL){
            return -1;
        }

        size_t cnt_read = vfs_read(fdp_arr[fd].file_path, fdp_arr[fd].current_offset, cnt, buf);
        fdp_arr[fd].current_offset += cnt_read-1;
        return cnt_read;
    #else
        return syscall(sys_read, fd, buf, cnt);
    #endif
}

int write(int fd, void* buf, size_t cnt){
    #if defined(__is_libk)
        if(fdp_arr[fd].file_path == NULL){
            return -1;
        }
        size_t cnt_wrote = vfs_write(fdp_arr[fd].file_path, fdp_arr[fd].current_offset, cnt, buf);
        fdp_arr[fd].current_offset += cnt_wrote-1;
        return cnt_wrote;
    #else
        return syscall(sys_write, fd, buf, cnt);
    #endif
}

int lseek(int fd, size_t offset, enum LSEEK_OP whence){
    #if defined(__is_libk)
        if(fdp_arr[fd].file_path == NULL){
            return -1;
        }
        size_t file_size = vfs_get_file_size(fdp_arr[fd].file_path);
        switch (whence)
        {
        case SEEK_SET:{
            if(offset < file_size){
                fdp_arr[fd].current_offset = offset;
            } else{
                return -3;
            }
            break;
        }

        case SEEK_CUR:{
            if(offset+fdp_arr[fd].current_offset < file_size){
                fdp_arr[fd].current_offset += offset;
            } else{
                return -3;
            }
            break;
        }

        case SEEK_END:{
            if(((int64_t)file_size)-offset-1 >= 0){
                fdp_arr[fd].current_offset = file_size-offset-1;
            } else{
                return -3;
            }
            break;
        }
        
        default:
            break;
        }
    #else
        return syscall(sys_lseek, fd, offset, whence);
    #endif
}