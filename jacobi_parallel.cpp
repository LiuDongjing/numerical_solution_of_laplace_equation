#include <iostream>
#include <vector>
#include <cmath>
#include <mpi.h>
#include <omp.h>
#include <assert.h>
#include <cstdio>
using namespace std;
#ifdef INFO
void print_array(float *data, int rows, int cols, int rank) {
    printf("\n***%d***\n", rank);
    for(int i = 0; i < rows; i++) {
        for(int j = 0; j < cols; j++) {
            printf("%7.3f ", data[i*cols+j]);
        }
        printf("\n");
    }
    printf("\n********\n");
}
#endif

float solve_part_equations(float *p0, float *pk, int rows, int cols) {
    float max_dif = -INFINITY;
    #pragma omp parallel for collapse(2) reduction (max:max_dif)
    for(int i = 1; i < rows-1; i++) {
        for(int j = 1; j < cols-1; j++) {
            float t = (p0[i*cols+j-1]+p0[i*cols+j+1]+p0[(i-1)*cols+j]+p0[(i+1)*cols+j])/4;
            pk[i*cols+j] = t;
            max_dif = max(max_dif, abs(t-p0[i*cols+j]));
        }
    }
    return max_dif;
}
void solve_equations(float* T, int length, float tolerance) {
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    float *Tn = NULL;
    if(world_rank == 0) {
        Tn = new float[length*length];
        memcpy(Tn, T, sizeof(float)*length*length);
    }
    assert(world_size <= length);
    // 为每个进程分配计算数据；发送时，行相邻的数据重叠两行；接收时数据无重叠
    int *sendcounts = new int[world_size];
    int *senddispls = new int[world_size];
    int *recvcounts = new int[world_size];
    int *recvdispls = new int[world_size];
    int d = length / world_size;
    sendcounts[0] = (d+2)*length;
    recvcounts[0] = d*length;
    senddispls[0] = 0;
    recvdispls[0] = length;
    for(int i = 1; i < world_size-1; i++) {
        sendcounts[i] = (d+2)*length;
        recvcounts[i] = d*length;
        senddispls[i] = d*length+senddispls[i-1];
        recvdispls[i] = d*length+recvdispls[i-1];
    }
    senddispls[world_size-1] = d*length+senddispls[world_size-2];
    recvdispls[world_size-1] = d*length+recvdispls[world_size-2];
    sendcounts[world_size-1] = length*length-senddispls[world_size-1];
    recvcounts[world_size-1] = length*length-recvdispls[world_size-1];
    
    float *part0 = new float[sendcounts[world_rank]];
    float *partk = new float[sendcounts[world_rank]];
    memcpy(part0, T+senddispls[world_rank], sizeof(float)*sendcounts[world_rank]);
    memcpy(partk, T+senddispls[world_rank], sizeof(float)*sendcounts[world_rank]);
    if(world_rank == 0) {
        printf("sendd\trecvd\tsendc\trecvc\n");
        for(int i = 0; i < world_size; i++) {
            printf("%d\t%d\t%d\t%d\n", senddispls[i], recvdispls[i], sendcounts[i], recvcounts[i]);
        }
        float *data0 = T;
        float *datak = Tn;
        while(true) {
            float max_dif = -INFINITY;

            for(int i = 1; i < world_size; i++) {
                MPI_Send(data0+senddispls[i], sendcounts[i],
                    MPI_FLOAT, i, 0, MPI_COMM_WORLD);
            }
            memcpy(part0, data0+senddispls[world_rank], sizeof(float)*sendcounts[world_rank]);
            max_dif = max(max_dif, solve_part_equations(
                part0, partk, sendcounts[world_rank]/length, length));
#ifdef INFO
            print_array(part0, sendcounts[world_rank]/length, length, world_rank);
#endif
            memcpy(datak+senddispls[world_rank], partk, sizeof(float)*sendcounts[world_rank]);
            for(int i = 1; i < world_size; i++) {
                MPI_Recv(datak+recvdispls[i], recvcounts[i],
                    MPI_FLOAT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                float dif = INFINITY;
                MPI_Recv(&dif, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                max_dif = max(max_dif, dif);
            }
            printf("%f --> %f\n", max_dif, tolerance);
            if(max_dif <= tolerance) {
                break;
            }
            else {
                swap(data0, datak);
            }
        }
        for(int i = 1; i < world_size; i++) {
            float t = 0;
            printf("Kill %d.\n", i);
            MPI_Send(&t, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
        }
        delete [] Tn;
    }
    else {
        float *datak = T;
        while(true) {
            MPI_Status status;
            int num;
            MPI_Probe(0, 0, MPI_COMM_WORLD, &status);
            MPI_Get_count(&status, MPI_INT, &num);
            if(num == 1) {
                break;
            }
            MPI_Recv(part0, sendcounts[world_rank],
                    MPI_FLOAT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
            float dif = solve_part_equations(
                part0, partk, sendcounts[world_rank]/length, length);
#ifdef INFO
            print_array(part0, sendcounts[world_rank]/length, length, world_rank);
#endif
            memcpy(datak+senddispls[world_rank], partk, sizeof(float)*sendcounts[world_rank]);
            MPI_Send(datak+recvdispls[world_rank], recvcounts[world_rank],
                    MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(&dif, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
}
