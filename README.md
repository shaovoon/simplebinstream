# C++ Simplistic Binary Stream

Bare minimal header-only binary stream based on C++ file streams where the stream operator can be overloaded for your custom type.

```cpp
#include <iostream>
#include "SimpleBinStream.h"

// Testing memory stream
void TestMem()
{
    simple::mem_ostream out;
    out << 23 << 24 << "Hello world!";

    simple::mem_istream in(out.get_internal_vec());
    int num1 = 0, num2 = 0;
    std::string str;
    in >> num1 >> num2 >> str;

    cout << num1 << "," << num2 << "," << str << endl;
}

// Testing file stream
void TestFile()
{
    simple::file_ostream out("file.bin", std::ios_base::out | std::ios_base::binary);
    out << 23 << 24 << "Hello world!";
    out.flush();
    out.close();

    simple::file_istream in("file.bin", std::ios_base::in | std::ios_base::binary);
    int num1 = 0, num2 = 0;
    std::string str;
    in >> num1 >> num2 >> str;

    cout << num1 << "," << num2 << "," << str << endl;
}
```

# Version 0.9.5 Breaking Changes

Requires C++11 compiler and standard library now.

The classes are templates now. 

```cpp
template<typename same_endian_type>
class file_istream {...}

template<typename same_endian_type>
class mem_istream  {...}

template<typename same_endian_type>
class ptr_istream  {...}

template<typename same_endian_type>
class file_ostream {...}

template<typename same_endian_type>
class mem_ostream  {...}
```

How to pass in same_endian_type to the class? Use std::is_same<>.

```cpp
// 1st parameter is data endian and 2 parameter is platform endian, if they are different, swap.
using same_endian_type = std::is_same<simple::BigEndian, simple::LittleEndian>;
simple::mem_ostream<same_endian_type> out;
out << (int64_t)23 << (int64_t)24 << "Hello world!";

simple::ptr_istream<same_endian_type> in(out.get_internal_vec());
int64_t num1 = 0, num2 = 0;
std::string str;
in >> num1 >> num2 >> str;

cout << num1 << "," << num2 << "," << str << endl;
```

If your data and platform shares the same endianness, you can skip the test by specifying std::true_type directly.

```cpp
simple::mem_ostream<std::true_type> out;
out << (int64_t)23 << (int64_t)24 << "Hello world!";

simple::ptr_istream<std::true_type> in(out.get_internal_vec());
int64_t num1 = 0, num2 = 0;
std::string str;
in >> num1 >> num2 >> str;

cout << num1 << "," << num2 << "," << str << endl;
```

## Advantages of compile-time check

* For same_endian_type = true_type, the swap function is a empty function which is optimised away.
* For same_endian_type = false_type, the swapping is done without any prior runtime check cost.

## Disadvantages of compile-time check

* Cannot parse file/data which is sometime different endian. I believe this scenario is rare.

Swap functions are listed below.

```cpp
enum class Endian
{
    Big,
    Little
};
using BigEndian = std::integral_constant<Endian, Endian::Big>;
using LittleEndian = std::integral_constant<Endian, Endian::Little>;

template<typename T>
void swap(T& val, std::true_type)
{
    // same endian so do nothing.
}

template<typename T>
void swap(T& val, std::false_type)
{
    std::is_integral<T> is_integral_type;
    swap_if_integral(val, is_integral_type);
}

template<typename T>
void swap_if_integral(T& val, std::false_type)
{
    // T is not integral so do nothing
}

template<typename T>
void swap_if_integral(T& val, std::true_type)
{
    swap_endian<T, sizeof(T)>()(val);
}

template<typename T, size_t N>
struct swap_endian
{
    void operator()(T& ui)
    {
    }
};

template<typename T>
struct swap_endian<T, 8>
{
    void operator()(T& ui)
    {
        union EightBytes
        {
            T ui;
            uint8_t arr[8];
        };

        EightBytes fb;
        fb.ui = ui;
        // swap the endian
        std::swap(fb.arr[0], fb.arr[7]);
        std::swap(fb.arr[1], fb.arr[6]);
        std::swap(fb.arr[2], fb.arr[5]);
        std::swap(fb.arr[3], fb.arr[4]);

        ui = fb.ui;
    }
};

template<typename T>
struct swap_endian<T, 4>
{
    void operator()(T& ui)
    {
        union FourBytes
        {
            T ui;
            uint8_t arr[4];
        };

        FourBytes fb;
        fb.ui = ui;
        // swap the endian
        std::swap(fb.arr[0], fb.arr[3]);
        std::swap(fb.arr[1], fb.arr[2]);

        ui = fb.ui;
    }
};

template<typename T>
struct swap_endian<T, 2>
{
    void operator()(T& ui)
    {
        union TwoBytes
        {
            T ui;
            uint8_t arr[2];
        };

        TwoBytes fb;
        fb.ui = ui;
        // swap the endian
        std::swap(fb.arr[0], fb.arr[1]);

        ui = fb.ui;
    }
};
```

[CodeProject Tutorial](http://www.codeproject.com/Tips/808776/Cplusplus-Simplistic-Binary-Streams)