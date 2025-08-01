// TODO(jwnimmer-tri) Write our own formatting logic instead of using Eigen IO,
// and add customization flags for how to display the matrix data.
#undef EIGEN_NO_IO
#include "utils/fmt_eigen.h"

#include <limits>
#include <sstream>

namespace common_utils {
namespace internal {

template <typename T>
using FormatterEigenRef =
    Eigen::Ref<const Eigen::Matrix<T, Eigen::Dynamic, Eigen::Dynamic>>;

template <typename Scalar>
std::string FormatEigenMatrix(const FormatterEigenRef<Scalar>& matrix)
  requires std::is_same_v<Scalar, double> || std::is_same_v<Scalar, float> ||
           std::is_same_v<Scalar, std::string>
{  // NOLINT(whitespace/braces)
  std::stringstream stream;
  // We'll print our matrix data using as much precision as we can, so that
  // console log output and/or error messages paint the full picture. Sadly,
  // the ostream family of floating-point formatters doesn't know how to do
  // "shortest round-trip precision". If we set the precision to max_digits,
  // then simple numbers like "1.1" print as "1.1000000000000001"; instead,
  // we'll use max_digits - 1 to avoid that problem, with the risk of losing
  // the last ulps in the printout in case we needed that last digit. This
  // will all be fixed once we stop using Eigen IO.
  stream.precision(std::numeric_limits<double>::max_digits10 - 1);
  Eigen::IOFormat format(Eigen::StreamPrecision, 0, ", ", "\n", "[", "]");
  stream << matrix.format(format);
  return stream.str();
}

// Explicitly instantiate for the allowed scalar types in our header.
template std::string FormatEigenMatrix<double>(
    const FormatterEigenRef<double>& matrix);
template std::string FormatEigenMatrix<float>(
    const FormatterEigenRef<float>& matrix);
template std::string FormatEigenMatrix<std::string>(
    const FormatterEigenRef<std::string>& matrix);

}  // namespace internal
}  // namespace common_utils
