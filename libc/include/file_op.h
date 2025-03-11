#ifndef FILE_OP_H

#define FILE_OP_H

#define MAX_NUM_OF_FILE_DESCRIPTORS 256

#include <types.h>
#include <vfs.h>
#include <memory.h>
#include <kernel.h>

// #define O_ACCMODE	00000003
#define O_RDONLY	00000000
#define O_WRONLY	00000001
#define O_RDWR		00000002
// #ifndef O_CREAT
// #define O_CREAT		00000100	/* not fcntl */
// #endif
// #ifndef O_EXCL
// #define O_EXCL		00000200	/* not fcntl */
// #endif
// #ifndef O_NOCTTY
// #define O_NOCTTY	00000400	/* not fcntl */
// #endif
// #ifndef O_TRUNC
// #define O_TRUNC		00001000	/* not fcntl */
// #endif
// #ifndef O_APPEND
// #define O_APPEND	00002000
// #endif
// #ifndef O_NONBLOCK
// #define O_NONBLOCK	00004000
// #endif
// #ifndef O_DSYNC
// #define O_DSYNC		00010000	/* used to be O_SYNC, see below */
// #endif
// #ifndef FASYNC
// #define FASYNC		00020000	/* fcntl, for BSD compatibility */
// #endif
// #ifndef O_DIRECT
// #define O_DIRECT	00040000	/* direct disk access hint */
// #endif
// #ifndef O_LARGEFILE
// #define O_LARGEFILE	00100000
// #endif
// #ifndef O_DIRECTORY
// #define O_DIRECTORY	00200000	/* must be a directory */
// #endif
// #ifndef O_NOFOLLOW
// #define O_NOFOLLOW	00400000	/* don't follow links */
// #endif
// #ifndef O_NOATIME
// #define O_NOATIME	01000000
// #endif
// #ifndef O_CLOEXEC
// #define O_CLOEXEC	02000000	/* set close_on_exec */
// #endif

typedef struct fdp
{
    char* file_path;
    size_t current_offset;
    int flags;
} fdp;


extern fdp fdp_arr[256];

void init_fdp_arr();

int open(const char* path, int flags);

int close(int fd);

int64_t read(int fd, void* buf, size_t cnt);

int64_t write(int fd, void* buf, size_t cnt);

enum LSEEK_OP{
    SEEK_SET,
    SEEK_CUR,
    SEEK_END
};

int lseek(int fd, size_t offset, enum LSEEK_OP whence);

int64_t ftell(int fd);

linkedListNode *list_dir(char *path);


#endif