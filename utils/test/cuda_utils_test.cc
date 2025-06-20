#include "utils/cuda_utils.h"

#include <cuda_runtime.h>
#include <gtest/gtest.h>

namespace common_utils::testing {

GTEST_TEST(CudaUtilsTest, IsCudaAvailableTest) {
  EXPECT_TRUE(IsCudaAvailable());
}

GTEST_TEST(CudaUtilsTest, CudaCheckSuccess) {
  // This test requires a CUDA-capable device.
  if (!IsCudaAvailable()) {
    GTEST_SKIP() << "CUDA device not available, skipping test.";
  }
  // Expect no death when checking a successful call.
  // cudaFree(nullptr) is a safe no-op that returns cudaSuccess.
  EXPECT_NO_FATAL_FAILURE(CUDA_CHECK(cudaFree(nullptr)));
}

GTEST_TEST(CudaUtilsTest, CudaCheckFailure) {
  // Expect death when checking a failing call.
  // cudaMalloc with a huge negative size should fail.
  // The ".*CUDA Runtime Error.*invalid argument.*" part checks the stderr
  // output.
  // if (!IsCudaAvailable()) {
  //   GTEST_SKIP() << "CUDA device not available, skipping test.";
  // }
  EXPECT_DEATH(CUDA_CHECK(cudaMalloc(nullptr, (size_t)-1)),
               ".*CUDA Runtime Error.*invalid argument.*");
}

}  // namespace common_utils::testing
