#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "sha256.h"
#include "functions.h"

void test_parse(){
    int i,j;
    int fd;
    txinput *debug;

    txinput *txinput_array;
    unsigned long long txinput_dim;
    unsigned char da_hash[32];
    for(i=0;i<32;i++){
        da_hash[i] = 0;
    }
    //unsigned char buffer[4];
    POSITION pos;

    unsigned long long tx_counter;

    const char *file_name = "../blk0-2.dat";
    if((fd = open(file_name, O_RDONLY)) == -1){
        perror(file_name);
        exit(1);
    }

    pos = 0;

    //for(i=0; i<136913; i++){
    for(i=0; i<136913; i++){
        pos = NextBlockPosition(fd, pos);
    }
    CalcBlockHash(fd, pos, da_hash);
    PrintHash(da_hash);
    printf("\n");

    tx_counter = GetTxCounter(fd, pos);
    printf("tx_counter: %llu\n", tx_counter);
    //pos = NextBlockPosition(fd, pos);

    pos = GoToFirstTx(fd, pos);

/*
    CalcTxHash(fd, pos, da_hash);
    printf("tx 0: ");
    PrintHash(da_hash);
    printf("\n");
*/
    //printf("\n");

    /* // IMPORTANTE::::::::
    for(i=0; i<tx_counter; i++){
        CalcTxHash(fd, pos, da_hash);
        printf("tx %02i: ", i);
        PrintHash(da_hash);
        printf("\n");
        //PrintTxInputs(fd, pos);
        txinput_dim = ExtractTxInputs(fd, pos, &txinput_array);
        printf("        ***    txinput_dim: %llu\n", txinput_dim);
        //free(txinput_array); FUNZIONA!!!!!

        for(j=0;j<txinput_dim;j++){
            printf("    ");
            PrintHash(txinput_array[j].prev_tx_hash);
            printf(" ---> %u ", txinput_array[j].prev_tx_index);
            //printf("   ------>  %02x", (txinput_array[j]).prev_tx_hash[0]);
            printf("\n");
        }
        pos = NextTxPosition(fd, pos);
    }
    */

    for(i=0; i<tx_counter; i++){
        CalcTxHash(fd, pos, da_hash);
        printf("    tx %02i: ", i);
        PrintHash(da_hash);
        printf("\n");
        //PrintTxInputs(fd, pos);
        PrintTxOutputs(fd,pos);
        pos = NextTxPosition(fd, pos);
    }





    close(fd);
}


int main(){
    test_parse();
}
