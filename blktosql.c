#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "sha256.h"
#include "functions.h"

void test_parse(){
    unsigned long long i;
    int j;
    int iterations;
    int fd;
    FILE *stream;
    POSITION pos = 0;

    const char *file_name = "../blk0-5.dat";
    if((fd = open(file_name, O_RDONLY)) == -1){
        perror(file_name);
        exit(1);
    }
    stream = fopen("result_.xml", "w");

    for(i=0; i<130000; i++){
        pos = NextBlockPosition(fd, pos);
    }

    for(iterations=0; iterations<10; iterations++){
        ParseBlockXML(fd, pos, stream);
        pos = NextBlockPosition(fd, pos);
    }
    close(fd);
}


int main(){
    test_parse();
}
