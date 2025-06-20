#include <vector>

#include <Eigen/Dense>
namespace common_utils {

/**
 * Packs the Eigen Matrix mat into a flat std::vector<double> in row major
 * order.
 * @tparam MatrixType
 * @param mat
 * @return std::vector<typename MatrixType::Scalar>
 */
template <typename MatrixType>
std::vector<typename MatrixType::Scalar> EigenPackAsRowMajorVector(
    const MatrixType& mat) {
  using Scalar = typename MatrixType::Scalar;
  const Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
      rowMajorMat = mat;
  return std::vector<Scalar>(rowMajorMat.data(),
                             rowMajorMat.data() + rowMajorMat.size());
}

/**
 * Packs the Eigen Matrix mat into the iterator in row major order.
 * @tparam MatrixType
 * @param mat
 * @return std::vector<typename MatrixType::Scalar>
 */
template <typename MatrixType, typename OutputIt>
void EigenPackAsRowMajorVector(const MatrixType& mat, OutputIt dest) {
  using Scalar = typename MatrixType::Scalar;
  Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
      rowMajorMat = mat;
  std::copy(rowMajorMat.data(), rowMajorMat.data() + mat.size(), dest);
}

/**
 * Unpacks the flat vector into an Eigen Matrix of size rows by cols in row
 * major order
 * @tparam MatrixType
 * @param mat
 * @return std::vector<typename MatrixType::Scalar>
 */
template <typename Scalar>
Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>
EigenUnpackFromRowMajorVector(const std::vector<Scalar>& vec, int rows,
                              int cols) {
  if (vec.size() != static_cast<size_t>(rows * cols)) {
    throw std::invalid_argument(
        "Vector size does not match matrix dimensions.");
  }

  Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
      rowMajorMat(rows, cols);
  std::copy(vec.begin(), vec.end(), rowMajorMat.data());

  // Convert to default (column-major) Eigen matrix
  return Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>(rowMajorMat);
}

/**
 * Unpacks the iterator into an Eigen Matrix of size rows by cols in row major
 * order
 * @tparam MatrixType
 * @param mat
 * @return std::vector<typename MatrixType::Scalar>
 */
template <typename InputIt,
          typename Scalar = typename std::iterator_traits<InputIt>::value_type>
Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>
EigenUnpackFromRowMajorVector(InputIt first, InputIt last, int rows, int cols) {
  size_t expected_size = static_cast<size_t>(rows * cols);
  if (std::distance(first, last) != static_cast<ptrdiff_t>(expected_size)) {
    throw std::invalid_argument(
        "Input range size does not match specified matrix dimensions.");
  }

  Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic, Eigen::RowMajor>
      rowMajorMat(rows, cols);
  std::copy(first, last, rowMajorMat.data());

  // Return as column-major (Eigen default)
  return Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>(rowMajorMat);
}

template <typename MatrixType>
std::vector<typename MatrixType::Scalar> EigenPackAsColumnMajorVector(
    const MatrixType& mat) {
  using Scalar = typename MatrixType::Scalar;
  // Eigen lazily evaluates some expressions, so this forces the evaluation.
  const Eigen::Matrix<typename MatrixType::Scalar, Eigen::Dynamic,
                      Eigen::Dynamic>
      evaluated = mat;
  return std::vector<Scalar>(evaluated.data(),
                             evaluated.data() + evaluated.size());
}

template <typename MatrixType, typename OutputIt>
void EigenPackAsColumnMajorVector(const MatrixType& mat, OutputIt dest) {
  // Eigen lazily evaluates some expressions, so this forces the evaluation.
  const Eigen::Matrix<typename MatrixType::Scalar, Eigen::Dynamic,
                      Eigen::Dynamic>
      evaluated = mat;

  std::copy(evaluated.data(), evaluated.data() + evaluated.size(), dest);
}

template <typename Scalar>
Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>
EigenUnpackFromColumnMajorVector(const std::vector<Scalar>& vec, int rows,
                                 int cols) {
  if (vec.size() != static_cast<size_t>(rows * cols)) {
    throw std::invalid_argument(
        "Vector size does not match matrix dimensions.");
  }

  Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> mat(rows, cols);
  std::copy(vec.begin(), vec.end(), mat.data());
  return mat;
}

template <typename InputIt,
          typename Scalar = typename std::iterator_traits<InputIt>::value_type>
Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic>
EigenUnpackFromColumnMajorVector(InputIt first, InputIt last, int rows,
                                 int cols) {
  size_t expected_size = static_cast<size_t>(rows * cols);
  if (std::distance(first, last) != static_cast<ptrdiff_t>(expected_size)) {
    throw std::invalid_argument(
        "Input range size does not match specified matrix dimensions.");
  }

  Eigen::Matrix<Scalar, Eigen::Dynamic, Eigen::Dynamic> mat(rows, cols);
  std::copy(first, last, mat.data());
  return mat;
}

}  // namespace common_utils
