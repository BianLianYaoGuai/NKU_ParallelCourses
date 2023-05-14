#include <iostream>
#include <cmath>
#include <string>
#include <cstring>
using namespace std;

const int N=256;
float m[N][N];
void m_ini();
void m_reset();

int main(){
    string str="./matrix/matrix_"+to_string(N)+"_";
    FILE* fp;
    for(int i=0;i<10;i++){
        m_ini();
        m_reset();
        cout<<"完成第"<<i+1<<"个矩阵的生成！"<<endl;
        //写入文件
        fp = fopen((str+to_string(i)+".txt").c_str(), "w");
        if(fp == NULL)
        {
            cout<<"Failed to open file!"<<endl;
            return -1;
        }
        for(int i = 0; i < N; i++)
        {
            for(int j = 0; j < N; j++)
            {
                fprintf(fp, "%lf ", m[i][j]);
            }
            fprintf(fp, "\n");
        }
        fclose(fp);
        cout<<"完成第"<<i+1<<"个矩阵的存储！"<<endl;
    }
    return 0;
}

void m_ini(){
    for(int i=0;i<N;i++)
        for(int j=0;j<N;j++)
            m[i][j]=0;
    
    for(int i=0;i<N;i++){
        m[i][i]=1.0;
        for(int j=i+1;j<N;j++)
            m[i][j]=static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }
}

void m_reset(){
    for(int k=0;k<N;k++)
        for(int i=k+1;i<N;i++)
            for(int j=0;j<N;j++)
                m[i][j]+=(m[k][j]/pow(2,k));
}
