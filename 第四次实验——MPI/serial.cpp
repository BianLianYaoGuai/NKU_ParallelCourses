#include <iostream>
#include <iostream>
#include <cmath>
using namespace std;

void m_ini(float **m,int N);
void m_reset(float **m,int N);
void lu(float **m,int N);

int main(int argc, char **argv){
    if(argc<2)
        return -1;
    float **m;
    int N = atoi(argv[1]);
    m = new float*[N];
    for(int i=0;i<N;i++)
        m[i] = new float[N];
    m_ini(m,N);
    m_reset(m,N);
    lu(m,N);
    
    
    for(int i=0;i<N;i++){
        for(int j=0;j<N;j++)
            cout<<m[i][j]<<' ';
        cout<<endl;
    }
    
    for(int i=0;i<N;i++)
        delete[] m[i];
    delete[] m;
    
    return 0;
}

void lu(float **A,int N){
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

