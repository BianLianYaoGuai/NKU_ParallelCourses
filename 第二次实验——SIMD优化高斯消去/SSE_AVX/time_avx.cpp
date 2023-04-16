#include <iostream>
#include <time.h>
#include <stdio.h>
#include <cmath>
#include <immintrin.h>
using namespace std;

const int N=1024;
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
// ===================================================================高斯消元部分=======================================================================
// SIMD算法
void lu_avx(float A[][N]){
    for (int k=0;k<N;k++){
        __m128 v = _mm_set1_ps(A[k][k]);

        int j=k+1;
        for(;j<((k+1)/4)*4+4&&j<N;j++){
            A[k][j]=A[k][j]/A[k][k];
        }
        for(;j+3<N;j+=4){
            __m128 va = _mm_load_ps(A[k]+j);
            va = _mm_div_ps(va,v);
            _mm_store_ps(A[k]+j,va);
        }
        for(;j<N;j++){
            A[k][j]=A[k][j]/A[k][k];
        }

        A[k][k]=1;

        for(int i=k+1;i<N;i++){
            __m128 vaik = _mm_set1_ps(A[i][k]);
            int j=k+1;
            for(;j<((k+1)/4)*4+4&&j<N;j++){
                A[i][j]=A[i][j]-A[i][k]*A[k][j];
            }
            for(;j+3<N;j+=4){
                __m128 vakj = _mm_load_ps(A[k]+j);
                __m128 vaij = _mm_load_ps(A[i]+j);
                __m128 vx = _mm_mul_ps(vakj,vaik);
                vaij = _mm_sub_ps(vaij,vx);
                _mm_store_ps(A[i]+j,vaij);
            }
            for(;j<N;j++){
                A[i][j]=A[i][j]-A[i][k]*A[k][j];
            }
            A[i][k]=0;
        }
    }
}



int main()
{
// -----------初始化工作---------------------------------
    // 矩阵初始化
    m_ini();
    m_reset();
    for(int i=0;i<10;i++)
    {
        // 克隆矩阵
        m_clone(m_2);
// -----------------------------------------------------
        struct timespec sts,ets;
        timespec_get(&sts, TIME_UTC);

// -----------计时部分的工作-----------------------------

        // SIMD算法
        lu_avx(m_2);
// -----------------------------------------------------
        timespec_get(&ets, TIME_UTC);
        time_t dsec=ets.tv_sec-sts.tv_sec;
        long dnsec=ets.tv_nsec-sts.tv_nsec;
        if (dnsec<0){
            dsec--;
            dnsec+=1000000000ll;
        }
        printf ("%ld.%09lds\n",dsec,dnsec);
// -----------收尾工作-----------------------------------
            
// ------------------------------------------------------
    }
    return 0;
}
