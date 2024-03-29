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

        //printf("Name: %s\n", header.name);
        //printf("checksum: %ld\n", TAR_INT(header.chksum));
        //printf("Version: %d\n", strcmp(header.version,(char *) TVERSION));

        //checking if the header is valide
        if (strcmp(header.magic,(char *) TMAGIC)) //strcmp return 0 if str1 and str2 are equal
        {
            return -1;
        }
        if (strcmp(header.version,TVERSION) == 0) //seems to be incorect
        {
            return -2;
        }

        long int checksum = TAR_INT(header.chksum);// copy checksum to compare it later
        memset(header.chksum, ' ',8); //header checksum as to be empty before calculing it
        int8_t* byte = (int8_t *) &header; //byte have to be a 8 bite, maybe better to set to int_8 as int in dependent on the machine
        long int homemade_checksum = 0; //int is too small
        for (int i = 0; i < BLOCKSIZE; i++)
        {
            homemade_checksum += *(byte++); //byte++ is some funcky pointer manipulation that i'm not sure to understand
        }
        //printf("homemade checksum: %ld\n", homemade_checksum);

        if (checksum != homemade_checksum)
        {
            return -3;
        }
        
        

        
        if (TAR_INT(header.size)%BLOCKSIZE == 0){ //if all blocks are full then offset by the number of 512 byte wich make the file
            nb_block += (1 + TAR_INT(header.size)/BLOCKSIZE);
        }else{ //if the blocks are not full
            nb_block += (2 + TAR_INT(header.size)/BLOCKSIZE); 
        }
        
        //printf("nb_block: %d\n", nb_block);
        
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
    int nb_block = 0;
    //lseek(tar_fd,0,SEEK_SET);
    //char header_test[BLOCKSIZE];
    while (1)
    {
        tar_header_t header;
        pread(tar_fd, &header, sizeof(tar_header_t), nb_block*sizeof(tar_header_t));
                
        if (!strcmp(header.name, "\0")){ //check if it is the last header in the archive, EOF
            return 0;
        }

        //printf("Name exists: %s\n", header.name); //check if the entry correspond to the header, if so [return 1] Succes, else try with the next header until EOF
        if (strcmp(header.name, path) == 0)
        {
            return 1;
        }    
        if (TAR_INT(header.size)%BLOCKSIZE == 0){ //if all blocks are full then offset by the number of 512 byte wich make the file
            nb_block += (1 + TAR_INT(header.size)/BLOCKSIZE);
        }else{ //if the blocks are not full
            nb_block += (2 + TAR_INT(header.size)/BLOCKSIZE); 
        }
        
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
    int nb_block = 0;
    //if (exists(tar_fd,path) == 0)
    //{
    //    return 0;
    //}
    while (1)
    {
        tar_header_t header;
        pread(tar_fd, &header, sizeof(tar_header_t), nb_block*sizeof(tar_header_t));
                
        if (!strcmp(header.name, "\0")){
            return 0; //is not dir or entry does not exist, EOF
        }

        // Check the type flag in the tar header
        //printf("is_dir typeflag; %d\n",header.typeflag);
        if ((strcmp(header.name, path) == 0) && header.typeflag == DIRTYPE) //check if exist and if the type flag is also correct
        {
            return 1; //succes
        }
                

        if (TAR_INT(header.size)%BLOCKSIZE == 0){ //if all blocks are full then offset by the number of 512 byte wich make the file
            nb_block += (1 + TAR_INT(header.size)/BLOCKSIZE);
        }else{ //if the blocks are not full
            nb_block += (2 + TAR_INT(header.size)/BLOCKSIZE); 
        }
        
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
    int nb_block = 0;
    while (1)
    {
        tar_header_t header;
        pread(tar_fd, &header, sizeof(tar_header_t), nb_block*sizeof(tar_header_t));
                
        if (!strcmp(header.name, "\0")){
            return 0;
        }

        //printf("Name: %s\n", header.name);
        //char header_test[BLOCKSIZE];
        // Check the type flag in the tar header
        if ((header.typeflag == REGTYPE || header.typeflag == AREGTYPE) && strcmp(header.name, path) == 0) { //checking if REGTYPE and AREGTYPE are correct and that entry exist
            return 1;
        }

        if (TAR_INT(header.size)%BLOCKSIZE == 0){ //if all blocks are full then offset by the number of 512 byte wich make the file
            nb_block += (1 + TAR_INT(header.size)/BLOCKSIZE);
        }else{ //if the blocks are not full
            nb_block += (2 + TAR_INT(header.size)/BLOCKSIZE); 
        }
        
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
    int nb_block = 0;
    if (exists(tar_fd,path) == 0)
    {
        return 0;
    }
    while (1)
    {
        tar_header_t header;
        pread(tar_fd, &header, sizeof(tar_header_t), nb_block*sizeof(tar_header_t));
                
        if (!strcmp(header.name, "\0")){
            return 0;
        }

        //printf("Name: %s\n", header.name);
        //char header_test[BLOCKSIZE];
        // Check the type flag in the tar header
        if ((header.typeflag == LNKTYPE || header.typeflag == SYMTYPE) && (strcmp(header.name, path) == 0)) {
            return 1;
        }
        
        if (TAR_INT(header.size)%BLOCKSIZE == 0){ //if all blocks are full then offset by the number of 512 byte wich make the file
            nb_block += (1 + TAR_INT(header.size)/BLOCKSIZE);
        }else{ //if the blocks are not full
            nb_block += (2 + TAR_INT(header.size)/BLOCKSIZE); 
        }
        
    }
    
    return 0;
}

int diff_slash(char *path, char *name){
    int pathe  = 0;
    int namee = 0;
    int diff = 0;

    for (int i = 0; i < strlen(path); i++)
    {
        if (path[i] == 47){
            pathe += 1;
        }
    }

    for (int j = 0; j < strlen(name); j++)
    {
        if (name[j] == 47){
            namee += 1;
        }
        if ((namee == pathe+1) && (strlen(name) > j+1)){
            //printf("file protection\n");
            return 0;
        }
    }
    //printf("Number slash name: %d, path: %d\n", namee, pathe);
    diff = namee - pathe;
    if (diff == 1 || diff == 0){
        return 1;
    }else
    {
        return 0;
    }
    
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
    int nb_block = 0;
    int i = 0;
    char* p;

    while(1){
        
        tar_header_t header;
        pread(tar_fd, &header, sizeof(tar_header_t), nb_block*sizeof(tar_header_t));// navigate through headers
        
        if (!strcmp(header.name, "\0")){ //test if we are at the end of the archive
            //printf("EOF\n");
            return 0;
        }
        
        if (strcmp(header.name, path) == 0){ //we test if we found the path
            //printf("symlink name ?: %s\n", header.name);
            if ((header.typeflag == LNKTYPE || header.typeflag == SYMTYPE)) {
                //printf("Defenetly a symlink: %s\n", header.name);
                return list(tar_fd, header.linkname, entries, no_entries); //if symlink, run list with the correct file path
            }
            //if not symlink do somthing
        }
        //printf("path: %s\n", path);
        p = strstr(header.name, path);
        //printf("p: %s\n", p);
        if (p){//if path is in header name then we have a entry, what about recusive folders? 
            //printf("Path is in name: %s\n", header.name);
            int diff = strlen(header.name)-strlen(path);
            //printf("diff: %d\n", diff);
            if (diff > 0){
                //printf("diff > 0: %s\n", header.name);
                if (diff_slash(path, header.name)) { 
                    //printf("Pass number of slash: %s\n", header.name);
                    //printf("i: %d\n", i);
                    memcpy(entries[i], header.name, strlen(header.name));
                    //entries[i] = header.name;
                    //printf("test\n");
                    //printf("entries: %s\n", entries[0]);
                    i++;
                    if (i == *no_entries){
                        *no_entries = i;
                        return 1;
                    }
                }
            }
        }
        //  if diff > 0
        //      if only one "/" in header.name[diff : ] then it is a file or folder but no recursive folder


        if (TAR_INT(header.size)%BLOCKSIZE == 0){ //if all blocks are full then offset by the number of 512 byte wich make the file
            nb_block += (1 + TAR_INT(header.size)/BLOCKSIZE);
        }else{ //if the blocks are not full
            nb_block += (2 + TAR_INT(header.size)/BLOCKSIZE); 
        }
        //printf("\n");
                
    }
    *no_entries = i;
    if (*no_entries == 0)
    {
        //printf("no_entry = 0\n");
        return 0;
    }
    
    return 1;
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
ssize_t read_file(int tar_fd, char *path, size_t offset, uint8_t *dest, size_t *len) { //AI is crap
    
    int nb_block = 0;
    int byte_2_read = 0;

    //should check the link
    if(is_file(tar_fd, path) == 0){
        return -1;
    }

    while(1){
        
        tar_header_t header;
        pread(tar_fd, &header, sizeof(tar_header_t), nb_block*sizeof(tar_header_t));// navigate through headers

        //test if symlink

        if (strcmp(header.name, path) == 0){ //if we find the file given by path and is not symlink
            
            if ((header.typeflag == LNKTYPE || header.typeflag == SYMTYPE) && (strcmp(header.name, path) == 0)) {
                return read_file(tar_fd, header.linkname, offset, dest, len); //if symlink, run read_file with the correct file path
            }
            
            
            if (TAR_INT(header.size) < offset){
                return -2;
            }
            byte_2_read = TAR_INT(header.size) - offset;
            //printf("byte_2_read: %d\n", byte_2_read);
            //printf("len before: %ld\n", *len);
            //printf("tar header size: %ld\n", TAR_INT(header.size));
            //printf("tar header name: %s\n", header.name);
            
            // byte_2_read is bigger than len, return byte_2_read-len
            // byte_2read is smaller or equal to len, return 0
            if (byte_2_read > *len)
            {
                //read to buffer     
                pread(tar_fd, dest, *len, offset + ((nb_block+1)*sizeof(tar_header_t))); 
                return byte_2_read - *len;
            }else if (byte_2_read <= *len)
            {
                //read to buffer
                *len = (size_t) byte_2_read; //in out argument, DAMN IT
                //printf("len after: %ld\n", *len);
                pread(tar_fd, dest, *len, offset + ((nb_block+1)*sizeof(tar_header_t))); 
                return 0;
            }            
        }
                

        if (!strcmp(header.name, "\0")){ //test if we are at the end of the archive
            return 0;
        }

        if (TAR_INT(header.size)%BLOCKSIZE == 0){ //if all blocks are full then offset by the number of 512 byte wich make the file
            nb_block += (1 + TAR_INT(header.size)/BLOCKSIZE);
        }else{ //if the blocks are not full
            nb_block += (2 + TAR_INT(header.size)/BLOCKSIZE); 
        }
                
    }
    return 0;
}

