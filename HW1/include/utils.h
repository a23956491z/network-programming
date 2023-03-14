#define KNRM  "\x1B[0m"
#define BRED  "\x1B[31;1m"
#define BGRN  "\x1B[32;1m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

#define BUFFER_SIZE 256
#define ARR_SIZE 256
#define BINARY_LIST_SIZE 32786

void readFolder(const char * folder, char ** fileList, int * listSize, char** folderList);
void refreshExecutableList(char** executables, int *listSize, char** variables, char** binaryFullPath);
char ** createStringArray(size_t arraySize, size_t bufferSize);