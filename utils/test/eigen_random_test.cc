#include "utils/eigen_random.h"

#include <gtest/gtest.h>

namespace common_utils::testing {
GTEST_TEST(TestEigenRandom, TestMatrixXdRandom) {
  const int rows = 3;
  const int cols = 5;
  RandomGenerator generator;
  Eigen::MatrixXd matrix = MatrixXdRandom(rows, cols, &generator);
  EXPECT_EQ(matrix.rows(), rows);
  EXPECT_EQ(matrix.cols(), cols);
  for (int i = 0; i < rows; i++) {
    for (int j = 0; j < cols; j++) {
      EXPECT_GE(matrix(i, j), -1);
      EXPECT_LE(matrix(i, j), 1);
    }
  }
}

GTEST_TEST(TestEigenRandom, TestVectorXdRandom) {
  const int rows = 3;
  RandomGenerator generator;
  Eigen::VectorXd vector = VectorXdRandom(rows, &generator);
  EXPECT_EQ(vector.rows(), rows);
  EXPECT_EQ(vector.cols(), 1);
  for (int i = 0; i < rows; i++) {
    EXPECT_GE(vector(i), -1);
    EXPECT_LE(vector(i), 1);
  }
}

GTEST_TEST(TestEigenRandom, TestMatrixXdRandn) {
  const int rows = 1000;
  const int cols = 1250;
  RandomGenerator generator;
  Eigen::MatrixXd matrix = MatrixXdRandn(rows, cols, &generator);
  EXPECT_EQ(matrix.rows(), rows);
  EXPECT_EQ(matrix.cols(), cols);

  const double mean = matrix.mean();
  const double stddev =
      std::sqrt((matrix.array() - mean).square().sum() / (rows * cols));

  const double abs_err = 1e-3;
  EXPECT_NEAR(mean, 0, abs_err);
  EXPECT_NEAR(stddev, 1, abs_err);
}

GTEST_TEST(TestEigenRandom, TestVectorXdRandn) {
  const int rows = 10000;
  RandomGenerator generator;
  Eigen::MatrixXd vector = VectorXdRandn(rows, &generator);
  EXPECT_EQ(vector.rows(), rows);
  EXPECT_EQ(vector.cols(), 1);

  const double mean = vector.mean();
  const double stddev =
      std::sqrt((vector.array() - mean).square().sum() / rows);

  const double abs_err = 1e-2;
  EXPECT_NEAR(mean, 0, abs_err);
  EXPECT_NEAR(stddev, 1, abs_err);
}

}  // namespace common_utils::testing
