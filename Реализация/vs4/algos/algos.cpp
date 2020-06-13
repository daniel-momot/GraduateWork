#include <string>

#include "aes1.h"
#include "aes2.h"
#include "aes3.h"

#include <time.h>

#if _WIN32 || _WIN64
#if _WIN64
#define ENVIRONMENT64
#else
#define ENVIRONMENT32
#endif
#endif

size_t func() {
	size_t i = 0;
	return i - 1;
}

#include "windows.h"

void aes_asm (const byte plaintext[16], const byte key[16], byte cipher[16])  {
	//typedef byte* (*cryptofunc)(const byte*,const byte*);
	typedef byte* (*cryptofunc)(void);

	void* page = VirtualAlloc(NULL, 4096, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	const cryptofunc func = (cryptofunc) get_func(page);

	//cipher = func(plaintext, key);
	cipher = func();

	VirtualFree(page, 4096, MEM_RELEASE);
}
void main() {
	/*printf("Hello World!");
	printf("Result of func(): %u, sizeof: %i bit\n", func(), sizeof(long long));
	system("pause");*/

    unsigned char plaintext[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff};
    unsigned char key[] = {0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};
    unsigned char expected_ciphertext[] = {0x69, 0xc4, 0xe0, 0xd8, 0x6a, 0x7b, 0x04, 0x30, 0xd8, 0xcd, 0xb7, 0x80, 0x70, 0xb4, 0xc5, 0x5a};
    unsigned char ciphertext[16];

	unsigned char concat_input[] = {0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff,
		0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f};

	size_t t1, t2;
	t1 = clock();
	for (size_t i = 0; i < 1000; i++)
		E(concat_input);
	t2 = clock();

	double diff1 = (double(t2 - t1)) / CLOCKS_PER_SEC;

	t1 = clock();
	for (size_t i = 0; i < 1000; i++)
		AESEncryption(plaintext, keyExpansion(key), ciphertext);
	t2 = clock();

	double diff2 = (double(t2 - t1)) / CLOCKS_PER_SEC;

	t1 = clock();
	for (size_t i = 0; i < 1000; i++)
		EncryptAES128(plaintext, key, ciphertext);
	t2 = clock();

	double diff3 = (double(t2 - t1)) / CLOCKS_PER_SEC;

	aes_asm(plaintext, key, ciphertext);

	printf("Time brief: %lf secs.\n", diff1);
	printf("Time expn1: %lf secs.\n", diff2);
	printf("Time expn2: %lf secs.\n", diff3);
	system("pause");
}

