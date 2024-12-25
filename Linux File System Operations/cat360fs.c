#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include "disk.h"


int main(int argc, char *argv[]) {
    superblock_entry_t sb;
    directory_entry_t de;
    int  i, ii;
    char *imagename = NULL;
    char *filename  = NULL;
    FILE *f;
    int *fat_data;
    char *file_data;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--image") == 0 && i+1 < argc) {
            imagename = argv[i+1];
            i++;
        } else if (strcmp(argv[i], "--file") == 0 && i+1 < argc) {
            filename = argv[i+1];
            i++;
        }
    }

    if (imagename == NULL || filename == NULL) {
        fprintf(stderr, "usage: cat360fs --image <imagename> " \
            "--file <filename in image>");
        exit(1);
    }
    
    // Opening the image file
    f = fopen(imagename, "rb");
    if (f == NULL){
        fprintf(stderr, "Error opening file %s\n", imagename);
        exit(1);
    }

    // Read the superblock and store it in sb
    if (fread(&sb, sizeof(sb),1,f) != 1){
        fprintf(stderr, "Error reading superblock\n");
        exit(1);
    }
    
    // Converting file information from big-endian to little-endian for Jhub
    sb.block_size = ntohs(sb.block_size);
    sb.num_blocks = ntohl(sb.num_blocks);
    sb.fat_start = ntohl(sb.fat_start);
    sb.fat_blocks = ntohl(sb.fat_blocks);
    sb.dir_start = ntohl(sb.dir_start);
    sb.dir_blocks = ntohl(sb.dir_blocks);
    
    // Allocating memory to store FAT data we are gonna read from the file
    fat_data = malloc(sb.block_size * sb.fat_blocks);
    if(fat_data == NULL){
        fprintf(stderr, "Error allocating memoery for FAT\n");
        exit(1);
    }
    // Moving cursor to section where the FAT should start in the file then reading it 
    fseek(f, sb.fat_start * sb.block_size, SEEK_SET);
    if(fread(fat_data, sb.block_size, sb.fat_blocks, f) != sb.fat_blocks){
        fprintf(stderr, "Error reading from FAT\n");
        exit(1);
    }
    
    // Converting FAT entries from big-endian to little-endian
    for (i = 0; i < sb.num_blocks; i++) {
        fat_data[i] = ntohl(fat_data[i]);
    }
    
    // Search for the file in the directory
    fseek(f, sb.dir_start * sb.block_size, SEEK_SET);
    for(i = 0; i < ((sb.block_size * sb.dir_blocks) / sizeof(directory_entry_t)); i++){
        if(fread(&de, sizeof(de), 1, f) != 1){
            fprintf(stderr, "Error reading from directory entry\n");
            exit(1);
        }
        if (strcmp(de.filename, filename) == 0) {                 // We have found the file in the directory
            file_data = malloc(sb.block_size);                    // Allocate memory for one block of file data
            if(file_data == NULL){
                fprintf(stderr, "Error allocating memory for file data\n");
                exit(1);
            }
            int block = ntohl(de.start_block);                    // Start of directory block
            for (i = 0; i < de.num_blocks; i++) {
                if(block < 0 || block >=  sb.num_blocks){
                    fprintf(stderr, "Invalid block in FAT\n");
                    exit(1);
                }
                fseek(f, block * sb.block_size, SEEK_SET);        // Move cursor to correct block in file
                size_t size_to_read;
                if (i == de.num_blocks - 1) {
                    size_to_read = de.file_size % sb.block_size;  // Size of last block
                } else {
                    size_to_read = sb.block_size;                 // Size of all other blocks
                }
                if (fread(file_data, size_to_read, 1, f) != 1) {  // Reading a block or less at a time into file data
                    fprintf(stderr, "Error reading file data\n");
                    exit(1);
                }
                fwrite(file_data, size_to_read, 1, stdout);       // Print the block
                if (fat_data[block] == -1) {                      // Check if this is the last block of the file
                    break;
                }
                block = fat_data[block];                          // Update the block index for the next iteration
            }
            free(file_data);
            free(fat_data);
            fclose(f);
            return 0;
        }
    }

    // If we get here, the file was not found
    fprintf(stderr, "File not found\n");
    // Free allocated memory and close file
    free(fat_data);
    fclose(f);
    return 0; 
}