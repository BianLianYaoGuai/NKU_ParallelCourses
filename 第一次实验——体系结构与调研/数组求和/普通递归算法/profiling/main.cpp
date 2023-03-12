const int N = 65536;
double sum, a[N];

void init()
{
    for (int i = 0; i < N; i++)
    {
        a[i] = i;
    }
}

void recursion(int n)
{
    if (n == 1)
        return;
    else
    {
        for (int i = 0; i < n / 2; i++)
            a[i] += a[n - i - 1];
        n = n / 2;
        recursion(n);
    }
}

int main()
{
    init();
    recursion(N);
    return 0;
}
