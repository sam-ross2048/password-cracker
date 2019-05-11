#include <stdio.h>
#include <stdlib.h>


void readFourLetterPasswords(char* dump){
    FILE* fp;
    fp = fopen("pwd4sha256", "r");
    const unsigned char* buff = (unsigned char*)malloc(sizeof(char)*33);
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
