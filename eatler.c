#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <dirent.h>

#define MAX_PATH 1024
#define MAX_EXTENSIONS 100

void print_usage(const char *program_name) {
    printf("Usage: %s -e <extension1,extension2,...> -d <directory> [-i <ignore1,ignore2,...>]\n", program_name);
}

int should_ignore(const char* name, const char* ignore[], int ignore_count) {
    for (int i = 0; i < ignore_count; i++) {
        if (strcmp(name, ignore[i]) == 0) {
            return 1;  // Match found, should ignore
        }
    }
    return 0;  // No match, don't ignore
}

int matches_extensions(const char* file_name, const char* extensions[], int ext_count) {
    const char *ext = strrchr(file_name, '.');
    if (ext == NULL) return 0;  // No extension found

    for (int i = 0; i < ext_count; i++) {
        if (strcmp(ext, extensions[i]) == 0) {
            return 1;  // Extension matches
        }
    }
    return 0;  // No match
}

int append_file_to_output(const char* file_name, FILE* output_file) {
    fprintf(output_file, "// File %s\n", file_name);
    FILE* f = fopen(file_name, "r");
    if (f == NULL) {
        perror("fopen input file");
        return 0;
    }

    char buffer[1024];
    size_t bytes;

    while ((bytes = fread(buffer, 1, sizeof(buffer), f)) > 0) {
        if (fwrite(buffer, 1, bytes, output_file) != bytes) {
            perror("fwrite");
            fclose(f);
            return 0;
        }
    }

    if (ferror(f)) {
        perror("fread");
        fclose(f);
        return 0;
    }

    fclose(f);
    return 1;
}

int parse_dir(const char *dir_name, FILE *output_file, const char *extensions[], int ext_count, const char *ignore[], int ignore_count) {
    DIR *dir = opendir(dir_name);
    if (dir == NULL) {
        perror("opendir");
        return 0;
    }

    struct dirent *entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;  
        }

        if (should_ignore(entry->d_name, ignore, ignore_count)) {
            printf("Ignoring: %s\n", entry->d_name);
            continue;
        }

        char full_path[MAX_PATH];
        snprintf(full_path, sizeof(full_path), "%s/%s", dir_name, entry->d_name);

        if (entry->d_type == DT_REG && matches_extensions(entry->d_name, extensions, ext_count)) {
            append_file_to_output(full_path, output_file);
        } else if (entry->d_type == DT_DIR) {
            parse_dir(full_path, output_file, extensions, ext_count, ignore, ignore_count);
        }
    }

    closedir(dir);
    return 1;
}

int main(int argc, char *argv[]) {
    char *file_ext_list = NULL;
    char *dir_name = ".";
    char *ignore_list = NULL;
    char *ignore[100];
    int ignore_count = 0;
    char *extensions[MAX_EXTENSIONS];
    int ext_count = 0;
    int opt;

    while ((opt = getopt(argc, argv, "e:d:i:")) != -1) {
        switch (opt) {
            case 'e':
                file_ext_list = optarg;
                break;
            case 'd':
                dir_name = optarg;
                break;
            case 'i':
                ignore_list = optarg;
                break;
            default:
                print_usage(argv[0]);
                return EXIT_FAILURE;
        }
    }

    if (file_ext_list == NULL) {
        print_usage(argv[0]);
        return EXIT_FAILURE;
    }

    char *token = strtok(file_ext_list, ",");
    while (token != NULL && ext_count < MAX_EXTENSIONS) {
        extensions[ext_count++] = token;
        token = strtok(NULL, ",");
    }

    if (ignore_list != NULL) {
        token = strtok(ignore_list, ",");
        while (token != NULL && ignore_count < 100) {
            ignore[ignore_count++] = token;
            token = strtok(NULL, ",");
        }
    }

    FILE *output_file = fopen("output.txt", "w");
    if (output_file == NULL) {
        perror("fopen output file");
        return EXIT_FAILURE;
    }

    parse_dir(dir_name, output_file, (const char **)extensions, ext_count, (const char **)ignore, ignore_count);

    fclose(output_file);
    return EXIT_SUCCESS;
}

