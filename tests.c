#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "lib_tar.h"

/**
 * You are free to use this file to write tests for your implementation
 */

void debug_dump(const uint8_t *bytes, size_t len) {
    for (int i = 0; i < len;) {
        printf("%04x:  ", (int) i);

        for (int j = 0; j < 16 && i + j < len; j++) {
            printf("%02x ", bytes[i + j]);
        }
        printf("\t");
        for (int j = 0; j < 16 && i < len; j++, i++) {
            printf("%c ", bytes[i]);
        }
        printf("\n");
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        printf("Usage: %s tar_file\n", argv[0]);
        return -1;
    }

    int fd = open(argv[1] , O_RDONLY);
    if (fd == -1) {
        perror("open(tar_file)");
        return -1;
    }

    int ret0 = check_archive(fd); // should worki
    printf("check_archive returned %d\n", ret0);

    int ret1 = exists(fd,"folder1/file1.txt"); // should worki
    printf("exists returned %d\n", ret1);

    int ret2 = is_dir(fd,"folder1/folder3/"); // should worki
    printf("Directory returned %d\n", ret2); 
    
    int ret3 = is_file(fd,"folder1/file1.txt"); // should worki
    printf("File returned %d\n", ret3);

    int ret4 = is_file(fd, "folder1/"); // should not work
    printf("File returned %d\n", ret4);

    int ret5 = is_symlink(fd, "symlink"); //should work lol
    printf("Symlink returned %d\n", ret5);

    int ret6 = read_file(fd,"folder1/file.txt",0,"folder1/",10); 
    printf("Read_file returned %d\n", ret6);
    



    

    return 0;
}