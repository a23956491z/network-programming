#pragma once

#define VARIABLE_HASHMAP_SIZE 1000

unsigned long hash(unsigned char *str);
void setEnv(char **map, char * key, char * value);
char* getEnv(char **map , char * key);