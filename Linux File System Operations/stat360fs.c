#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include "disk.h"


int main(int argc, char *argv[]) {
    superblock_entry_t sb;
    int  i;
    char *imagename = NULL;
    FILE  *f;
    int   *fat_data;
    int free_blocks = 0;
    int reserved_blocks = 0;
    int allocated_blocks = 0;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--image") == 0 && i+1 < argc) {
            imagename = argv[i+1];
            i++;
        }
    }

    if (imagename == NULL)
    {
        fprintf(stderr, "usage: stat360fs --image <imagename>\n");
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
    
    // Print file suberblock information in desired format
    printf("%s (%s)\n\n", sb.magic, imagename);
    printf("------------------------------------------------------------------\n");
    printf("  Bsz  Bcnt  FATst  FATcnt  DIRst  DIRcnt\n");
    printf("%5d%6d%7d%8d%7d%8d\n\n", sb.block_size, sb.num_blocks, sb.fat_start, sb.fat_blocks, sb.dir_start, sb.dir_blocks);
    printf("------------------------------------------------------------------\n");
    
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
    
    // Converting FAT entries from big-endian to little-endian and count block types
    for (i = 0; i < sb.num_blocks; i++) { // All block types should sum up to total number of blocks
        fat_data[i] = ntohl(fat_data[i]);
        if (fat_data[i] == FAT_AVAILABLE) {
            free_blocks++;
        } else if (fat_data[i] == FAT_RESERVED) {
            reserved_blocks++;
        } else {
            allocated_blocks++;
        }
        
    }

    // Print FAT information in desired format
    printf(" Free  Resv  Alloc\n");
    printf("%5d%6d%7d\n\n", free_blocks, reserved_blocks, allocated_blocks);

    // Free up and close file
    free(fat_data);
    fclose(f);
    return 0; 
}
