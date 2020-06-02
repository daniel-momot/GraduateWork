#pragma once

typedef unsigned char byte;

// Xor's all elements in a n byte array a by b
static void xor(byte *a, const byte *b, int n);

// Xor the current cipher state by a specific round key
static void xor_round_key(byte *state, const byte *keys, int round);

// Apply and reverse the rijndael s-box to all elements in an array
static void sub_bytes(byte *a,int n);
static void sub_bytes_inv(byte *a,int n);

// Rotate the first four bytes of the input eight bits to the left
static inline void rot_word(byte *a);
// Perform the core key schedule transform on 4 bytes, as part of the key expansion process
static void key_schedule_core(byte *a, int i);

// Expand the 16-byte key to 11 round keys (176 bytes)
static void expand_key128(const byte *key, byte *keys);

// Expand the 32-byte key to 15 round keys (240 bytes)
static void expand_key256(const byte *key, byte *keys);

// Apply / reverse the shift rows step on the 16 byte cipher state
static void shift_rows(byte *state);
static void shift_rows_inv(byte *state);

// Perform the mix columns matrix on one column of 4 bytes
static void mix_col (byte *state);

// Perform the mix columns matrix on each column of the 16 bytes
static void mix_cols (byte *state);

// Perform the inverse mix columns matrix on one column of 4 bytes
static void mix_col_inv (byte *state);

// Perform the inverse mix columns matrix on each column of the 16 bytes
static void mix_cols_inv (byte *state);

// Encrypt a single 128 bit block by a 128 bit key using AES
static void EncryptAES128(const byte *msg, const byte *key, byte *c);

// Encrypt a single 128 bit block by a 256 bit key using AES
static void EncryptAES256(const byte *msg, const byte *key, byte *c);
// Decrypt a single 128 bit block by a 128 bit key using AES
static void DecryptAES128(const byte *c, const byte *key, byte *m);

// Decrypt a single 128 bit block by a 256 bit key using AES
static void DecryptAES256(const byte *c, const byte *key, byte *m);

// Pretty-print a key (or any smallish buffer) onto screen as hex
static void Pretty(const byte* b,int len,char* label);

int test_expand_key_128();

int test_encrypt_128();

int test_decrypt_128();

int test_expand_key_256();

int test_encrypt_256();

int test_decrypt_256();

int aes_main(void);