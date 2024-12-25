#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <string.h>
#include "disk.h"

char *month_to_string(short m) {
    switch(m) {
    case 1: return "Jan";
    case 2: return "Feb";
    case 3: return "Mar";
    case 4: return "Apr";
    case 5: return "May";
    case 6: return "Jun";
    case 7: return "Jul";
    case 8: return "Aug";
    case 9: return "Sep";
    case 10: return "Oct";
    case 11: return "Nov";
    case 12: return "Dec";
    default: return "?!?";
    }
}


void unpack_datetime(unsigned char *time, short *year, short *month, 
    short *day, short *hour, short *minute, short *second)
{
    assert(time != NULL);

    memcpy(year, time, 2);
    *year = htons(*year);

    *month = (unsigned short)(time[2]);
    *day = (unsigned short)(time[3]);
    *hour = (unsigned short)(time[4]);
    *minute = (unsigned short)(time[5]);
    *second = (unsigned short)(time[6]);
}


int main(int argc, char *argv[]) {
    superblock_entry_t sb;
    directory_entry_t de;
    int  i;
    char *imagename = NULL;
    FILE *f;
    short year, month, day, hour, minute, second;

    for (i = 1; i < argc; i++) {
        if (strcmp(argv[i], "--image") == 0 && i+1 < argc) {
            imagename = argv[i+1];
            i++;
        }
    }

    if (imagename == NULL)
    {
        fprintf(stderr, "usage: ls360fs --image <imagename>\n");
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
    
    // Moving cursor to where directory should start
    fseek(f, sb.dir_start * sb.block_size, SEEK_SET);
    
    // Looping through all direcotry blocks
    for(i = 0; i < ((sb.block_size * sb.dir_blocks) / sizeof(directory_entry_t)); i++){
        if(fread(&de, sizeof(de), 1, f) != 1){
            fprintf(stderr, "Error reading from directory entry\n");
            exit(1);
        }// Reading the information of directory entry each block
        
        if(de.status == DIR_ENTRY_AVAILABLE){
            continue;
        }// If directory is available then it is empty so we nothing to print
        
        // Unpacking creation date then printing everything in desired format 
        unpack_datetime(de.create_time, &year, &month, &day, &hour, &minute, &second);
        printf("%8d %4d-%s-%02d %02d:%02d:%02d %s\n", ntohl(de.file_size), year, month_to_string(month), day, hour, minute, second, de.filename);
    }
    
    fclose(f);
    return 0; 
}

