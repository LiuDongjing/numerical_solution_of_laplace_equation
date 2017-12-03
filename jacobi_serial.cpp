#include <iostream>
#include <vector>
#include <cmath>
using namespace std;
void solve_equations(float* T, int length, float tolerance) {
    float *Tn = new float[length*length];
    for(int i = 0; i < length; i++) {
        for (int j = 0; j < length; j++) {
            Tn[i*length+j] = T[i*length+j];
        }
    }
    float *T0 = T, *Tk = Tn;
    int N = length - 1;
    while(true) {
        float max_dif = -INFINITY;
        for(int i = 1; i < N; i++) {
            for(int j = 1; j < N; j++) {
                float t = (T0[length*(i-1)+j]+
                        T0[length*(i+1)+j]+
                        T0[length*i+j-1]+
                        T0[length*i+j+1])/4;
                Tk[i*length+j] = t;
                max_dif = max(max_dif, abs(t-T0[i*length+j]));
            }
        }
        if(max_dif <= tolerance) {
            break;
        }
        else {
            swap(T0, Tk);
        }
    }
    delete [] Tn;
}
