#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "sha256.h"

unsigned int FourByteToInt(unsigned char *buffer){
    // This function takes a 4 byte array and returns the corresponding int
    unsigned int result = buffer[3]<<24 | buffer[2]<<16 | buffer[1]<<8 | buffer[0];
    return result;
}

int CheckMagicNo(unsigned char *buffer){
    // This function returns true if *buffer contains the magic number
    if((buffer[0] == 0xF9) && (buffer[1] == 0xBE) && (buffer[2] == 0xB4) && (buffer[3] == 0xD9)){
        return 1;
    }
    else return 0;
}

unsigned long long VarIntToLong(int fd, int pos){
    unsigned char first_byte;
    unsigned char other_bytes[8];
    int lfd = dup(fd);
    unsigned long long result;
    if(lseek(lfd, pos, SEEK_SET) == -1){
        perror("lseek");
        exit(1);
    }
    if(read(lfd, &first_byte, 1) == -1){
        perror("read");
        exit(1);
    }
    if(first_byte < 0xFD){
        result = first_byte;
        return result;
    }
    else if(first_byte == 0xFD){
        if(read(lfd, other_bytes, 2) == -1){
            perror("read");
            exit(1);
        }
        result = (unsigned long long)other_bytes[1]<<8 | (unsigned long long)other_bytes[0];
        return result;
    }
    else if(first_byte == 0xFE){
        if(read(lfd, other_bytes, 4) == -1){
            perror("read");
            exit(1);
        }
        result = (unsigned long long)other_bytes[3]<<24 | (unsigned long long)other_bytes[2]<<16
                | (unsigned long long)other_bytes[1]<<8 | (unsigned long long)other_bytes[0];
        return result;
    }
    else if(first_byte == 0xFF){
        if(read(lfd, other_bytes, 8) == -1){
            perror("read");
            exit(1);
        }
        result = (unsigned long long)other_bytes[7]<<56 | (unsigned long long)other_bytes[6]<<48
                | (unsigned long long)other_bytes[5]<<40 | (unsigned long long)other_bytes[4]<<32
                | (unsigned long long)other_bytes[3]<<24 | (unsigned long long)other_bytes[2]<<16
                | (unsigned long long)other_bytes[1]<<8 | (unsigned long long)other_bytes[0];
        return result;
    }
    close(lfd);
    return 0;
}

void PrintFourByteLittleEndian(unsigned char *buffer){
    // This function prints a 4 byte little endian array
    printf("%02X %02X %02X %02X\n", buffer[3], buffer[2], buffer[1], buffer[0]);
}

void PrintBlockHeader(unsigned char *buffer){
    // This function prints the 80 bytes of a block header contained in *buffer
    int i;
    for(i=0; i<80; i++){
        printf("%02X ", buffer[i]);
    }
    printf("\n");
}

void PrintHash(unsigned char *buffer){
    // This function prints the 32 bytes of a hash contained in *buffer
    int i;
    for(i=31;i>=0;i--){
        printf("%02x", buffer[i]);
    }
    //printf("\n");
}

int AdvancePositionVarInt(int fd, int pos){
    int lfd = dup(fd);
    unsigned char first_byte;
    if(lseek(lfd, pos, SEEK_SET) == -1){
        perror("lseek");
        exit(1);
    }
    if(read(lfd, &first_byte, 1) == -1){
        perror("read");
        exit(1);
    }
    close(lfd);
    if(first_byte < 0xFD){
        return pos+1;
    }
    else if(first_byte == 0xFD){
        return pos+3;
    }
    else if(first_byte == 0xFE){
        return pos+5;
    }
    else if(first_byte == 0xFF){
        return pos+9;
    }
    return 0;
}

int NextBlockPosition(int fd, int pos){
    // This function takes as input the position of a block in a file (seek index), checks that it is really
    // the beginning of a block (the first 4 bytes must be the magic number 0xD9B4BEF9) and returns the position
    // in the file of the next block. It returns -1 in case of error
    int lfd = dup(fd);
    if(lseek(lfd, pos, SEEK_SET) == -1){
        perror("lseek");
        exit(1);
    }

    int blocksize;
    int nextpos;
    unsigned char buffer[4];

    if(read(lfd, buffer, 4) == -1){
        perror("read");
        exit(1);
    }
    if(CheckMagicNo(buffer)){
        if(read(lfd, buffer, 4) == -1){
            perror("read");
            exit(1);
        }
        blocksize = FourByteToInt(buffer);
        if((nextpos = lseek(lfd, blocksize, SEEK_CUR)) == -1){
            perror("lseek");
            exit(1);
        };
        if(read(lfd, buffer, 4) == -1){
            perror("read");
            exit(1);
        }
        if(CheckMagicNo(buffer)){
            close(lfd);
            return nextpos;
        }
    }
    close(lfd);
    return -1;
}

void GetHashPreviousBlock(int fd, int pos, unsigned char *hash){
    // This function gets the hash of the previous block contained in the header of the block at position pos
    // and it puts it in *hash. *hash must be an array of 32 byte
    int lfd = dup(fd);
    if(lseek(lfd, pos, SEEK_SET) == -1){
        perror("lseek");
        exit(1);
    }

    unsigned char buffer[4];
    int i;

    if(read(lfd, buffer, 4) == -1){
        perror("read");
        exit(1);
    }
    if(CheckMagicNo(buffer)){
        if(lseek(lfd, 8, SEEK_CUR) == -1){
            perror("lseek");
            exit(1);
        }
        if(read(lfd, hash, 32) == -1){
            perror("read");
            exit(1);
        }
    }
    close(lfd);
}

void GetBlockHeader(int fd, int pos, unsigned char *header){
    int lfd = dup(fd);
    if(lseek(lfd, pos, SEEK_SET) == -1){
        perror("lseek");
        exit(1);
    }
    unsigned char buffer[4];

    if(read(lfd, buffer, 4) == -1){
        perror("read");
        exit(1);
    }
    if(CheckMagicNo(buffer)){
        if(lseek(lfd, 4, SEEK_CUR) == -1){
            perror("lseek");
            exit(1);
        }
        if(read(lfd, header, 80) == -1){
            perror("read");
            exit(1);
        }
    }
    close(lfd);
}

void CalcBlockHash(int fd, int pos, unsigned char *hash){
    //This function calculates the hash of the block that starts at pos and store the result to *hash.
    //*hash must be an array of 32 bytes.
    unsigned char hash_final[32];
    unsigned char header[80];
    SHA256_CTX hasher;
    sha256_init(&hasher);

    GetBlockHeader(fd, pos, header);
    sha256_update(&hasher, (BYTE*)&header, 80);
    sha256_final(&hasher, hash_final);
    sha256_init(&hasher);
    sha256_update(&hasher, (BYTE*)&hash_final, 32);
    sha256_final(&hasher, hash);
}

unsigned long long NextTxPosition(int fd, int pos){
    unsigned long long i;
    int lfd = dup(fd);
    if(lseek(lfd, pos, SEEK_SET) == -1){
        perror("lseek");
        exit(1);
    }

    unsigned long long inputs, outputs;
    unsigned long long txinscriptlen, txoutscriptlen;


    pos += 4;
    inputs = VarIntToLong(lfd, pos);
    printf("inputs: %llu\n",inputs);
    pos = AdvancePositionVarInt(lfd, pos);
    for(i=0; i<inputs; i++){
        pos+=32;
        pos+=4;
        txinscriptlen = VarIntToLong(lfd, pos);
        pos = AdvancePositionVarInt(lfd, pos);
        pos += txinscriptlen;
        pos += 4;
    }
    outputs = VarIntToLong(lfd, pos);
    printf("outputs: %llu\n",outputs);
    pos = AdvancePositionVarInt(lfd, pos);
    for(i=0; i<outputs; i++){
        pos+=8;
        txoutscriptlen = VarIntToLong(lfd, pos);
        pos = AdvancePositionVarInt(lfd, pos);
        pos += txoutscriptlen;
    }
    pos +=4;
    close(lfd);
    return pos;
}
