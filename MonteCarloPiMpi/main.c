#include <mpi.h>
#include <stdio.h>
#include <time.h>
#include <stdlib.h>

#define ROOT_PROCESS 0
#define ull unsigned long long int

int main(void) {
    int comm_sz;
    int my_rank;

    time_t t;
    srand((unsigned) time(&t));

    ull totalTosses;
    ull circleTosses = 0;
    ull circleTosses0 = 0;

    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0) {
        printf("Enter the total number of tosses:\n");
        scanf("%llu", &totalTosses);
    }
    MPI_Bcast(&totalTosses, 1, MPI_UNSIGNED_LONG_LONG, ROOT_PROCESS, MPI_COMM_WORLD);

    ull remainder = totalTosses % comm_sz;
    ull per = totalTosses / comm_sz;
    ull max = (my_rank + 1) * per;
    if (my_rank < remainder) max += 1;
    for (ull i = my_rank * per; i < max; i++) {
        long double x = 2 * (rand() * (long double) 1.0 / RAND_MAX) - 1;
        long double y = 2 * (rand() * (long double) 1.0 / RAND_MAX) - 1;
        if (x * x + y * y <= 1) circleTosses += 1;
    }

    MPI_Reduce(&circleTosses, &circleTosses0, 1, MPI_UNSIGNED_LONG_LONG, MPI_SUM, ROOT_PROCESS, MPI_COMM_WORLD);

    if (my_rank == 0) {
        printf("pi estimate = %Lf\n", 4 * circleTosses0 / (long double) totalTosses);
    }
    MPI_Finalize();
    return 0;
}
