#include <chrono>
#include <iostream>
#include <CL/sycl.hpp>
#include <cmath>

using namespace std;
using namespace sycl;

// Matrix initialization
void m_ini(float *m,int N){
    for(int i=0;i<N;i++)
        for(int j=0;j<N;j++)
            m[i*N+j]=0;
    for(int i=0;i<N;i++){
        m[i*N+i]=1.0;
        for(int j=i+1;j<N;j++)
            m[i*N+j]=static_cast<float>(rand()) / static_cast<float>(RAND_MAX);
    }
    for(int k=0;k<N;k++)
        for(int i=k+1;i<N;i++)
            for(int j=0;j<N;j++)
                m[i*N+j]+=(m[k*N+j]/pow(2,k));
}

// return execution time
double gpu_kernel(float* A,int N, sycl::queue& q) {
    buffer<float> buf(A,N*N);
    double duration = 0.0;
    std::chrono::high_resolution_clock::time_point s, e;
    s = std::chrono::high_resolution_clock::now();
    for (int k = 0; k < N; k++) {
        q.submit([&](handler& h) {
            accessor m{ buf, h, read_write };
            h.parallel_for(range(N - k), [=](auto idx) {
                int j = k + idx;
                m[k*N+j] = m[k*N+j] / m[k*N+k];
                });
            });
        q.submit([&](handler& h) {
            accessor m{ buf, h, read_write };
            h.parallel_for(range(N - (k + 1), N - (k + 1)), [=](auto idx) {
                int i = k + 1 + idx.get_id(0);
                int j = k + 1 + idx.get_id(1);
                m[i*N+j] = m[i*N+j] - m[i*N+k] * m[k*N+j];
                });
            });
        q.submit([&](handler& h) {
            accessor m{ buf, h, read_write };
            h.parallel_for(range(N - (k + 1)), [=](auto idx) {
                int i = k + 1 + idx;
                m[i*N+k] = 0;
                });
        });
    }
    //q.wait();
    e = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration<float, std::milli>(e - s).count();
    return(duration);
}

// return execution time
double cpu_kernel(float* A, int N) {
    buffer<float> buf(A,N*N);
    host_accessor m{ buf ,read_write };
    double duration = 0.0;
    std::chrono::high_resolution_clock::time_point s, e;
    s = std::chrono::high_resolution_clock::now();
    {
        for (int k=0;k<N;k++){
            for (int j=k+1;j<N;j++){
                m[k*N+j]=m[k*N+j]/m[k*N+k];
            }
            m[k*N+k]=1;
            for(int i=k+1;i<N;i++){
                for(int j=k+1;j<N;j++){
                    m[i*N+j]=m[i*N+j]-m[i*N+k]*m[k*N+j];
                }
                m[i*N+k]=0;
            }
        }
    }
    e = std::chrono::high_resolution_clock::now();
    duration = std::chrono::duration<float, std::milli>(e - s).count();
    return(duration);
}

int verify(float* cpu_res, float* gpu_res, int length) {
    int err = 0;
    for (int i = 0; i < length; i++) {
        if (fabs(cpu_res[i] - gpu_res[i]) > 1e-3) {
            err++;
            //printf("\n%lf, %lf", cpu_res[i], gpu_res[i]);
        }
    }
    return(err);
}
int comparison(const int N, const int iterations) {
    auto propList = cl::sycl::property_list{ cl::sycl::property::queue::enable_profiling() };
    queue q_gpu(sycl::gpu_selector_v, propList);
    std::cout << "Device: " << q_gpu.get_device().get_info<info::device::name>() << std::endl;
    queue q_cpu(sycl::cpu_selector_v, propList);
    std::cout << "Device: " << q_cpu.get_device().get_info<info::device::name>() << std::endl;
    cout << "Problem size: (" << N << "," << N << ")\n";
    auto A_gpu = (float *) malloc (sizeof(float)*N*N);
    auto A_cpu = (float *) malloc (sizeof(float)*N*N);
    // init the A_gpu/A_cpu
    m_ini(A_gpu,N);
    for (int i = 0; i < N * N; i++) {
        A_cpu[i] = A_gpu[i];
    }
    
    double duration_gpu = 0.0f;
    double duration_cpu = 0.0f;
    
    // GPU compuation and timer 
    int warmup = 10;
    for (int run = 0; run < iterations + warmup; run++) {
        float duration = gpu_kernel(A_gpu, N, q_gpu);
        if (run >= warmup) duration_gpu += duration;
    }
    duration_gpu = duration_gpu / iterations;
    
    // CPU compuation and timer 
    warmup = 2;
    for (int run = 0; run < iterations / 2 + warmup; run++) {
        float duration = cpu_kernel(A_cpu, N);
        if (run >= warmup) duration_cpu += duration;
    }
    duration_cpu = duration_cpu / (iterations / 2);
    
    // Compare the resutls of CPU and GPU 
    int errCode = 0;
    errCode = verify(A_cpu, A_gpu, N * N);
    if (errCode > 0) printf("\nThere are %d errors\n", errCode);
    printf("\nmatrix size N = %d", N);
    printf("\nGPU Computation Time = %lf (ms); \n"
        "CPU Computaiton Time = %lf (ms); \n",
        duration_gpu, duration_cpu);
    free(A_gpu);
    free(A_cpu);
    return(errCode);
}

int main() {
    int errCode = comparison(64, /* matrix size, N */
        10            /* repeat time */
    );
    return(errCode);
}
