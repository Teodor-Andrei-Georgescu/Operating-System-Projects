/* gopipe.c
 *
 * CSC 360, Summer 2023
 *
 * Execute up to four instructions, piping the output of each into the
 * input of the next.
 *
 * Please change the following before submission:
 *
 * Author: Teodor Andrei Georgescu
 */


/* Note: The following are the **ONLY** header files you are
 * permitted to use for this assignment! */

#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <wait.h>

//defining max length constants as per assignment specifications
#define cmd_lmt 4
#define arg_lmt 8
#define char_lmt 80
int main() {
    
    //Initlaizing an array to hold the commands and a counter to hold the number of commands
    char cmd[cmd_lmt][char_lmt]; 
    int cmd_count = 0; 

    //A loop to read commands from stdin and store them to the array.
    for(int i =0; i < cmd_lmt; i++){ 
        ssize_t cmd_len = read(0, cmd[i], char_lmt-1); //Storing length of current command 
        if( cmd_len <= 1){
               break; // If length is 1 char or less then '\n' char is placed indicating an empty line so no command given
           }
        cmd[i][cmd_len-1] = '\0'; //If length is more than 1 we want to replace that '\n' at the end with a '\0'
        cmd_count++;
    }
    
    //The pipe array, 0 is input/read from end ,1 is output/write to end
    int fd[2];  
    int saved_in = 0; //Storing file descripter from which to read
    
    //This loop does the piping of the commands
    for(int i=0; i < cmd_count; i++){
        //Creating a new child process each loop and pipe between parent and that child
        pipe(fd);  
        pid_t pid =fork(); 
        
        //If we are in child process 
        if(pid == 0){
            dup2(saved_in,0); //Replacing reading from stdin to reading from file descriptor in "saved_in" (This is the file descriptor pointing to he last comand)
            
            //If it is the last command we want to write to stdout not the pipe 
            if( i < cmd_count-1){
                dup2(fd[1],1); //Otherwrise instead of writing to stdout we write to the input side of the pipe
            }
            
            //Dont need childs fd[0] (reading input) from pipe file descriptor since we will read from "saved_in"
            close(fd[0]); 
            
            //Initalizing an array each the command and its arguments
            //+1 ensures there is null character at the end of the cmd_arry if all arguments are filled -- this is needed for execv command
            char* cmd_args[arg_lmt+1] = {0};  

            //A loop to go through the command and store its acrugments for execuation 
            char* token =strtok(cmd[i]," ");
            for(int j = 0; j < arg_lmt && token != NULL; j++){ //looping until either 8 arguments including command or token = NULL (out of commands)
                cmd_args[j] = token; //storing command plus arguments in the array
                token = strtok(NULL, " "); 
            }
            
            //Executing the current command and its arugments
            //Output will be printed into pipe as per redications above
            execv(cmd_args[0], cmd_args);
        }

        //If we are in the parent process
        else{
            wait(NULL); //Wait until child process is done  
            close(fd[1]); //Dont need parents fd[1] (writing output) file descriptor since it only writes to the pipe
            saved_in = fd[0];//Pass current file descriptors output into "saved_in" to be read as input for next child/command
        }
        
    }
    
    return 0;
}
