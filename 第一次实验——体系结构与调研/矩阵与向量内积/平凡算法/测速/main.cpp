#include <iostream>
#include <windows.h>
#include <stdlib.h>

using namespace std;

const int N = 512;

double b[N][N], sum[N], a[N];

void init()
{
    for (int i = 0; i < N; i++)
    {
        a[i] = i;
        for (int j = 0; j < N; j++)
            b[i][j] = i + j;
    }
}

int main()
{
    init();
    //------------------------------------------------------------------------------------------
    double aveTime = 0;
    int repeatTimes = 500;
    for (int k = 0; k < repeatTimes; k++)
    {
        long long head, tail, freq;
        QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
        QueryPerformanceCounter((LARGE_INTEGER *)&head);

        //=======================================================================================
        for (int i = 0; i < N; i++)
        {
            sum[i] = 0.0;
            for (int j = 0; j < N; j++)
                sum[i] += b[j][i] * a[j];
        }
        //=======================================================================================

        QueryPerformanceCounter((LARGE_INTEGER *)&tail);
        //        cout << "Col:"<< (tail-head)*1000.0 /freq << "ms"<< endl ;
        aveTime += (tail - head) * 1000.0 / freq;
    }
    //------------------------------------------------------------------------------------------
    cout << "Col:" << aveTime / repeatTimes << "ms" << endl;
    return 0;
}
