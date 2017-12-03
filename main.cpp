#include <iostream>
#include <sstream>
#include <cstdio>
#include <ctime>
#include <assert.h>

#ifdef PARALLEL
#include <mpi.h>
#endif

#define OUTPUT_FILE "result.txt"

using namespace std;

void solve_equations(float* T, int length, float tolerance=1e-3);
int str2int(char *chs) {
    int t;
    stringstream cvt;
    cvt << chs;
    cvt >> t;
    return t;
}
int main(int argc, char *argv[]) {
    int node_num = 10;//一行内部节点的数量
    int C0 = 100, C1 = 100, C2 = 100, C3 = 0;
#ifdef PARALLEL
    MPI_Init(&argc, &argv);
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    if(world_rank == 0) {
        printf("size: %d.\n", world_size);
    }
#endif
    if(argc >= 2) {
        node_num = str2int(argv[1]);
        if(argc >= 6) {
            C0 = str2int(argv[2]);
            C1 = str2int(argv[3]);
            C2 = str2int(argv[4]);
            C3 = str2int(argv[5]);
        }
    }
    // 
    int length = node_num + 2;
    float *T = new float[length*length];
    for(int i = 0; i < length; i++) {
        for(int j = 0; j < length; j++) {
            T[i*length+j] = 0;
        }
    }
    for(int j=0; j < length; j++) T[j] = C0;
    for(int i=0; i < length; i++) T[i*length] = C1;
    for(int j=0; j < length; j++) T[(node_num+1)*length+j] = C2;
    for(int i=0; i < length; i++) T[i*length+node_num+1] = C3;
    const clock_t begin_time = clock();
#ifdef PARALLEL
    if(world_rank == 0) {
#endif
        printf("\nNodes: %d, C0: %d, C1: %d, C2: %d, C3: %d.\n",
        node_num, C0, C1, C2, C3);
#ifdef INFO
        for(int i = 0; i < length; i++) {
            for(int j = 0; j < length; j++) {
                printf("%7.3f ", T[i*length+j]);
            }
            printf("\n");
        }
#endif
        solve_equations(T, length);
	FILE *fh = fopen(OUTPUT_FILE, "w");
        for(int i = 0; i < length; i++) {
            for(int j = 0; j < length; j++) {
                fprintf(fh, "%7.3f ", T[i*length+j]);
            }
            fprintf(fh, "\n");
        }
	fclose(fh);
        printf("\nTotal time cost: %0.2f.\n", float(clock()-begin_time) / CLOCKS_PER_SEC);
#ifdef PARALLEL
    }
    else {
        solve_equations(T, length);
    }
#endif
    return 0;
}
