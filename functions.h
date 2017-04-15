#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "sha256.h"

typedef unsigned long long POSITION;

unsigned int FourByteToInt(unsigned char *buffer);
unsigned int CheckMagicNo(unsigned char *buffer);
POSITION NextBlockPosition(int fd, POSITION pos);
POSITION GoToTxCounter(int fd, POSITION pos);
POSITION GoToFirstTx(int fd, POSITION pos);
unsigned long long VarIntToUnsignedLongLong(int fd, int pos);
unsigned long long GetTxCounter(int fd, POSITION pos);
POSITION JumpAfterVarInt(int fd, POSITION pos);
POSITION NextTxPosition(int fd, POSITION pos);
void CalcTxHash(int fd, POSITION pos, unsigned char *hash);
void PrintHash(unsigned char *buffer);


void CalcBlockHash(int fd, POSITION pos, unsigned char *hash);
void GetBlockHeader(int fd, POSITION pos, unsigned char *header);

void PrintTxInputs(int fd, POSITION pos);
