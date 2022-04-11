#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>
#include <string.h>

#define RMAX 1000
#define ROOT_PROCESS 0

void Generate_list(int local_A[], int local_n, int my_rank) {
    srandom(my_rank + 1);
    for (int i = 0; i < local_n; i++)
        local_A[i] = random() % RMAX;
}

void print_array(int a[], int n) {
    for (int i = 0; i < n; i++) {
        printf("%d", a[i]);
        if (i < n - 1) {
            printf(" ");
        }
    }
}

void print_p_array(int my_rank, int a[], int n) {
    printf("%d: ", my_rank);
    print_array(a, n);
    printf("\n");
}

void merge_sort_merge(int a[], int l, int m, int r) {
    int p, p1, p2;
    int *tmp = malloc(sizeof(int) * (r - l + 1));
    for (p = 0, p1 = l, p2 = m + 1; p1 <= m && p2 <= r;) {
        if (a[p1] < a[p2]) {
            tmp[p++] = a[p1++];
        } else {
            tmp[p++] = a[p2++];
        }
    }
    while (p1 <= m) {
        tmp[p++] = a[p1++];
    }
    while (p2 <= r) {
        tmp[p++] = a[p2++];
    }
    memcpy(a + l, tmp, sizeof(int) * (r - l + 1));
    free(tmp);
}

void merge_sort(int a[], int l, int r) {
    if (l >= r) {
        return;
    }
    int m = l + (r - l) / 2;
    merge_sort(a, l, m);
    merge_sort(a, m + 1, r);
    merge_sort_merge(a, l, m, r);
}

void internal_sort(int a[], int n) {
    merge_sort(a, 0, n - 1);
}

void merge_arrays(int *a, int *b, int n) {
    int *tmp = malloc(sizeof(int) * n * 2);
    int p = 0, p1 = 0, p2 = 0;
    while (p1 < n && p2 < n) {
        if (a[p1] < b[p2]) {
            tmp[p++] = a[p1++];
        } else {
            tmp[p++] = b[p2++];
        }
    }
    while (p1 < n) {
        tmp[p++] = a[p1++];
    }
    while (p2 < n) {
        tmp[p++] = b[p2++];
    }
    memcpy(a, tmp, sizeof(int) * n);
    memcpy(b, tmp + n, sizeof(int) * n);
    free(tmp);
}

int main() {
    int comm_sz;
    int my_rank;
    int local_n;
    int n;
    MPI_Init(NULL, NULL);
    MPI_Comm_size(MPI_COMM_WORLD, &comm_sz);
    MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);

    if (my_rank == 0) {
        printf("Input the value of local_n:\n");
        scanf("%d", &n);
    }
    local_n = n / comm_sz;
    MPI_Bcast(&local_n, 1, MPI_INT, ROOT_PROCESS, MPI_COMM_WORLD);

    int *local_A = malloc(sizeof(int) * local_n);
    Generate_list(local_A, local_n, my_rank);

    if (my_rank != 0) {
        MPI_Send(local_A, local_n, MPI_INT, ROOT_PROCESS, 0, MPI_COMM_WORLD);
    } else {
        print_p_array(0, local_A, local_n);
        int *a = malloc(sizeof(int) * local_n);
        for (int q = 1; q < comm_sz; q++) {
            MPI_Recv(a, local_n, MPI_INT, q, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            print_p_array(q, a, local_n);
        }
        free(a);
    }

    internal_sort(local_A, local_n);
    int round_num = comm_sz;
    int *tmp_buf = malloc(sizeof(int) * local_n);
    while (round_num--) {
        // get partner
        int partner, idle;
        if (round_num % 2) {
            if (my_rank % 2) {
                partner = my_rank + 1;
                idle = 0;
            } else {
                partner = my_rank - 1;
                idle = 1;
            }
        } else {
            if (my_rank % 2) {
                partner = my_rank - 1;
                idle = 1;
            } else {
                partner = my_rank + 1;
                idle = 0;
            }
        }
        if (partner <= -1 || partner >= comm_sz) {
            continue;
        }

        if (idle) {
            // 空闲的进程
            MPI_Send(local_A, local_n, MPI_INT, partner, my_rank, MPI_COMM_WORLD);
            MPI_Recv(local_A, local_n, MPI_INT, partner, my_rank, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        } else {
            // 工作进程
            MPI_Recv(tmp_buf, local_n, MPI_INT, partner, partner, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            merge_arrays(local_A, tmp_buf, local_n);
            MPI_Send(tmp_buf, local_n, MPI_INT, partner, partner, MPI_COMM_WORLD);
        }
    }

    if (my_rank != 0) {
        MPI_Send(local_A, local_n, MPI_INT, ROOT_PROCESS, 0, MPI_COMM_WORLD);
    } else {
        int* a = malloc(sizeof(int) * local_n * comm_sz);
        memcpy(a, local_A, sizeof(int)*local_n);
        for (int q = 1; q < comm_sz; q++) {
            MPI_Recv(a + q * local_n, local_n, MPI_INT, q, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        printf("The global list is:\n");
        print_array(a, local_n * comm_sz);
        printf("\n");

        free(a);
    }

    free(tmp_buf);
    free(local_A);

    MPI_Finalize();

    return 0;
}
