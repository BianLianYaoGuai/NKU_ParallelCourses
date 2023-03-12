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

void aXb_0()
{
    for (int i = 0; i < N; i++)
    {
        sum[i] = 0.0;
        for (int j = 0; j < N; j++)
            sum[i] += b[j][i] * a[j];
    }
}

int main()
{
    init();
    aXb_0();
    return 0;
}
