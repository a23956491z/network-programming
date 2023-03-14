#include "container.h"
#include <stdlib.h>
#include <string.h>

unsigned long hash(unsigned char *str)
{
    unsigned long hash = 5381;
    int c;

    while (c = *str++)
        hash = ((hash << 5) + hash) + c; /* hash * 33 + c */

    return hash;
}

void setEnv(char **map, char * key, char * value){


    unsigned long idx = hash((unsigned char * )key) % VARIABLE_HASHMAP_SIZE;

    strcpy(map[idx], value);

}

char* getEnv(char **map , char * key){

    unsigned long idx = hash((unsigned char * )key) % VARIABLE_HASHMAP_SIZE;
    return map[idx];
}