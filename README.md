# Long numbers in C++.

Supported and tested:
* +, -
* /, %
* <=>
* *mul(left,right,module)* and *pow(base,power,module)*
* Operations with uint64_t and strings in HEX format
* Full-featured class operations, error codes returned
* RSA encryption in example

# Compilation

Requires C++14 and CMake.

## Linux

Goto project root directory and execute the following:

```bash
mkdir build
cd build
cmake ../
make
```

Now you can find use *build/libling-numbers.a*!

## Windows

* Open *CMakeLists.txt* with CMake
* Use CMake to generate VisualStudio project
* Open the project and press *build->Build All*

