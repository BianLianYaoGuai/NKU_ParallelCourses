#include <iostream>
#include <time.h>
#include <stdio.h>
#include <cmath>
#include <arm_neon.h>
using namespace std;

const int N=1024;
float m[N][N];
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
void lu_neon(float A[][N]){
    for (int k=0;k<N;k++){
        float32x4_t v = vdupq_n_f32(A[k][k]);

        int j=k+1;
        for(;j+7<N;j+=8){
            float32x4_t va1 = vld1q_f32(A[k]+j);
            float32x4_t va2 = vld1q_f32(A[k]+j+4);
            va1 = vdivq_f32(va1,v);
            va2 = vdivq_f32(va2,v);
            vst1q_f32(A[k]+j,va1);
            vst1q_f32(A[k]+j+4,va2);
        }
        for(;j<N;j++){
            A[k][j]=A[k][j]/A[k][k];
        }
        A[k][k]=1;
        for(int i=k+1;i<N;i++){
            float32x4_t vaik = vdupq_n_f32(A[i][k]);
            int j=k+1;
            for(;j+7<N;j+=8){
                float32x4_t vakj1 = vld1q_f32(A[k]+j);
                float32x4_t vakj2 = vld1q_f32(A[k]+j+4);
                float32x4_t vaij1 = vld1q_f32(A[i]+j);
                float32x4_t vaij2 = vld1q_f32(A[i]+j+4);
                float32x4_t vx1 = vmulq_f32(vakj1,vaik);
                float32x4_t vx2 = vmulq_f32(vakj2,vaik);
                vaij1 = vsubq_f32(vaij1,vx1);
                vaij2 = vsubq_f32(vaij2,vx2);
                vst1q_f32(A[i]+j,vaij1);
                vst1q_f32(A[i]+j+4,vaij2);
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
        lu_neon(m_2);
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
