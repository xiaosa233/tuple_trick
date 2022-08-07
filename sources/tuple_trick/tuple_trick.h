#pragma once

#include <tuple>

namespace tuple_trick {

template <class T, class = void>
struct HasNoLimitIndex {
  static constexpr bool value = false;
};

template <class T>
struct HasNoLimitIndex<T, std::void_t<decltype(T::kNoLimitIndex)>> {
  static constexpr bool value = T::kNoLimitIndex;
};

template <size_t Index, size_t N, class Tuple, template <size_t, class> class FindIf,
          template <size_t, class> class Operator>
struct Worker {
  static_assert(N == std::tuple_size_v<Tuple>, "N is not equal to Tuple!");
  constexpr Worker(int in_i) : i(in_i) {}
  template <class... Args>
  constexpr decltype(auto) operator()(Args&&... args) const {
    if (FindIf<Index, Tuple>()(std::forward<Args>(args)...) || i == Index) {
      return Operator<Index, Tuple>()(std::forward<Args>(args)...);
    }
    return Worker<Index + 1, N, Tuple, FindIf, Operator>(i)(std::forward<Args>(args)...);
  }

  int i = 0;
};

template <size_t N, class Tuple, template <size_t, class> class FindIf,
          template <size_t, class> class Operator>
struct Worker<N, N, Tuple, FindIf, Operator> {
  static_assert(N == std::tuple_size_v<Tuple>, "N is not equal to Tuple!");
  constexpr Worker(int in_i) : i(in_i) {}

  template <class... Args>
  constexpr decltype(auto) operator()(Args&&... args) const {
    constexpr size_t kOperatorIndex = HasNoLimitIndex<Operator<0, Tuple>>::value ? N : N - 1;
    return Operator<kOperatorIndex, Tuple>()(std::forward<Args>(args)...);
  }

  int i = 0;
};

template <size_t Index, class TupleT>
struct DummyFindIf {
  constexpr bool operator()(...) const noexcept { return false; }
};

template <class TupleT, template <size_t, class> class FuncOperator>
struct IndexOperator {
  template <class... Args>
  constexpr decltype(auto) operator()(int i, Args&&... args) const {
    return Worker<0, std::tuple_size_v<TupleT>, TupleT, DummyFindIf, FuncOperator>(i)(
        std::forward<Args>(args)...);
  }
};

template <size_t Index, class TupleT>
struct DummyFindAction {
  static constexpr bool kNoLimitIndex = true;
  constexpr size_t operator()(...) const noexcept { return Index; }
};

template <class TupleT, template <size_t, class> class FindFunc,
          template <size_t, class> class ActionOperator = DummyFindAction>
struct FindIfOperator {
  template <class... Args>
  constexpr decltype(auto) operator()(Args&&... args) const {
    return Worker<0, std::tuple_size_v<TupleT>, TupleT, FindFunc, ActionOperator>(
        std::tuple_size_v<TupleT>)(std::forward<Args>(args)...);
  }
};

template <class TupleT, template <size_t, class> class FuncOperator>
struct ForEachOperator {
  template <size_t Index, class TupleE>
  struct OperatorType {
    template <class... Args>
    constexpr bool operator()(Args&&... args) const {
      FuncOperator<Index, TupleE>()(std::forward<Args>(args)...);
      return false;
    }
  };

  template <class... Args>
  constexpr void operator()(Args&&... args) const {
    Worker<0, std::tuple_size_v<TupleT>, TupleT, OperatorType, DummyFindAction>(
        std::tuple_size_v<TupleT> - 1)(std::forward<Args>(args)...);
  }
};

} // namespace tuple_trick
