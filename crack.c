#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sha256.h"

#define NUM_HASHES 30
#define HASH_LENGTH 32

/* Function to convert bytes to hexadecimal string. Code written by Brad Conte
    found from same source code as sha256.c and sha256.h */
char *sha256_byteToHexString(BYTE data[]) {
	char *hexC = "0123456789abcdef";
	char *hexS = malloc(65);
	if (!hexS) return NULL;
	for(BYTE i; i<32; i++) {
		hexS[i*2]   = hexC[data[i]>>4];
		hexS[i*2+1] = hexC[data[i]&0xF];
	}
	hexS[64] = 0;
	return hexS;
}

/* Function to find the hashed version of a guessed password. Code written by
    Brad Conte, found from same source code as sha256.c and sha256.h */
char *sha256S(const char *bufferToHash) {
	if (!bufferToHash) return NULL;
	SHA256_CTX ctx;
	sha256_init(&ctx);
	sha256_update(&ctx, bufferToHash, strlen(bufferToHash));
	BYTE result[32];
	sha256_final(&ctx, result);
	char *hexS = sha256_byteToHexString(result);
	return hexS;
}


void readSinglePassword(char* dump, FILE* fp){
    unsigned char* buff = (unsigned char*)malloc(sizeof(char)*33);
    fread(buff, 32, 1, fp);
    for(int i=0;i<32;i++){
        sprintf(dump+(i*2), "%02x", buff[i]);
    }
}

int findNumberPasswords(char* fileName){
    FILE* fp = fopen(fileName, "r");
    fseek(fp, 0L, SEEK_END);
    long int res = ftell(fp);
    fclose(fp);
    return (int)res/HASH_LENGTH;
}


void readPasswords(char** passwords, char* filename){
    FILE* fp;
    fp = fopen(filename, "r");

    int numPasswords = findNumberPasswords(filename);
    passwords = (char**)malloc(sizeof(char*)*numPasswords);
    for(int i=0;i<numPasswords;i++){
        char dump[65];
        readSinglePassword(dump, fp);
        passwords[i] = (char*)malloc(sizeof(char)*65);
        strcpy(passwords[i], dump);
    }
}


void guessPasswords(char** passwords){
    char* guess = "samr";
    char* hashedGuess = sha256S(guess);
    for(int i=0;i<NUM_FOUR_LETTER;i++){
        printf("%s\n", passwords[i]);
        if(strcmp(passwords[i], hashedGuess)==0){
            printf("MATCH!\n");
        }
    }
}

int main(int argc, char* argv[]){

    char** fourLetterPasswords;
    readPasswords(passwords, "pwd4sha256");
    guessPasswords(passwords);
    return 0;
}
