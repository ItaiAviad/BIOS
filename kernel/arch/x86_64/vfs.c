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

vfs_node *get_hardware_create_node_in_path(char *path) {
    path = preprocess_path(path);

    if (path[0] != ROOT_DIR_NAME[0]) { // This function always expects absulute path
        printf("VFS: Invalid Path!");
        return NULL;
    }

    char *current_name_to_search = strtok(path, PATH_SEPERATOR);
    vfs_node *current_node = vfs_root;
    while (current_name_to_search) {
        switch (current_node->type) {
            case VFS_NODE_TYPE_DIR: {
                linkedListNode *current_list_item = current_node->data;
                int found = false;
                while (current_list_item) {
                    if (strcmp(((vfs_node *)current_list_item->data)->name, current_name_to_search) == 0) {
                        current_node = current_list_item->data;
                        found = true;
                        break;
                    }
                    current_list_item = current_list_item->next;
                }
                if (!found) {
                    vfs_node *new_node = malloc(sizeof(vfs_node));
                    memcpy(new_node->name, current_name_to_search, strlen(current_name_to_search) + 1);
                    new_node->type = VFS_NODE_TYPE_DIR;
                    append_node((linkedListNode **)&(current_node->data), new_node);
                    current_node = new_node;
                }

                current_name_to_search = strtok(NULL, PATH_SEPERATOR);
                break;
            }

            default: {
                printf("The path contains part with name: %s which isn't a valid type", current_node->name);
                return NULL;
            }
        }
    }

    free(path);
    return current_node;
}

vfs_node *mount_file_system(char *path, uint64_t disk_number, uint64_t disk_offset, enum file_system_type type) {
    vfs_node *mount_node = get_hardware_create_node_in_path(path);
    filesystem_data *data = malloc(sizeof(filesystem_data));
    data->disk_number = disk_number;
    data->start_offset = disk_offset;
    data->type = type;
    mount_node->type = VFS_NODE_TYPE_FILE_SYSTEM;
    mount_node->data = data;
    return mount_node;
}
