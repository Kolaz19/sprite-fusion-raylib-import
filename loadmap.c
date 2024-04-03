#include <stdio.h>
#include "tilemapSF.h" 
#include "include/cJSON.h" 

static int readFromFile(char* buf, char* filename, int buflen) {
    FILE* file = fopen(filename,"r");
    if (file == NULL) {
	return 0;
    }
    //Read 1 byte BUF_LEN times or until EOF
    int bytesRead = fread(buf, 1, buflen, file);
    if (bytesRead == EOF) {
	fclose(file);
	return 0;
    }
    fclose(file);
    return 1;
}
