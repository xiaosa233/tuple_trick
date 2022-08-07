# Reflection On C++

--------------

> C++ reflection library.

std=C++17
# Features
 - static reflection. No need RTTI option.
 - class member variables reflection
 - class member functions reflection
 - serialize to string and bytes. big-endian and little-endian both be supported.

Support defined types:
1. Integral and floating type. int, double, char, etc.
2. string
3. vector
4. optional

# How to use

## Declaration

1. All reflected class need to derived from `reflection::meta_object`.
2. `reflected` macro use to declare class members. Format is `reflected(Class, (type, name, [default value]), ...)`
3. `reflected_func` macro use to make function reflection. Format is `reflected_func(Class, (function name), ...)`

```
#include "reflection.h"

struct widget : public reflection::meta_object {

  int scale(double ratio) {
    width = static_cast<int>(width * ratio);
    height = static_cast<int>(height * ratio);
    return id;
  }


  reflected(widget, 
    (int, id),
    (int, width, =720),
    (int, height, =1024)
  );
  /* equal to
  int id;
  int widget=720;
  int height=1024;
  */

  reflected_func(widget, (scale));
};

```

These only need to be defined in the head file.

## Get All class member info

```
widget inst;
const meta_info& info_v = inst.get_meta_info();
for(const auto& meta_item_v : info_v.item_vec()) {
    printf("%s %s\n", meta_item_v.type(), meta_item_v.name());
}
```

Order make sense. Output will be :
```
int id
int width
int height
```

## Set and get by object

```
widget inst;
inst.set<int>("id", 233);
int value = inst.get<int>("id");

// Also support from string
inst.member_from_string("id", "233");

std::string id_str;
status status_v = inst.member_to_string("id", &id_str);
status_v = inst.member_from_string("id", "233");

// And please don't forget access by C++ way.
inst.id = 233;
```

# Serialization

```
// order make sense.
widget inst = aggregate_initialize<widget>(233, 100, 200);

format_context format_context_v;
std::string output;
status status_v = inst.serialize_to_string(&output);
/*
    output is :
    {"id":233,"width":100,"height":200}
*/
status_v = inst.serialize_to_string(&output, &format_context_v);
/*
    The difference is, if format_context is provided, result will be more human friendly.
    output is :
    {
        "id": 233,
        "width": 100,
        "height": 200
    }
*/

status_v = inst.serialize_from_string(output.data());

// Also support serialize with the bytes. refer to the meta_object's declaration.
```

# Invoke function
```
widget inst;
inst.id = 233;
context_status<int> status = inst.invoke<int>("scale", 3.0);
// status.get() == inst.id;

// Call in wrong way
// 1. return type is not match.
// 2. missing args.
status = inst.invoke("scale");
// status == false

// Call by strings
strng_status status = inst.invoke_by_string("scale", {"3.0"});
//status.get() = "233"
```

Invoke function will check the args information. If they are not match, false status will be return.
Call by string will be helpful in C++ RPC projects.


# Compile 
I use one of my own tools to generate cmake files. 

Tool is [quick_cmake](https://github.com/xiaosa233/quick_cmake)

Fortunately, the ready-made cmake file is here: project_files/CmakeLists.txt

Windows(VS2022) && Ubuntu(G++) are both supported.

```
cd project_files
cmake .
```

or use quick_cmake
```
python quick_cmake.py
```
