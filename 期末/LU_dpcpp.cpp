#include <iostream>
#include <cmath>
#include <CL/sycl.hpp>

using namespace sycl;
using namespace std;

void m_ini(float **m,int N);
void m_reset(float **m,int N);
void lu(float **m,int N);

int main(int argc, char **argv){
    float **m;
    int N = 1024;
    m = new float*[N];
    for(int i=0;i<N;i++)
        m[i] = new float[N];
    m_ini(m,N);
    m_reset(m,N);
    lu(m,N);
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++)
            std::cout<<m[i][j]<<' ';
        std::cout<<std::endl;
    }
    for(int i=0;i<N;i++)
        delete[] m[i];
    delete[] m;
    return 0;
}
void lu(float **A, int N) {
    queue q(sycl::gpu_selector_v);
    range<2> range(N, N);
    buffer<float, 2> bufA(reinterpret_cast<float *>(A[0]), range);
    for (int k=0;k<N;k++){
        for (int j=k+1;j<N;j++){
            A[k][j]=A[k][j]/A[k][k];
        }
        A[k][k]=1;
        q.submit([&](handler &h) {
            auto accA = bufA.get_access<access::mode::read_write>(h);
            h.parallel_for(range, [=](id<2> idx) {
                int i = idx[0];
                int j = idx[1];
                if(i>k && j>k)
                    accA[i][j]=accA[i][j]-accA[i][k]*accA[k][j];
            });
        }).wait();
        for(int i=k+1;i<N;i++){
            A[i][k]=0;
        }
    }
}
void m_ini(float **m,int N){
    for(int i=0;i<N;i++)
        for(int j=0;j<N;j++)
            m[i][j]=0;
    
    for(int i=0;i<N;i++){
        m[i][i]=1.0;
        for(int j=i+1;j<N;j++)
            m[i][j]=static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }
}
void m_reset(float **m,int N){
    for(int k=0;k<N;k++)
        for(int i=k+1;i<N;i++)
            for(int j=0;j<N;j++)
                m[i][j]+=(m[k][j]/pow(2,k));
}
