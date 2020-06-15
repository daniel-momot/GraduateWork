#include <string>
#include <windows.h>
#include <vector>

#include "aes1.h"
#include "aes2.h"
#include "aes3.h"

#include <time.h>
#include <stdlib.h>


double get_frequency() {
	LARGE_INTEGER li;
    if(!QueryPerformanceFrequency(&li)) {
		throw std::exception("Can not get counter frequency.\n");
	}
	return double(li.QuadPart);
}

std::vector<byte*> get_blocks(size_t blocks_num, size_t chunk_sz) {
	srand((unsigned int) time(NULL));
	std::vector<byte*> out(blocks_num);
	for (size_t i = 0; i < blocks_num; i++) {
		out[i] = (byte*) malloc(chunk_sz);
		for (size_t j = 0; j < chunk_sz; j++) {
			out[i][j] = (byte) rand();
		}
	}
	return out;
}

void free_blocks(std::vector<byte*>& mem) {
	for (size_t i = 0; i < mem.size(); i++) {
		free(mem[i]);
	}
}

double get_time1(size_t packets_size, size_t packets_number) {
	std::vector<byte*> inputs = get_blocks(packets_size, 16);
	std::vector<byte*> keys = get_blocks(packets_size, 16);
	std::vector<byte*> results = get_blocks(packets_size, 16);
	LARGE_INTEGER point1, point2;
	double time = 0;
	byte control = 0; // чтобы не вызовы функции не заоптимизировались

	for (size_t j = 0; j < packets_number; j++) {
		QueryPerformanceCounter(&point1);
		for (size_t i = 0; i < packets_size; i++) {
			AESEncryption(inputs[i], keys[i], results[i]);
		}
		QueryPerformanceCounter(&point2);
		for (size_t i = 0; i < packets_size; i++) {
			control += results[i][0];
		}
		time += ((double) point2.QuadPart - point1.QuadPart) / get_frequency();
	}
	
	//printf("Func1: ps = %i, pn = %i, time: %f, ctrl: %c", packets_size, packets_number, time, control);
	printf("%c", control);
	free_blocks(inputs);
	free_blocks(keys);
	free_blocks(results);

	return time;
}

double get_time2(size_t packets_size, size_t packets_number) {
	std::vector<byte*> inputs = get_blocks(packets_size, 16);
	std::vector<byte*> keys = get_blocks(packets_size, 16);
	std::vector<byte*> results = get_blocks(packets_size, 16);
	LARGE_INTEGER point1, point2;
	double time = 0;
	byte control = 0; // чтобы не вызовы функции не заоптимизировались

	for (size_t j = 0; j < packets_number; j++) {
		QueryPerformanceCounter(&point1);
		for (size_t i = 0; i < packets_size; i++) {
			EncryptAES128(inputs[i], keys[i], results[i]);
		}
		QueryPerformanceCounter(&point2);
		for (size_t i = 0; i < packets_size; i++) {
			control += results[i][0];
		}
		time += ((double) point2.QuadPart - point1.QuadPart) / get_frequency();
	}
	
	//printf("Func1: ps = %i, pn = %i, time: %f, ctrl: %c", packets_size, packets_number, time, control);
	printf("%c", control);
	free_blocks(inputs);
	free_blocks(keys);
	free_blocks(results);

	return time;
}

double get_time_c(size_t packets_size, size_t packets_number) {
	std::vector<byte*> inputs = get_blocks(packets_size, 32);
	LARGE_INTEGER point1, point2;
	double time = 0;
	byte control = 0; // чтобы не вызовы функции не заоптимизировались

	for (size_t j = 0; j < packets_number; j++) {
		QueryPerformanceCounter(&point1);
		for (size_t i = 0; i < packets_size; i++) {
			E(inputs[i]);
		}
		QueryPerformanceCounter(&point2);
		for (size_t i = 0; i < packets_size; i++) {
			control += inputs[i][0];
		}
		time += ((double) point2.QuadPart - point1.QuadPart) / get_frequency();
	}
	
	//printf("Func1: ps = %i, pn = %i, time: %f, ctrl: %c", packets_size, packets_number, time, control);
	printf("%c", control);
	free_blocks(inputs);

	return time;
}

void main() {

	printf("Control values: ");

	size_t sizes[] = { 50, 100, 150, 200, 250, 300, 350, 400, 450, 600 };

	size_t row_length = sizeof(sizes)/sizeof(size_t);
	size_t rows_num = 10;
	size_t call_times = 100;

	std::vector< std::vector<double> > rows1(rows_num, std::vector<double>(row_length));
	std::vector< std::vector<double> > rows2(rows_num, std::vector<double>(row_length));
	std::vector< std::vector<double> > rows_m(rows_num, std::vector<double>(row_length));

	for (size_t i = 0; i < rows_num; i++) {
		for (size_t p = 0; p < row_length; p++) {
			rows1[i][p] = get_time1(sizes[p], call_times);
			rows2[i][p] = get_time2(sizes[p], call_times);
			rows_m[i][p] = get_time_c(sizes[p], call_times);
		}
	}

	printf("\n");
	printf("Row length: %i.\n", row_length);
	printf("Rows number: %i.\n", rows_num);
	printf("Calls: %i.\n", call_times);

	printf("x = [ ");
	for (size_t i = 0; i < row_length; i++) {
		printf("%i, ", sizes[i]);
	}
	printf("]\n\n");

	for (size_t i = 0; i < rows_num; i++) {
		printf("Alg1.append( (", i);
		for (size_t p = 0; p < row_length; p++) {
				printf("%f, ", rows1[i][p]);
		}
		printf(") )\n");
	}

	printf("\n");
	for (size_t i = 0; i < rows_num; i++) {
		printf("Alg2.append( (", i);
		for (size_t p = 0; p < row_length; p++) {
				printf("%f, ", rows2[i][p]);
		}
		printf(") )\n");
	}

	printf("\n");
	for (size_t i = 0; i < rows_num; i++) {
		printf("MyAlg.append( (", i);
		for (size_t p = 0; p < row_length; p++) {
				printf("%f, ", rows_m[i][p]);
		}
		printf(") )\n");
	}


	//double t1 = get_time1(10, 1000);
	//double t2 = get_time2(10, 1000);
	//double t3 = get_time_c(10, 1000);

	//printf("\nResult: t1 = %f, t2 = %f, t3 = %f.\n", t1, t2, t3);


	//printf("Time brief: %lf secs.\n", diff1);
	//printf("Time expn1: %lf secs.\n", diff2);
	//printf("Time expn2: %lf secs.\n", diff3);
	//system("pause");
}

