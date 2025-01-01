#include <memory.h>
#include <string.h>
#include <vfs.h>

vfs_node *vfs_root = (vfs_node *)NULL;

static inline char *preprocess_path(char *original_string) {
    char *ret_str = malloc(strlen(original_string) + 1);
    char *current = ret_str;
    while (*(original_string) && !isspace(*(original_string))) {
        *current = *original_string;
        original_string++;
        current++;
    }
    return ret_str;
}

void init_vfs() {
    vfs_root = (vfs_node *)malloc(sizeof(vfs_node));
    memset(vfs_root, 0, sizeof(vfs_node));
    memcpy(vfs_root->name, ROOT_DIR_NAME, strlen(ROOT_DIR_NAME));
}

vfs_node *vfs_get_create_node_in_path(char *path) {
    char *current_path_position = preprocess_path(path);
    if (!current_path_position || path[0] != ROOT_DIR_NAME[0]) { // Ensure valid absolute path
        printf("VFS: Invalid Path!\n");
        return NULL;
    }

    char *next_separator_position;
    vfs_node *current_node = vfs_root;

    while ((next_separator_position = strchr(current_path_position, PATH_SEPERATOR)) != NULL) {
        size_t segment_length = next_separator_position - current_path_position;

        if (segment_length == 0) { // Skip redundant separators
            current_path_position++;
            continue;
        }

        switch (current_node->type) {
            case VFS_NODE_TYPE_DIR: {
                linkedListNode *current_list_item = current_node->data;
                int found = 0;

                while (current_list_item) {
                    vfs_node *child_node = (vfs_node *)current_list_item->data;
                    if (memcmp(child_node->name, current_path_position, segment_length) == 0 &&
                        child_node->name[segment_length] == '\0') {
                        current_node = child_node;
                        found = 1;
                        break;
                    }
                    current_list_item = current_list_item->next;
                }

                if (!found) {
                    vfs_node *new_node = malloc(sizeof(vfs_node));
                    if (!new_node) {
                        printf("VFS: Memory allocation failed!\n");
                        return NULL;
                    }
                    memcpy(new_node->name, current_path_position, segment_length);
                    new_node->name[segment_length] = '\0';
                    new_node->type = VFS_NODE_TYPE_DIR;
                    new_node->data = NULL; // Initialize other fields as needed
                    append_node((linkedListNode **)&(current_node->data), new_node);
                    current_node = new_node;
                }

                current_path_position = next_separator_position + 1;
                break;
            }

            default:
                printf("VFS: The path contains part with name: %s which isn't a valid type\n", current_node->name);
                return NULL;
        }
    }

    if (*current_path_position) {
        printf("VFS: Unhandled path segment: %s\n", current_path_position);
    }

    free(current_path_position); // Only if dynamically allocated
    return current_node;
}


vfs_node *mount_file_system(char *path, uint64_t disk_number, uint64_t disk_offset, enum file_system_type type) {
    vfs_node *mount_node = vfs_get_create_node_in_path(path);
    filesystem_data *data = malloc(sizeof(filesystem_data));
    data->disk_number = disk_number;
    data->start_offset = disk_offset;
    data->type = type;
    mount_node->type = VFS_NODE_TYPE_FILE_SYSTEM;
    mount_node->data = data;
    return mount_node;
}
