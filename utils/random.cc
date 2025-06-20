#include "utils/random.h"

#include <Eigen/Dense>
/**
 * This file is copied from Drake's common/random.cc
 * implementation with appropriate modifications.
 * Consider revisiting this file if we need AutoDiffXd.
 */
namespace common_utils {
std::unique_ptr<RandomGenerator::Engine> RandomGenerator::CreateEngine(
    result_type seed) {
  return std::make_unique<RandomGenerator::Engine>(seed);
}

double CalcProbabilityDensity(RandomDistribution distribution,
                              const Eigen::Ref<const Eigen::VectorXd>& x) {
  switch (distribution) {
    case RandomDistribution::kUniform: {
      for (int i = 0; i < x.rows(); ++i) {
        if (x(i) < 0.0 || x(i) > 1.0) {
          return 0.;
        }
      }
      return 1.;
    }
    case RandomDistribution::kGaussian: {
      return ((-0.5 * x.array() * x.array()).exp() / std::sqrt(2 * M_PI))
          .prod();
    }
    case RandomDistribution::kExponential: {
      for (int i = 0; i < x.rows(); ++i) {
        if (x(i) < 0.0) {
          return 0.;
        }
      }
      return (-x.array()).exp().prod();
    }
  }
  COMMON_UTILS_UNREACHABLE();
}

}  // namespace common_utils
