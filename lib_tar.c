#include "lib_tar.h"
#include <string.h>
#include <stdio.h>

#define BLOCKSIZE 512

/**
 * Checks whether the archive is valid.
 *
 * Each non-null header of a valid archive has:
 *  - a magic value of "ustar" and a null,
 *  - a version value of "00" and no null,
 *  - a correct checksum
 *
 * @param tar_fd A file descriptor pointing to the start of a file supposed to contain a tar archive.
 *
 * @return a zero or positive value if the archive is valid, representing the number of non-null headers in the archive,
 *         -1 if the archive contains a header with an invalid magic value,
 *         -2 if the archive contains a header with an invalid version value,
 *         -3 if the archive contains a header with an invalid checksum value
 */
int check_archive(int tar_fd) {
    int nb_headers = 0;
    int nb_block = 0;
    //int x = 1;
    //int start = tar_fd;
    
    //tar_header_t header;
    //pread(tar_fd, &header, sizeof(tar_header_t), nb_block*sizeof(tar_header_t));

    while (1) //while(1)
    {
        //use of pread because you can offset the copy
        tar_header_t header;
        pread(tar_fd, &header, sizeof(tar_header_t), nb_block*sizeof(tar_header_t));
                
        if (!strcmp(header.name, "\0")){
            return nb_headers;
        }

        printf("Name: %s\n", header.name);


        //checking if the header is valide
        /* if (header.magic != (char *) TMAGIC) //Fonctionne pas... return toujours -1
        {
            return -1;
        }
        if (!strcmp(header.version,(char *) TVERSION))
        {
            return -2;
        }
        
        */
        

        
        if (TAR_INT(header.size)%BLOCKSIZE == 0){ //if all blocks are full then offset by the number of 512 byte wich make the file
            nb_block += (1 + TAR_INT(header.size)/BLOCKSIZE);
        }else{ //if the blocks are not full
            nb_block += (2 + TAR_INT(header.size)/BLOCKSIZE); 
        }
        
        printf("nb_block: %d\n", nb_block);
        
        nb_headers += 1;
        //x+=1;
    }    
    return nb_headers;
}

/**
 * Checks whether an entry exists in the archive.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 *
 * @return zero if no entry at the given path exists in the archive,
 *         any other value otherwise.
 */
int exists(int tar_fd, char *path) {
    int nb_headers = 0;
    int nb_block = 0;
    //int x = 1;
    //int start = tar_fd;
    
    //tar_header_t header;
    //pread(tar_fd, &header, sizeof(tar_header_t), nb_block*sizeof(tar_header_t));

    while (1) //while(1)
    {
        //use of pread because you can offset the copy
        tar_header_t header;
        pread(tar_fd, &header, sizeof(tar_header_t), nb_block*sizeof(tar_header_t));
                
        if (!strcmp(header.name, "\0")){
            return nb_headers;
        }

        printf("Name: %s\n", header.name);
        if (strcmp(header.name, path))
        {
            return 1;
        }    
        if (TAR_INT(header.size)%BLOCKSIZE == 0){ //if all blocks are full then offset by the number of 512 byte wich make the file
            nb_block += (1 + TAR_INT(header.size)/BLOCKSIZE);
        }else{ //if the blocks are not full
            nb_block += (2 + TAR_INT(header.size)/BLOCKSIZE); 
        }
        
        printf("nb_block: %d\n", nb_block);
        
        nb_headers += 1;
        //x+=1;
    } 
    return 0;
}

/**
 * Checks whether an entry exists in the archive and is a directory.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 *
 * @return zero if no entry at the given path exists in the archive or the entry is not a directory,
 *         any other value otherwise.
 */
int is_dir(int tar_fd, char *path) {
    //Typeflag = 5
    int nb_headers = 0;
    int nb_block = 0;
    //int x = 1;
    //int start = tar_fd;
    
    //tar_header_t header;
    //pread(tar_fd, &header, sizeof(tar_header_t), nb_block*sizeof(tar_header_t));

    while (1) //while(1)
    {
        //use of pread because you can offset the copy
        tar_header_t header;
        pread(tar_fd, &header, sizeof(tar_header_t), nb_block*sizeof(tar_header_t));
                
        if (!strcmp(header.name, "\0")){
            return nb_headers;
        }

        printf("Name: %s\n", header.name);
        if (strcmp(header.name, path))
        {
            if (header.typeflag == 5)
            {
                return 1;
            }
            return 0;
    
        }    
        if (TAR_INT(header.size)%BLOCKSIZE == 0){ //if all blocks are full then offset by the number of 512 byte wich make the file
            nb_block += (1 + TAR_INT(header.size)/BLOCKSIZE);
        }else{ //if the blocks are not full
            nb_block += (2 + TAR_INT(header.size)/BLOCKSIZE); 
        }
        
        printf("nb_block: %d\n", nb_block);
        
        nb_headers += 1;
        //x+=1;
    } 
    return 0;
}

/**
 * Checks whether an entry exists in the archive and is a file.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 *
 * @return zero if no entry at the given path exists in the archive or the entry is not a file,
 *         any other value otherwise.
 */
int is_file(int tar_fd, char *path) {
    //Typeflag = 0
    int nb_headers = 0;
    int nb_block = 0;
    //int x = 1;
    //int start = tar_fd;
    
    //tar_header_t header;
    //pread(tar_fd, &header, sizeof(tar_header_t), nb_block*sizeof(tar_header_t));

    while (1) //while(1)
    {
        //use of pread because you can offset the copy
        tar_header_t header;
        pread(tar_fd, &header, sizeof(tar_header_t), nb_block*sizeof(tar_header_t));
                
        if (!strcmp(header.name, "\0")){
            return nb_headers;
        }

        printf("Name: %s\n", header.name);
        if (strcmp(header.name, path)) //Je sais pas si ça marche atm
        {
            if (header.typeflag == 0)
            {
                return 1;
            }
            return 0;
    
        }    
        if (TAR_INT(header.size)%BLOCKSIZE == 0){ //if all blocks are full then offset by the number of 512 byte wich make the file
            nb_block += (1 + TAR_INT(header.size)/BLOCKSIZE);
        }else{ //if the blocks are not full
            nb_block += (2 + TAR_INT(header.size)/BLOCKSIZE); 
        }
        
        printf("nb_block: %d\n", nb_block);
        
        nb_headers += 1;
        //x+=1;
    } 
    return 0;
}

/**
 * Checks whether an entry exists in the archive and is a symlink.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive.
 * @return zero if no entry at the given path exists in the archive or the entry is not symlink,
 *         any other value otherwise.
 */
int is_symlink(int tar_fd, char *path) {
    return 0;
}


/**
 * Lists the entries at a given path in the archive.
 * list() does not recurse into the directories listed at the given path.
 *
 * Example:
 *  dir/          list(..., "dir/", ...) lists "dir/a", "dir/b", "dir/c/" and "dir/e/"
 *   ├── a
 *   ├── b
 *   ├── c/
 *   │   └── d
 *   └── e/
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive. If the entry is a symlink, it must be resolved to its linked-to entry.
 * @param entries An array of char arrays, each one is long enough to contain a tar entry path.
 * @param no_entries An in-out argument.
 *                   The caller set it to the number of entries in `entries`.
 *                   The callee set it to the number of entries listed.
 *
 * @return zero if no directory at the given path exists in the archive,
 *         any other value otherwise.
 */
int list(int tar_fd, char *path, char **entries, size_t *no_entries) {
    return 0;
}

/**
 * Reads a file at a given path in the archive.
 *
 * @param tar_fd A file descriptor pointing to the start of a valid tar archive file.
 * @param path A path to an entry in the archive to read from.  If the entry is a symlink, it must be resolved to its linked-to entry.
 * @param offset An offset in the file from which to start reading from, zero indicates the start of the file.
 * @param dest A destination buffer to read the given file into.
 * @param len An in-out argument.
 *            The caller set it to the size of dest.
 *            The callee set it to the number of bytes written to dest.
 *
 * @return -1 if no entry at the given path exists in the archive or the entry is not a file,
 *         -2 if the offset is outside the file total length,
 *         zero if the file was read in its entirety into the destination buffer,
 *         a positive value if the file was partially read, representing the remaining bytes left to be read to reach
 *         the end of the file.
 *
 */
ssize_t read_file(int tar_fd, char *path, size_t offset, uint8_t *dest, size_t *len) {
    return 0;
}

