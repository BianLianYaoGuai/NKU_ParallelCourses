#include <iostream>
#include <time.h>
#include <stdio.h>
#include <cmath>

using namespace std;

const int N=1024;
float m[N][N];
float m_1[N][N];

// ===================================================================矩阵部分=======================================================================
// 初始化上三角矩阵
void m_ini(){
    for(int i=0;i<N;i++){
        m[i][i]=1.0;
        for(int j=i+1;j<N;j++)
            m[i][j]=static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
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
// ===================================================================主函数=======================================================================
int main()
{
// -----------初始化工作---------------
    // 矩阵初始化
    m_ini();
    m_reset();
    for(int i=0;i<10;i++){
    // 克隆矩阵
    m_clone(m_1);

// -----------------------------------
   struct timespec sts,ets;
    timespec_get(&sts, TIME_UTC);

// -----------计时部分的工作-----------

    // 串行算法
    lu(m_1);


// -----------------------------------
    timespec_get(&ets, TIME_UTC);
    time_t dsec=ets.tv_sec-sts.tv_sec;
    long dnsec=ets.tv_nsec-sts.tv_nsec;
    if (dnsec<0){
        dsec--;
        dnsec+=1000000000ll;
    }
    printf ("%ld.%09lds\n",dsec,dnsec);
// -----------收尾工作-----------------

    }

// -----------------------------------
    return 0;
}
