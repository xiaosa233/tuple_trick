#include "tuple_trick/tuple_trick.h"

#include <string>
#include <string_view>

#include "gtest/gtest.h"

namespace tuple_trick {

struct FirstWidget {
  static constexpr std::string_view WidgetName() { return "First"; }

  static constexpr int value = 0;
};

struct SecondWidget {
  static constexpr std::string_view WidgetName() { return "Second"; }

  static constexpr int value = 1;
};

std::string ViewToString(const std::string_view& str_view) {
  return std::string(str_view.data(), str_view.size());
}

using TupleType = std::tuple<FirstWidget, SecondWidget>;
using void_operator_tag = std::integral_constant<size_t, 0>;
using foreach_tag = std::integral_constant<size_t, 1>;

template <size_t Index, class TupleT>
struct TupleOperator {
  static_assert(std::is_same_v<TupleT, TupleType>, "Not same type!");
  using TupleElement =
      std::tuple_element_t <
      Index<std::tuple_size_v<TupleT> ? Index : std::tuple_size_v<TupleT> - 1, TupleT>;
  constexpr std::string_view operator()() const { return TupleElement::WidgetName(); }

  constexpr int operator()(int i) const { return TupleElement::value + i; }

  void operator()(void_operator_tag, bool* verify_value) {
    // Do nothing.
    *verify_value = true;
  }

  void operator()(foreach_tag, std::vector<std::string_view>* name) { name->push_back((*this)()); }
};

TEST(TupleTrickTest, IndexOperation) {

  // Test with no-parameter
  {
    const int i = 0;
    const std::string_view res = IndexOperator<TupleType, TupleOperator>()(i);
    EXPECT_EQ("First", res);
  }
  {
    const std::string_view res = IndexOperator<TupleType, TupleOperator>()(1);
    EXPECT_EQ("Second", res);
  }

  // Test with parameter
  {
    const int value = IndexOperator<TupleType, TupleOperator>()(0, 2);
    EXPECT_EQ(2, value);
  }
  {
    const int value = IndexOperator<TupleType, TupleOperator>()(1, 2);
    EXPECT_EQ(3, value);
  }
  {
    // Test with constexpr
    constexpr int value = IndexOperator<TupleType, TupleOperator>()(0, 2);
    EXPECT_EQ(2, value);
  }

  // Test void return
  {
    bool verify_call = false;
    IndexOperator<TupleType, TupleOperator>()(0, void_operator_tag{}, &verify_call);
    EXPECT_TRUE(verify_call);
  }
}

template <size_t Index, class TupleT>
struct FindIfFunc {
  constexpr bool operator()(const std::string_view& str_view) const {
    return str_view == TupleOperator<Index, TupleT>()();
  }
};

template <size_t Index, class TupleT>
struct FindIfWithoutParameterFunc {
  constexpr bool operator()(...) const { return "Second" == TupleOperator<Index, TupleT>()(); }
};

TEST(TupleTrickTest, FindIfOperation) {
  // Test without action, return index.
  {
    size_t value = FindIfOperator<TupleType, FindIfFunc>()("Second");
    EXPECT_EQ(1, value);
  }
  {
    constexpr size_t value = FindIfOperator<TupleType, FindIfFunc>()("First");
    EXPECT_EQ(0, value);
  }
  {
    constexpr size_t value = FindIfOperator<TupleType, FindIfFunc>()("NotFoundWidgetName");
    EXPECT_EQ(2, value);
  }

  // Test with action.
  {
    bool verify_value = false;
    const int value = FindIfOperator<TupleType, FindIfWithoutParameterFunc, TupleOperator>()(2);
    EXPECT_EQ(3, value);
  }
}

template <size_t Index, class TupleT>
struct SelfIncreament {
  void operator()(TupleT* tuple_value) const { ++std::get<Index>(*tuple_value); }
};

TEST(TupleTrickTest, ForEachOperation) {
  {
    std::vector<std::string_view> names;
    ForEachOperator<TupleType, TupleOperator>()(foreach_tag{}, &names);
    EXPECT_EQ(2, names.size());
    EXPECT_EQ("First", names[0]);
    EXPECT_EQ("Second", names[1]);
  }

  {
    auto tuple_value = std::make_tuple<int, size_t, double>(-1, 0, 1.0);
    ForEachOperator<decltype(tuple_value), SelfIncreament>()(&tuple_value);
    EXPECT_EQ(0, std::get<0>(tuple_value));
    EXPECT_EQ(1, std::get<1>(tuple_value));
    EXPECT_EQ(2.0, std::get<2>(tuple_value));
  }
}

} // namespace tuple_trick
