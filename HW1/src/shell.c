#include <stdlib.h>
#include<stdio.h>
#include <string.h>
#include <unistd.h>

#include <sys/wait.h>
#include <errno.h>
#include "utils.h"
#include "container.h"
#include "utils.h"



#define MAX_PIPE_SIZE 12
#define CMD_QUIT "quit"
#define CMD_PRINTENV "printenv"
#define CMD_SETENV "setenv"

void closePipe(int pipe[2]){
    close(pipe[0]);
    close(pipe[1]);
}
int main(void){
    
    printf("--------- SHELL STARTED ---------\n");
    const char *quitCommand = CMD_QUIT;
    const char *printenvCommand = CMD_PRINTENV;
    const char *setenvCommand = CMD_SETENV;


    int listSize = 0;
    char ** executableList = createStringArray(BINARY_LIST_SIZE, BUFFER_SIZE);
    char ** variables = createStringArray(VARIABLE_HASHMAP_SIZE, BUFFER_SIZE);

    setEnv((char**)variables, "PATH", "bin:.");

    char **binaryFullPath = createStringArray(BINARY_LIST_SIZE, BUFFER_SIZE);
    refreshExecutableList(executableList, &listSize, variables, binaryFullPath);


    int numberedPipeHeapCounter = -1;
    int numberedPipe[2];
    while(1){


        
        setbuf(stdout, NULL);

        // start of the prompt
        printf("%% ");
        fflush(stdout);


        // ********* PARSE THE PROMPT ********* //
        char* buffer = malloc(BUFFER_SIZE * sizeof(char));
        size_t bufferSize = BUFFER_SIZE;

        int getlineSize = getline(&buffer, &bufferSize, stdin);

        // EOF
        if(getlineSize == -1){
            break;
        }

        // only get new line character
        if(getlineSize == 1){
            continue;
        }

        // remove new line character
        buffer[getlineSize-1] = '\0';
        getlineSize--;

        // QUIT the program
        // if the compare result is 0 then is equal
        if(strcmp(quitCommand, buffer) == 0){
            break;
            exit(EXIT_SUCCESS);
        }


        int pipeListSize = 0;
        char **pipeList = createStringArray(ARR_SIZE, BUFFER_SIZE);

        // parse the full command line to differnet pipe strings
        char *pipeStr ;
        while((pipeStr = strstr(buffer, " | ")) != NULL){

            strncpy(pipeList[pipeListSize], buffer, bufferSize);
            pipeList[pipeListSize++][pipeStr-buffer] = '\0';

            strcpy(buffer, pipeStr+3);

        }
        strncpy(pipeList[pipeListSize++], buffer, bufferSize);
        // ********* PARSE THE PROMPT ********* //


        // Create the pipes to redirect STDIN & STDOUT
        int pipes[MAX_PIPE_SIZE][2];
        for(int j = 0; j != pipeListSize-1; j++){
            pipe(pipes[j]); 
        }
        

        int finishProcessCounter = 0;
        
        int skipPipeCounter = 0;
        for(int j = 0; j != pipeListSize; j++){
 

            // ********* PARSE THE COMMAND OUT ********* //
            strcpy(buffer, pipeList[j]);
            char *commandRoot  = strtok(buffer, " ");

            char* fullname = "";
            int commandNotFound = 1;
            for(int i = 0; i != listSize; i++){
                if(!strcmp(commandRoot, executableList[i])){
                    commandNotFound = 0;
                    fullname = binaryFullPath[i];
                    break;
                }
            }

            if(commandNotFound){
                printf("Unknown command: [%s].\n", commandRoot);
                skipPipeCounter = 1;
                
            }


            // Args for exec commands
            char **execArgs = createStringArray(ARR_SIZE, BUFFER_SIZE);
            strcpy(execArgs[0], commandRoot);


            // extract command with parameter to list of tokens 
            int ArgsSize = 1;
            char *tkn = strtok(NULL, " ");
            while(tkn != NULL){

                strcpy ( execArgs[ArgsSize++], tkn);
                tkn = strtok(NULL, " ");
            }
            // list end with null
            execArgs[ArgsSize]= NULL;
            
            // ********* PARSE THE COMMAND OUT ********* //

            // ********* CHECK FOR SPECIAL COMMAND & SYNTAX ********* //

            // check if numbered pipe syntax exists in the command line
            int numberedPipeActive = 0;
            if( sscanf(execArgs[ArgsSize-1],"|%d", &numberedPipeHeapCounter) ){

                execArgs[ArgsSize-1]= NULL;
                numberedPipeActive = 1;

                pipe(pipes[j]);
                numberedPipe[0] = pipes[j][0];
                numberedPipe[1] = pipes[j][1];
            }

            // print ENV
            if(strcmp(printenvCommand, commandRoot) == 0){
                
                if(ArgsSize >= 2){

                    printf("\t%s\n", getEnv(variables, execArgs[1]));
                }
                break;
            }
            // set ENV
            if(strcmp(setenvCommand, commandRoot) == 0){
                if(ArgsSize >= 3){

                    setEnv((char**)variables, execArgs[1], execArgs[2]);

                    // update our binary executable list due to the PATH is updated.
                    if(strcmp(execArgs[1], "PATH" ) == 0){
                        refreshExecutableList(executableList, &listSize, variables, binaryFullPath);
                    }
                }
                break;
            }
            // ********* CHECK FOR SPECIAL COMMAND & SYNTAX ********* //
                

            // ********* HANDLE PIPE & CREATE PROCESS ********* //
            switch(fork()){

                    case -1:
                        perror("FORK ERROR");
                        exit(EXIT_FAILURE);

                    case 0:{

                        // printf("\tcreate fork - [pid:%d] command: %d - %s%s%s\n",
                        //     getpid(), 
                        //     j, 
                        //     KBLU,pipeList[j],KNRM  );
                        
                        if(j != 0){
                            dup2(pipes[j-1][0], STDIN_FILENO);
                        }else{
                            if(numberedPipeHeapCounter == 0){

                                dup2(numberedPipe[0], STDIN_FILENO);
                                closePipe(numberedPipe);
                            }
                        }

                        if(j != pipeListSize-1 || numberedPipeActive){
                            dup2(pipes[j][1],STDOUT_FILENO);
                        }
                        

                        for(int g = 0; g != pipeListSize-1; g++){
                            close(pipes[g][0]);
                            close(pipes[g][1]);
                        }

                        
                        execv(fullname, execArgs);
                        exit(EXIT_SUCCESS);

                        } 

                    default:
                        break;
            }
            numberedPipeActive = 0;
            // ********* HANDLE PIPE & CREATE PROCESS ********* //
        }



        // ********* CLOSE PIPES & WAIT SUBPROCESS ********* //
        if(numberedPipeHeapCounter == 0) closePipe(numberedPipe);
        for(int g = 0; g != pipeListSize-1; g++){
            close(pipes[g][STDOUT_FILENO]);
        }
        

        while(1){
            int childPid = wait(NULL);
            if(childPid == -1){
                if(errno == ECHILD){
                    
                    // printf("\tFinish process all!\n\n");

                    pipeListSize=0;
                    break;
                }else{
                    perror("wait\n");
                    exit(EXIT_FAILURE);
                }
            }

                    

            // printf("\t[pid:%d] command: %d - %s %sFINISHED%s\n", 
            //     getpid(), 
            //     pipeListSize-deadChProcess-1, 
            //     pipeList[pipeListSize-deadChProcess-1],BGRN, KNRM);
            finishProcessCounter++;
            
        }
        if(skipPipeCounter) continue;
        numberedPipeHeapCounter--;
        // ********* CLOSE PIPES & WAIT SUBPROCESS ********* //


        // end of the response
        // printf("--------------\n");
    }
}