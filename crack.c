#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sha256.h"

#define NUM_FOUR_LETTER 10

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


void readPasswordFromFile(char* dump, FILE* fp){
    unsigned char* buff = (unsigned char*)malloc(sizeof(char)*33);
    fread(buff, 32, 1, fp);
    for(int i=0;i<32;i++){
        sprintf(dump+(i*2), "%02x", buff[i]);
    }
}


void readFourLetterPasswords(char** passwords){
    FILE* fp;
    fp = fopen("pwd4sha256", "r");
    struct stat sb;
    stat("pwd4sha256", &sb);
    printf("FileSize: %d", sb.st_size);
    for(int i=0;i<NUM_FOUR_LETTER;i++){
        char dump[65];
        readPasswordFromFile(dump, fp);
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

    char* passwords[NUM_FOUR_LETTER];
    readFourLetterPasswords(passwords);
    guessPasswords(passwords);
    return 0;
}
