#include <string>
#include <type_traits>

#include "utils/scalar_types.h"
#include <gtest/gtest.h>

namespace common_utils {
namespace {
__global__ void checkCommonUtilsScalarTypes(bool* results) {
  results[0] = CommonUtilsScalarType<float>;
  results[1] = CommonUtilsScalarType<double>;
}
// Test that concept works for supported types in CUDA
TEST(ScalarTypesTest, ConceptSupportsFloatAndDoubleInCuda) {
  bool h_results[2];
  bool* d_results;
  cudaMalloc(&d_results, 2 * sizeof(bool));

  // Call the global kernel function instead of using a lambda directly
  checkCommonUtilsScalarTypes<<<1, 1>>>(d_results);
  cudaDeviceSynchronize();

  cudaMemcpy(h_results, d_results, 2 * sizeof(bool), cudaMemcpyDeviceToHost);
  cudaFree(d_results);

  EXPECT_TRUE(h_results[0]);
  EXPECT_TRUE(h_results[1]);
}

template <CommonUtilsScalarType T>
__global__ void deviceTestKernel(bool* result, T input);

template <typename T>
struct CudaTemplateTest {
  // Device function that can be called from kernels
  __device__ static bool DeviceTest(T value) { return value > T(0); }

  // Definition of our device kernel function

  // Host function that launches a kernel to test the device function
  static bool RunTest(T value) {
    bool h_result;
    bool* d_result;
    cudaMalloc(&d_result, sizeof(bool));

    // Launch the kernel
    deviceTestKernel<T><<<1, 1>>>(d_result, value);
    cudaDeviceSynchronize();

    // Get the result
    cudaMemcpy(&h_result, d_result, sizeof(bool), cudaMemcpyDeviceToHost);
    cudaFree(d_result);

    return h_result;
  }
};
template <CommonUtilsScalarType T>
__global__ void deviceTestKernel(bool* result, T input) {
  *result = CudaTemplateTest<T>::DeviceTest(input);
}

// Use the macro to instantiate the template for all supported types
INSTANTIATE_FOR_SUPPORTED_SCALARS(CudaTemplateTest);

// Test that the template was instantiated correctly for all types
TEST(ScalarTypesTest, CudaInstantiationWorks) {
  EXPECT_TRUE(CudaTemplateTest<float>::RunTest(1.0f));
  EXPECT_FALSE(CudaTemplateTest<float>::RunTest(-1.0f));
  EXPECT_TRUE(CudaTemplateTest<double>::RunTest(2.0));
  EXPECT_FALSE(CudaTemplateTest<double>::RunTest(-2.0));
}

// CUDA device function template
template <typename T>
__device__ T cuda_multiply_by_two(T value) {
  return value * T(2);
}

template <typename T>
__global__ void cuda_multiply_by_two_kernel(T* result, T input) {
  *result = cuda_multiply_by_two(input);
}

// Host wrapper function template that launches a kernel
template <typename T>
T cuda_wrapper(T value) {
  T h_result;
  T* d_result;
  cudaMalloc(&d_result, sizeof(T));

  // Launch the kernel
  cuda_multiply_by_two_kernel<<<1, 1>>>(d_result, value);
  cudaDeviceSynchronize();

  // Get the result
  cudaMemcpy(&h_result, d_result, sizeof(T), cudaMemcpyDeviceToHost);
  cudaFree(d_result);

  return h_result;
}

// Use the macro to instantiate the function for all supported types
INSTANTIATE_FUNCTION_FOR_SCALAR_TYPES(cuda_wrapper);

// Test that the function was instantiated correctly for all types
TEST(ScalarTypesTest, CudaFunctionInstantiationWorks) {
  EXPECT_FLOAT_EQ(cuda_wrapper(2.5f), 5.0f);  // Test with float
  EXPECT_DOUBLE_EQ(cuda_wrapper(3.0), 6.0);   // Test with double
}
}  // namespace
}  // namespace common_utils
