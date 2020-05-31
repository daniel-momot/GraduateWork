#include <time.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

static inline void * my_memcpy(void * to, const void * from, size_t n)
{
	int d0, d1, d2; // выделяем память под промежуточные переменные
	__asm__ 
	__volatile__ // отключаем оптимизации  https://gcc.gnu.org/onlinedocs/gcc/Extended-Asm.html#Volatile
	(
	"rep ; movsl\n\t"
	"testb $2,%b4\n\t"
	"je 1f\n\t"
	"movsw\n"
	"1:\ttestb $1,%b4\n\t"
	"je 2f\n\t"
	"movsb\n"
	"2:"
	
	// выходные (де-факто промежуточные) переменные в формате constraint (cvariablename)
	// & - запрет перезаписи входа
	// S - esi/si, D - edi/di, аналогично a, b, c, d
	: "=&c" (d0), "=&D" (d1), "=&S" (d2)
	
	// входные операнды
	: "0" (n/4), "q" (n), "1" ((long) to), "2" ((long) from)
	
	// разрушаемые регистры: memory указывает на изменение памяти, служит также как memory barrier
	: "memory"
	
	);		

	//printf("d0: %i, D: %i, S: %i.\n", d0, d1, d2);
	
	return (to);
}


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
	
	int n = 10000000;
	double time;
	int clocks;
	bool result = eval_time(n, &clocks, &time);
	if (result == true) {
		printf("Time spent: %i clocks (%f secs).\n", clocks, time);
	}
	else {
		printf("Error occured. Cannot get time.\n");
	}
		
	return 0;
}