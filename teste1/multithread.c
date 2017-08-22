//
//  multithread.c
//
//  Created by Julianny Favinha Donda
//  RA 156059
//  MC714 2s 017
//

//
// Fontes
// https://stackoverflow.com/questions/7918529/measuring-time-of-multi-threading-program
// https://stackoverflow.com/questions/876605/multiple-child-process
//

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <pthread.h>
#include <sys/time.h>
//#include <ctime>

int N, k;
long int *random_elements;
static long int sum = 0;
pthread_mutex_t sum_lock;

// returns the time of day in miliseconds
double my_clock() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (double) (t.tv_usec/1E3 + t.tv_sec*1E3);
}

/*
	each thread is responsible for 
	-> creating (N * 2^20)/k random numbers * 
	-> calculating the sum of these numbers 

	* except the last thread that creates (N * 2^20) - (k-1)*((N * 2^20)/k) numbers */
void* f_thread(void *v) {
	int i, id = *(int*) v;
	int inicio, fim;
	long int partial_sum = 0;
	unsigned int seed;
	
	if (id < k-1) {
		inicio = id*(N/k);
		fim = id*(N/k) + N/k;
	}
	else {
		// última thread. se N/k não é exato, pega os elementos que "sobraram"
		inicio = id*(N/k);
		fim = N;
	}

	/*std::clock_t start;
	start = clock();*/

	for (i = inicio; i < fim; i++) {
		partial_sum += random_elements[i] = (long int) rand_r(&seed) % 1001;
	}

	/*printf("for thread %d %ld\n", id, (clock() - start)/CLOCKS_PER_SEC);*/

	pthread_mutex_lock(&sum_lock);
	sum += partial_sum;
	pthread_mutex_unlock(&sum_lock);

  	return NULL; 
}

int main(int argc, char *argv[]) {
    double start_time, end_time;
    
    start_time = my_clock();
    
    int i, *id_thread;
	pthread_t *threads;

    N = atoi(argv[1]);
    k = atoi(argv[2]);

	// N Mega numbers
	N = N * pow(2, 20);

	pthread_mutex_init(&sum_lock, NULL);

	// allocates N Mega 64-bit integers
	random_elements = (long int *)malloc(N * sizeof(long int));
	
	// allocates k threads e k ids
	threads = (pthread_t *)malloc(k * sizeof(pthread_t));
  	id_thread = (int *)malloc(k * sizeof(int));

	// creates k threads  
    for (i = 0; i < k; i++) {
        id_thread[i] = i;
        pthread_create(&threads[i], NULL, f_thread, (void*) &id_thread[i]);
    }

	// waits for k threads
  	for (i = 0; i < k; i++) {
    	pthread_join(threads[i], NULL);
    }

    // calculates the average of the random numbers
    printf("Média = %lf\n", (double)sum / (double)N);
    
	free(random_elements);
	free(threads);
	free(id_thread);
    
    end_time = my_clock();
    
    printf("Tempo %ld ms\n", (long int) ((end_time - start_time)));

    return 0;
}
