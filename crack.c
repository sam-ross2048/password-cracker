#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sha256.h"

#define NUM_FOUR_LETTER 10


void readPasswordFromFile(char* dump, FILE* fp){
    unsigned char* buff = (unsigned char*)malloc(sizeof(char)*33);
    fread(buff, 32, 1, fp);
    for(int i=0;i<32;i++){
        sprintf(dump+(i*2), "%02x", buff[i]);
    }
}

void readPassword(char* dump, SHA256_CTX* ctx){
    //unsigned char* buff = (unsigned char*)malloc(sizeof(char*33);
    for(int i=0;i<32;i++){
        sprintf(dump+(i*2), "%02x", ctx->data[i]);
    }
}

void readFourLetterPasswords(char** passwords){
    FILE* fp;
    fp = fopen("pwd4sha256", "r");
    for(int i=0;i<NUM_FOUR_LETTER;i++){
        char dump[65];
        readPasswordFromFile(dump, fp);
        passwords[i] = (char*)malloc(sizeof(char)*65);
        strcpy(passwords[i], dump);
    }
}


void guessPasswords(char** passwords){
    char* guess = "samr";
    SHA256_CTX* data = (SHA256_CTX*)malloc(sizeof(SHA256_CTX));
    sha256_init(data);
    sha256_update(data, guess, strlen(guess));
    for(int i=0;i<NUM_FOUR_LETTER;i++){
        char* dump;
        readPassword(dump, data);
        printf("%s\n", dump);
        printf("%s\n\n", passwords[i]);
        if(strcmp(passwords[i], dump)==0){
            printf("MATCH!\n");
        }
    }

}

int main(int argc, char* argv[]){

    char* passwords[NUM_FOUR_LETTER];
    readFourLetterPasswords(passwords);
    guessPasswords(passwords);
    return 0;
}
