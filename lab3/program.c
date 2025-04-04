#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

void reverse_line(char *line)
{
    int len = (int)strlen(line);
    for(int i = 0; i < len / 2; i++)
    {
        char tmp = line[i];
        line[i] = line[len - 1 - i];
        line[len - 1 - i] = tmp;
    }
}

void process_file(const char *src_path, const char *dest_path)
{
    FILE *src_file = fopen(src_path, "r");
    FILE *dest_file = fopen(dest_path, "w");

    char curr_line[1000];
    while(fgets(curr_line, 1000, src_file))
    {
        curr_line[strcspn(curr_line, "\n")] = 0;
        reverse_line(curr_line);
        fprintf(dest_file, "%s\n", curr_line);
    }
    fclose(src_file);
    fclose(dest_file);
}

void process_directory(const char *src_dir, const char *dest_dir) {
    struct dirent *entry;
    DIR *dir = opendir(src_dir);
    
    mkdir(dest_dir, 0777);
    
    while ((entry = readdir(dir))) 
    {
        if (entry->d_type == DT_REG) 
        {
            const char *ext = strrchr(entry->d_name, '.');
            if (ext && strcmp(ext, ".txt") == 0)
            {
                char src_path[1000], dest_path[1000];
                snprintf(src_path, 1000, "%s/%s", src_dir, entry->d_name);
                snprintf(dest_path, 1000, "%s/%s", dest_dir, entry->d_name); 
                printf("curr file: %s -> %s\n", src_path, dest_path);
                process_file(src_path, dest_path);
            }
        }
    }
    
    closedir(dir);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        return 0;
    }
    
    process_directory(argv[1], argv[2]);
    
    return 0;
}