#include "functions.h"

//***** -> Typedefs and Data Structures
typedef unsigned long long POSITION;




//***** -> Utilities Functions
unsigned int FourByteToInt(unsigned char *buffer){
    // This function takes a 4 byte array and returns the corresponding int
    unsigned int result = buffer[3]<<24 | buffer[2]<<16 | buffer[1]<<8 | buffer[0];
    return result;
}
unsigned long EightByteToLongLong(unsigned char *buffer){
    // This function takes a 4 byte array and returns the corresponding int
    unsigned long long result;
    result = (unsigned long long)buffer[7]<<56 | (unsigned long long)buffer[6]<<48
            | (unsigned long long)buffer[5]<<40 | (unsigned long long)buffer[4]<<32
            | (unsigned long long)buffer[3]<<24 | (unsigned long long)buffer[2]<<16
            | (unsigned long long)buffer[1]<<8 | (unsigned long long)buffer[0];
    return result;
}
unsigned int CheckMagicNo(unsigned char *buffer){
    // This function returns true if *buffer contains the magic number
    if((buffer[0] == 0xF9) && (buffer[1] == 0xBE) && (buffer[2] == 0xB4) && (buffer[3] == 0xD9)){
        return 1;
    }
    else return 0;
}
unsigned long long VarIntToUnsignedLongLong(int fd, int pos){
    unsigned char first_byte;
    unsigned char other_bytes[8];
    int lfd = dup(fd);
    unsigned long long result;
    if(lseek(lfd, pos, SEEK_SET) == -1){
        close(lfd);
        perror("lseek_varinttounsignedlonglong");
        exit(1);
    }
    if(read(lfd, &first_byte, 1) == -1){
        close(lfd);
        perror("read");
        exit(1);
    }
    if(first_byte < 0xFD){
        close(lfd);
        result = (unsigned long long)first_byte;
        return result;
    }
    else if(first_byte == 0xFD){
        if(read(lfd, other_bytes, 2) == -1){
            close(lfd);
            perror("read");
            exit(1);
        }
        close(lfd);
        result = (unsigned long long)other_bytes[1]<<8 | (unsigned long long)other_bytes[0];
        return result;
    }
    else if(first_byte == 0xFE){
        if(read(lfd, other_bytes, 4) == -1){
            close(lfd);
            perror("read");
            exit(1);
        }
        close(lfd);
        result = (unsigned long long)other_bytes[3]<<24 | (unsigned long long)other_bytes[2]<<16
                | (unsigned long long)other_bytes[1]<<8 | (unsigned long long)other_bytes[0];
        return result;
    }
    else if(first_byte == 0xFF){
        if(read(lfd, other_bytes, 8) == -1){
            perror("read");
            exit(1);
        }
        close(lfd);
        result = (unsigned long long)other_bytes[7]<<56 | (unsigned long long)other_bytes[6]<<48
                | (unsigned long long)other_bytes[5]<<40 | (unsigned long long)other_bytes[4]<<32
                | (unsigned long long)other_bytes[3]<<24 | (unsigned long long)other_bytes[2]<<16
                | (unsigned long long)other_bytes[1]<<8 | (unsigned long long)other_bytes[0];
        return result;
    }
    close(lfd);
    return 0;
}
void PrintHash(unsigned char *buffer){
    // This function prints the 32 bytes of a hash contained in *buffer
    int i;
    for(i=31;i>=0;i--){
        printf("%02x", buffer[i]);
    }
    //printf("\n");
}

//***** -> Positioning Functions
POSITION NextBlockPosition(int fd, POSITION pos){
    // This function takes as input the position of a block in a file (seek index), checks that it is really
    // the beginning of a block (the first 4 bytes must be the magic number 0xD9B4BEF9) and returns the position
    // in the file of the next block. It returns -1 in case of error
    unsigned int blocksize;
    POSITION nextpos;
    unsigned char buffer[4];

    int lfd = dup(fd);
    if(lseek(lfd, pos, SEEK_SET) == -1){
        perror("lseek_nextblockposition");
        exit(1);
    }

    if(read(lfd, buffer, 4) == -1){
        close(lfd);
        perror("read_nextblockposition");
        exit(1);
    }
    if(CheckMagicNo(buffer)){
        if(read(lfd, buffer, 4) == -1){
            close(lfd);
            perror("read_nextblockposition");
            exit(1);
        }
        blocksize = FourByteToInt(buffer);
        if((nextpos = lseek(lfd, blocksize, SEEK_CUR)) == -1){
            close(lfd);
            perror("lseek_nextblockposition");
            exit(1);
        };
        if(read(lfd, buffer, 4) == -1){
            close(lfd);
            perror("read_nextblockposition");
            exit(1);
        }
        if(CheckMagicNo(buffer)){
            close(lfd);
            return nextpos;
        }
    }
    close(lfd);
    printf("ERROR_nextblockposition: position is not the beginning of a block\n");
    return 0xffffffffffffffff;
}
POSITION NextTxPosition(int fd, POSITION pos){
    unsigned long long i;
    int lfd = dup(fd);
    if(lseek(lfd, pos, SEEK_SET) == -1){
        perror("lseek_nexttxposition");
        exit(1);
    }

    unsigned long long inputs, outputs;
    unsigned long long txinscriptlen, txoutscriptlen;

    pos += 4;
    inputs = VarIntToUnsignedLongLong(lfd, pos);
    pos = JumpAfterVarInt(lfd, pos);

    for(i=0; i<inputs; i++){
        pos+=32;
        pos+=4;
        txinscriptlen = VarIntToUnsignedLongLong(lfd, pos);
        pos = JumpAfterVarInt(lfd, pos);
        pos += txinscriptlen;
        pos += 4;
    }
    outputs = VarIntToUnsignedLongLong(lfd, pos);
    pos = JumpAfterVarInt(lfd, pos);
    for(i=0; i<outputs; i++){
        pos+=8;
        txoutscriptlen = VarIntToUnsignedLongLong(lfd, pos);
        pos = JumpAfterVarInt(lfd, pos);
        pos += txoutscriptlen;
    }
    pos +=4;
    close(lfd);
    return pos;
}
POSITION JumpAfterVarInt(int fd, POSITION pos){
    int lfd = dup(fd);
    unsigned char first_byte;
    if(lseek(lfd, pos, SEEK_SET) == -1){
        close(lfd);
        perror("lseek_jumpaftervarint");
        exit(1);
    }
    if(read(lfd, &first_byte, 1) == -1){
        close(lfd);
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
POSITION GoToTxCounter(int fd, POSITION pos){
    return pos+88;
}
POSITION GoToFirstTx(int fd, POSITION pos){
    pos = GoToTxCounter(fd, pos);
    pos = JumpAfterVarInt(fd, pos);
    return pos;
}

//***** -> Blocks-Related Functions
void CalcBlockHash(int fd, POSITION pos, unsigned char *hash){
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
void GetBlockHeader(int fd, POSITION pos, unsigned char *header){
    int lfd = dup(fd);
    if(lseek(lfd, pos, SEEK_SET) == -1){
        close(lfd);
        perror("lseek_getblockheader");
        exit(1);
    }
    unsigned char buffer[4];

    if(read(lfd, buffer, 4) == -1){
        close(lfd);
        perror("read_getblockheader");
        exit(1);
    }
    if(CheckMagicNo(buffer)){
        if(lseek(lfd, 4, SEEK_CUR) == -1){
            close(lfd);
            perror("lseek_getblockheader");
            exit(1);
        }
        if(read(lfd, header, 80) == -1){
            close(lfd);
            perror("read_getblockheader");
            exit(1);
        }
    }
    close(lfd);
}

//***** -> Tx-Related Functions
unsigned long long GetTxCounter(int fd, POSITION pos){
    return VarIntToUnsignedLongLong(fd, GoToTxCounter(fd, pos));
}
void CalcTxHash(int fd, POSITION pos, unsigned char *hash){
    unsigned long long i;
    POSITION ntxp;
    POSITION tx_size;
    unsigned char *tx;
    unsigned char hash_final[32];

    SHA256_CTX hasher;
    sha256_init(&hasher);

    ntxp = NextTxPosition(fd, pos);
    tx_size = ntxp - pos;
    //printf("tx_size: %llu\n", tx_size);

    tx = malloc(sizeof(unsigned char) * tx_size);
    for(i=0;i<tx_size;i++){
        tx[i] = 0;
    }

    int lfd = dup(fd);
    if(lseek(lfd, pos, SEEK_SET) == -1){
        perror("lseek_calctxhash");
        exit(1);
    }

    if(read(lfd, tx, tx_size) == -1){
        perror("read_calctxhash");
        exit(1);
    }

    close(lfd);

    /*
    printf("tx_bytes: ");
    for(i=0;i<tx_size;i++){
        printf("%02X ", tx[i]);
    }
    printf("\n");
    */

    sha256_init(&hasher);
    sha256_update(&hasher, (BYTE*)tx, tx_size);
    sha256_final(&hasher, hash_final);
    sha256_init(&hasher);
    sha256_update(&hasher, (BYTE*)&hash_final, 32);
    sha256_final(&hasher, hash);
    free(tx);
}
void PrintTxInputs(int fd, POSITION pos){
    unsigned long long i;
    int lfd = dup(fd);
    unsigned long long inputs, outputs;
    unsigned long long txinscriptlen;
    unsigned char prev_tx_hash[32];
    unsigned char prev_tx_index[4];
    unsigned int prev_tx_index_int;

    pos += 4;
    inputs = VarIntToUnsignedLongLong(lfd, pos);
    pos = JumpAfterVarInt(lfd, pos);

    for(i=0; i<inputs; i++){
        if(lseek(lfd, pos, SEEK_SET) == -1){
            perror("lseek_printtxinputs");
            exit(1);
        }
        read(lfd, prev_tx_hash, 32);
        printf("       ");
        PrintHash(prev_tx_hash);
        read(lfd, prev_tx_index, 4);
        prev_tx_index_int = FourByteToInt(prev_tx_index);
        if(prev_tx_index_int == 0xFFFFFFFF){
            printf(" ---> coinbase");
        }
        else{
            printf(" ---> %u", prev_tx_index_int);
        }
        printf("\n");
        pos+=32;
        pos+=4;
        txinscriptlen = VarIntToUnsignedLongLong(lfd, pos);
        pos = JumpAfterVarInt(lfd, pos);
        pos += txinscriptlen;
        pos += 4;
    }
    close(lfd);
}

void PrintTxOutputs(int fd, POSITION pos){
    unsigned long long i;
    int j;
    int lfd = dup(fd);
    unsigned long long inputs, outputs;
    unsigned long long txinscriptlen, txoutscriptlen;
    //unsigned char prev_tx_hash[32];
    unsigned char satoshis[8];
    unsigned char *script;
    unsigned char address[40];
    for(j=0; j<40; j++){
        address[j] = 0;
    }

    pos += 4;
    inputs = VarIntToUnsignedLongLong(lfd, pos);
    pos = JumpAfterVarInt(lfd, pos);

    for(i=0; i<inputs; i++){
        pos+=32;
        pos+=4;
        txinscriptlen = VarIntToUnsignedLongLong(lfd, pos);
        pos = JumpAfterVarInt(lfd, pos);
        pos += txinscriptlen;
        pos += 4;
    }

    outputs = VarIntToUnsignedLongLong(lfd, pos);
    pos = JumpAfterVarInt(lfd, pos);

    for(i=0; i<outputs; i++){
        if(lseek(lfd, pos, SEEK_SET) == -1){
            perror("lseek_printtxinputs");
            exit(1);
        }
        read(lfd, satoshis, 8);
        unsigned long long amount;
        amount = EightByteToLongLong(satoshis);
        printf("     amount: %llu\n", amount);
        pos += 8;
        txoutscriptlen = VarIntToUnsignedLongLong(lfd, pos);
        pos = JumpAfterVarInt(lfd, pos);
        script = malloc(sizeof(unsigned char) *txoutscriptlen);
        if(lseek(lfd, pos, SEEK_SET) == -1){
            perror("lseek_printtxinputs");
            exit(1);
        }
        read(lfd, script, txoutscriptlen);
        printf("------> script: ");
        for(j=0;j<txoutscriptlen;j++){
            printf("%02x ", script[j]);
        }
        printf("\n");
        ScriptToAddress(script, txoutscriptlen, address);
        printf("address: ");
        for(j=0; j<40; j++){
            printf("%02x ", address[j]);
        }
        pos += txoutscriptlen;
    }
    free(script);
    close(lfd);
}

unsigned long long ExtractTxInputs(int fd, POSITION pos, txinput **result){
    unsigned long long i;
    int j;
    int lfd = dup(fd);
    unsigned long long inputs;
    unsigned long long txinscriptlen;
    unsigned char prev_tx_hash[32];
    unsigned char prev_tx_index[4];
    unsigned int prev_tx_index_int;

    pos += 4;
    inputs = VarIntToUnsignedLongLong(lfd, pos);
    pos = JumpAfterVarInt(lfd, pos);

    *result = malloc(inputs * sizeof(txinput));

    for(i=0; i<inputs; i++){
        if(lseek(lfd, pos, SEEK_SET) == -1){
            perror("lseek_extracttxinputs");
            exit(1);
        }
        if(read(lfd, prev_tx_hash, 32) == -1){
            perror("read_extracttxinputs");
            exit(1);
        }
        if(read(lfd, prev_tx_index, 4) == -1){
            perror("read_extracttxinputs");
            exit(1);
        }
        prev_tx_index_int = FourByteToInt(prev_tx_index);

        for(j=0;j<32;j++){
            (*result)[i].prev_tx_hash[j] = prev_tx_hash[j];
        }
        (*result)[i].prev_tx_index = prev_tx_index_int;

        pos+=32;
        pos+=4;
        txinscriptlen = VarIntToUnsignedLongLong(lfd, pos);
        pos = JumpAfterVarInt(lfd, pos);
        pos += txinscriptlen;
        pos += 4;
    }
    close(lfd);
    return inputs;
}


void ScriptToAddress(unsigned char *script, unsigned long long len, unsigned char *address){
    int i;
    unsigned char sha_hash[32];
    unsigned char ripemd_hash[25];
    size_t s_stringa = 40;
    char stringa[s_stringa];

    SHA256_CTX hasher;
    sha256_init(&hasher);
    sha256_update(&hasher, (BYTE*)script+1, 0x41);
    sha256_final(&hasher, sha_hash);
    RIPEMD160(sha_hash, 32, ripemd_hash+1);
    // A QUESTO PUNTO SIAMO ALLO STEP 3
    //https://en.bitcoin.it/wiki/Technical_background_of_version_1_Bitcoin_addresses#How_to_create_Bitcoin_Address
    ripemd_hash[0] = 0x00;
    printf("ripemd: ");
    for(i=0; i<21; i++){
        printf("%02x ", ripemd_hash[i]);
    }
    printf("\n");
    sha256_init(&hasher);
    sha256_update(&hasher, (BYTE*)ripemd_hash, 21);
    sha256_final(&hasher, sha_hash);
    sha256_init(&hasher);
    sha256_update(&hasher, (BYTE*)sha_hash, 32);
    sha256_final(&hasher, sha_hash);
    printf("hash_hash: ");
    for(i=0; i<32; i++){
        printf("%02x ", sha_hash[i]);
    }
    printf("\n");
    for(i=0; i<4; i++){
        ripemd_hash[21+i] = sha_hash[i];
    }
    printf("ripemd_withchecksum: ");
    for(i=0; i<25; i++){
        printf("%02x ", ripemd_hash[i]);
    }
    printf("\n");

    //base58(ripemd_hash, stringa);
    b58enc(stringa, &s_stringa, ripemd_hash, 25);
    printf("s_stringa: %zu\n", s_stringa);


    printf("stringa: ");
    printf("%s", stringa);
    printf("\n");
}
