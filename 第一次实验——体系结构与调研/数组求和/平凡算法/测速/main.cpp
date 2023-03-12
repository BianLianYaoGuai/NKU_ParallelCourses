#include <iostream>
#include <windows.h>
#include <stdlib.h>

using namespace std;

const int N = 65536;
double sum, a[N];

void init()
{
    for (int i = 0; i < N; i++)
    {
        a[i] = i;
    }
}

void chain_unroll()
{
    sum = 0;
    for (int i = 0; i < N; i++)
        sum += a[i];
}

int main()
{
    init();
    double aveTime = 0;
    int repeatTimes = 500;
    for (int k = 0; k < repeatTimes; k++)
    {

        long long head, tail, freq;
        QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
        QueryPerformanceCounter((LARGE_INTEGER *)&head);

        chain_unroll();

        QueryPerformanceCounter((LARGE_INTEGER *)&tail);
        //    cout << "Col:"<< (tail-head)*1000.0 /freq << "ms"<< endl ;
        aveTime += (tail - head) * 1000.0 / freq;
    }
    cout << "Col:" << aveTime / repeatTimes << "ms" << endl;
    return 0;
}
