#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include "sha256.h"

typedef unsigned long long POSITION;

unsigned int FourByteToInt(unsigned char *buffer);
unsigned int CheckMagicNo(unsigned char *buffer);
unsigned long long VarIntToUnsignedLongLong(int fd, int pos);
void PrintHash(unsigned char *buffer);

POSITION NextBlockPosition(int fd, POSITION pos);
POSITION NextTxPosition(int fd, POSITION pos);
POSITION JumpAfterVarInt(int fd, POSITION pos);
POSITION GoToTxCounter(int fd, POSITION pos);
POSITION GoToFirstTx(int fd, POSITION pos);

void CalcBlockHash(int fd, POSITION pos, unsigned char *hash);
void GetBlockHeader(int fd, POSITION pos, unsigned char *header);
unsigned long long GetTxCounter(int fd, POSITION pos);
void CalcTxHash(int fd, POSITION pos, unsigned char *hash);
void PrintTxInputs(int fd, POSITION pos);
