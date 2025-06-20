#pragma once
#include <gtest/gtest.h>

namespace common_utils::testing {
template <typename Tuple,
          typename Indices = std::make_index_sequence<std::tuple_size_v<Tuple>>>
struct TupleToTestingTypes;

template <typename Tuple, std::size_t... Is>
struct TupleToTestingTypes<Tuple, std::index_sequence<Is...>> {
  using type = ::testing::Types<std::tuple_element_t<Is, Tuple>...>;
};

}  // namespace common_utils::testing
