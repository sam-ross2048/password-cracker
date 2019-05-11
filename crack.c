#include <stdio.h>
#include <stdlib.h>

#define NUM_FOUR_LETTER 10


void readFourLetterPasswords(char* dump){
    FILE* fp;
    fp = fopen("pwd4sha256", "r");
    unsigned char* buff = (unsigned char*)malloc(sizeof(char)*33);
    fread(buff, 32, 1 , fp);
    for(int i=0;i<32;i++){
        sprintf(dump+(i*2), "%02x", buff[i]);
    }
}


int main(int argc, char* argv[]){

    char dump[65];
    readFourLetterPasswords(dump);
    printf("%s\n", dump);
    return 0;
}
