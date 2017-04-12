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


    unsigned char buffer[4];
    unsigned char single_byte;
    unsigned char curr_hash[32];
    unsigned char prev_hash[32];
    unsigned char header[80];
    const char *file_name = "../blk00000_c.dat";
    if((fd = open(file_name, O_RDONLY)) == -1){
        perror(file_name);
        exit(1);
    }

    pos = 0;
    read(fd, buffer, 4);
    if(CheckMagicNo(buffer)){
        printf("MAGICNO OK!\n");
    }
    unsigned long long txcount;
    txcount = VarIntToLong(fd, pos+88);
    printf("%llu\n", txcount);


    //REACHING THE TX NUMBER 496, first with multiple inputs
    /*
    for(i=0; i<100000; i++){
        pos = NextBlockPosition(fd, pos);
    }

    CalcBlockHash(fd, pos, curr_hash);
    PrintHash(curr_hash);
    printf("\n");

    lseek(fd, pos, SEEK_SET);
    printf("pos: %i\n",pos);
    read(fd, buffer, 4);
    if(CheckMagicNo(buffer)){
        printf("MAGICNO OK!\n");
    }
    lseek(fd, 84, SEEK_CUR);
    unsigned long int txcount;

    txcount = VarIntToLong(fd, pos+88);

    printf("%li\n", txcount);

    */
    /*
    for(i=0; i<496; i++){
        CalcBlockHash(fd, pos, curr_hash);
        printf("%02i: ",i);
        PrintHash(curr_hash);
        printf(" -> ");
        pos = NextBlockPosition(fd, pos);
        GetHashPreviousBlock(fd, pos, prev_hash);
        PrintHash(prev_hash);
        printf("\n");
    }
    */
    /*
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
    */




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
