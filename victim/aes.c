#include "aes.h"

#define MUL2(x) (((x) << 1) ^ (0x1B & (((x) >> 7) * 0xFF)))
#define MUL3(x) ((x) ^ MUL2(x))

#ifdef DEBUG
#include <stdio.h>
#include <stdint.h>

void print_hex(uint8_t* b, size_t len) {
    for (size_t i = 0; i < len; i++)
        printf("%02x", b[i]);
    printf("\n");
}
#endif

uint8_t sbox[256] = {
  0x63, 0x7C, 0x77, 0x7B, 0xF2, 0x6B, 0x6F, 0xC5, 0x30, 0x01, 0x67, 0x2B, 0xFE, 0xD7, 0xAB, 0x76,
  0xCA, 0x82, 0xC9, 0x7D, 0xFA, 0x59, 0x47, 0xF0, 0xAD, 0xD4, 0xA2, 0xAF, 0x9C, 0xA4, 0x72, 0xC0,
  0xB7, 0xFD, 0x93, 0x26, 0x36, 0x3F, 0xF7, 0xCC, 0x34, 0xA5, 0xE5, 0xF1, 0x71, 0xD8, 0x31, 0x15,
  0x04, 0xC7, 0x23, 0xC3, 0x18, 0x96, 0x05, 0x9A, 0x07, 0x12, 0x80, 0xE2, 0xEB, 0x27, 0xB2, 0x75,
  0x09, 0x83, 0x2C, 0x1A, 0x1B, 0x6E, 0x5A, 0xA0, 0x52, 0x3B, 0xD6, 0xB3, 0x29, 0xE3, 0x2F, 0x84,
  0x53, 0xD1, 0x00, 0xED, 0x20, 0xFC, 0xB1, 0x5B, 0x6A, 0xCB, 0xBE, 0x39, 0x4A, 0x4C, 0x58, 0xCF,
  0xD0, 0xEF, 0xAA, 0xFB, 0x43, 0x4D, 0x33, 0x85, 0x45, 0xF9, 0x02, 0x7F, 0x50, 0x3C, 0x9F, 0xA8,
  0x51, 0xA3, 0x40, 0x8F, 0x92, 0x9D, 0x38, 0xF5, 0xBC, 0xB6, 0xDA, 0x21, 0x10, 0xFF, 0xF3, 0xD2,
  0xCD, 0x0C, 0x13, 0xEC, 0x5F, 0x97, 0x44, 0x17, 0xC4, 0xA7, 0x7E, 0x3D, 0x64, 0x5D, 0x19, 0x73,
  0x60, 0x81, 0x4F, 0xDC, 0x22, 0x2A, 0x90, 0x88, 0x46, 0xEE, 0xB8, 0x14, 0xDE, 0x5E, 0x0B, 0xDB,
  0xE0, 0x32, 0x3A, 0x0A, 0x49, 0x06, 0x24, 0x5C, 0xC2, 0xD3, 0xAC, 0x62, 0x91, 0x95, 0xE4, 0x79,
  0xE7, 0xC8, 0x37, 0x6D, 0x8D, 0xD5, 0x4E, 0xA9, 0x6C, 0x56, 0xF4, 0xEA, 0x65, 0x7A, 0xAE, 0x08,
  0xBA, 0x78, 0x25, 0x2E, 0x1C, 0xA6, 0xB4, 0xC6, 0xE8, 0xDD, 0x74, 0x1F, 0x4B, 0xBD, 0x8B, 0x8A,
  0x70, 0x3E, 0xB5, 0x66, 0x48, 0x03, 0xF6, 0x0E, 0x61, 0x35, 0x57, 0xB9, 0x86, 0xC1, 0x1D, 0x9E,
  0xE1, 0xF8, 0x98, 0x11, 0x69, 0xD9, 0x8E, 0x94, 0x9B, 0x1E, 0x87, 0xE9, 0xCE, 0x55, 0x28, 0xDF,
  0x8C, 0xA1, 0x89, 0x0D, 0xBF, 0xE6, 0x42, 0x68, 0x41, 0x99, 0x2D, 0x0F, 0xB0, 0x54, 0xBB, 0x16
};

void sub_word(uint8_t *word) {
    word[0] = sbox[word[0]];
    word[1] = sbox[word[1]];
    word[2] = sbox[word[2]];
    word[3] = sbox[word[3]];
}

void rot_word(uint8_t *word) {
    uint8_t t = word[0];
    word[0] = word[1];
    word[1] = word[2];
    word[2] = word[3];
    word[3] = t;
}

uint8_t rcon[10] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80, 0x1B, 0x36 };

void sub_bytes(aes_block_t* st) {
    uint8_t i = AES_BLOCKSIZE - 1;
    do {
        st->bytes[i] = sbox[st->bytes[i]];
    } while (i--);
}

void mix_columns(aes_block_t* st) {
    for (uint8_t c = 0; c < 4; c++) {
        uint8_t r1 = MUL2(st->matrix[c][0]) ^ MUL3(st->matrix[c][1]) ^      st->matrix[c][2]  ^      st->matrix[c][3];
        uint8_t r2 =      st->matrix[c][0]  ^ MUL2(st->matrix[c][1]) ^ MUL3(st->matrix[c][2]) ^      st->matrix[c][3];
        uint8_t r3 =      st->matrix[c][0]  ^      st->matrix[c][1]  ^ MUL2(st->matrix[c][2]) ^ MUL3(st->matrix[c][3]);
        uint8_t r4 = MUL3(st->matrix[c][0]) ^      st->matrix[c][1]  ^      st->matrix[c][2]  ^ MUL2(st->matrix[c][3]);
        st->matrix[c][0] = r1;
        st->matrix[c][1] = r2;
        st->matrix[c][2] = r3;
        st->matrix[c][3] = r4;
    }
}


void shift_rows(aes_block_t* st) {
    uint8_t t;

    // row 1
    t = st->matrix[0][1];
    st->matrix[0][1] = st->matrix[1][1];
    st->matrix[1][1] = st->matrix[2][1];
    st->matrix[2][1] = st->matrix[3][1];
    st->matrix[3][1] = t;

    // row 2
    t = st->matrix[0][2];
    st->matrix[0][2] = st->matrix[2][2];
    st->matrix[2][2] = t;

    t = st->matrix[1][2];
    st->matrix[1][2] = st->matrix[3][2];
    st->matrix[3][2] = t;

    // row 3
    t = st->matrix[0][3];
    st->matrix[0][3] = st->matrix[3][3];
    st->matrix[3][3] = st->matrix[2][3];
    st->matrix[2][3] = st->matrix[1][3];
    st->matrix[1][3] = t;
}

void add_key(aes_block_t* st, uint8_t *key) {
    uint8_t i = AES_BLOCKSIZE - 1;
    do {
        st->bytes[i] ^= key[i];
    } while (i--);
}

void xor_word(uint8_t* dst, uint8_t* src1, uint8_t* src2) {
    dst[0] = src1[0] ^ src2[0];
    dst[1] = src1[1] ^ src2[1];
    dst[2] = src1[2] ^ src2[2];
    dst[3] = src1[3] ^ src2[3];
}

void aes_expand(aes_expanded_key_t* ekey, const uint8_t key[AES_KEYSIZE]) {
    // first round key is the cipherkey
    for (uint8_t i = 0; i < AES_BLOCKSIZE; i++)
        ekey->keys[0][i] = key[i];

    // remaining keys
    for (uint8_t r = 0; r < AES_ROUNDS; r++) {
        aes_block_t* old = (aes_block_t*) &ekey->keys[r];
        aes_block_t* new = (aes_block_t*) &ekey->keys[r+1];

        uint8_t temp[sizeof(uint32_t)];

        new->matrix[0][0] = old->matrix[AES_ROWS - 1][0];
        new->matrix[0][1] = old->matrix[AES_ROWS - 1][1];
        new->matrix[0][2] = old->matrix[AES_ROWS - 1][2];
        new->matrix[0][3] = old->matrix[AES_ROWS - 1][3];

        rot_word(new->matrix[0]);
        sub_word(new->matrix[0]);

        new->matrix[0][0] ^= rcon[r];

        xor_word(new->matrix[0], old->matrix[0], new->matrix[0]);
        xor_word(new->matrix[1], old->matrix[1], new->matrix[0]);
        xor_word(new->matrix[2], old->matrix[2], new->matrix[1]);
        xor_word(new->matrix[3], old->matrix[3], new->matrix[2]);
    }
}

void aes_encrypt(uint8_t *pt, aes_expanded_key_t* const key) {
    aes_block_t* st = (aes_block_t*) pt;

    // pre-whitening
    add_key(st, key->keys[0]);


    // normal rounds
    uint8_t rnd = 1;
    for (;rnd < 10; rnd++) {

#ifdef DEBUG
    printf("\nstart: ");
    print_hex((uint8_t*) st, AES_BLOCKSIZE);
#endif

        sub_bytes(st);

#ifdef DEBUG
    printf("s_box: ");
    print_hex((uint8_t*) st, AES_BLOCKSIZE);
#endif

        shift_rows(st);

#ifdef DEBUG
    printf("s_row: ");
    print_hex((uint8_t*) st, AES_BLOCKSIZE);
#endif

        mix_columns(st);

#ifdef DEBUG
    printf("m_col: ");
    print_hex((uint8_t*) st, AES_BLOCKSIZE);
#endif

        add_key(st, key->keys[rnd]);
    }

    // final round
    sub_bytes(st);
    shift_rows(st);
    add_key(st, key->keys[rnd]);
}
