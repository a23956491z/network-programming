
#include <dirent.h> 
#include <sys/stat.h>
#include "container.h"
#include "utils.h"


void readFolder(const char * folder, char ** fileList, int * listSize, char** folderList){

    DIR *d;
    struct dirent *dir;
    d = opendir(folder);

    char fullName[BUFFER_SIZE];
    if (d) {
        while ((dir = readdir(d)) != NULL) {

            
            // if the target is directory
            if(dir->d_type == DT_DIR){
                continue;
            }

            // if the target is executable
            struct stat statFile;


            strcpy(fullName,folder);
            strcat(fullName,"/");
            strcat(fullName, dir->d_name);
            if(stat(fullName, &statFile) == 0 && statFile.st_mode & S_IXUSR){
                
                printf("\tFound executable : %s\n", dir->d_name);

                strcpy(fileList[*listSize], dir->d_name);
                strcpy(folderList[*listSize], fullName);
                (*listSize) ++;
            }

        }
    closedir(d);
}

}



void refreshExecutableList(char** executables, int *listSize, char** variables, char** binaryFullPath){
    char paths[BUFFER_SIZE];

    strcpy(paths, getEnv(variables, "PATH"));

    (*listSize) = 0;



    char *tkn = strtok(paths, ":");
    while(tkn != NULL){

        printf("path: %s\n", tkn);

        readFolder(tkn, executables, listSize, binaryFullPath);
        tkn = strtok(NULL, ":");
    }

    strcpy(executables[ (*listSize)++],CMD_PRINTENV);
    strcpy(executables[ (*listSize)++],CMD_SETENV);

    printf("--------- PATH SCANNED ---------\n");
}

char ** createStringArray(size_t arraySize, size_t bufferSize){
        char **arr = (char**)malloc(arraySize * sizeof(char*));
        for(int i = 0; i != arraySize; i++){
        arr[i] = (char*) malloc(bufferSize * sizeof(char));
    }

    return arr;
}