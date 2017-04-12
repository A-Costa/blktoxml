#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "sha256.h"
#include "functions.h"

void test_parse(){
    int i;
    int fd;
    int pos;
    int blocksize;

    unsigned char hash_final[32];

    unsigned char buffer[4];
    unsigned char hash[32];
    unsigned char header[80];
    const char *file_name = "../blk00000.dat";
    if((fd = open(file_name, O_RDONLY)) == -1){
        perror(file_name);
        exit(1);
    }

    pos = 0;
    GetBlockHeader(fd, pos, header);
    PrintBlockHeader(header);

    CalcBlockHash(fd, pos, hash_final);
    for(i=0; i<32; i++){
        printf("%02x ", hash_final[i]);
    }
    printf("\n");

    pos = NextBlockPosition(fd, pos);
    GetBlockHeader(fd, pos, header);
    PrintBlockHeader(header);

    CalcBlockHash(fd, pos, hash_final);
    for(i=0; i<32; i++){
        printf("%02x ", hash_final[i]);
    }
    printf("\n");




    /*

    for(i=0; i<100; i++){
        printf("%i -> ", i);
        GetHashPreviousBlock(fd, pos, hash);
        pos = NextBlockPosition(fd, pos);
        for(int j=31;j>-1;j--){
            printf("%02X", hash[j]);
        }
        printf("\n");
    }

    lseek(fd, pos, SEEK_SET);


    sha256_init(&hasher);
    const unsigned char message = 'a';
    unsigned char hash_final[32];
    sha256_update(&hasher, &message, 1);
    sha256_final(&hasher, hash_final);
    for(i=0; i<32; i++){
        printf("%02x", hash_final[i]);
    }
    printf("\n");

    */

    close(fd);
}


int main(){
    test_parse();
}
