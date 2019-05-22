
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <math.h>
#include <unistd.h>
#include <fcntl.h>
#include "sha256.h"

#define NUM_HASHES 30
#define HASH_LENGTH 32
#define HASH_SIZE 65
#define MAX_PASSWORD_LEN 10000

#define FOUR_LETTER_FILE "pwd4sha256"
#define SIX_LETTER_FILE "pwd6sha256"
#define COMMON_FILE "common_passwords.txt"
#define BRUTE_FILE "bruteGenerated.txt"

#define ALPHABET_LENGTH 94
#define ALPHABET_OFFSET 32

#define NUM_FOUR_LETTER 10
#define NUM_SIX_LETTER 20

void findAllEdits(char** passwords, char* word, int alphaLen, int alphaOffset, bool stop);
void guess(char** passwords, char* guess, int length, int numPasswords);

void readSinglePassword(char* dump, FILE* fp){
    unsigned char* buff = (unsigned char*)malloc(sizeof(char)*(HASH_LENGTH+1));
    fread(buff, HASH_LENGTH, 1, fp);
    for(int i=0;i<HASH_LENGTH;i++){
        sprintf(dump+(i*2), "%02x", buff[i]);
    }
	free(buff);
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


// Function finds all words 1 edit-distance away from a word.
/*void findAllEdits(char** passwords, char* word, int alphaLen, int alphaOffset, bool stop){
	if(stop){
		return;
	}
	char alphabet[] = "abcdefghijklmnopqrstuvwxyz\0";
	int n = strlen(word);
	int i, j;
	char* newWord = (char*)malloc(sizeof(char)*(n+1));
	strcpy(newWord, word);

	// Finds all substitutions and adds to edits.
	for(i=0;i<n;i++){
		for(j=0;j<alphaLen;j++){
			newWord = (char*)malloc(sizeof(char)*(n+1));
			strcpy(newWord, word);
			newWord[i] = (char)(i+alphaOffset);
			guess(passwords, newWord, n, true);
			free(newWord);
		}
	}

	// Finds all insertions and adds to edits.
	int z;
	for(i=0;i<=n;i++){
		for(j=0;j<alphaLen;j++){
			newWord = (char*)malloc(sizeof(char)*(n+2));
			strcpy(newWord, word);
			for(z=n+1;z>i;z--){
				newWord[z] = newWord[z-1];
			}
			newWord[i] = (char)(i+alphaOffset);//alphabet[j];
			guess(passwords, newWord, n, true);
		}
	}
}*/


void guess(char** passwords, char* guess, int length, int numPasswords){
    char* hashedGuess = sha256S(guess);
	int offset=0;
	if(length==6){
		offset = NUM_FOUR_LETTER-1;
	}
	for(int i=0;i<numPasswords;i++){
        if(strcmp(passwords[i], hashedGuess)==0){
            printf("%s %d\n", guess, i+1+offset);
        }
    }
	free(hashedGuess);
}


/* Code taken from https://github.com/Jsdemonsim/Stackoverflow/blob/master/alphabet/alphabet.c
Generates brute force guesses and inserts into text file. */
static void bruteForce(int maxlen, int alphaLen, int alphaOffset){
    int   len      = 0;
    char *buffer   = malloc((maxlen + 1) * alphaLen * alphaLen);
    int  *letters  = malloc(maxlen * sizeof(int));

	//int fd = open("bruteGenerated.txt", O_CREATE | O_RDWR);
	FILE *fp = fopen("bruteGenerated.txt", "w");
    if (buffer == NULL || letters == NULL) {
		fprintf(stderr, "Not enough memory.\n");
		exit(1);
    }

    // This for loop generates all 1 letter patterns, then 2 letters, etc,
    // up to the given maxlen.
    for (len=maxlen;len<=maxlen;len++) {
		// The stride is one larger than len because each line has a '\n'.
		int i;
		int stride = len+1;
		int bufLen = stride * alphaLen * alphaLen;

		if (len == 1) {
		    // Special case.  The main algorithm hardcodes the last two
		    // letters, so this case needs to be handled separately.
		    int j = 0;
		    bufLen = (len + 1) * alphaLen;
		    for (i=0;i<alphaLen;i++) {
				buffer[j++] = (char)(i+alphaOffset);
				buffer[j++] = '\n';
		    }
		    //write(fd, buffer, bufLen);
			fprintf(fp, "%s", buffer);
		    continue;
		}

		// Initialize buffer to contain all first letters.
		memset(buffer, (char)(alphaOffset), bufLen);

		// Now write all the last 2 letters and newlines, which
		// will after this not change during the main algorithm.
		{
		    // Let0 is the 2nd to last letter.  Let1 is the last letter.
		    int let0 = 0;
		    int let1 = 0;
		    for (i=len-2;i<bufLen;i+=stride) {
			buffer[i]   = (char)(let0 + alphaOffset);
			buffer[i+1] = (char)(let1 + alphaOffset);
			let1++;
			buffer[i+2] = '\n';
			if (let1 == alphaLen) {
			    let1 = 0;
			    let0++;
			    if (let0 == alphaLen)
				let0 = 0;
			}
		    }
		}

		// Write the first sequence out.
		//write(fd, buffer, bufLen);
		fprintf(fp, "%s", buffer);

		// Special case for length 2, we're already done.
		if (len == 2){
		    continue;
		}

		// Set all the letters to 0.
		for (i=0;i<len;i++){
		    letters[i] = 0;
		}

		// Now on each iteration, increment the the third to last letter.
		i = len-3;
		do {
		    char c;
		    int  j;

		    // Increment this letter.
		    letters[i]++;

		    // Handle wraparound.
		    if (letters[i] >= alphaLen)
			letters[i] = 0;

		    // Set this letter in the proper places in the buffer.
		    c = (char)(letters[i]+alphaOffset);
		    for (j=i;j<bufLen;j+=stride)
			buffer[j] = c;

		    if (letters[i] != 0) {
			// No wraparound, so we finally finished incrementing.
			// Write out this set.  Reset i back to third to last letter.
			//write(fd, buffer, bufLen);
			fprintf(fp, "%s", buffer);
			i = len - 3;
			continue;
		    }

		    // The letter wrapped around ("carried").  Set up to increment
		    // the next letter on the left.
		    i--;
		    // If we carried past last letter, we're done with this
		    // whole length.
		    if (i < 0)
			break;
		} while(1);
    }

    // Clean up.
    free(letters);
    free(buffer);
	fclose(fp);

}


char* zeroPad(int number, int numDigits){
    char* guess = (char*)malloc(sizeof(char)*(numDigits+1));
	if(numDigits==4){
		sprintf(guess, "%04d", number);
	}
	else{
		sprintf(guess, "%06d", number);
	}
    return guess;
}


void guessNumbers(char** passwords, int numDigits, int numPasswords){
    int maxValue = pow(10, numDigits);
    char* word = (char*)malloc(sizeof(char)*(numDigits+1));
    for(int i=0;i<maxValue;i++){
        word = zeroPad(i, numDigits);
        guess(passwords, word, numDigits, numPasswords);
		free(word);
    }
}


void changeLetter(char letter, char replacement, char* word, char** passwords, int length, int numPasswords){
	char* copy = (char*)malloc(sizeof(char)*(length+1));
	strcpy(copy, word);
	char* position = strstr(copy, &letter);
	if(position){
		position[0] = replacement;
		guess(passwords, copy, length, numPasswords);
	}
	free(copy);
}


void alphabetToDigit(char* word, char** passwords, int length, int numPasswords){
	changeLetter('i', '1', word, passwords, length, numPasswords);
	changeLetter('l', '1', word, passwords, length, numPasswords);
	changeLetter('a', '4', word, passwords, length, numPasswords);
	changeLetter('o', '0', word, passwords, length, numPasswords);
	changeLetter('I', '1', word, passwords, length, numPasswords);
	changeLetter('A', '4', word, passwords, length, numPasswords);
	changeLetter('O', '0', word, passwords, length, numPasswords);
	changeLetter('L', '1', word, passwords, length, numPasswords);
	changeLetter('s', '$', word, passwords, length, numPasswords);
	changeLetter('S', '$', word, passwords, length, numPasswords);
}



void upperCaseGuess(char* word, char** passwords, int length, int numPasswords){
	char* copy = (char*)malloc(sizeof(char)*(length+1));
	strcpy(copy, word);
	if(copy[0]>='a' && copy[0]<='z'){
		copy[0] = copy[0] - 32;
	}
	guess(passwords, copy, length, numPasswords);
	free(copy);
}


void guessArg2(char** hashes, char* password, int numHashes){
	char* hashedGuess = sha256S(password);

	for(int i=0;i<numHashes;i++){
        if(strcmp(hashes[i], hashedGuess)==0){
            printf("%s %d\n", password, i);
        }
    }
	free(hashedGuess);
}


bool readBigPasswords(FILE*fp, char* word){
	int i=0;
	bool end = false;

	for(char c=getc(fp);c!='\n';c=getc(fp)){
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


void checkHashesAgainstFile(char* filename, char** hashes, int numHashes){
	char word[MAX_PASSWORD_LEN+1];
	FILE* fp = fopen(filename, "r");
	while(readBigPasswords(fp, word)!=true){
		guess(hashes, word, MAX_PASSWORD_LEN, numHashes);
	}
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
	for(i;i<length;i++){
		word[i] = ' ';
	}
	word[strcspn(word, "\r\n")] = '\0';
	return end;
}


void checkFilePasswords(char* filename, char** passwords, int length, int numPasswords){
	char word[length+1];
	FILE *fp;
	fp = fopen(filename, "r");

	while(readFilePassword(fp, word, length)!=true){
		word[length] = '\0';
		guess(passwords, word, length, numPasswords);
		//upperCaseGuess(word, passwords, length, numPasswords);
		//alphabetToDigit(word, passwords, length, numPasswords);
	}
	fclose(fp);
}


void fileGeneration(char* filename, int length, int maxGuesses, int* numGuesses){
	char word[length+1];
	FILE* fp = fopen(filename, "r");
	while(readFilePassword(fp, word, length)!=true && *numGuesses < maxGuesses){
		word[length] = '\0';
		printf("%s\n", word);
		*numGuesses+=1;
	}
	fclose(fp);
}


void numberGeneration(int length, int maxGuesses, int* numGuesses){
	int maxValue = pow(10, length);
    char* word = (char*)malloc(sizeof(char)*(length+1));
    for(int i=0;i<maxValue && *numGuesses < maxGuesses;i++){
        word = zeroPad(i, length);
        printf("%s\n", word);
		free(word);
		*numGuesses+=1;
    }
}


void generateGuesses(int maxGuesses, int length){
	int* numGuesses = malloc(sizeof(int));
	fileGeneration(COMMON_FILE, length, maxGuesses, numGuesses);
	numberGeneration(length, maxGuesses, numGuesses); // IMPLEMENT THIS FUNCTION
	if(*numGuesses < maxGuesses){
		bruteForce(6, 25, 97);
		fileGeneration(BRUTE_FILE, length, maxGuesses, numGuesses);
	}
	free(numGuesses);
}


int main(int argc, char* argv[]){
	if(argc == 1){
		char* fourLetter[findNumberPasswords(FOUR_LETTER_FILE)];
	    char* sixLetter[findNumberPasswords(SIX_LETTER_FILE)];
	    readPasswords(fourLetter, FOUR_LETTER_FILE);
	    readPasswords(sixLetter, SIX_LETTER_FILE);
	    guessNumbers(fourLetter, 4, NUM_FOUR_LETTER);
	    guessNumbers(sixLetter, 6, NUM_SIX_LETTER);
		checkFilePasswords(COMMON_FILE, fourLetter, 4, NUM_FOUR_LETTER);
		checkFilePasswords(COMMON_FILE, sixLetter, 6, NUM_SIX_LETTER);
		bruteForce(4, ALPHABET_LENGTH, ALPHABET_OFFSET);
		checkFilePasswords(BRUTE_FILE, fourLetter, 4, NUM_FOUR_LETTER);
		bruteForce(6, 25, 97);
		checkFilePasswords(BRUTE_FILE, sixLetter, 6, NUM_SIX_LETTER);
	}

	else if(argc == 2){
		int maxGuesses = atoi(argv[1]);
		generateGuesses(maxGuesses, 6);
	}
	else if(argc == 3){
		char* passwordFile = argv[1];
		char* hashFile = argv[2];
		int numHashes = findNumberPasswords(hashFile);
		char* hashes[numHashes];
		readPasswords(hashes, hashFile);
		checkHashesAgainstFile(passwordFile, hashes, numHashes);
	}
    return 0;
}
