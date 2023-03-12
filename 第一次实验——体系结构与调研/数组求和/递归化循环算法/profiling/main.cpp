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

void recursion_2()
{
    for (int m = N; m > 1; m /= 2)
        for (int i = 0; i < m / 2; i++)
            a[i] = a[i * 2] + a[i * 2 + 1];
}

int main()
{
    double aveTime = 0;
    int repeatTimes = 500;
    for (int k = 0; k < repeatTimes; k++)
    {
        init();
        long long head, tail, freq;
        QueryPerformanceFrequency((LARGE_INTEGER *)&freq);
        QueryPerformanceCounter((LARGE_INTEGER *)&head);

        recursion_2();

        QueryPerformanceCounter((LARGE_INTEGER *)&tail);
        //    cout << "Col:"<< (tail-head)*1000.0 /freq << "ms"<< endl ;
        aveTime += (tail - head) * 1000.0 / freq;
    }
    cout << "Col:" << aveTime / repeatTimes << "ms" << endl;
    return 0;
}
