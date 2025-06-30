#pragma once
#include <tuple>
#include <type_traits>

namespace common_utils {

// Define all supported scalar types in one place as a tuple. If you want to
// update the scalars ONLY CHANGE THIS.
using SupportedScalarTypes = std::tuple<float, double>;

// Helper meta-function to check if a type is in the tuple
template <typename T, typename Tuple>
struct is_in_tuple;

template <typename T>
struct is_in_tuple<T, std::tuple<>> : std::false_type {};

template <typename T, typename U, typename... Rest>
struct is_in_tuple<T, std::tuple<U, Rest...>>
    : std::conditional_t<std::is_same_v<T, U>, std::true_type,
                         is_in_tuple<T, std::tuple<Rest...>>> {};

// The concept definition using the tuple
template <typename T>
concept CommonUtilsScalarType = is_in_tuple<T, SupportedScalarTypes>::value;

namespace detail {
// Helper for expanding tuple types to template instantiations
template <typename Func, typename... Ts, std::size_t... Is>
void for_each_type_impl(Func&& f, std::tuple<Ts...>,
                        std::index_sequence<Is...>) {
  (f.template operator()<std::tuple_element_t<Is, std::tuple<Ts...>>>({}), ...);
}

template <typename Func, typename... Ts>
void for_each_type(Func&& f, std::tuple<Ts...> t) {
  for_each_type_impl(std::forward<Func>(f), t,
                     std::index_sequence_for<Ts...>{});
}

// Simpler helper specifically for template instantiation
template <template <typename> class TemplateName, typename... Ts,
          std::size_t... Is>
void instantiate_for_types(std::tuple<Ts...>, std::index_sequence<Is...>) {
  // Force instantiation of template for each type in tuple
  (void(sizeof(TemplateName<std::tuple_element_t<Is, std::tuple<Ts...>>>)),
   ...);
}

// Helper for generating explicit instantiations for all types in a tuple
template <typename TypeTuple, typename Handler>
struct ExplicitInstantiationHelper;

template <typename... Types, typename Handler>
struct ExplicitInstantiationHelper<std::tuple<Types...>, Handler> {
  static void Apply() { Handler::template ForEach<Types...>(); }
};

// Function template instantiation helper
template <template <typename> typename Func>
struct FunctionInstantiator {
  template <typename... Types>
  static void ForEach() {
    // Using fold expression to expand for all types
    (void(sizeof(Func<Types>)), ...);
  }
};

// Class template instantiation helper (for explicit instantiation)
template <template <typename> typename Class>
struct ClassInstantiator {
  template <typename... Types>
  static void ForEach() {
    // This will generate "template Class<Type>;" for each Type
    // The comma operator and void cast are to handle the expansion
    ((void)sizeof(Class<Types>), ...);
  }
};

}  // namespace detail

#define INSTANTIATE_FOR_SUPPORTED_SCALARS(template_name)                 \
  namespace {                                                            \
  struct InstantiationHelper_##template_name {                           \
    InstantiationHelper_##template_name() {                              \
      common_utils::detail::instantiate_for_types<template_name>(        \
          common_utils::SupportedScalarTypes{},                          \
          std::make_index_sequence<                                      \
              std::tuple_size_v<common_utils::SupportedScalarTypes>>{}); \
    }                                                                    \
  };                                                                     \
  static InstantiationHelper_##template_name                             \
      instantiation_helper__##template_name;                             \
  }

// For functions that need explicit instantiation
// This uses a dynamic approach to generate all needed instantiations
#define INSTANTIATE_FUNCTION_FOR_SCALAR_TYPES(func_template)          \
  namespace {                                                         \
  template <typename T>                                               \
  using func_template_type = decltype(func_template<T>);              \
  struct func_template##_instantiator {                               \
    template <typename... Types>                                      \
    static void ForEach() {                                           \
      /* This will expand to separate instantiations for each type */ \
      using expander = int[];                                         \
      (void)expander{0, (void(func_template<Types>), 0)...};          \
    }                                                                 \
  };                                                                  \
  /* Force instantiation at global scope */                           \
  static const auto func_template##_instantiations = []() {           \
    common_utils::detail::ExplicitInstantiationHelper<                \
        common_utils::SupportedScalarTypes,                           \
        func_template##_instantiator>::Apply();                       \
    return 0;                                                         \
  }();                                                                \
  }

// Stubs that might work for header files.
/*
// // Macro for declaring explicit instantiations in headers
// #define DECLARE_FOR_SUPPORTED_SCALARS(template_expr)              \
//   namespace {                                                     \
//   template <typename T>                                           \
//   struct template_expr##_declarer {};                             \
//                                                                   \
//   template <>                                                     \
//   struct template_expr##_declarer<void> {                         \
//     template_expr##_declarer() {                                  \
//       common_utils::detail::for_each_type(                               \
//           [](auto dummy) {                                        \
//             using T = decltype(dummy);                            \
//             extern template_expr<T>;                              \
//           },                                                      \
//           common_utils::SupportedScalarTypes{});                         \
//     }                                                             \
//   };                                                              \
//                                                                   \
//   static template_expr##_declarer<void> template_expr##_instance; \
//   }

// Macro for defining explicit instantiations in implementation files
// #define DEFINE_FOR_SUPPORTED_SCALARS(template_expr)              \
//   namespace {                                                    \
//   template <typename T>                                          \
//   struct template_expr##_definer {};                             \
//                                                                  \
//   template <>                                                    \
//   struct template_expr##_definer<void> {                         \
//     template_expr##_definer() {                                  \
//       common_utils::detail::for_each_type(                              \
//           [](auto dummy) {                                       \
//             using T = decltype(dummy);                           \
//             template template_expr<T>;                           \
//           },                                                     \
//           common_utils::SupportedScalarTypes{});                        \
//     }                                                            \
//   };                                                             \
//                                                                  \
//   static template_expr##_definer<void> template_expr##_instance; \
//   }
*/
}  // namespace common_utils
