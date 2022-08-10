#pragma once

#include <tuple>

namespace tuple_trick {

/* Worker is the unit trait class to help combine functions.
   It accept an target index to set the end runtime condition from the process:
     0 --> i
   FindIf template class to set the find condition, return true to stop the next derivation.
   Operator template class to do the final aciton when meet the condition( FindIf or index).
   LimitIndex set to if we should limit the Index to N-1 when pass to the Operator class.

   Regardless of the result of runtime condition, it will be instantiated to the step where Index==N
   is deduced at compile time, so we need to specify this.

   For any find semantic operator, Index == N can be return, as it means no result meet the
   condition.
*/
template <size_t Index, size_t N, class Tuple, template <size_t, class> class FindIf,
          template <size_t, class> class Operator, bool LimitIndex = true>
struct Worker {
  static_assert(N == std::tuple_size_v<Tuple>, "N is not equal to Tuple!");
  constexpr Worker(int in_i) : i(in_i) {}
  template <class... Args>
  constexpr decltype(auto) operator()(Args&&... args) const {
    if (FindIf<Index, Tuple>()(std::forward<Args>(args)...) || i == Index) {
      return Operator<Index, Tuple>()(std::forward<Args>(args)...);
    }
    return Worker<Index + 1, N, Tuple, FindIf, Operator, LimitIndex>(i)(
        std::forward<Args>(args)...);
  }

  int i = 0;
};

template <size_t N, class Tuple, template <size_t, class> class FindIf,
          template <size_t, class> class Operator, bool LimitIndex>
struct Worker<N, N, Tuple, FindIf, Operator, LimitIndex> {
  static_assert(N == std::tuple_size_v<Tuple>, "N is not equal to Tuple!");
  constexpr Worker(int in_i) : i(in_i) {}

  template <class... Args>
  constexpr decltype(auto) operator()(Args&&... args) const {
    constexpr size_t kOperatorIndex = LimitIndex ? N - 1 : N;
    return Operator<kOperatorIndex, Tuple>()(std::forward<Args>(args)...);
  }

  int i = 0;
};

// Call FuncOperator at the index i.
template <class TupleT, template <size_t, class> class FuncOperator>
struct IndexOperator;

// Find if meet the condition. Return index default.
// When ActionOperator also provide, then the result will pass to the ActionOerator.
template <class TupleT, template <size_t, class> class FindFunc,
          template <size_t, class> class ActionOperator>
struct FindIfOperator;

// Call FuncOperator for each element.
template <class TupleT, template <size_t, class> class FuncOperator>
struct ForEachOperator;


// ---------------- Implementation for Operator -------------------------

// return false always for Findif operator.
template <size_t Index, class TupleT>
struct DummyFindIf {
  constexpr bool operator()(...) const noexcept { return false; }
};

// Return Index for Find Action.
template <size_t Index, class TupleT>
struct DummyFindAction {
  constexpr size_t operator()(...) const noexcept { return Index; }
};

template <class TupleT, template <size_t, class> class FuncOperator>
struct IndexOperator {
  template <class... Args>
  constexpr decltype(auto) operator()(int i, Args&&... args) const {
    return Worker<0, std::tuple_size_v<TupleT>, TupleT, DummyFindIf, FuncOperator>(i)(
        std::forward<Args>(args)...);
  }
};

template <class TupleT, template <size_t, class> class FindFunc,
          template <size_t, class> class ActionOperator = DummyFindAction>
struct FindIfOperator {
  template <class... Args>
  constexpr decltype(auto) operator()(Args&&... args) const {
    return Worker<0, std::tuple_size_v<TupleT>, TupleT, FindFunc, ActionOperator, false>(
        std::tuple_size_v<TupleT>)(std::forward<Args>(args)...);
  }
};

template <class TupleT, template <size_t, class> class FuncOperator>
struct ForEachOperator {
  // Mock find operator to traval each type.
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
