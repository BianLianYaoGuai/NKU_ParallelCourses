const int N = 65536;
double sum, a[N];

void init()
{
    for (int i = 0; i < N; i++)
    {
        a[i] = i;
    }
}

void chain_2()
{
    double sum1 = 0, sum2 = 0;
    for (int i = 0; i < N; i += 2)
    {
        sum1 += a[i];
        sum2 += a[i + 1];
    }
    sum = sum1 + sum2;
}

int main()
{
    init();
    chain_2();
    return 0;
}
