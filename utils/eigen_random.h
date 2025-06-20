#include "utils/random.h"
#include <Eigen/Dense>

namespace common_utils {

/**
 * Returns a random Eigen Matrix of size @param rows by @param cols with
 * entries distributed uniformly in [-1,1] using the source of randomness
 * provided by
 * @param generator
 */
Eigen::MatrixXd MatrixXdRandom(int rows, int cols, RandomGenerator* generator);
/**
 * Returns a random Eigen Matrix of size @param rows by @param cols with
 * entries distributed according to a standard Gaussian using the source of
 * randomness provided by
 * @param generator
 */
Eigen::MatrixXd MatrixXdRandn(int rows, int cols, RandomGenerator* generator);
/**
 * Returns a random Eigen VectorXd of size @param rows with
 * entries distributed uniformly in [-1,1] using the source of
 * randomness provided by
 * @param generator
 */
Eigen::VectorXd VectorXdRandom(int rows, RandomGenerator* generator);
/**
 * Returns a random Eigen VectoXdr of size @param rows with
 * entries distributed according to a standard Gaussian using the source of
 * randomness provided by
 * @param generator
 */
Eigen::VectorXd VectorXdRandn(int rows, RandomGenerator* generator);

}  // namespace common_utils
