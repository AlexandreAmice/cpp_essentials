#pragma once

#include <cassert>
#include <cstdio>
#include <stdexcept>

#include <cublas_v2.h>
#include <cuda_runtime.h>

namespace common_utils {

// Helper function to check if CUDA is available.
inline bool IsCudaAvailable() {
  int deviceCount = 0;
  cudaError_t error = cudaGetDeviceCount(&deviceCount);
  return error == cudaSuccess && deviceCount > 0;
}

/**
 * @brief Checks the result of a CUDA runtime API call and exits if an error
 * occurred.
 *
 * @param result The result code returned by the CUDA API call.
 * @param file The source file where the check is performed (__FILE__).
 * @param line The line number where the check is performed (__LINE__).
 */
inline void cudaCheckError(cudaError_t result, const char* file, int line) {
  if (result != cudaSuccess) {
    fprintf(stderr, "CUDA Runtime Error at %s:%d: %s (%d)\n", file, line,
            cudaGetErrorString(result), result);
    // Consider throwing an exception instead of exiting in library code
    exit(EXIT_FAILURE);
  }
}

// Macro to wrap CUDA API calls for easy error checking
#define CUDA_CHECK(val) common_utils::cudaCheckError((val), __FILE__, __LINE__)

#define CUSOLVER_CHECK(call)                                               \
  do {                                                                     \
    cusolverStatus_t status = call;                                        \
    if (status != CUSOLVER_STATUS_SUCCESS) {                               \
      fprintf(stderr, "cuSOLVER error in %s:%d: %d\n", __FILE__, __LINE__, \
              status);                                                     \
      exit(EXIT_FAILURE);                                                  \
    }                                                                      \
  } while (0)

#define CUBLAS_CHECK(call)                                                   \
  do {                                                                       \
    cublasStatus_t status = call;                                            \
    if (status != CUBLAS_STATUS_SUCCESS) {                                   \
      fprintf(stderr, "cuBLAS error in %s:%d: %s, %s\n", __FILE__, __LINE__, \
              cublasGetStatusName(status), cublasGetStatusString(status));   \
      exit(EXIT_FAILURE);                                                    \
    }                                                                        \
  } while (0)

// Custom deleter for CUDA device memory. Wrap your device pointers as unique
// pointers with this to avoid having to remember to free the memory when the
// function goes out of scope.
struct CudaDeleter {
  void operator()(void* ptr) const {
    if (ptr) {
      cudaFree(ptr);
    }
  }
};

/**
 * @brief A structure for managing GPU memory.
 *
 * This struct provides a simple interface for allocating and managing memory
 * that can be shared between host and device in CUDA applications.
 * Poached from Peter's cuda_utilities.h
 */
template <typename T>
struct CudaPtr {
  T* host;    ///< Pointer to host memory
  T* device;  ///< Pointer to device (GPU) memory
  unsigned long size;

  CudaPtr() : host(nullptr), device(nullptr), size(0) {}

  CudaPtr(T* data, unsigned long size) : host(data), size(size) {
    if (cudaMalloc(&device, size * sizeof(T)) != ::cudaSuccess) {
      throw std::runtime_error("Failed to cudaMalloc!");
    }
  }

  CudaPtr(const CudaPtr&) = delete;
  CudaPtr& operator=(const CudaPtr&) = delete;

  ~CudaPtr() { CUDA_CHECK(cudaFree((void*)device)); }

  void copyHostToDevice() {
    if (host == nullptr) {
      throw std::runtime_error("Tried to copy from a dangling host pointer!");
    }

    cudaMemcpy((void*)device, host, size * sizeof(T), cudaMemcpyHostToDevice);
  }

  void copyDeviceToHost() {
    if (host == nullptr) {
      throw std::runtime_error("Tried to copy to dangling host pointer!");
    }

    cudaMemcpy((void*)host, device, size * sizeof(T), cudaMemcpyDeviceToHost);
  }
};
}  // namespace common_utils
