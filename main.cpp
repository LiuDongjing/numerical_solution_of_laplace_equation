#include <iostream>
#include <sstream>
#include <vector>
#include <cstdio>
#define C0 100
#define C1 100
#define C2 100
#define C3 0

using namespace std;

vector<vector<float> > &solve_equations(vector<vector<float> > &T, float tolerance=1e-3);

int main(int argc, char *argv[]) {
    int node_num = 10;//一行内部节点的数量
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
    vector<vector<float> > result = solve_equations(T);
    for(int i = 0; i < result.size(); i++) {
        for(int j = 0; j < result.size(); j++) {
            printf("%7.3f ", result[i][j]);
        }
        printf("\n");
    }
    return 0;
}
