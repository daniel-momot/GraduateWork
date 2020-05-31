#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

#include "aes.c"

static int get_int() {
	srand(time(NULL));
	return rand();
}
static void get_ints(int* arr, size_t n) {
	srand(time(NULL));
	for (size_t i = 0; i < n; i++) {
		arr[i] = rand();
	}
}


//static inline void * my_memcpy(void * to, const void * from, size_t n);

bool eval_time(size_t n, int * clocks, double* res) {
	
	int a[10] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9 };
	int b[10] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	
	clock_t error_val = (clock_t) -1, time_start, time_stop;
	
	time_start = clock();
	
	for (int j = 0; j < n; j++) {
		my_memcpy(b, a, 10 * sizeof(int));
	}
	
	time_stop = clock();
	
	printf("Contents of b: ");
	for (size_t i = 0; i < 10; i++) {
		printf("%i", b[i]);
	}
	printf("\n");
	
	if ( (time_start == error_val) || (time_stop == error_val)) {
		printf("Error occured. Cannot get time.\n");
		return false;
	}
	else {
		*clocks = (int) (time_stop - time_start);
		*res = ((double) *clocks) / CLOCKS_PER_SEC;
		printf("CLOCKS_PER_SEC = %i.\n", CLOCKS_PER_SEC);
		return true;
	}
}

int main() {
	
	int n = 10;
	double time;
	int clocks;
	bool result = eval_time(n, &clocks, &time);
	if (result == true) {
		printf("Time spent: %i clocks (%f secs).\n", clocks, time);
	}
	else {
		printf("Error occured. Cannot get time.\n");
	}
	
	/*unsigned char[16] plain = {
		'a', 'b', 'c', 'd',
		'e', 'f', 'g', 'h',
		'i', 'j', 'k', 'l',
		'm', 'n', 'o', 'p'
	};*/
	
	unsigned char* plain = "abcdabcdabcdabcd";
	unsigned char* key = "abcdabcdabcdabcd";
	unsigned char* cipher = "1234123412341234";
	
	AESEncryption(plain, key, cipher);
	
	printf("AES. PLAIN: %s KEY: %s CIPHER: %s CIPHBYTES: ", plain, key, cipher);
	for (size_t i = 0; i < 16; i++) {
		printf("%i ", cipher[i]);
	}
	printf("\n");
	
	
	
	
	
	
		
	return 0;
}