#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define NUM_FOUR_LETTER 10


void readFourLetterPasswords(char* passwords[]){
    FILE* fp;
    fp = fopen("pwd4sha256", "r");

    for(int i=0;i<NUM_FOUR_LETTER;i++){
        unsigned char* buff = (unsigned char*)malloc(sizeof(char)*33);
        char dump[65];
        fread(buff, 32, 1, fp);
        for(int j=0;j<32;j++){
            sprintf(dump+(j*2), "%02x", buff[j]);
        }
        strcpy(passwords[i], dump);
        fp+=32;
    }


}


int main(int argc, char* argv[]){

    char* hashedPasswords[10];
    readFourLetterPasswords(hashedPasswords);
    for(int i=0;i<NUM_FOUR_LETTER;i++){
        printf("%s\n", hashedPasswords[i]);
    }
    return 0;
}
