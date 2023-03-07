#define CGLTF_IMPLEMENTATION
#define CGLTF_WRITE_IMPLEMENTATION
#include "cgltf_write.h"

int main(int argc, char *argv[]) {
    cgltf_options options = {};
    options.type = cgltf_file_type_glb;
    options.json_token_count = 0;
    options.memory.alloc_func = nullptr;
    options.memory.free_func = nullptr;
    options.memory.user_data = nullptr;
    options.file.read = nullptr;
    options.file.release = nullptr;
    cgltf_data *data = nullptr;
    char *input_path = nullptr;
    char *output_path = nullptr;
    if (argc < 2) {
        printf("glbToUnlitGlb.exe lit_input.glb unlit_output.glb\n");
        return 1;
    } else if (argc == 2) {
        input_path = argv[1];
        printf("input path=%s\n", input_path);
    } else if (argc >= 3) {
        output_path = argv[2];
        printf("output path=%s\n", output_path);
    }
    cgltf_result result = cgltf_parse_file(&options, input_path, &data);
    if (result != cgltf_result_success) {
        printf("failed, returnCode=%d\n", result);
        cgltf_free(data);
        return 2;
    }

    cgltf_size old_extensions_count = data->extensions_used_count;
    if (old_extensions_count == 0) {
        data->extensions_used = (char **) malloc(sizeof(char *) * (old_extensions_count + 1));
        data->extensions_used[0] = (char *) "KHR_materials_unlit";
    } else if (old_extensions_count > 0) {
        // check whether current glb is unlit?
        bool is_unlit = false;
        for (int i = 0; i < old_extensions_count; ++i) {
            if (strcmp(data->extensions_used[i], "KHR_materials_unlit") == 0) {
                is_unlit = true;
                break;
            }
        }
        char **old_ptr = data->extensions_used;
        if (!is_unlit && old_ptr) {
            data->extensions_used = (char **) malloc(sizeof(char *) * (old_extensions_count + 1));
            for (int i = 0; i < old_extensions_count; ++i) {
                printf("i=%d, str=%s\n", i, old_ptr[i]);
                data->extensions_used[i] = old_ptr[i];
            }
            // add KHR_materials_unlit extension flag
            data->extensions_used[old_extensions_count] = (char *) "KHR_materials_unlit";
            free(old_ptr);
        }
    }

    for (int i = 0; i < data->materials_count; ++i) {
        // set every material to unlit
        printf("material[%d]=%d\n", i, data->materials[i].unlit);
        data->materials[i].unlit = 1;
    }
    printf("unlit ok\n");

    // export as unlit glb
    cgltf_options export_options = {};
    export_options.type = cgltf_file_type_glb;
    export_options.json_token_count = 0;
    export_options.memory.alloc_func = nullptr;
    export_options.memory.free_func = nullptr;
    export_options.memory.user_data = nullptr;
    export_options.file.read = nullptr;
    export_options.file.release = nullptr;
    cgltf_result result2 = cgltf_write_file(&export_options, output_path, data);
    cgltf_free(data);
    if (result2 != cgltf_result_success) {
        printf("fail to export unlit glb\n");
        return -2;
    }

    printf("finish\n");
    //printf("argc=%d, first argv=%s\n", argc, argv[0]);
    return 0;
}
