#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "sha256.h"

unsigned int FourByteToInt(unsigned char *buffer);
int CheckMagicNo(unsigned char *buffer);
void PrintFourByteLittleEndian(unsigned char *buffer);
int NextBlockPosition(int fd, int pos);
void GetHashPreviousBlock(int fd, int pos, unsigned char *hash);
void GetBlockHeader(int fd, int pos, unsigned char *header);
void PrintBlockHeader(unsigned char *buffer);
void CalcBlockHash(int fd, int pos, unsigned char *hash);
