//
//  sequencial.c
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
    
    int i, N;
    int64_t *random_elements, cumulative_sum = 0;
    
    N = atoi(argv[1]);
    
    // allocate N Mega 64-bit integers
    random_elements = malloc(N * pow(2, 20) * sizeof(int64_t));
    
    // seed for the rand() function
    srand(time(NULL));
    
    // generate N Mega 64-bit numbers between 0 and 1000
    for (i = 0; i < N * pow(2, 20); i++) {
        cumulative_sum += random_elements[i] = rand() % 1001;
    }

    // calculates the average of the random numbers
    printf("Média = %lf\n", (double)cumulative_sum / (double) (N * pow(2, 20)));
    
    free(random_elements);
    
    end_time = my_clock();
    
    printf("Tempo %ld ms\n", (long int) (end_time - start_time));

    return 0;
}
