#include "gpu.h"

#include <algorithm>
#include <cuda_runtime.h>

__constant__ WorkNum gpu_powers[8192];
__device__ unsigned int solution_count = 0;

inline __device__ int
gpu_binary_search(const IndexNum elements_count,
                  const WorkNum search) {
  IndexNum l = 0;
  IndexNum r = elements_count - 1;
  IndexNum m;
  while (l <= r) {
    m = (l + r) / 2;
    if (search < gpu_powers[m])
      r = m - 1;
    else if (search > gpu_powers[m])
      l = m + 1;
    else
     return l;
  }
  return -1;
}

__global__ void
AntonCrechetovGPUKernel(const IndexNum elements_count, Solution* solutions) {
  const int x0 = blockIdx.x + 1;
  const int x1 = threadIdx.x + blockIdx.y * blockDim.x + 1;
  if (x1 <= x0)
    return;
  for (int x2 = x1 + 1; x2 < elements_count; ++x2) {
    for (int x3 = x2 + 1; x3 < elements_count; ++x3) {
      const WorkNum sum = gpu_powers[x0] + gpu_powers[x1] + gpu_powers[x2] + gpu_powers[x3];
      auto s = gpu_binary_search(elements_count, sum);
      if (s > 0) {
        size_t indx = atomicInc(&solution_count, 1);
        solutions[indx].a = x0;
        solutions[indx].b = x1;
        solutions[indx].c = x2;
        solutions[indx].d = x3;
        solutions[indx].e = s;
        //printf("%d %d %d %d %d\n", x0, x1, x2, x3, s);
      }
    }
  }
}

__global__ void
NaiveGPUKernel(const IndexNum elements_count, Solution* solutions) {
  const int x0 = blockIdx.x + 1;
  const int x1 = threadIdx.x + blockIdx.y * blockDim.x + 1;
  if (x1 >= x0)
    return;
  const WorkNum s1 = gpu_powers[x0] + gpu_powers[x1];
  for (int x2 = 1; x2 < x1; ++x2) {
    const WorkNum s2 = s1 + gpu_powers[x2];
    for (int x3 = 1; x3 < x2; ++x3) {
      const WorkNum sum = s2 + gpu_powers[x3];
      auto s = gpu_binary_search(elements_count, sum);
      if (s > 0) {
        size_t indx = atomicInc(&solution_count, 1);
        solutions[indx].a = x0;
        solutions[indx].b = x1;
        solutions[indx].c = x2;
        solutions[indx].d = x3;
        solutions[indx].e = s;
        //printf("%d %d %d %d %d\n", x0, x1, x2, x3, s);
      }
    }
  }
}

__global__ void
FirstOptimizedGPUKernel(const IndexNum elements_count, Solution* solutions) {
  const int x0 = blockIdx.x + 1;
  const int x1 = threadIdx.x + blockIdx.y * blockDim.x + 1;
  if (x1 >= x0)
    return;
  IndexNum rs = 5;
  for (int x2 = 1; x2 < x1; ++x2) {
    const WorkNum s2 = gpu_powers[x0] + gpu_powers[x1] + gpu_powers[x2];
    while (rs > 0 && gpu_powers[rs] > s2)
      --rs;
    for (int x3 = 1; x3 <x2; ++x3) {
      const WorkNum sum = s2 + gpu_powers[x3];
      while (rs < elements_count - 1 && gpu_powers[rs] < sum)
        ++rs;
      auto s = gpu_binary_search(elements_count, sum);
      if (s > 0) {
        size_t indx = atomicInc(&solution_count, 1);
        solutions[indx].a = x0;
        solutions[indx].b = x1;
        solutions[indx].c = x2;
        solutions[indx].d = x3;
        solutions[indx].e = s;
        //printf("%d %d %d %d %d\n", x0, x1, x2, x3, s);
      }
    }
  }
}

__global__ void
SecondOptimizedGPUKernel(const IndexNum elements_count, Solution* solutions) {
  const int x0 = blockIdx.x + 1;
  const int x1 = threadIdx.x + blockIdx.y * blockDim.x + 1;
  if (x1 >= x0)
    return;
  IndexNum rs = 5;
  const WorkNum s1 = gpu_powers[x0] + gpu_powers[x1];
  for (int x2 = 1; x2 < x1; ++x2) {
    const WorkNum s2 = s1 + gpu_powers[x2];
    while (rs > 0 && gpu_powers[rs] > s2)
      --rs;
    for (IndexNum x3 = 1; x3 < x2; x3++) {
      // go straight to the first appropriate x3, mod 30
      if (WorkNum err30 = (x0 + x1 + x2 + x3 - rs) % 30)
        x3 += 30 - err30;
      if (x3 >= x2)
        break;
      auto sum = s2 + gpu_powers[x3];
      while (rs < elements_count - 1 && gpu_powers[rs] < sum)
        ++rs;
      if (gpu_powers[rs] == sum) {
        size_t indx = atomicInc(&solution_count, 1);
        solutions[indx].a = x0;
        solutions[indx].b = x1;
        solutions[indx].c = x2;
        solutions[indx].d = x3;
        solutions[indx].e = rs;
        //printf("%d %d %d %d %d\n", x0, x1, x2, x3, rs);
      }
    }
  }
}

template<typename F>
std::vector<Solution>
ComputeOnGpu(const IndexNum elements_count, const std::vector<WorkNum>& powers, F kernel) {
  std::vector<Solution> result;
  cudaDeviceSynchronize();
  cudaError_t err = cudaMemcpyToSymbol(gpu_powers,
                                       powers.data(),
                                       sizeof(WorkNum) * elements_count);
  if (err != cudaSuccess) {
    fprintf(stderr, "Failed to cudaMemcpyToSymbol (error code %s)!\n", cudaGetErrorString(err));
    return result;
  }

  // Allocate the device output vector for solutions
  Solution *device_solutions = nullptr;
  size_t solutions_size = sizeof(Solution) * 100;
  err = cudaMalloc(&device_solutions, solutions_size);
  if (err != cudaSuccess) {
    fprintf(stderr,
            "Failed to allocate device vector for solutions (error code %s)!\n", 
            cudaGetErrorString(err));
    return result;
  }

  // Launch CUDA Kernel
  int blocks_x = elements_count;
  int threads = std::min(1024, static_cast<int>(elements_count));
  int blocks_y = (elements_count + threads - 1) / threads;
  kernel(elements_count,
         blocks_x,
         blocks_y,
         threads,
         device_solutions);
  err = cudaGetLastError();
  if (err != cudaSuccess) {
    fprintf(stderr, "Failed to launch kernel (error code %s)!\n", cudaGetErrorString(err));
    return result;
  }
  err = cudaDeviceSynchronize();
  if (err != cudaSuccess) {
    fprintf(stderr, "Failed to wait kernel (error code %s)!\n", cudaGetErrorString(err));
    return result;
  }
  return result;
}

std::vector<Solution>
AntonCrechetovGPU(const IndexNum elements_count, const std::vector<WorkNum>& powers,
                  const IndexNum from, const IndexNum to) {
  auto kernel = [](int elements_count,
                   int blocks_x,
                   int blocks_y,
                   int threads,
                   Solution *device_solutions) {
    dim3 grid(blocks_x, blocks_y);
    AntonCrechetovGPUKernel<<<grid, threads>>>(
        elements_count, device_solutions);
  };
  return ComputeOnGpu(elements_count, powers, kernel);
}

std::vector<Solution>
NaiveGPU(const IndexNum elements_count, const std::vector<WorkNum>& powers,
         const IndexNum from, const IndexNum to) {
  auto kernel = [](int elements_count,
                   int blocks_x,
                   int blocks_y,
                   int threads,
                   Solution *device_solutions) {
    dim3 grid(blocks_x, blocks_y);
    NaiveGPUKernel<<<grid, threads>>>(
        elements_count, device_solutions);
  };
  return ComputeOnGpu(elements_count, powers, kernel);
}

std::vector<Solution>
FirstOptimizedGPU(const IndexNum elements_count, const std::vector<WorkNum>& powers,
                  const IndexNum from, const IndexNum to) {
  auto kernel = [](int elements_count,
                   int blocks_x,
                   int blocks_y,
                   int threads,
                   Solution *device_solutions) {
    dim3 grid(blocks_x, blocks_y);
    FirstOptimizedGPUKernel<<<grid, threads>>>(
      elements_count, device_solutions);
  };
  return ComputeOnGpu(elements_count, powers, kernel);
}

std::vector<Solution>
SecondOptimizedGPU(const IndexNum elements_count, const std::vector<WorkNum>& powers,
                   const IndexNum from, const IndexNum to) {
  auto kernel = [](int elements_count,
                   int blocks_x,
                   int blocks_y,
                   int threads,
                   Solution *device_solutions) {
    dim3 grid(blocks_x, blocks_y);
    SecondOptimizedGPUKernel<<<grid, threads>>>(
      elements_count, device_solutions);
  };
  return ComputeOnGpu(elements_count, powers, kernel);
}
