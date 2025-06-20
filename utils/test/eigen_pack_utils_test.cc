#include "utils/eigen_pack_utils.h"

#include "utils/test_utils/eigen_matrix_compare.h"
#include <gtest/gtest.h>

namespace common_utils::testing {

GTEST_TEST(MatrixConversionTest, EigenPackAsRowMajorVectorBasic) {
  Eigen::Matrix2d mat;
  mat << 1, 2, 3, 4;

  std::vector<double> vec = EigenPackAsRowMajorVector(mat);
  std::vector<double> expected = {1, 2, 3, 4};

  EXPECT_EQ(vec, expected);
}

GTEST_TEST(MatrixConversionTest, EigenPackAsRowMajorVectorRectangular) {
  Eigen::Matrix<double, 2, 3> mat;
  mat << 1, 2, 3, 4, 5, 6;

  std::vector<double> vec = EigenPackAsRowMajorVector(mat);
  std::vector<double> expected = {1, 2, 3, 4, 5, 6};

  EXPECT_EQ(vec, expected);
}

GTEST_TEST(MatrixConversionTest, EigenUnpackFromRowMajorVectorBasic) {
  std::vector<double> vec = {1, 2, 3, 4};
  Eigen::Matrix2d mat = EigenUnpackFromRowMajorVector<double>(vec, 2, 2);

  Eigen::Matrix2d expected;
  expected << 1, 2, 3, 4;

  EXPECT_TRUE(CompareMatrices(mat, expected));
}

GTEST_TEST(MatrixConversionTest, EigenUnpackFromRowMajorVectorRectangular) {
  std::vector<double> vec = {1, 2, 3, 4, 5, 6};
  Eigen::MatrixXd mat = EigenUnpackFromRowMajorVector<double>(vec, 2, 3);

  Eigen::MatrixXd expected(2, 3);
  expected << 1, 2, 3, 4, 5, 6;

  EXPECT_TRUE(CompareMatrices(mat, expected));
}

GTEST_TEST(MatrixConversionTest,
           EigenUnpackFromRowMajorVectorThrowsOnSizeMismatch) {
  std::vector<double> vec = {1, 2, 3};

  EXPECT_THROW(EigenUnpackFromRowMajorVector<double>(vec, 2, 2),
               std::invalid_argument);
}

GTEST_TEST(MatrixIteratorTest, UnpackMatrixExactFit) {
  std::vector<double> vec = {1, 2, 3, 4};
  auto mat = EigenUnpackFromRowMajorVector(vec.begin(), vec.end(), 2, 2);

  Eigen::Matrix2d expected;
  expected << 1, 2, 3, 4;

  EXPECT_TRUE(CompareMatrices(mat, expected));
}

GTEST_TEST(MatrixIteratorTest, UnpackMatrixThrowsOnMismatch) {
  std::vector<float> vec = {1, 2, 3};  // Too short

  EXPECT_THROW(EigenUnpackFromRowMajorVector(vec.begin(), vec.end(), 2, 2),
               std::invalid_argument);
}

GTEST_TEST(MatrixIteratorTest, RoundTripPackUnpack) {
  Eigen::MatrixXd mat(2, 3);
  mat << 1, 2, 3, 4, 5, 6;

  std::vector<double> buffer;
  EigenPackAsRowMajorVector(mat, std::back_inserter(buffer));
  EXPECT_EQ(buffer.size(), 6);

  Eigen::MatrixXd result =
      EigenUnpackFromRowMajorVector(buffer.begin(), buffer.end(), 2, 3);
  EXPECT_TRUE(CompareMatrices(result, mat));
}

GTEST_TEST(MatrixIteratorTest, UnpackFromArray) {
  std::array<float, 6> arr = {1, 2, 3, 4, 5, 6};
  auto mat = EigenUnpackFromRowMajorVector(arr.begin(), arr.end(), 2, 3);

  Eigen::Matrix<float, 2, 3> expected;
  expected << 1, 2, 3, 4, 5, 6;

  EXPECT_TRUE(CompareMatrices(mat, expected));
}

GTEST_TEST(MatrixIteratorTest, PackInSeveral) {
  const Eigen::MatrixXd mat1 = Eigen::MatrixXd::Constant(2, 3, 1);
  const Eigen::MatrixXd mat2 = Eigen::MatrixXd::Constant(1, 5, 2);
  std::vector<double> buffer;
  EigenPackAsRowMajorVector(mat1, std::back_inserter(buffer));
  EigenPackAsRowMajorVector(mat2, std::back_inserter(buffer));

  std::vector<double> expected;
  for (int i = 0; i < 6; ++i) {
    expected.push_back(1);
  }
  for (int i = 0; i < 5; ++i) {
    expected.push_back(2);
  }
  EXPECT_EQ(buffer, expected);

  Eigen::MatrixXd mat1_round_trip(2, 3);
  mat1_round_trip =
      EigenUnpackFromRowMajorVector(buffer.begin(), buffer.begin() + 6, 2, 3);
  EXPECT_TRUE(CompareMatrices(mat1_round_trip, mat1));

  const Eigen::MatrixXd mat2_round_trip =
      EigenUnpackFromRowMajorVector(buffer.begin() + 6, buffer.end(), 1, 5);
  EXPECT_TRUE(CompareMatrices(mat2_round_trip, mat2));
}

GTEST_TEST(MatrixConversionTest, EigenPackAsColumnMajorVectorBasic) {
  Eigen::Matrix2d mat;
  mat << 1, 2, 3, 4;  // Column-major: [1, 3, 2, 4]

  std::vector<double> vec = EigenPackAsColumnMajorVector(mat);
  std::vector<double> expected = {1, 3, 2, 4};

  EXPECT_EQ(vec, expected);
}

GTEST_TEST(MatrixConversionTest, EigenPackAsColumnMajorVectorRectangular) {
  Eigen::Matrix<double, 2, 3> mat;
  mat << 1, 3, 5, 2, 4, 6;  // Cols: [1 2], [3 4], [5 6]

  std::vector<double> vec = EigenPackAsColumnMajorVector(mat);
  std::vector<double> expected = {1, 2, 3, 4, 5, 6};

  EXPECT_EQ(vec, expected);
}

GTEST_TEST(MatrixConversionTest, EigenUnpackFromColumnMajorVectorBasic) {
  std::vector<double> vec = {1, 3, 2, 4};  // 2x2 matrix, col-major
  Eigen::Matrix2d mat = EigenUnpackFromColumnMajorVector<double>(vec, 2, 2);

  Eigen::Matrix2d expected;
  expected << 1, 2, 3, 4;

  EXPECT_TRUE(CompareMatrices(mat, expected));
}

GTEST_TEST(MatrixConversionTest, EigenUnpackFromColumnMajorVectorRectangular) {
  std::vector<double> vec = {1, 2, 3, 4, 5, 6};  // 2x3 matrix, col-major
  Eigen::MatrixXd mat = EigenUnpackFromColumnMajorVector<double>(vec, 2, 3);

  Eigen::MatrixXd expected(2, 3);
  expected << 1, 3, 5, 2, 4, 6;

  EXPECT_TRUE(CompareMatrices(mat, expected));
}

GTEST_TEST(MatrixConversionTest,
           EigenUnpackFromColumnMajorVectorThrowsOnSizeMismatch) {
  std::vector<double> vec = {1, 2, 3};

  EXPECT_THROW(EigenUnpackFromColumnMajorVector<double>(vec, 2, 2),
               std::invalid_argument);
}

GTEST_TEST(MatrixIteratorTest, UnpackMatrixExactFitColMajor) {
  std::vector<double> vec = {1, 3, 2, 4};  // 2x2, column-major
  auto mat = EigenUnpackFromColumnMajorVector(vec.begin(), vec.end(), 2, 2);

  Eigen::Matrix2d expected;
  expected << 1, 2, 3, 4;

  EXPECT_TRUE(CompareMatrices(mat, expected));
}

GTEST_TEST(MatrixIteratorTest, UnpackMatrixThrowsOnMismatchColMajor) {
  std::vector<float> vec = {1, 2, 3};  // Too short

  EXPECT_THROW(EigenUnpackFromColumnMajorVector(vec.begin(), vec.end(), 2, 2),
               std::invalid_argument);
}

GTEST_TEST(MatrixIteratorTest, RoundTripPackUnpackColMajor) {
  Eigen::MatrixXd mat(2, 3);
  mat << 1, 2, 3, 4, 5, 6;

  std::vector<double> buffer;
  EigenPackAsColumnMajorVector(mat, std::back_inserter(buffer));
  EXPECT_EQ(buffer.size(), 6);

  Eigen::MatrixXd result =
      EigenUnpackFromColumnMajorVector(buffer.begin(), buffer.end(), 2, 3);
  EXPECT_TRUE(CompareMatrices(result, mat));
}

GTEST_TEST(MatrixIteratorTest, UnpackFromArrayColMajor) {
  std::array<float, 6> arr = {1, 2, 3, 4, 5, 6};
  auto mat = EigenUnpackFromColumnMajorVector(arr.begin(), arr.end(), 2, 3);

  Eigen::Matrix<float, 2, 3> expected;
  expected << 1, 3, 5, 2, 4, 6;

  EXPECT_TRUE(CompareMatrices(mat, expected));
}

GTEST_TEST(MatrixIteratorTest, PackInSeveralColMajor) {
  const Eigen::MatrixXd mat1 = Eigen::MatrixXd::Constant(2, 3, 1);
  const Eigen::MatrixXd mat2 = Eigen::MatrixXd::Constant(1, 5, 2);
  std::vector<double> buffer;
  EigenPackAsColumnMajorVector(mat1, std::back_inserter(buffer));
  EigenPackAsColumnMajorVector(mat2, std::back_inserter(buffer));

  std::vector<double> expected;
  for (int i = 0; i < 6; ++i) expected.push_back(1);
  for (int i = 0; i < 5; ++i) expected.push_back(2);
  EXPECT_EQ(buffer, expected);

  Eigen::MatrixXd mat1_round_trip = EigenUnpackFromColumnMajorVector(
      buffer.begin(), buffer.begin() + 6, 2, 3);
  EXPECT_TRUE(CompareMatrices(mat1_round_trip, mat1));

  Eigen::MatrixXd mat2_round_trip =
      EigenUnpackFromColumnMajorVector(buffer.begin() + 6, buffer.end(), 1, 5);
  EXPECT_TRUE(CompareMatrices(mat2_round_trip, mat2));
}

GTEST_TEST(MatrixIteratorTest, PackInSeveralOfTypes) {
  const int n = 2;

  const Eigen::MatrixXd mat1 = Eigen::MatrixXd::Constant(2, 2, 1);
  std::vector<double> buffer;
  EigenPackAsColumnMajorVector(mat1, std::back_inserter(buffer));
  EigenPackAsColumnMajorVector(mat1 + Eigen::MatrixXd::Identity(n, n),
                               std::back_inserter(buffer));

  Eigen::MatrixXd mat1_round_trip = EigenUnpackFromColumnMajorVector(
      buffer.begin(), buffer.begin() + mat1.size(), n, n);
  EXPECT_TRUE(CompareMatrices(mat1_round_trip, mat1));

  Eigen::MatrixXd mat2_round_trip = EigenUnpackFromColumnMajorVector(
      buffer.begin() + mat1.size(), buffer.end(), n, n);
  EXPECT_TRUE(
      CompareMatrices(mat1 + Eigen::MatrixXd::Identity(n, n), mat2_round_trip));
}

}  // namespace common_utils::testing
