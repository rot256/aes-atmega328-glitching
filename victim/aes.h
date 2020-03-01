#include <stdint.h>

#define AES_BLOCKSIZE (16)
#define AES_KEYSIZE (16)
#define AES_ROWS (4)
#define AES_COLS (4)
#define AES_ROUNDS (10)
#define AES_ROUNDKEYS (AES_ROUNDS + 1)

typedef union aes_block_t {
    uint8_t bytes[AES_BLOCKSIZE];
    uint8_t matrix[AES_ROWS][AES_COLS];
} aes_block_t;

typedef union aes_expanded_key_t {
    uint8_t keys[AES_ROUNDS + 1][AES_BLOCKSIZE];
} aes_expanded_key_t;

void aes_encrypt(uint8_t *pt, aes_expanded_key_t* const key);
void aes_expand(aes_expanded_key_t* ekey, const uint8_t key[AES_KEYSIZE]);

