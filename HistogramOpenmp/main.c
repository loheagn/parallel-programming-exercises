#include <omp.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* argv[]) {
    int b = atoi(argv[1]);
    int* counts = (int*)malloc(b * sizeof(int));
    for (int i = 0; i < b; i++) {
        counts[i] = 0;
    }
    int thread_count = atoi(argv[2]);
    double l = atof(argv[3]);
    double r = atof(argv[4]);
    double gap = (r - l) / b;
    int n;
    scanf("%d", &n);
    double* nums = (double*)malloc(n * sizeof(double));
    for (int i = 0; i < n; i++) {
        scanf("%lf", &nums[i]);
    }

#pragma omp parallel num_threads(thread_count)
    {
        int my_rank = omp_get_thread_num();
        int pre = n / thread_count;
        int start = my_rank * pre;
        int end = start + pre;
        for (int i = start; i < end; i++) {
            double a = nums[i];
            int index = (a - l) / gap;
#pragma omp critical
            counts[index]++;
        }
    }

    for (int i = 0; i < b; i++) {
        printf("From %lf to %lf: %d\n", l + i * gap, l + (i + 1) * gap,
               counts[i]);
    }
    return 0;
}