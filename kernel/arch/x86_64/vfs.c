#include <memory.h>
#include <string.h>
#include <vfs.h>

vfs_node *vfs_root = (vfs_node *)NULL;

linkedListNode *vfs_list_dir(char *path) {
    linkedListNode *ret = NULL;

    // Find the parent folder
    vfs_get_node_return_t vfs_get_node_ret = vfs_get_node(path);

    vfs_node *found = vfs_get_node_ret.found_node;
    if (!found) {
        VFS_DEBUG_PRINT("%s couldn't be found!\n", path);
        goto list_dir_cleanup;
    }

    switch (found->type) {
        case VFS_NODE_TYPE_DIR: {

            linkedListNode *current_list_item = found->data;
            while (current_list_item) {
                vfs_node *child_node = (vfs_node *)current_list_item->data;
                size_t stringlen = strlen(child_node->name);
                char* str_cp = malloc(strlen(child_node->name)+1);
                memcpy(str_cp, child_node->name, strlen(child_node->name));
                str_cp[stringlen] = 0;
                append_node(&ret, str_cp);

                
                current_list_item = current_list_item->next;
            }
            break;
        }
        case VFS_NODE_TYPE_FILE_SYSTEM: {
            if(vfs_get_node_ret.status != CONTINUES_IN_FILE_SYSTEM && vfs_get_node_ret.status != OK){
                goto list_dir_cleanup;
            }
            filesystem_data *fs = ((filesystem_data *)found->data);
            char* remaining_path = vfs_get_node_ret.remaining_path;
            if(!remaining_path){
                remaining_path = "/";
            }
            switch (fs->type) {
                case FILESYSTEM_TYPE_EXT2: {
                    ret = ext2_list_dir(fs, remaining_path);
                    break;
                }

                default: {
                    break;
                }
            }

            break;
        }

        default: {
            goto list_dir_cleanup;
            break;
        }
    }

list_dir_cleanup:
    if (vfs_get_node_ret.path_searched) {
        free(vfs_get_node_ret.path_searched);
    }
    return ret;
}
int64_t vfs_write(char *path, size_t offset_bytes, size_t count_bytes, void* out_buffer){
    // TODO!!
    return 0;
}


int vfs_path_exists(char* path){
    size_t ret = 0;

    // Find the parent folder
    vfs_get_node_return_t vfs_get_node_ret = vfs_get_node(path);

    vfs_node *found = vfs_get_node_ret.found_node;
    if (!found) {
        VFS_DEBUG_PRINT("%s couldn't be found!\n", path);
        ret = 0;
        goto path_exists_cleanup;
    }

    switch (found->type) {
        case VFS_NODE_TYPE_DIR:{
            if(vfs_get_node_ret.status == OK && vfs_get_node_ret.remaining_path == NULL){
                ret = 1;
            }
            break;
        }
        case VFS_NODE_TYPE_FILE_SYSTEM: {
            filesystem_data *fs = ((filesystem_data *)found->data);
            char* remaining_path = vfs_get_node_ret.remaining_path;
            if(!remaining_path){
                remaining_path = "/";
            }
            switch (fs->type) {
                case FILESYSTEM_TYPE_EXT2: {
                    ext2_super_block s_block = ext2_read_super_block(fs);
                    uint64_t inode_num = ext2_get_inode_number_at_path(fs, &s_block, remaining_path);
                    if(inode_num == 0){
                        VFS_DEBUG_PRINT("%s couldn't be found!\n", path);
                        ret = 0;
                        goto path_exists_cleanup;
                    }
                    ext2_inode* inode = ext2_read_inode_metadata(fs, &s_block, inode_num);
                    if ((inode->i_mode & EXT2_S_IFMT) == EXT2_S_IFDIR) {
                        ret = 1;
                    } else if((inode->i_mode & EXT2_S_IFMT) == EXT2_S_IFREG){
                        ret = 2;
                    }
                    else{
                        ret = -1;
                    }
                }

                default: {
                    break;
                }
            }

            break;
        }

        default: {
            if(vfs_get_node_ret.status == OK){
                ret = 2;
            }
            break;
        }
    }

path_exists_cleanup:
    if (vfs_get_node_ret.path_searched) {
        free(vfs_get_node_ret.path_searched);
    }
    return ret;
}

int64_t vfs_read(char *path, size_t offset_bytes, size_t count_bytes, void* out_buffer) {
    size_t ret = 0;

    // Find the parent folder
    vfs_get_node_return_t vfs_get_node_ret = vfs_get_node(path);

    vfs_node *found = vfs_get_node_ret.found_node;
    if (!found) {
        VFS_DEBUG_PRINT("%s couldn't be found!\n", path);
        ret = -1;
        goto list_dir_cleanup;
    }

    switch (found->type) {
        case VFS_NODE_TYPE_FILE_SYSTEM: {
            if(vfs_get_node_ret.status != CONTINUES_IN_FILE_SYSTEM && vfs_get_node_ret.status != OK){
                goto list_dir_cleanup;
            }
            filesystem_data *fs = ((filesystem_data *)found->data);
            char* remaining_path = vfs_get_node_ret.remaining_path;
            if(!remaining_path){
                remaining_path = "/";
            }
            switch (fs->type) {
                case FILESYSTEM_TYPE_EXT2: {
                    ext2_super_block s_block = ext2_read_super_block(fs);
                    uint64_t inode_num = ext2_get_inode_number_at_path(fs, &s_block, remaining_path);
                    if(inode_num == 0){
                        VFS_DEBUG_PRINT("%s couldn't be found!\n", path);
                        ret = -1;
                        goto list_dir_cleanup;
                    }
                    ret = ext2_read_inode(fs, &s_block, inode_num, offset_bytes, count_bytes, out_buffer);
                    break;
                }

                default: {
                    break;
                }
            }

            break;
        }

        default: {
            goto list_dir_cleanup;
            break;
        }
    }

list_dir_cleanup:
    if (vfs_get_node_ret.path_searched) {
        free(vfs_get_node_ret.path_searched);
    }
    return ret;
}

int64_t vfs_get_file_size(char *path) {
    size_t ret = 0;

    // Find the parent folder
    vfs_get_node_return_t vfs_get_node_ret = vfs_get_node(path);

    vfs_node *found = vfs_get_node_ret.found_node;
    if (!found) {
        VFS_DEBUG_PRINT("%s couldn't be found!\n", path);
        ret = -1;
        goto list_dir_cleanup;
    }

    switch (found->type) {
        case VFS_NODE_TYPE_FILE_SYSTEM: {
            if(vfs_get_node_ret.status != CONTINUES_IN_FILE_SYSTEM && vfs_get_node_ret.status != OK){
                goto list_dir_cleanup;
            }
            filesystem_data *fs = ((filesystem_data *)found->data);
            char* remaining_path = vfs_get_node_ret.remaining_path;
            if(!remaining_path){
                remaining_path = "/";
            }
            switch (fs->type) {
                case FILESYSTEM_TYPE_EXT2: {
                    ext2_super_block s_block = ext2_read_super_block(fs);
                    uint64_t inode_num = ext2_get_inode_number_at_path(fs, &s_block, remaining_path);
                    if(inode_num == 0){
                        VFS_DEBUG_PRINT("%s couldn't be found!\n", path);
                        ret = -1;
                        goto list_dir_cleanup;
                    }
                    ret = ext2_get_inode_size(fs, &s_block, inode_num);
                    break;
                }

                default: {
                    break;
                }
            }

            break;
        }

        default: {
            goto list_dir_cleanup;
            break;
        }
    }

list_dir_cleanup:
    if (vfs_get_node_ret.path_searched) {
        free(vfs_get_node_ret.path_searched);
    }
    return ret;
}

char *preprocess_path(const char *original_string) {
    if (!original_string || *original_string == '\0') {
        return NULL; // Invalid input
    }
    size_t len = strlen(original_string);
    char *ret_str = malloc(len + 1);
    if (!ret_str) {
        return NULL; // Allocation failed
    }
    char *current = ret_str;
    int last_was_slash = 0;

    while (*original_string && !isspace(*original_string)) {
        if (*original_string == '/') {
            if (!last_was_slash) { // Only add if last wasn't a slash
                *current++ = '/';
            }
            last_was_slash = 1;
        } else {
            *current++ = *original_string;
            last_was_slash = 0;
        }
        original_string++;
    }
    *current = '\0'; // Null-terminate

    // Remove trailing separators (but keep root `/` intact)
    while (current > ret_str + 1 && *(current - 1) == '/') {
        *(--current) = '\0';
    }
    return ret_str;
}


vfs_node *find_child_node(vfs_node *parent, const char *segment, size_t segment_length) {
    if (!parent || parent->type != VFS_NODE_TYPE_DIR) {
        return NULL;
    }
    linkedListNode *current_list_item = parent->data;
    while (current_list_item) {
        vfs_node *child_node = (vfs_node *)current_list_item->data;
        if (memcmp(child_node->name, segment, segment_length) == 0 && child_node->name[segment_length] == '\0') {
            return child_node;
        }
        current_list_item = current_list_item->next;
    }
    return NULL;
}

void init_vfs() {
    vfs_root = (vfs_node *)malloc(sizeof(vfs_node));
    memset(vfs_root, 0, sizeof(vfs_node));
    vfs_root->type = VFS_NODE_TYPE_DIR;
    vfs_root->data = NULL;
    memcpy(vfs_root->name, ROOT_DIR_NAME, strlen(ROOT_DIR_NAME));
}

vfs_node *vfs_mkdir(char *path) {

    VFS_DEBUG_PRINT("Creating directory for path: %s\n", path);

    // Find the parent folder
    vfs_get_node_return_t vfs_get_node_ret = vfs_get_node(path);

    // If no remaining path, the directory already exists
    if (vfs_get_node_ret.remaining_path == NULL) {
        VFS_ERR_PRINT("The directory already exists: %s\n", path);
        goto cleanup_mkdir;
    }

    vfs_node *parent_folder = vfs_get_node_ret.found_node;
    if (!parent_folder) {
        VFS_DEBUG_PRINT("Parent folder not found for path: %s\n", path);
        goto cleanup_mkdir;
    }

    if (parent_folder->type != VFS_NODE_TYPE_DIR) {
        VFS_DEBUG_PRINT("Parent is not a directory: %s\n", parent_folder->name);
        goto cleanup_mkdir;
    }

    switch (parent_folder->type) {
        case VFS_NODE_TYPE_DIR: {
            // Extract the directory name
            char *dir_name = vfs_get_node_ret.remaining_path + 1; // Added one to skip seperator

            if (strchr(dir_name, PATH_SEPERATOR) != NULL) {
                VFS_DEBUG_PRINT("The dir before doesn't exist!\n");
                goto cleanup_mkdir;
            }

            VFS_DEBUG_PRINT("Parent folder name: %s, creating child directory: %s\n", parent_folder->name, dir_name);

            // Create the new directory node
            vfs_node *new_node = malloc(sizeof(vfs_node));
            if (!new_node) {
                VFS_ERR_PRINT("Memory allocation failed!\n");
                goto cleanup_mkdir;
            }

            // Initialize the new node
            memset(new_node, 0, sizeof(vfs_node));
            memcpy(new_node->name, dir_name, strlen(dir_name) + 1);
            new_node->type = VFS_NODE_TYPE_DIR;

            VFS_DEBUG_PRINT("Appending new node with name: %s to parent folder: %s\n", new_node->name, parent_folder->name);

            append_node((linkedListNode **)&(parent_folder->data), new_node);

            if (vfs_get_node_ret.path_searched) {
                free(vfs_get_node_ret.path_searched);
            }

            return new_node;
        }
        case VFS_NODE_TYPE_FILE_SYSTEM: {
            // TODO:
            break;
        }

        default: {
            goto cleanup_mkdir;
        }
    }

cleanup_mkdir:
    if (vfs_get_node_ret.path_searched) {
        free(vfs_get_node_ret.path_searched);
    }
    return NULL;
}

// Doesn't free path_searched, should be freed afterwards
vfs_get_node_return_t vfs_get_node(char *path) {
    vfs_get_node_return_t ret;
    ret.found_node = NULL;
    ret.status = OK;
    ret.remaining_path = NULL;

    char *path_preproccesed = preprocess_path(path);
    if (!path_preproccesed) {
        VFS_ERR_PRINT("Memory allocation failed!");
        ret.status = MEMORY_ERR;
        return ret;
    }

    ret.path_searched = path_preproccesed;
    ret.remaining_path = path_preproccesed;

    if (*path_preproccesed == '\0') {
        ret.status = INVALID_PATH;
        VFS_ERR_PRINT("Invalid Path!\n");
        free(path_preproccesed);  // Free before returning
        return ret;
    }

    VFS_DEBUG_PRINT("Searching for node with path: %s\n", path_preproccesed);

    vfs_node *current_node = vfs_root;
    char *current_path_position = path_preproccesed;
    char *current_seperator_position = path_preproccesed;

    // Skip the root `/` explicitly
    if (*current_path_position == PATH_SEPERATOR) {
        current_path_position++;
    }

    while (*current_path_position) {
        char *next_separator_position = strchr(current_path_position, PATH_SEPERATOR);
        if (!next_separator_position) {
            next_separator_position = strchr(current_path_position, '\0');
        }

        // Ensure the current node is a directory
        switch (current_node->type) {
            case VFS_NODE_TYPE_DIR:
                break;

            case VFS_NODE_TYPE_FILE_SYSTEM:
                ret.found_node = current_node;
                ret.remaining_path = current_seperator_position;
                ret.status = CONTINUES_IN_FILE_SYSTEM;
                VFS_DEBUG_PRINT("Returning last found node: %s\n", current_node->name);
                return ret;  // Caller must free `ret.path_searched`
            
            default:
                VFS_DEBUG_PRINT("Node type at %s is not a directory!\n", current_node->name);
                free(path_preproccesed); // Free memory before returning
                ret.status = NO_DIR;
                return ret;
        }

        // Search for the child node directly
        vfs_node *child_node = find_child_node(current_node, current_path_position, next_separator_position - current_path_position);

        if (!child_node) {
            ret.found_node = current_node;
            ret.remaining_path = current_seperator_position;
            VFS_DEBUG_PRINT("Returning last found node: %s\n", current_node->name);
            return ret;  // Caller must free `ret.path_searched`
        }

        current_node = child_node;
        if (!next_separator_position) {
            break;
        }
        current_seperator_position = next_separator_position;
        current_path_position = current_seperator_position + 1; // Skip separator
    }

    ret.found_node = current_node;
    ret.remaining_path = NULL;
    return ret;  // Caller must free `ret.path_searched`
}


vfs_node *mount_file_system(char *path, uint64_t disk_number, uint64_t disk_offset, enum file_system_type type) {
    vfs_get_node_return_t vfs_get_node_ret = vfs_get_node(path);
    if (!vfs_get_node_ret.found_node || vfs_get_node_ret.remaining_path != NULL) {
        VFS_ERR_PRINT("Couldn't mount filesystem, The path doesn't exist!\n");
        goto cleanup;
    }
    if (vfs_get_node_ret.found_node->type != VFS_NODE_TYPE_DIR) {
        VFS_ERR_PRINT("Couldn't mount filesystem, Can't mount in a node which isn't a dir\n");
        goto cleanup;
    }

    if (vfs_get_node_ret.found_node->data != NULL) {
        VFS_ERR_PRINT("Couldn't mount filesystem, Can't mount a non empty dir\n");
        goto cleanup;
    }

    filesystem_data *data = malloc(sizeof(filesystem_data));
    data->disk_number = disk_number;
    data->start_offset = disk_offset;
    data->type = type;
    vfs_get_node_ret.found_node->type = VFS_NODE_TYPE_FILE_SYSTEM;
    vfs_get_node_ret.found_node->data = data;

    if (vfs_get_node_ret.path_searched) {
        free(vfs_get_node_ret.path_searched);
    }

    return vfs_get_node_ret.found_node;
cleanup:
    if (vfs_get_node_ret.path_searched) {
        free(vfs_get_node_ret.path_searched);
    }
    return NULL;
}
