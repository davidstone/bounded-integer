# bounded_integer installation instructions

## Prerequisites

* gcc version 4.8.2 or newer, with c++ support. gcc version 4.8.1 and older will not work. `bounded_integer` makes use of C++1y (C++14) features and needs some of the bug fixes added in 4.8.2 to compile correctly.
* Boost. Listed as boost-devel in Fedora repositories. `bounded_integer` is currently tested against version 1.53.0.
* SCons is used for the build process for the test set up, but it is not needed to use the library.

## Building and running tests (Linux)

* `cd path/to/bounded_integer`
* `scons && ./test && ./test-debug`. The default target (by simply typing `scons`) will build debug and release-mode versions of the tests. Most of the tests are actually done at compile time, but there are also run-time tests.

## Using

`bounded_integer` is a header-only library. The .cpp files only contain tests. All you have to do to use the program is `#include <bounded_integer/bounded_integer.hpp>`.

# Limitations

`bounded_integer` can replace most uses of built-in integer types. It does have some limitations, however.

* A `bounded_integer` cannot be used as a non-type template parameter. The C++ language rules do not permit any user-defined type to be used as a non-type template parameter, even if the user-defined type is a literal type.
* `bounded_integer` uses `intmax_t` as the template parameter to determine its bounds. This means that it cannot store an integer larger than `std::numeric_limits<intmax_t>::max()`. The alternative is to not allow users of the library to specify the bounds as plain integer values. Instead, they would have to pass it as some sort of type that encodes a value, which increases the burden of use.
* Doing math with `uintmax_t` (which is typically the same as `size_t`) can easily cause overflow issues. This can typically be resolved by narrowing the bounds of your values (and if you cannot do so, that usually means that your calculation could overflow).
