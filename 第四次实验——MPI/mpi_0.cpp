#include <iostream>
#include <mpi.h>
#include <cmath>
using namespace std;


void m_ini(float **m,int N);
void m_reset(float **m,int N);
void mpi_lu(float **m,int N);


int main(int argc, char **argv){
    if(argc<2)
        return -1;
    MPI_Init(&argc, &argv);
    float **m;
    int N = atoi(argv[1]);
    m = new float*[N];
    for(int i=0;i<N;i++)
        m[i] = new float[N];
    
    m_ini(m,N);
    m_reset(m,N);
    mpi_lu(m,N);
    
    int rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    if (rank==0){
        for(int i=0;i<N;i++){
            for(int j=0;j<N;j++)
                cout<<m[i][j]<<' ';
            cout<<endl;
        }
    }
    for(int i=0;i<N;i++)
        delete[] m[i];
    delete[] m;
    
    MPI_Finalize();
    return 0;
}



void mpi_lu(float **A,int N){
    int num, rank;
    MPI_Comm_size(MPI_COMM_WORLD, &num);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    int r1 = N/num*rank;
    int r2 ;
    if (rank!=num-1)
        r2 = N/num*(rank+1)-1;
    else
        r2 = N-1;
    for(int k=0;k<N;k++){
        if(r1<=k && k<=r2){
            for(int j=k+1;j<N;j++)
                A[k][j] = A[k][j] / A[k][k];
            A[k][k] = 1;
            for(int j=0;j<num;j++){
                if (j!=rank)
                    MPI_Send(&A[k][0], N, MPI_FLOAT , j, 0, MPI_COMM_WORLD);
            }
        }
        else{
            MPI_Recv(&A[k][0], N, MPI_FLOAT , MPI_ANY_SOURCE, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        }
        if(r2-1>=k){
            int start =(r1 < k+1)? k+1 : r1;
            for(int i=start;i<=r2;i++){
                for(int j=k+1;j<N;j++)
                    A[i][j] = A[i][j] - A[k][j] * A[i][k];
                A[i][k] = 0;
            }
        }
        for(int i=N;i<k;i--){
            int from=0;
            while(N/num*from<i)
                from++;
            if(from<num)
                MPI_Bcast(&A[i][0], N, MPI_FLOAT, from, MPI_COMM_WORLD);
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
