#include "utils/identifier.h"

#include <set>
#include <sstream>
#include <unordered_map>
#include <unordered_set>
#include <utility>

#include "utils/sorted_pair.h"
#include "utils/test_utils/expect_no_throw.h"
#include "utils/test_utils/expect_throws_message.h"
#include <gtest/gtest.h>

namespace common_utils {
namespace {

// Creates various dummy index types to test.
using std::set;
using std::stringstream;
using std::unordered_map;
using std::unordered_set;
using AId = Identifier<class ATag>;
using BId = Identifier<class BTag>;

class IdentifierTests : public ::testing::Test {
 protected:
  // Configuration of test *case* variables (instead of per-test) is important.
  // The tests can evaluate in any order and run in the same memory space.
  // Those tests that depend on identifier *value* could easily become invalid
  // based on execution order. This guarantees a fixed set of identifiers
  // with known values.
  static void SetUpTestCase() {
    a1_ = AId::get_new_id();  // Should have the value 1.
    a2_ = AId::get_new_id();  // Should have the value 2.
    a3_ = AId::get_new_id();  // Should have the value 3.
    b_ = BId::get_new_id();   // Should have the value 1.
  }

  static AId a1_;
  static AId a2_;
  static AId a3_;
  static BId b_;
};

AId IdentifierTests::a1_;
AId IdentifierTests::a2_;
AId IdentifierTests::a3_;
BId IdentifierTests::b_;

// Verifies the copy constructor. This implicitly tests the expected property
// of the get_new_id() factory method and the get_value() method.
TEST_F(IdentifierTests, Constructor) {
  EXPECT_EQ(a1_.get_value(), 1);
  EXPECT_EQ(a2_.get_value(), 2);
  EXPECT_EQ(a3_.get_value(), 3);
  AId temp(a2_);
  EXPECT_EQ(temp.get_value(), 2);
  AId bad;
  EXPECT_FALSE(bad.is_valid());
  EXPECT_TRUE(a2_.is_valid());
}

// Confirms that assignment behaves correctly. This also implicitly tests
// equality and inequality.
TEST_F(IdentifierTests, AssignmentAndComparison) {
  EXPECT_TRUE(a2_ != a3_);
  AId temp = a2_;
  EXPECT_TRUE(temp == a2_);
  temp = a3_;
  EXPECT_TRUE(temp == a3_);
}

// Check the specialized internal-use compare that can be used on an invalid
// Identifier.
TEST_F(IdentifierTests, InvalidOrSameComparison) {
  AId same_as_a1 = a1_;
  EXPECT_TRUE(same_as_a1.is_same_as_valid_id(a1_));
  EXPECT_FALSE(a1_.is_same_as_valid_id(a2_));

  AId invalid;
  EXPECT_FALSE(invalid.is_same_as_valid_id(a1_));
}

// Confirms that ids are configured to serve as unique keys in
// STL containers.
TEST_F(IdentifierTests, ServeAsMapKey) {
  unordered_set<AId> ids;

  // This is a *different* id with the *same* value as a1. It should *not*
  // introduce a new value to the set.
  AId temp = a1_;

  EXPECT_EQ(ids.size(), 0);
  ids.insert(a1_);
  EXPECT_NE(ids.find(a1_), ids.end());
  EXPECT_NE(ids.find(temp), ids.end());

  EXPECT_EQ(ids.size(), 1);
  ids.insert(a2_);
  EXPECT_EQ(ids.size(), 2);

  ids.insert(temp);
  EXPECT_EQ(ids.size(), 2);

  EXPECT_EQ(ids.find(a3_), ids.end());
}

// Checks the abseil-specific hash function. When a class does not provide an
// abseil-specific hash function, abseil will fall back to invoking std::hash
// on the class to obtain a size_t hash value and then feeding that size_t into
// the abseil hasher. This leads to slow and bloated object code. We want to
// prove that our abseil-specific hash function is being used; we can do that
// by checking which hash value comes out of an absl container hasher.
// WE OMIT THE ABSEIL TEST IN THESE UTILITIES. TODO(Alexandre.Amice) include
// abseil?
//  TEST_F(IdentifierTests, AbslHash) {
//   // Compute the hash value used by an absl unordered container.
//   // We'll want to demonstrate that this is the specialized hash value.
//   absl::flat_hash_set<AId>::hasher absl_id_hasher;
//   const size_t absl_hash = absl_id_hasher(a1_);

//   // Compute the unspecialized hash value that would be seen in case absl
//   // delegated to the std hasher.
//   const size_t std_hash = std::hash<AId>{}(a1_);
//   absl::flat_hash_set<size_t>::hasher absl_uint_hasher;
//   const size_t absl_hash_via_std_hash = absl_uint_hasher(std_hash);

//   // To demonstrate that the specialization worked, the specialized hash must
//   // differ from the fallback hash.
//   EXPECT_NE(absl_hash, absl_hash_via_std_hash);
// }

// Confirms that SortedPair<FooId> can serve as a key in STL containers.
// This shows that Identifier is not just hashable, but implicitly shows that
// it is compatible with the Drake hash mechanism (because it assumes that the
// SortedPair is compatible).
TEST_F(IdentifierTests, SortedPairAsKey) {
  unordered_set<SortedPair<AId>> ids;

  EXPECT_EQ(ids.size(), 0u);
  ids.insert({a1_, a2_});
  EXPECT_EQ(ids.size(), 1u);

  // An equivalent pair to what was inserted.
  SortedPair<AId> pair{a1_, a2_};
  EXPECT_NE(ids.find(pair), ids.end());
}

// Confirms that ids are configured to serve as values in STL containers.
TEST_F(IdentifierTests, ServeAsMapValue) {
  unordered_map<BId, AId> ids;

  BId b1 = BId::get_new_id();
  BId b2 = BId::get_new_id();
  BId b3 = BId::get_new_id();
  ids.emplace(b1, a1_);
  ids.emplace(b2, a2_);
  EXPECT_EQ(ids.find(b3), ids.end());
  EXPECT_NE(ids.find(b2), ids.end());
  EXPECT_NE(ids.find(b1), ids.end());
  ids[b3] = a3_;
  EXPECT_NE(ids.find(b3), ids.end());
}

// Confirms that ids can be put into a set.
TEST_F(IdentifierTests, PutInSet) {
  set<AId> ids;
  AId a1 = AId::get_new_id();
  AId a2 = AId::get_new_id();

  EXPECT_EQ(ids.size(), 0u);
  ids.insert(a1);
  EXPECT_EQ(ids.size(), 1u);
  EXPECT_TRUE(ids.contains(a1));

  ids.insert(a2);
  EXPECT_EQ(ids.size(), 2u);
  EXPECT_TRUE(ids.contains(a2));

  ids.insert(a1);
  EXPECT_EQ(ids.size(), 2u);
  EXPECT_TRUE(ids.contains(a1));
}

// Tests the streaming behavior.
TEST_F(IdentifierTests, StreamOperator) {
  stringstream ss;
  ss << a2_;
  EXPECT_EQ(ss.str(), "2");
}

// Tests the ability to convert the id to string via std::to_string.
TEST_F(IdentifierTests, ToString) {
  using std::to_string;
  EXPECT_EQ(to_string(a2_), to_string(a2_.get_value()));
}

// These tests confirm that behavior that *shouldn't* be compilable isn't.

// This code allows us to turn compile-time errors into run-time errors that
// we can incorporate in a unit test.  The macro simplifies the boilerplate.
// This macro confirms binary operations are *valid* between two ids of
// the same type, but invalid between an id and objects of *any* other type.
// (Although the space of "all other types", is sparsely sampled).
//
// The use is:
//    BINARY_TEST( op, op_name )
// It produces the templated method: has_op_name<T, U>(), which returns true
// if `t op u` is a valid operation for `T t` and `U u`.
//
// Examples of invocations:
//    op    |   op_name
// ---------+-------------
//   ==     |   equals
//    <     |   less_than
//    +     |   add
#define BINARY_TEST(OP, OP_NAME)                                         \
  template <typename T, typename U,                                      \
            typename = decltype(std::declval<T>() OP std::declval<U>())> \
  bool has_##OP_NAME##_helper(int) {                                     \
    return true;                                                         \
  }                                                                      \
  template <typename T, typename U>                                      \
  bool has_##OP_NAME##_helper(...) {                                     \
    return false;                                                        \
  }                                                                      \
  template <typename T, typename U>                                      \
  bool has_##OP_NAME() {                                                 \
    return has_##OP_NAME##_helper<T, U>(1);                              \
  }                                                                      \
  TEST_F(IdentifierTests, OP_NAME##OperatorAvailiblity) {                \
    EXPECT_FALSE((has_##OP_NAME<AId, BId>()));                           \
    EXPECT_TRUE((has_##OP_NAME<AId, AId>()));                            \
    EXPECT_FALSE((has_##OP_NAME<AId, int>()));                           \
    EXPECT_FALSE((has_##OP_NAME<AId, size_t>()));                        \
    EXPECT_FALSE((has_##OP_NAME<AId, int64_t>()));                       \
  }
BINARY_TEST(==, Equals)
BINARY_TEST(!=, NotEquals)
BINARY_TEST(=, Assignment)

// This test should pass, as long as it compiles. If the Identifier class were
// to change and allow conversion between identifiers (or between identifiers
// and ints), this would fail to compile.
TEST_F(IdentifierTests, Convertible) {
  static_assert(!std::is_convertible_v<AId, BId>,
                "Identifiers of different types should not be convertible.");
  static_assert(!std::is_convertible_v<AId, int>,
                "Identifiers should not be convertible to ints.");
  static_assert(!std::is_convertible_v<int, AId>,
                "Identifiers should not be convertible from ints");
}

// Attempting to acquire the value is an error.
TEST_F(IdentifierTests, InvalidGetValueCall) {
  if (kCommonUtilsAssertIsDisarmed) {
    return;
  }
  AId invalid;
  COMMON_UTILS_EXPECT_THROWS_MESSAGE(invalid.get_value(),
                                     ".*is_valid.*failed.*");
}

// Comparison of invalid ids is an error.
TEST_F(IdentifierTests, InvalidEqualityCompare) {
  if (kCommonUtilsAssertIsDisarmed) {
    return;
  }
  AId invalid;
  COMMON_UTILS_EXPECT_THROWS_MESSAGE(invalid == a1_, ".*is_valid.*failed.*");
}

// Comparison of invalid ids is an error.
TEST_F(IdentifierTests, InvalidInequalityCompare) {
  if (kCommonUtilsAssertIsDisarmed) {
    return;
  }
  AId invalid;
  COMMON_UTILS_EXPECT_THROWS_MESSAGE(invalid != a1_, ".*is_valid.*failed.*");
}

// Comparison of invalid ids is an error.
TEST_F(IdentifierTests, BadInvalidOrSameComparison) {
  if (kCommonUtilsAssertIsDisarmed) {
    return;
  }
  AId invalid;
  COMMON_UTILS_EXPECT_THROWS_MESSAGE(a1_.is_same_as_valid_id(invalid),
                                     ".*is_valid.*failed.*");
}

// Hashing an invalid id is *not* an error.
TEST_F(IdentifierTests, InvalidHash) {
  if (kCommonUtilsAssertIsDisarmed) {
    return;
  }
  std::unordered_set<AId> ids;
  AId invalid;
  COMMON_UTILS_EXPECT_NO_THROW(ids.insert(invalid));
}

// Streaming an invalid id is an error.
TEST_F(IdentifierTests, InvalidStream) {
  if (kCommonUtilsAssertIsDisarmed) {
    return;
  }
  AId invalid;
  std::stringstream ss;
  COMMON_UTILS_EXPECT_THROWS_MESSAGE(ss << invalid, ".*is_valid.*failed.*");
}

}  // namespace
}  // namespace common_utils
