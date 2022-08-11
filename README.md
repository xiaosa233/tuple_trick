# C++ Tricks For Tuple

--------------

tricks for c++ tuple

# How to use

Firstly, you need to define some tuples.

For example:
```

enum class WidgetType {
    kFirst,
    kSecond
};

template<WidgetType TType>
struct Widget;


template<>
struct Widget<WidgetType::kFirst> {
  static constexpr std::string_view Name() {
    return "First";
  }

  int value = 0;
};
using FirstWidget = Widget<WidgetType::kFirst>;


template<>
struct Widget<WidgetType::kSecond> {
  static constexpr std::string_view Name() {
    return "Second";
  }

  int value = 0;
};

using SecondWidget = Widget<WidgetType::kSecond>;

using WidgetTuple = std::tuple<FirstWidget, SecondWidget>;


```


Now, we may want to do something on the `WidgetTuple`.

Maybe we want to find the index which name equal to some value.

Maybe we want to do some action on a running-time index for `WidgetTuple` instance. running-time means:
```
int i = 0;
std::get<i>(instance); // Not work.
```

Here is how this library work:

```

template<size_t Index, class TupleT>
struct FindName {
    constexpr bool operator()(const std::string_view& widget_name) const {
      return std::tuple_element_t<Index, TupleT>::Name() == widget_name;
    }
};

// Both ok. The range of result is [0, tuple_size].
int index = tuple_trick::FindIfOperator<WidgetTuple, FindName>;
constexpr int index = tuple_trick::FindIfOperator<WidgetTuple, FindName>;

```

For running-time index operation:

```

template<size_t Index, class TupleT>
struct GetValue {
    int operator()(const TupleT& instance) const {
        return std::get<Index>(instance).value;
    }
};


WidgetTuple instance;
int value = IndexOperator<TupleT, GetValue>(1)(instance);

```


So, what you do is just define the template operator in format:
```
template<size_t Index, class TupleT>
struct xxx {
    //operator()
};
```

# API

## IndexOperator

IndexOperator accept an int value as the constructor parameter.

```
IndexOperator<TupleT, ActionTemplateClass>(index)(args ...);
```

If index is invalid, then the tuple_size-1 will be passed to the `ActionTemplateClass`

## FindIfOperator

FindIfOperator will return the index when meet the condition. But if none of tuple element find, the tuple size will be return.

```
int res_idnex = FindIfOperator<TupleT, FindIfTemplateClass>()(args ...);
```

A more advanced usage is, you can offer an action template class for it. It means if we find the tuple element, we will pass to the action template class.

```
FindIfOperator<TupleT, FindIfTemplateClass, ActionTemplateClass>()(args ...);
```

Similar as follows:
```
int res_idnex = FindIfOperator<TupleT, FindIfTemplateClass>()(args ...);
IndexOperator<TupleT, ActionTemplateClass>(res_idnex)(args ...);
```

Note, in this case, `FindIfTemplateClass` && `ActionTemplateClass` share the same function parameter types.

And the max `Index` in `ActionTemplateClass` is tuple_size. It is in compiled-time, so you need to get the valid index before get the tuple element.

```
constexpr size_t kIndex = Index < std::tuple_size_v<TupleT> ? Index : std::tuple_size_v<TupleT>-1;
```

## ForEachOperator

The limitation of for each operator is the return type is not work. Default is `void`.

```
ForEachOperator<TupleT, ActionTemplateClass>()(args ...);
```

## Custom Operator

Actually, it is support to define custom operator.

All the magic is implemented with the help of the worker class.

Please refer to the other operator.

# Get started

Testing to Ubuntu (G++ )&& Windows(VS2022) in std C++17.

It is head-only library.Just copy the header file:
```
tuple_trick/
   - tuple_trick.h
```

# How to run test

one of the following ways:

1. Using CMake
```
cd project_files
cmake .
```

2. Using quick_cmake
```
python quick_cmake.py
```
refer to: https://github.com/xiaosa233/quick_cmake
