#include <iostream>
#include <cmath>
#include <immintrin.h>


using namespace std;

const int N=64;
const float epsilon = 0.01;
float m[N][N];
float m_1[N][N];
float m_2[N][N];

// ===================================================================矩阵部分=======================================================================
// 初始化上三角矩阵
void m_ini(){
    for(int i=0;i<N;i++){
        m[i][i]=1.0;
        for(int j=i+1;j<N;j++)
            m[i][j]=static_cast<float>(rand()) / static_cast<float>(RAND_MAX);//rand(1);
    }
}
// 破坏矩阵的上三角性
void m_reset(){
    for(int k=0;k<N;k++)
        for(int i=k+1;i<N;i++)
            for(int j=0;j<N;j++)
                m[i][j]+=(m[k][j]/pow(2,k));
}
// 克隆矩阵
void m_clone(float A[][N]){
    for(int k=0;k<N;k++)
        for(int j=0;j<N;j++)
            A[k][j]=m[k][j];
}
// 对比矩阵
void compare(float A[][N],float B[][N])
{
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            float diff = A[i][j] - B[i][j];
            if (diff < 0) {
                diff = -diff;
            }
            if (diff > epsilon) {
                cout<<"结果不一致\n";
                return;
            }
        }
    }
    cout<<"结果一致\n";
    return;
}
// 展示矩阵
void display(float A[][N]){
    for(int i=0;i<N;i++)
    {
        for(int j=0;j<N;j++)
            cout<<A[i][j]<<' ';
        cout<<'\n';
    }
}

// ===================================================================高斯消元部分=======================================================================
// 串行算法
void lu(float A[][N]){
    for (int k=0;k<N;k++){
        for (int j=k+1;j<N;j++){
            A[k][j]=A[k][j]/A[k][k];
        }
        A[k][k]=1;
        for(int i=k+1;i<N;i++){
            for(int j=k+1;j<N;j++){
                A[i][j]=A[i][j]-A[i][k]*A[k][j];
            }
            A[i][k]=0;
        }
    }
}
// SIMD算法
void lu_avx(float A[][N]){
    for (int k=0;k<N;k++){
        __m256 v = _mm256_set1_ps(A[k][k]);

        int j=k+1;
        for(;j+8<N+1;j+=8){
            __m256 va = _mm256_loadu_ps(A[k]+j);
            va = _mm256_div_ps(va,v);
            _mm256_storeu_ps(A[k]+j,va);
        }
        for(;j<N;j++){
            A[k][j]=A[k][j]/A[k][k];
        }


        A[k][k]=1;


        for(int i=k+1;i<N;i++){
            __m256 vaik = _mm256_set1_ps(A[i][k]);
            int j=k+1;
            for(;j+8<N+1;j+=8){
                __m256 vakj = _mm256_loadu_ps(A[k]+j);
                __m256 vaij = _mm256_loadu_ps(A[i]+j);
                __m256 vx = _mm256_mul_ps(vakj,vaik);
                vaij = _mm256_sub_ps(vaij,vx);
                _mm256_storeu_ps(A[i]+j,vaij);
            }
            for(;j<N;j++){
                A[i][j]=A[i][j]-A[i][k]*A[k][j];
            }
            A[i][k]=0;
        }
    }
}
// ===================================================================主函数=======================================================================
int main(){
    // 矩阵初始化
    m_ini();
    m_reset();
    // display(m);

    // 克隆矩阵
    m_clone(m_1);
    m_clone(m_2);

    // 串行算法
    lu(m_1);
    // display(m_1);

    // SIMD算法
    lu_avx(m_2);
    // display(m_2);

    // 对比结果
    compare(m_1,m_2);
}
