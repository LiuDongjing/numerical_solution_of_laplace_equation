#include <iostream>
#include <sstream>
#include <vector>
#include <cstdio>

#ifdef PARALLEL
#include <mpi.h>
#endif

#define C0 100
#define C1 100
#define C2 100
#define C3 0

using namespace std;
#ifdef PARALLEL
void solve_equations(float* T, int length, float tolerance=1e-3);
#else
vector<vector<float> > &solve_equations(vector<vector<float> > &T, float tolerance=1e-3);
#endif

int main(int argc, char *argv[]) {
    int node_num = 10;//一行内部节点的数量
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
    if(argc > 1) {
        stringstream cvt;
        cvt << argv[1];
        cvt >> node_num;
    }
    //在内部节点外加一层边界节点，并初始化边界节点
    vector<vector<float> > T(node_num+2, vector<float>(node_num+2, 0));
    for(int j=1; j < node_num+1; j++) T[0][j] = C0;
    for(int i=1; i < node_num+1; i++) T[i][0] = C1;
    for(int j=1; j < node_num+1; j++) T[node_num+1][j] = C2;
    for(int i=1; i < node_num+1; i++) T[i][node_num+1] = C3;
#ifdef PARALLEL
    int length = T.size();
    float *T0 = new float[length*length];
    for(int i = 0; i < length; i++) {
        for(int j = 0; j < length; j++) {
            T0[i*length+j] = T[i][j];
        }
    }
    if(world_rank == 0) {
        solve_equations(T0, length);
        for(int i = 0; i < length; i++) {
            for(int j = 0; j < length; j++) {
                printf("%7.3f ", T0[i*length+j]);
            }
            printf("\n");
        }
    }
    else {
        solve_equations(T0, length);
    }
#else
    vector<vector<float> > result = solve_equations(T);
    for(int i = 0; i < result.size(); i++) {
        for(int j = 0; j < result.size(); j++) {
            printf("%7.3f ", result[i][j]);
        }
        printf("\n");
    }
#endif
    return 0;
}
