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
    chain_unroll();
    return 0;
}
