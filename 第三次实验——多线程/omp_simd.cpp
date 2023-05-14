#include <iostream>
#include <omp.h>
using namespace std;

const int N=256;
const float epsilon = 0.0001;
float m[N][N];
float m_1[N][N];

void read_matrix(float A[][N],const char* filename);

void compare(float A[][N],float B[][N]);


void fun_omp(float A[][N]){
    for (int k=0;k<N;k++){
        #pragma omp simd
        for (int j=k+1;j<N;j++){
            A[k][j]/=A[k][k];
        }
        
        {
        A[k][k]=1;
            #pragma omp parallel for
        for (int i = k + 1; i < N; i++) {
            float tmp = A[i][k];
            for (int j = k + 1; j < N; j++) {
                A[i][j] -= tmp * A[k][j];
            }
            A[i][k] = 0;
            }
        }
    }
}

int main(){
    string str="./matrix/matrix_"+to_string(N)+"_";
    string str2="./matrix/matrix_output_"+to_string(N)+"_";
    for(int i=0;i<10;i++){
        read_matrix(m,(str+to_string(i)+".txt").c_str());
        fun_omp(m);
        read_matrix(m_1,(str2+to_string(i)+".txt").c_str());
        compare(m,m_1);
    }
}

void read_matrix(float A[][N],const char* filename){
    FILE* fp;
    fp = fopen(filename, "r");
    if(fp == NULL)
    {
        printf("Failed to open file!\n");
        return ;
    }
    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++)
        {
            fscanf(fp, "%f", &A[i][j]);
        }
    }
    fclose(fp);
}
void compare(float A[][N],float B[][N])
{
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            float diff = A[i][j] - B[i][j];
            if (diff < 0) {
                diff = -diff;
            }
            if (diff > epsilon) {
                printf("结果不一致!\n");
                return;
            }
        }
    }
    printf("结果一致!\n");
    return;
}