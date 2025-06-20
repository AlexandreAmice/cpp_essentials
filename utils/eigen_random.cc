#include "utils/eigen_random.h"

#include <random>

namespace common_utils {

Eigen::MatrixXd MatrixXdRandom(int rows, int cols, RandomGenerator* generator) {
  std::uniform_real_distribution<double> distribution(-1, 1);
  Eigen::MatrixXd matrix(rows, cols);
  matrix = matrix.unaryExpr([&](double) {
    return distribution(*generator);
  });
  return matrix;
};

Eigen::MatrixXd MatrixXdRandn(int rows, int cols, RandomGenerator* generator) {
  std::normal_distribution<double> distribution;
  Eigen::MatrixXd matrix(rows, cols);
  matrix = matrix.unaryExpr([&](double) {
    return distribution(*generator);
  });
  return matrix;
};

Eigen::VectorXd VectorXdRandom(int rows, RandomGenerator* generator) {
  std::uniform_real_distribution<double> distribution(-1, 1);
  Eigen::VectorXd vector(rows);
  vector = vector.unaryExpr([&](double) {
    return distribution(*generator);
  });
  return vector;
};

Eigen::VectorXd VectorXdRandn(int rows, RandomGenerator* generator) {
  std::normal_distribution<double> distribution;
  Eigen::VectorXd vector(rows);
  vector = vector.unaryExpr([&](double) {
    return distribution(*generator);
  });
  return vector;
};

}  // namespace common_utils
