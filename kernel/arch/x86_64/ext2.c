#include <ext2.h>
#include <disk.h>
 

ext2_super_block ext2_read_super_block(filesystem_data* fs_data){
    ext2_super_block super_block;
    read(fs_data->disk_number, fs_data->start_offset, sizeof(ext2_super_block), &super_block);
    return super_block;
}

