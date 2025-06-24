#pragma once

#include <memory>
#include <random>

#include "utils/copyable_unique_ptr.h"
#include "utils/common_utils_copyable.h"
#include <Eigen/Core>

/**
 * This file is copied from Drake's common/random.h
 * implementation with appropriate modifications.
 */

namespace common_utils {
/// Defines CommonUtils's canonical implementation of the UniformRandomBitGenerator
/// C++ concept (as well as a few conventional extras beyond the concept, e.g.,
/// seeds).  This uses the 32-bit Mersenne Twister mt19937 by Matsumoto and
/// Nishimura, 1998.  For more information, see
/// https://en.cppreference.com/w/cpp/numeric/random/mersenne_twister_engine
class RandomGenerator {
 public:
  COMMON_UTILS_DEFAULT_COPY_AND_MOVE_AND_ASSIGN(RandomGenerator);

  using result_type = std::mt19937::result_type;

  /// Creates a generator using the `default_seed`.  Actual creation of the
  /// generator is deferred until first use so this constructor is fast and
  /// does not allocate any heap memory.
  RandomGenerator() = default;

  /// Creates a generator using given `seed`.
  explicit RandomGenerator(result_type seed) : generator_(CreateEngine(seed)) {}

  static constexpr result_type min() { return Engine::min(); }
  static constexpr result_type max() { return Engine::max(); }

  /// Generates a pseudo-random value.
  result_type operator()() {
    // Use lazy construction here, so that the default constructor can be fast.
    if (generator_ == nullptr) {
      generator_ = CreateEngine(default_seed);
    }
    return (*generator_)();
  }

  static constexpr result_type default_seed = std::mt19937::default_seed;

 private:
  using Engine = std::mt19937;

  static std::unique_ptr<Engine> CreateEngine(result_type seed);

  copyable_unique_ptr<Engine> generator_;
};

/// CommonUtils supports explicit reasoning about a few carefully chosen random
/// distributions.
enum class RandomDistribution {
  kUniform = 0,   ///< Vector elements are independent and uniformly distributed
                  ///  ∈ [0.0, 1.0).
  kGaussian = 1,  ///< Vector elements are independent and drawn from a
                  ///  mean-zero, unit-variance normal (Gaussian) distribution.
  kExponential = 2,  ///< Vector elements are independent and drawn from an
                     ///  exponential distribution with λ=1.0.
};

/**
 * Calculates the density (probability density function) of the multivariate
 * distribution.
 * @param distribution The distribution type.
 * @param x The value of the sampled vector.
 * @tparam_nonsymbolic_scalar
 *
 * @note When instantiating this function, the user needs to explicitly pass in
 * the scalar type, for example CalcProbabilityDensity<double>(...), the
 * compiler might have problem to deduce the scalar type automatically.
 */
double CalcProbabilityDensity(RandomDistribution distribution,
                              const Eigen::Ref<const Eigen::VectorXd>& x);
}  // namespace common_utils
