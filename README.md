# simplebinstream
C++ Simplistic Binary Stream

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

[CodeProject Tutorial](http://www.codeproject.com/Tips/808776/Cplusplus-Simplistic-Binary-Streams)