#include <iostream>
#include <vector>
#include <cmath>
using namespace std;
vector<vector<float> > &solve_equations(vector<vector<float> > &T, float tolerance) {
    vector<vector<float> > Tn(T);
    vector<vector<float> > *T0 = &T, *Tk = &Tn;
    int N = T.size()-1;
    while(true) {
        float max_dif = -INFINITY;
        for(int i = 1; i < N; i++) {
            for(int j = 1; j < N; j++) {
                vector<vector<float> > &x = *T0;
                vector<vector<float> > &y = *Tk;
                float t = (x[i-1][j]+x[i+1][j]+x[i][j-1]+x[i][j+1])/4;
                y[i][j] = t;
                max_dif = max(max_dif, abs(t-x[i][j]));
            }
        }
        if(max_dif <= tolerance) {
            break;
        }
        else {
            swap(T0, Tk);
        }
    }
    return *Tk;
}
