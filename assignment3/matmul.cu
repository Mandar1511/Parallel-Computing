#include <cuda_runtime.h>
#include <iostream>
#include <cstdlib>
#include <cmath>
#include <ctime>
#include <chrono>
using namespace std;

__global__ void matrixMul(float *devA, float *devB, float *devC, int n) {
    int i = blockIdx.y * blockDim.y + threadIdx.y; // row
    int j = blockIdx.x * blockDim.x + threadIdx.x; // col
    float element = 0.0f;
    if (i < n && j < n) {
        for (int k = 0; k < n; k++) {
            // dot product
            element += devA[i * n + k] * devB[k * n + j];
        }
        devC[i * n + j] = element;
    }
}

void initializeMatrix(float *matrix, int n) {
    for (int i = 0; i < n * n; ++i) {
        matrix[i] = -1.0f + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / 2.0f));
    }
}

void validate(float *hostC, float *hostA, float *hostB, int n) {
    auto now = chrono::system_clock::now();
    float *cpu_result = new float[n * n];
    // compute matrix multiplication on host
    for (int i = 0; i < n; ++i) {
        for (int j = 0; j < n; ++j) {
            float element = 0.0f;
            for (int k = 0; k < n; ++k) {
                element += hostA[i * n + k] * hostB[k * n + j];
            }
            cpu_result[i * n + j] = element;
        }
    }
    cout << "Host Computation time in ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(chrono::system_clock::now() - now).count() << "\n\n";

    bool isCorrect = true;
    const float threshold = 2 * (1e-2);
    for (int i = 0; i < n * n; ++i) {
        if (fabs(hostC[i] - cpu_result[i]) > threshold) {
            isCorrect = false;
            cout << "Mismatch at index " << i << " "<<hostC[i] << " "<<cpu_result[i]<<endl;
        }
    }
    if (isCorrect) {
        cout << "Results are correct." << endl;
    } else {
        cout << "Results are incorrect." << endl;
    }
    delete[] cpu_result;
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        cout << "provide matrix size" << endl;
        return 1;
    }
    int n = atoi(argv[1]);
    int size_in_bytes = n * n * sizeof(float);

    // Allocate host memory
    float *hostA = new float[size_in_bytes];
    float *hostB = new float[size_in_bytes];
    float *hostC = new float[size_in_bytes];

    // Initialize matrices in host memory
    initializeMatrix(hostA, n);
    initializeMatrix(hostB, n);
    for (int i = 0; i < n * n; ++i) {
        hostC[i] = 0.0f;
    }

    // Allocate device memory
    float *devA, *devB, *devC;
    cudaMalloc(&devA, size_in_bytes);
    cudaMalloc(&devB, size_in_bytes);
    cudaMalloc(&devC, size_in_bytes);
    auto now1 = chrono::system_clock::now();
    cudaMemcpy(devA, hostA, size_in_bytes, cudaMemcpyHostToDevice);
    cudaMemcpy(devB, hostB, size_in_bytes, cudaMemcpyHostToDevice);

    dim3 threadsPerBlock(32, 32);
    dim3 blocksPerGrid(ceil(n/32.0), ceil(n/32.0));

    auto now = chrono::system_clock::now();
    // Lauch kernel function
    matrixMul<<<blocksPerGrid, threadsPerBlock>>>(devA, devB, devC, n);
    cudaDeviceSynchronize();
    cout << "Device Computation time in ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(chrono::system_clock::now() - now).count() << "\n\n";

    // Copy data from device to host
    cudaMemcpy(hostC, devC, size_in_bytes, cudaMemcpyDeviceToHost);
    cout << "Device Computation time including data transfer in ms: " << std::chrono::duration_cast<std::chrono::milliseconds>(chrono::system_clock::now() - now1).count() << "\n\n";

    // Validate results
    validate(hostC, hostA, hostB, n);
    // free memory on host
    delete[] hostA;
    delete[] hostB;
    delete[] hostC;
    // free memory on device
    cudaFree(devA);
    cudaFree(devB);
    cudaFree(devC);
    cudaDeviceReset();

    return 0;
}
