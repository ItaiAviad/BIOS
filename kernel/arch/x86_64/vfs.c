#include <memory.h>
#include <string.h>
#include <vfs.h>

vfs_node *vfs_root = (vfs_node *)NULL;

static inline char *preprocess_path(const char *original_string) {
    if (!original_string || *original_string == '\0') {
        return NULL; // Invalid input
    }
    size_t len = strlen(original_string);
    char *ret_str = malloc(len + 1);
    if (!ret_str) {
        return NULL; // Allocation failed
    }
    char *current = ret_str;
    while (*original_string && !isspace(*original_string)) {
        *current++ = *original_string++;
    }
    *current = '\0'; // Null-terminate

    // Remove trailing separators
    while (current > ret_str && *(current - 1) == '/') {
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

vfs_node *vfs_mknode(char *path) { // Doesn't support file system only top level os vfs
    char *path_preproccesed = preprocess_path(path);
    char *stripped_path = malloc(strlen(path_preproccesed)+1);
    memcpy(stripped_path, path_preproccesed, strlen(path_preproccesed)+1);
    char *stripped_path_end = stripped_path;
    while (stripped_path_end) {
        char* current_end = strchr(stripped_path_end, PATH_SEPERATOR);
        if(!current_end){
            *stripped_path_end = '\0';
            break;
        }
        stripped_path_end = current_end;
    }
    char* dir_name = stripped_path_end+1;

    vfs_node* parent_folder = vfs_get_node(stripped_path, false);
    if(!parent_folder){
        return NULL;
    }

    vfs_node* new_node = malloc(sizeof(vfs_node));
    memcpy(new_node->name, dir_name, strlen(dir_name));
    new_node->type = VFS_NODE_TYPE_DIR;
    new_node->data = NULL;



    append_node((linkedListNode **)&(parent_folder->data), new_node);
    return new_node;
}


vfs_node *vfs_mkdir(char *path) {
    char *path_preproccesed = preprocess_path(path);
    if (!path_preproccesed) {
        printf("VFS: Invalid path!\n");
        return NULL;
    }

    printf("VFS: Creating directory for path: %s\n", path_preproccesed);

    // Find the parent folder
    vfs_node *parent_folder = vfs_get_node(path_preproccesed, true);
    if (!parent_folder) {
        printf("VFS: Parent folder not found for path: %s\n", path_preproccesed);
        free(path_preproccesed);
        return NULL;
    }


    printf("VFS: Creating dir in path: %s", path_preproccesed);

    // Extract the directory name
    char *dir_name = strrchr(path_preproccesed, PATH_SEPERATOR)+1;
    // if(!dir_name){
    //     dir_name = path_preproccesed;
    // }
    // if(dir_name && *dir_name == '/' ){
    //     dir_name += 1;
    // }

    printf("VFS: Parent folder name: %s, creating child directory: %s\n", parent_folder->name, dir_name);

    // Check if the directory already exists
    if (find_child_node(parent_folder, dir_name, strlen(dir_name))) {
        printf("VFS: Directory already exists: %s\n", path_preproccesed);
        free(path_preproccesed);
        return NULL;
    }

    // Create the new directory node
    vfs_node *new_node = malloc(sizeof(vfs_node));
    if (!new_node) {
        printf("VFS: Memory allocation failed!\n");
        free(path_preproccesed);
        return NULL;
    }

    memcpy(new_node->name, dir_name, strlen(dir_name) + 1);
    new_node->type = VFS_NODE_TYPE_DIR;
    new_node->data = NULL;

    printf("VFS: Appending new node with name: %s to parent folder: %s\n", new_node->name, parent_folder->name);

    append_node((linkedListNode **)&(parent_folder->data), new_node);

    free(path_preproccesed);
    return new_node;
}


vfs_node *vfs_get_node(char *path, int return_last_node) {
    char *path_preproccesed = preprocess_path(path);
    if (!path_preproccesed || *path_preproccesed == '\0') {
        printf("VFS: Invalid Path!\n");
        free(path_preproccesed);
        return NULL;
    }

    printf("VFS: Searching for node with path: %s\n", path_preproccesed);

    vfs_node *current_node = vfs_root;
    char *current_path_position = path_preproccesed;

    // Skip the root `/` explicitly
    if (*current_path_position == PATH_SEPERATOR) {
        current_path_position++;
    }

    while (*current_path_position) {
        char *next_separator_position = strchr(current_path_position, PATH_SEPERATOR);
        if (!next_separator_position) {
            // No more separators, print the remaining path directly
            printf("VFS: Current node: %s, looking for segment: %s\n", current_node->name, current_path_position);
        } else {
            size_t segment_length = next_separator_position - current_path_position;
            char *segment = malloc(segment_length + 1);
            if (!segment) {
                printf("VFS: Memory allocation failed!\n");
                free(path_preproccesed);
                return NULL;
            }
            memcpy(segment, current_path_position, segment_length);
            segment[segment_length] = '\0';
            printf("VFS: Current node: %s, looking for segment: %s\n", current_node->name, segment);
            free(segment);
        }

        // Ensure the current node is a directory
        if (current_node->type != VFS_NODE_TYPE_DIR) {
            printf("VFS: Node type at %s is not a directory!\n", current_node->name);
            free(path_preproccesed);
            return NULL;
        }

        // Search for the child node directly
        linkedListNode *current_list_item = current_node->data;
        vfs_node *child_node = NULL;

        while (current_list_item) {
            vfs_node *node = (vfs_node *)current_list_item->data;
            if (!next_separator_position) {
                // Compare the full remaining path
                if (strcmp(node->name, current_path_position) == 0) {
                    child_node = node;
                    break;
                }
            } else {
                // Compare the current segment
                if (memcmp(node->name, current_path_position, next_separator_position - current_path_position) == 0 &&
                    node->name[next_separator_position - current_path_position] == '\0') {
                    child_node = node;
                    break;
                }
            }
            current_list_item = current_list_item->next;
        }

        if (!child_node) {
            if (return_last_node) {
                printf("VFS: Returning last found node: %s\n", current_node->name);
                free(path_preproccesed);
                return current_node;
            }
            printf("VFS: Segment not found: %s under node %s\n", current_path_position, current_node->name);
            free(path_preproccesed);
            return NULL;
        }

        current_node = child_node;

        if (!next_separator_position) {
            break;
        }
        current_path_position = next_separator_position + 1; // Skip separator
    }

    free(path_preproccesed);
    return current_node;
}


vfs_node *mount_file_system(char *path, uint64_t disk_number, uint64_t disk_offset, enum file_system_type type) {
    vfs_node *mount_node = vfs_get_node(path, 0);
    if(!mount_node){
        printf("Couldn't mount filesystem the path doesn't exist!");
    }
    filesystem_data *data = malloc(sizeof(filesystem_data));
    data->disk_number = disk_number;
    data->start_offset = disk_offset;
    data->type = type;
    mount_node->type = VFS_NODE_TYPE_FILE_SYSTEM;
    mount_node->data = data;
    return mount_node;
}
