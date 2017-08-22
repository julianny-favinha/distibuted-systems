//
//  processo.c
//
//  Created by Julianny Favinha Donda
//  RA 156059
//  MC714 2s 017
//


#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/time.h>

// returns the time of day in miliseconds
double my_clock() {
    struct timeval t;
    gettimeofday(&t, NULL);
    return (t.tv_usec/1E3 + t.tv_sec*1E3);
}

int main(int argc, char *argv[]) {
    double start_time, end_time;
    
    start_time = my_clock();
    
	int i, N, k;
    int inicio, fim;
	pid_t pid;
    unsigned long sum = 0;

    N = atoi(argv[1]);
    k = atoi(argv[2]);

	// N Mega numbers
	N = N * pow(2, 20);

    // seed for the rand() function
    srand(time(NULL));

	const char *name = "mem_share";
	int mem = shm_open(name, O_CREAT | O_RDWR, 0666);
	ftruncate(mem, N*8);
	//int status;
	void *ptr = mmap(0, N*8, PROT_READ | PROT_WRITE, MAP_SHARED, mem, 0);
	long *ptr_random_numbers = (long*) ptr;
    
    const char *name2 = "mem_share_sum";
    int mem2 = shm_open(name2, O_CREAT | O_RDWR, 0666);
    ftruncate(mem2, k*8);
    void *ptr2 = mmap(0, k*8, PROT_READ | PROT_WRITE, MAP_SHARED, mem2, 0);
    unsigned long *ptr_cumulative_sum = (unsigned long*) ptr2;

	// fork to k processes
	for (i = 0; i < k; i++) {
		if ((pid = fork()) == 0) {
     		if (i < k-1) {
				inicio = i*(N/k);
				fim = i*(N/k) + N/k;
			}
			else {
				// último processo. se N/k não é exato, pega os elementos que "sobraram"
				inicio = i*(N/k);
				fim = N;
			}
            
            // calculates partial sum
            ptr_cumulative_sum[i] = 0;
            for (int j = inicio; j < fim; j++) {
                ptr_cumulative_sum[i] += ptr_random_numbers[i] = rand() % 1001;
            }
            
            exit(0);
        }
	}
    
    // wait for k processes
    int status_death, k2 = k;
    while(k2 > 0) {
        wait(&status_death);
        k2--;
    }

    // calculates the total sum of the parcial sums
    for(i = 0; i < k; i++) {
        sum += ptr_cumulative_sum[i];
    }
    
    // calculates the average of the random numbers
    printf("Média = %lf\n", (double)sum / (double)N);

	shm_unlink("mem_share");
    shm_unlink("mem_share_sum");
    
    end_time = my_clock();
    
    printf("Tempo %ld ms\n", (long int) (end_time - start_time));
    
    return 0;
}
