#include <iostream>
#include <stdio.h>
using namespace std;

const int N=512;
float m[N][N];
void read_matrix(float [][N],const char*);
void write_matrix(float [][N],const char*);
void lu(float [][N]);
int main(){
    
    string str="./matrix/matrix_"+to_string(N)+"_";
    string str_out="./matrix/matrix_output_"+to_string(N)+"_";
    FILE* fp;
    for(int i=0;i<10;i++){
        read_matrix(m,(str+to_string(i)+".txt").c_str());
        cout<<"完成第"<<i+1<<"个矩阵的读取！"<<endl;
        lu(m);
    
        cout<<"完成第"<<i+1<<"个矩阵的生成！"<<endl;
        write_matrix(m,(str_out+to_string(i)+".txt").c_str());
        
        cout<<"完成第"<<i+1<<"个矩阵的存储！"<<endl;
    }
}

void read_matrix(float A[][N],const char* filename){
    //读取文件
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

void write_matrix(float A[][N],const char* filename){
    //读取文件
    FILE* fp;
    fp = fopen(filename, "w");
    if(fp == NULL)
    {
        printf("Failed to open file!\n");
        return ;
    }
    for(int i = 0; i < N; i++)
    {
        for(int j = 0; j < N; j++)
        {
            fprintf(fp, "%f ", A[i][j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);
}

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