#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_FOUR_LETTER 10


void readPassword(char* dump, FILE* fp){
    unsigned char* buff = (unsigned char*)malloc(sizeof(char)*33);
    fread(buff, 32, 1, fp);
    for(int i=0;i<32;i++){
        sprintf(dump+(i*2), "%02x", buff[i]);
    }
}


void readFourLetterPasswords(char** passwords){
    FILE* fp;
    fp = fopen("pwd4sha256", "r");
    for(int i=0;i<NUM_FOUR_LETTER;i++){
        char dump[65];
        readPassword(dump, fp);
        passwords[i] = (char*)malloc(sizeof(char)*65);
        strcpy(passwords[i], dump);
        printf("%s\n", passwords[i]);
        //fp+=32;
    }
}


int main(int argc, char* argv[]){

    char* passwords[NUM_FOUR_LETTER];
    readFourLetterPasswords(passwords);
    //printf("%s\n", dump);
    return 0;
}
