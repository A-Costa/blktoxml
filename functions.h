#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <openssl/ripemd.h>
#include "sha256.h"
#include "libbase58.h"

typedef unsigned long long POSITION;
typedef struct s_txinput{
    unsigned char prev_tx_hash[32];
    unsigned int prev_tx_index;
} txinput;

typedef struct s_txoutput{
    char address[64];
    unsigned int s_address;
    unsigned int index;
} txoutput;



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
void PrintTxOutputs(int fd, POSITION pos);

unsigned long long ExtractTxInputs(int fd, POSITION pos, txinput **result);
void ScriptToAddress(unsigned char *script, unsigned long long len, char *result, unsigned int *s_result);
