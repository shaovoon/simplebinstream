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

[CodeProject Tutorial](http://www.codeproject.com/Tips/808776/Cplusplus-Simplistic-Binary-Streams)