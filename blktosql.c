#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "sha256.h"
#include "functions.h"

void test_parse(){
    unsigned long long i,j;
    int ii;
    int iterations;
    int fd;
    POSITION pos = 0;
    unsigned char block_hash[32];
    unsigned char tx_hash[32];
    unsigned long long tx_counter;

    txinput *extractedinputs;
    txoutput *extractedoutputs;
    unsigned long long n_extractedinputs;
    unsigned long long n_extractedoutputs;

    const char *file_name = "../blk0-5.dat";
    if((fd = open(file_name, O_RDONLY)) == -1){
        perror(file_name);
        exit(1);
    }

    for(i=0; i<130000; i++){
        pos = NextBlockPosition(fd, pos);
    }
    for(iterations=0; iterations<10000; iterations++){
        //printf("*********************************************************************************\n");
        CalcBlockHash(fd, pos, block_hash);
        printf("Block Hash: ");
        PrintHash(block_hash);
        printf(", ");

        tx_counter = GetTxCounter(fd, pos);
        printf("tx_counter: %llu,", tx_counter);

        pos = GoToFirstTx(fd, pos);
        for(i=0;i<tx_counter;i++){
            CalcTxHash(fd, pos, tx_hash);
            /* DEBUG
            printf("tx: ");
            PrintHash(tx_hash);
            printf(", ");
            */
            n_extractedinputs = ExtractTxInputs(fd, pos, &extractedinputs);
            /* DEBUG

            for(j=0; j<n_extractedinputs; j++){
                printf("     ");
                printf("%u: " , (extractedinputs[j]).prev_tx_index);
                for(ii=0; ii<32; ii++){
                    printf("%02x", (extractedinputs[j]).prev_tx_hash[ii]);
                }
                printf(",");
            }

            */
            pos = NextTxPosition(fd, pos);
            free(extractedinputs);
        }
        printf("\n");
        pos = NextBlockPosition(fd, pos);
    }



    //printf("*********************************************************************************\n");
    close(fd);
}


int main(){
    getchar();
    test_parse();
    getchar();
}
