#ifndef __SHA_H
#define __SHA_H


#include <stdint.h>

#ifdef __cplusplus
extern "C"{
#endif


/*********** sha1 ***************/
#define SHA1_BLOCK_SIZE 20
typedef struct
{
    uint8_t data[64];
    uint32_t datalen;
    unsigned long long bitlen;
    uint32_t state[5];
    uint32_t k[4];
}SHA1_CTX;

void sha1_init(SHA1_CTX *ctx);
void sha1_update(SHA1_CTX *ctx,const uint8_t data[],unsigned int len);
void sha1_final(SHA1_CTX *ctx,uint8_t hash[]);

/************ sha256 ***********/
typedef struct
{
    uint8_t data[64];
    uint32_t datalen;
    unsigned long long bitlen;
    uint32_t state[8];
}SHA256_CTX;

void sha256_init(SHA256_CTX *ctx);
void sha256_update(SHA256_CTX *ctx,const uint8_t data[],unsigned int len);
void sha256_final(SHA256_CTX *ctx,uint8_t hash[]);




void sha1_string(const char *str,char *output);

void sha256_string(const char *input,char *output);

#ifdef __cplusplus
}
#endif

#endif
