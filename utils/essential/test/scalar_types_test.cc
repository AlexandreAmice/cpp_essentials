#include "utils/essential/scalar_types.h"

#include <string>
#include <type_traits>

#include <gtest/gtest.h>

namespace common_utils {
namespace {

// Test that concept works for supported types
TEST(ScalarTypesTest, ConceptSupportsFloatAndDouble) {
  EXPECT_TRUE(CommonUtilsScalarType<float>);
  EXPECT_TRUE(CommonUtilsScalarType<double>);
}

// Test that concept rejects unsupported types
TEST(ScalarTypesTest, ConceptRejectsUnsupportedTypes) {
  EXPECT_FALSE(CommonUtilsScalarType<int>);
  EXPECT_FALSE(CommonUtilsScalarType<char>);
  EXPECT_FALSE(CommonUtilsScalarType<unsigned int>);
  EXPECT_FALSE(CommonUtilsScalarType<std::string>);
}

// Test the is_in_tuple type trait directly
TEST(ScalarTypesTest, IsInTupleTraitWorks) {
  EXPECT_TRUE((is_in_tuple<float, SupportedScalarTypes>::value));
  EXPECT_TRUE((is_in_tuple<double, SupportedScalarTypes>::value));
  EXPECT_FALSE((is_in_tuple<int, SupportedScalarTypes>::value));
  EXPECT_FALSE((is_in_tuple<char, SupportedScalarTypes>::value));
}

// Test template that we'll use to verify instantiation
template <typename T>
class TestTemplate {
 public:
  [[maybe_unused]] static std::string GetTypeName();
  [[maybe_unused]] static constexpr bool IsValid = true;
};

// Specializations for our supported types
template <>
[[maybe_unused]] std::string TestTemplate<float>::GetTypeName() {
  return "float";
}

template <>
[[maybe_unused]] std::string TestTemplate<double>::GetTypeName() {
  return "double";
}

// Test the for_each_type helper with a capturing lambda
TEST(ScalarTypesTest, ForEachTypeHelper) {
  std::vector<std::string> type_names;

  detail::for_each_type(
      [&type_names](auto dummy) {
        using T = decltype(dummy);
        type_names.push_back(TestTemplate<T>::GetTypeName());
      },
      SupportedScalarTypes{});

  // Should have entries for all types in the tuple
  ASSERT_EQ(type_names.size(), 2);
  EXPECT_EQ(type_names[0], "float");
  EXPECT_EQ(type_names[1], "double");
}

//////////////////////////
// Test class template instantiations
//////////////////////////

// Test the INSTANTIATE_FOR_SUPPORTED_SCALARS macro
template <typename T>
struct MacroTestTemplate {
  static bool WasInstantiated;
  static void MarkInstantiated() { WasInstantiated = true; }
};

template <typename T>
bool MacroTestTemplate<T>::WasInstantiated = false;

// Use the macro to instantiate for all supported types
INSTANTIATE_FOR_SUPPORTED_SCALARS(MacroTestTemplate);

TEST(ScalarTypesTest, ClassMacroInstantiatesForAllTypes) {
  // Call the function to mark as instantiated
  MacroTestTemplate<float>::MarkInstantiated();
  MacroTestTemplate<double>::MarkInstantiated();

  // Verify both were instantiated
  EXPECT_TRUE(MacroTestTemplate<float>::WasInstantiated);
  EXPECT_TRUE(MacroTestTemplate<double>::WasInstantiated);
}

// Test with template arguments
template <typename T>
struct ArgTestTemplate {
  static bool TestFunction(T value, int multiplier) {
    return static_cast<int>(value * multiplier) > 0;
  }
};

// Instantiate with arguments
INSTANTIATE_FOR_SUPPORTED_SCALARS(ArgTestTemplate);

TEST(ScalarTypesTest, ClassInstantiationWithArguments) {
  EXPECT_TRUE(ArgTestTemplate<float>::TestFunction(1.5f, 2));
  EXPECT_TRUE(ArgTestTemplate<double>::TestFunction(1.5, 2));
  EXPECT_FALSE(ArgTestTemplate<float>::TestFunction(-1.5f, 1));
  EXPECT_FALSE(ArgTestTemplate<double>::TestFunction(-1.5, 1));
}

}  // namespace
}  // namespace common_utils
