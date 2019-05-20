
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
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


void guess(char** fourLetter, char* guess, int length){
    char* hashedGuess = sha256S(guess);
	int number;
	int offset=0;
	if(length==4){
		 number = NUM_FOUR_LETTER;
	 }
	else {
		number = NUM_SIX_LETTER;
		offset = NUM_FOUR_LETTER;
	}

	for(int i=0;i<number;i++){
        if(strcmp(fourLetter[i], hashedGuess)==0){
            printf("%s %d   %s\n", guess, i+offset, hashedGuess);
        }
    }
}


void recurBrute(char* buff, char** passwords, int index, int depth, int length){
	for(int i=0;i<94;++i){
		buff[index] = (char)(i+32);
		if(index==depth-1){
			guess(passwords, buff, length);
		}
		else{
			recurBrute(buff, passwords, index+1, depth, length);
		}
	}
}

void bruteForce(char** passwords, int length){
	char* buff = (char*)malloc(sizeof(char)*(length+1));
	for(int i=1;i<=length;++i){
		memset(buff, 32, length);
		recurBrute(buff, passwords, 0, i, length);
	}
	free(buff);
}


char* zeroPad(int number, int numDigits){
    char* guess = (char*)malloc(sizeof(char)*(numDigits+1));
    sprintf(guess, "%04d", number);
    return guess;
}


void guessNumbers(char** passwords, int numDigits){
    int maxValue = pow(10, numDigits);
    char* word = (char*)malloc(sizeof(char)*(numDigits+1));
    for(int i=0;i<maxValue;i++){
        word = zeroPad(i, numDigits);
        guess(passwords, word, numDigits);
    }
}


void changeLetter(char letter, char replacement, char* word, char** passwords, int length){
	char* copy = (char*)malloc(sizeof(char)*(length+1));
	strcpy(copy, word);
	char* position = strstr(copy, &letter);
	if(position){
		position[0] = replacement;
		guess(passwords, copy, length);
	}
	free(copy);
}


void alphabetToDigit(char* word, char** passwords, int length){
	changeLetter('i', '1', word, passwords, length);
	changeLetter('l', '1', word, passwords, length);
	changeLetter('a', '4', word, passwords, length);
	changeLetter('o', '0', word, passwords, length);
	changeLetter('I', '1', word, passwords, length);
	changeLetter('A', '4', word, passwords, length);
	changeLetter('O', '0', word, passwords, length);
	changeLetter('L', '1', word, passwords, length);
}



void upperCaseGuess(char* word, char** passwords, int length){
	char* copy = (char*)malloc(sizeof(char)*(length+1));
	strcpy(copy, word);
	if(copy[0]>='a' && copy[0]<='z'){
		copy[0] = copy[0] - 32;
	}
	guess(passwords, copy, length);
	free(copy);
}

bool readFilePassword(FILE* fp, char* word, int length){
	int i=0;
	bool end = false;
	for(char c=getc(fp);c!='\n'||i==length-1;c=getc(fp)){
		word[i] = c;
		i++;
		if(c==EOF){
			end = true;
			break;
		}
	}
	word[i] = '\0';
	return end;
}

void checkFilePasswords(char* filename, char** passwords, int length){
	char word[length+1];
	FILE *fp;
	fp = fopen(filename, "r");

	while(readFilePassword(fp, word, length)!=true){
		word[length] = '\0';
		guess(passwords, word, length);
		upperCaseGuess(word, passwords, length);
		alphabetToDigit(word, passwords, length);
	}

}

int main(int argc, char* argv[]){

    char* fourLetter[findNumberPasswords(FOUR_LETTER_FILE)];
    char* sixLetter[findNumberPasswords(SIX_LETTER_FILE)];
    readPasswords(fourLetter, FOUR_LETTER_FILE);
    readPasswords(sixLetter, SIX_LETTER_FILE);
    guessNumbers(fourLetter, 4);
    guessNumbers(sixLetter, 6);
	checkFilePasswords("common_passwords.txt", fourLetter, 4);
	checkFilePasswords("common_passwords.txt", sixLetter, 6);
	bruteForce(fourLetter, 4);
    //guessPasswords(sixLetter);
    return 0;
}
