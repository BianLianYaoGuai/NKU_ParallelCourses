#include <iostream>
#include <pthread.h>
#include <immintrin.h>
using namespace std;

const int N=256;
const float epsilon = 0.01;
float m[N][N];
float m_1[N][N];


void read_matrix(float A[][N],const char* filename);

void compare(float A[][N],float B[][N]);

const int THREAD_NUM = 2;// 双线程
typedef struct{
    float (*A)[N];
    int r;
}threadParm_t;

pthread_barrier_t barrier;

void* fun_thread_1(void* parm){
    threadParm_t* p=(threadParm_t *) parm;
    float (*A)[N]=p->A;
    int r=p->r;
    int k=0;
    for (;k<N;k++){
        int w = (N-1-k)/THREAD_NUM;
        int begin = r*w + (k+1);
        int last;
        if (r==THREAD_NUM-1)
            last = N;
        else
            last = (r+1)*w + (k+1);
        
        
        __m128 v = _mm_set1_ps(A[k][k]);
        int j=begin;
        for(;j<((begin)/4)*4+4&&j<last;j++){
            A[k][j]=A[k][j]/A[k][k];
        }
        for(;j+3<last;j+=4){
            __m128 va = _mm_load_ps(A[k]+j);
            va = _mm_div_ps(va,v);
            _mm_store_ps(A[k]+j,va);
        }
        for(;j<last;j++){
            A[k][j]=A[k][j]/A[k][k];
        }
        
        
        pthread_barrier_wait(&barrier);
        
        A[k][k]=1;
        for (int task=begin;task<last;task++){
            for(int j=k+1;j<N;j++){
                    A[task][j]=A[task][j]-A[task][k]*A[k][j];
            }
            A[task][k]=0;
        }
        
        pthread_barrier_wait(&barrier);
    }
    pthread_exit(nullptr);
}


void fun_pthread(float A[][N]){
    pthread_barrier_init(&barrier,NULL,THREAD_NUM);
    pthread_t thread[THREAD_NUM];
    threadParm_t threadParm[THREAD_NUM];
    for(int i=0;i<THREAD_NUM;i++){
        threadParm[i].A=A;
        threadParm[i].r=i;
    }
        
    for(int i=0;i<THREAD_NUM;i++){
        pthread_create(&thread[i],nullptr,fun_thread_1,(void*)&threadParm[i]);
    }
        
    for(int i=0;i<THREAD_NUM;i++){
        pthread_join(thread[i],nullptr);
    }
}

int main(){
    string str="./matrix/matrix_"+to_string(N)+"_";
    string str2="./matrix/matrix_output_"+to_string(N)+"_";
    for(int i=0;i<10;i++){
        read_matrix(m,(str+to_string(i)+".txt").c_str());
        fun_pthread(m);
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
                printf("结果不一致\n");
                return;
            }
        }
    }
    printf("结果一致\n");
    return;
}