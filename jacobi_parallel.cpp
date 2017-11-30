#include <iostream>
#include <vector>
#include <cmath>
#include <mpi.h>
#include <omp.h>
#include <assert.h>
using namespace std;
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

vector<vector<float> > &solve_equations(vector<vector<float> > &T, float tolerance) {
    int length = T.size();
    vector<vector<float> > Tn(T);
    vector<vector<float> > *T0 = &T, *Tk = &Tn;
    
    MPI_Init(NULL, NULL);

    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    // 为每个进程分配计算数据；发送时，行相邻的数据重叠两行；接收时数据无重叠
    int *sendcounts = new int[world_size];
    int *senddispls = new int[world_size];
    int *recvcounts = new int[world_size];
    int *recvdispls = new int[world_size];
    int d = length / world_size;
    sendcounts[0] = d*length+length;
    recvcounts[0] = d*length;
    senddispls[0] = 0;
    recvdispls[0] = 0;
    for(int i = 1; i < world_size-1; i++) {
        sendcounts[i] = (d+2)*length;
        recvcounts[0] = d*length;
        if(i == 1)
            senddispls[i] = (d-1)*length;
        else {
            senddispls[i] = d*length+senddispls[i-1];
        }
        recvdispls[i] = d*length+recvdispls[i-1];
    }
    senddispls[world_size-1] = d*length+senddispls[world_size-2];
    recvdispls[world_size-1] = d*length+recvdispls[world_size-2];
    sendcounts[world_size-1] = length*length-senddispls[world_size-1];
    recvcounts[world_size-1] = length*length-recvdispls[world_size-1];
    
    float *part0 = new float[sendcounts[world_rank]];
    float *partk = new float[sendcounts[world_rank]];
    if(world_rank == 0) {
        while(true) {
            float max_dif = -INFINITY;
            float *data0 = &((*T0)[0][0]);
            float *datak = &((*Tk)[0][0]);
            for(int i = 1; i < world_size; i++) {
                MPI_Send(data0+senddispls[i], sendcounts[i],
                    MPI_FLOAT, i, 0, MPI_COMM_WORLD);
            }
            memcpy(data0+senddispls[world_rank], part0, sizeof(float)*sendcounts[world_rank]);
            max_dif = max(max_dif, solve_part_equations(
                part0, partk, sendcounts[world_rank]/length, length));
            memcpy(datak+senddispls[world_rank], partk, sizeof(float)*sendcounts[world_rank]);
            for(int i = 1; i < world_size; i++) {
                MPI_Recv(datak+recvdispls[i], recvcounts[i],
                    MPI_FLOAT, world_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                float dif = INFINITY;
                MPI_Recv(&dif, 1, MPI_FLOAT, world_rank, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
                max_dif = max(max_dif, dif);
            }
            if(max_dif <= tolerance) {
                break;
            }
            else {
                swap(T0, Tk);
            }
        }
        for(int i = 1; i < world_size; i++) {
            float t = 0;
            MPI_Send(&t, 1, MPI_FLOAT, i, 0, MPI_COMM_WORLD);
        }
    }
    else {
        float *datak = &((*Tk)[0][0]);
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
            memcpy(datak+senddispls[world_rank], partk, sizeof(float)*sendcounts[world_rank]);
            MPI_Send(datak+recvdispls[world_rank], recvcounts[world_rank],
                    MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
            MPI_Send(&dif, 1, MPI_FLOAT, 0, 0, MPI_COMM_WORLD);
        }
    }
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Finalize();
    return T;
}