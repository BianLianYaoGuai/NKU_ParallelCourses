```python
%%writefile LU.cpp
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
```

    Overwriting src/LU.cpp



```python
! chmod 755 q; chmod 755 compile_run.sh;if [ -x "$(command -v qsub)" ]; then ./q compile_run.sh; else ./compile_run.sh; fi
```

    Job has been submitted to Intel(R) DevCloud and will execute soon.
    
    Job ID                    Name             User            Time Use S Queue
    ------------------------- ---------------- --------------- -------- - -----
    2331655.v-qsvr-1           ...ub-singleuser u196000         00:02:44 R jupyterhub     
    2331829.v-qsvr-1           compile_run.sh   u196000                0 Q batch          
    
    Waiting for Output ██████████████ Done⬇
    
    ########################################################################
    #      Date:           Mon 26 Jun 2023 08:59:04 PM PDT
    #    Job ID:           2331829.v-qsvr-1.aidevcloud
    #      User:           u196000
    # Resources:           cput=75:00:00,neednodes=1:gpu:ppn=2,nodes=1:gpu:ppn=2,walltime=06:00:00
    ########################################################################
    
    ## u196000 is compiling
    Device: Intel(R) UHD Graphics [0x9a60]
    Device: 11th Gen Intel(R) Core(TM) i9-11900KB @ 3.30GHz
    Problem size: (64,64)
    
    matrix size N = 64
    GPU Computation Time = 0.989658 (ms); 
    CPU Computaiton Time = 0.007248 (ms); 
    
    ########################################################################
    # End of output for job 2331829.v-qsvr-1.aidevcloud
    # Date: Mon 26 Jun 2023 08:59:15 PM PDT
    ########################################################################
    
    Job Completed in 14 seconds.



```python
%%writefile LU.cpp
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
    q.wait();
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

void comparison(const int N,
    const int iterations,
    sycl::queue& q) {
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
        float duration = gpu_kernel(A_gpu, N, q);
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

    printf("\nmatrix size N = %d", N);
    printf("\nGPU Computation Time = %lf (ms); \n"
        "CPU Computaiton Time = %lf (ms); \n",
        duration_gpu, duration_cpu);
    free(A_gpu);
    free(A_cpu);
}

int main() {
    auto propList = cl::sycl::property_list{ cl::sycl::property::queue::enable_profiling() };
    queue my_gpu_queue(sycl::gpu_selector_v, propList);
    std::cout << "Device: " << my_gpu_queue.get_device().get_info<info::device::name>() << std::endl;
    comparison(64, /* matrix size, N */
        10,            /* repeat time */
        my_gpu_queue);
    comparison(128, /* matrix size, N */
        10,            /* repeat time */
        my_gpu_queue);
    comparison(256, /* matrix size, N */
        10,            /* repeat time */
        my_gpu_queue);
    comparison(512, /* matrix size, N */
        10,            /* repeat time */
        my_gpu_queue);
    comparison(1024, /* matrix size, N */
        10,            /* repeat time */
        my_gpu_queue);
    comparison(2048, /* matrix size, N */
        10,            /* repeat time */
        my_gpu_queue);
    return(0);
}
```

    Overwriting src/LU.cpp



```python
! chmod 755 q; chmod 755 compile_run.sh;if [ -x "$(command -v qsub)" ]; then ./q compile_run.sh; else ./compile_run.sh; fi
```

    Job has been submitted to Intel(R) DevCloud and will execute soon.
    
    Job ID                    Name             User            Time Use S Queue
    ------------------------- ---------------- --------------- -------- - -----
    2331655.v-qsvr-1           ...ub-singleuser u196000         00:02:29 R jupyterhub     
    2331791.v-qsvr-1           compile_run.sh   u196000                0 Q batch          
    
    Waiting for Output ████████████████████████████████████████████████████████████████████████████████ Done⬇
    
    ########################################################################
    #      Date:           Mon 26 Jun 2023 08:19:28 PM PDT
    #    Job ID:           2331791.v-qsvr-1.aidevcloud
    #      User:           u196000
    # Resources:           cput=75:00:00,neednodes=1:gpu:ppn=2,nodes=1:gpu:ppn=2,walltime=06:00:00
    ########################################################################
    
    ## u196000 is compiling
    Device: Intel(R) UHD Graphics [0x9a60]
    Problem size: (64,64)
    
    matrix size N = 64
    GPU Computation Time = 1.073477 (ms); 
    CPU Computaiton Time = 0.029331 (ms); 
    Problem size: (128,128)
    
    matrix size N = 128
    GPU Computation Time = 1.932031 (ms); 
    CPU Computaiton Time = 0.246249 (ms); 
    Problem size: (256,256)
    
    matrix size N = 256
    GPU Computation Time = 4.059777 (ms); 
    CPU Computaiton Time = 2.119116 (ms); 
    Problem size: (512,512)
    
    matrix size N = 512
    GPU Computation Time = 15.052124 (ms); 
    CPU Computaiton Time = 16.682967 (ms); 
    Problem size: (1024,1024)
    
    matrix size N = 1024
    GPU Computation Time = 230.856429 (ms); 
    CPU Computaiton Time = 135.206744 (ms); 
    Problem size: (2048,2048)
    
    matrix size N = 2048
    GPU Computation Time = 1979.039124 (ms); 
    CPU Computaiton Time = 1086.467505 (ms); 
    
    ########################################################################
    # End of output for job 2331791.v-qsvr-1.aidevcloud
    # Date: Mon 26 Jun 2023 08:20:36 PM PDT
    ########################################################################
    
    Job Completed in 80 seconds.



```python

```
