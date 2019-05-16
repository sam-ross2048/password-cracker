#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sha256.h"

#define NUM_HASHES 30
#define HASH_LENGTH 32
#define HASH_SIZE 65

#define FOUR_LETTER_FILE "pwd4sha256"
#define SIX_LETTER_FILE "pwd6sha256"

#define NUM_FOUR_LETTER 10
#define NUM_SIX_LETTER 20

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
    unsigned char* buff = (unsigned char*)malloc(sizeof(char)*(HASH_LENGTH+1));
    fread(buff, HASH_LENGTH, 1, fp);
    for(int i=0;i<HASH_LENGTH;i++){
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


void readPasswords(char* passwords[], char* filename){
    FILE* fp;
    fp = fopen(filename, "r");

    int numPasswords = findNumberPasswords(filename);
    for(int i=0;i<numPasswords;i++){
        char dump[HASH_SIZE];
        readSinglePassword(dump, fp);
        passwords[i] = (char*)malloc(sizeof(char)*HASH_SIZE);
        strcpy(passwords[i], dump);
    }
}


void guessFourLetter(char** fourLetter, char* guess){
    char* hashedGuess = sha256S(guess);
    for(int i=0;i<NUM_FOUR_LETTER;i++){
        if(strcmp(fourLetter[i], hashedGuess)==0){
            printf("%s %d\n", guess, i);
        }
    }
}


void guessSixLetter(char** sixLetter, char* guess){
    char* hashedGuess = sha256S(guess);
    for(int i=0;i<NUM_SIX_LETTER;i++){
        if(strcmp(sixLetter[i], hashedGuess)==0){
            printf("%s %d\n", guess, i+NUM_FOUR_LETTER);
        }
    }
}

void zeroPad(char* guess){
    for(int i=0;i<strlen(guess);i++){
        if(guess[i] == ' ') guess[i] = '0';
    }
}


void guessNumbers(char** passwords, int numDigits){
    int maxValue = pow(10, numDigits);
    char* guess = (char*)malloc(sizeof(char)*maxValue);
    for(int i=0;i<maxValue;i++){
        guess = itoa(i);
        if(numDigits==4){
            sprintf("%04s", guess);
            zeroPad(guess);
            printf("%s\n", guess);
            guessFourLetter(passwords, guess);
        }
        else{
            sprintf("%06s", guess);
            zeroPad(guess);
            guessSixLetter(passwords, guess);
        }
    }
}


int main(int argc, char* argv[]){

    char* fourLetter[findNumberPasswords(FOUR_LETTER_FILE)];
    char* sixLetter[findNumberPasswords(SIX_LETTER_FILE)];
    readPasswords(fourLetter, FOUR_LETTER_FILE);
    readPasswords(sixLetter, SIX_LETTER_FILE);
    guessNumbers(fourLetter, 4);
    printf("\n\n");
    //guessPasswords(sixLetter);
    return 0;
}
