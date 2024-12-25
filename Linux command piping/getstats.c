/* getstats.c 
 *
 * CSC 360, Summer 2023
 *
 * - If run without an argument, dumps information about the PC to STDOUT.
 *
 * - If run with a process number created by the current user, 
 *   dumps information about that process to STDOUT.
 *
 * Please change the following before submission:
 *
 * Author: Teodor Andrei Georgescu
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Note: You are permitted, and even encouraged, to add other
 * support functions in order to reduce duplication of code, or
 * to increase the clarity of your solution, or both.
 */

/*
This function searches through computer files for a specific process and prints out different information about the process.
This information is:
    Process number: #
    Name:  ___
    Filename (if any): ___
    Threads: #
    Total context swtiches: #

Please not the Filname refers to the file that started this process.
Also the threads refers to how many threads are being used to execute this process.
*/
void print_process_info(char * process_num) {
    // Some initalizations are done for variables.
    char sentance[256];
    char threads[256];
    int voluntary_ctxt_switches;
    int nonvoluntary_ctxt_switches;

    // Creating an array to store desired paths
    char file_to_open[256]; 
    strcpy(file_to_open, "/proc/");
    strcat(file_to_open, process_num);
    strcat(file_to_open, "/status");
    
    //Opening the first stored path and printing the desired information
    FILE *file = fopen(file_to_open, "r");
    if(!file){
        printf("Process number %s not found.\n", process_num); 
        return;
    }
    printf("Process number: %s\n", process_num);  
    
    fgets(sentance, 256, file); 
    printf("%s", sentance);
    
    //Same file holds other desired information which will be stored now and printed later
    //This delayed printing is purely due to assignment specifications 
    while(fgets(sentance, 256, file) !=NULL){ 
        if (strncmp(sentance,"Threads", 7) ==0){
            memcpy(threads,sentance, sizeof(sentance));
        }
        else if(strncmp(sentance,"voluntary_ctxt_switches", 23) ==0){
            sscanf(sentance, "voluntary_ctxt_switches:%d", &voluntary_ctxt_switches);
        }
        else if(strncmp(sentance,"nonvoluntary_ctxt_switches", 26) ==0){
            sscanf(sentance, "nonvoluntary_ctxt_switches:%d", &nonvoluntary_ctxt_switches);
        }
    } 
    fclose(file);
    
    //Overwriting old path with new one that holds other desired information
    strcpy(file_to_open, "/proc/");
    strcat(file_to_open, process_num);
    strcat(file_to_open, "/cmdline");
    
    //Storing the desired information from this path
    file = fopen(file_to_open, "r"); 
    fgets(sentance,256,file);
    char *token = strtok(sentance, "-");
    fclose(file); 
    
    //Printing out all information including those stored before
    printf("Filename (if any): %s\n", token); 
    printf("%s", threads);
    printf("Total context switches: %d \n", (voluntary_ctxt_switches + nonvoluntary_ctxt_switches));
    return;
} 

/*
This function will print out various information about the operating system.
This information is:
    model name: ___
    cpu cores: #
    Linux version: ___
    MemTotal: ___
    Uptime: # days # hours, # minutes 43 seconds
*/
void print_full_info() {
    //Defining an array to store and print information
    char sentance[256];

    //Accessing cpuinfo file for desired information and printing it
    FILE *file = fopen("/proc/cpuinfo","r"); //This file holds model name and cpu cores info
    while(fgets(sentance,256, file) != NULL){
            if (strncmp(sentance,"model name", 10) ==0){ //10 is length of "model name"
                printf("%s", sentance);
                break;
            }
        }
    while(fgets(sentance,256, file) != NULL){
            if (strncmp(sentance,"cpu cores", 9) ==0){ //9 is length of "cpu cores"
                printf("%s", sentance);
                break;
            }
        }
    fclose(file);
    
    //Accessing file with linux version information
    file = fopen("/proc/version", "r"); 
    fgets(sentance,256, file);
    printf("%s", sentance);
    fclose(file);
    
    //Accessing file with total memeory information
    file = fopen("/proc/meminfo", "r"); 
    fgets(sentance,256, file);
    printf("%s", sentance);
    fclose(file);
    
    //Accessing file with uptime information
    int uptime;
    file = fopen("/proc/uptime", "r");
    fscanf(file, "%d", &uptime);
    printf("Uptime: %d days, %d hours, %d minutes, %d seconds \n", uptime/86400, (uptime%86400)/3600, ((uptime%86400)%3600)/60, ((uptime%86400)%3600)%60);
    //86400 is number of seconds in a day so uptime/86400 is the days
    //Then uptime % 86400 would give remainder value of seconds left after days are taken out and then /3600 (hours in seconds) we get the hours
    //Apply that idea of for all time values given above
    fclose(file);
    return;
}

/*
This main function check if an argument was given when running the command or not.

If an argument is given it should be a process numer and in that case the print_process_info function runs.

Otherwise general operating system information is printed through print_full_info.
*/
int main(int argc, char ** argv) {  
    if (argc == 1) {
        print_full_info();
    } else {
        print_process_info(argv[1]);
    }
    return 0;
}
