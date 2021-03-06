# Design decisions

## The situation so far

The built-in integer types in C++ (`int`, `unsigned`, `long long`, etc.) are mostly unusable because of the lax requirements on bounds. The types defined in `cstdint` are better because they give you stronger guarantees.

However, integer comparisons between signed and unsigned types are broken. `cstdint` integers are merely typedefs and thus inherit the same brokenness. Integer arithmetic is also broken. Signed overflow invokes undefined behavior, and unsigned overflow gives a result modulo the upper bound + 1.

There have been many attempts to fix this situation.

In the abstract sense, we usually just want an "integer" that we can do math on and get the results we expect, regardless of the value. However, attempting to replace all integer types with a boundless or `BigInt` type adds overhead to every calculation, which in many domains is unacceptable. Most implementations recognize this fact, but try to resolve it by adding less overhead in the form of checked integers. They perform arithmetic as usual, but if the result would be out of bounds, the typical implementation throws an exception.

## What bounded::integer does differently

The `bounded::integer` philosophy is the same as the C++ philosophy: you don't pay for what you don't use. Both of those solutions put overhead even if it isn't needed (I know my numbers are small enough and it is safe to add them). If there is any overhead at all in terms of space or time, then there is still room for an integer type below whatever "safe" integer you define. Therefore, rightly or wrongly, many will not use such a type. `bounded::integer` attempts to replace built-in integer types (and their typedefs in `cstdint`) for all use cases.

`bounded::integer` requires all integers to have more explicit bounds. These bounds, however, can be calculated for the user by the compiler. The following code snippet shows this principle:

	bounded::integer<1, 100> const x = f();
	bounded::integer<-3, 7> const y = g();
	auto z = x + y;
	static_assert(std::is_same<decltype(z), bounded::integer<-2, 107>>::value, "Type of z incorrect.");

The type of `z` is calculated as the smallest type that can hold all possible values of the calculation `x + y`. The `integer` type by defaults says that when modifying or constructing a value, the compiler should not do any run time checks.

`bounded::integer` has the following goals:
1. Never perform a run-time check when a static check would work instead
2. Never allow a conversion that is definitely wrong
3. Allow conversions that might be wrong (assigning a value between 1 and 10 to a value between 5 and 20), but only when explicitly requested.
4. Allow implicit conversions to larger types.
5. Have no space or time overhead, assuming basic compiler optimizations like inlining, so that `bounded::integer` can be used on very large data sets or systems with hard real-time requirements.

# bounded::integer installation instructions

## Prerequisites

* Compile with clang master
* cmake (3.14+) is used to build tests, but it is not needed to use the library (this library is header-only).

## Building and running tests (Linux)

* `cd path/to/bounded_integer`
* `mkdir build && cd build && cmake .. -GNinja && ninja && ./bounded_test`. Most of the tests are actually done at compile time, but there are also run-time tests.

## Using

`bounded::integer` is a header-only library. The .cpp files only contain tests. All you have to do to use the program is `#include <bounded/integer.hpp>`. There is also an experimental containers / ranges library that makes use of `bounded::integer` throughout, which is located at `containers/...`. `containers/integer_range.hpp` allows an integer range generating function similar to `range` in Python. `bounded/optional.hpp` contains an `optional` class (similar to `std::optional`) with a space optimization for most `bounded::integer` types, and hooks that allows most types to opt-in to this space optimization.

# Limitations

`bounded::integer` can replace most uses of built-in integer types. It does have some limitations, however.

* A `bounded::integer` cannot be used as a non-type template parameter prior to C++20. No compiler currently is able to compile `bounded::integer` that also has support for class types as non-type template parameters.
* `bounded::integer` uses built-in integers as the template parameter to determine its bounds. This means that it cannot store an integer larger than an unsigned 64-bit integer or smaller than a signed 64-bit integer. This restriction should be removed at some point in the future.
* `bounded::integer` is currently still under active development, so some interfaces are still subject to change.
* `bounded::integer` cannot be used as the argument to a `switch` statement. There is work in C++23 to add pattern matching, which would be a replacement for `switch` statements, and pattern matching would work with `bounded::integer`.
* `bounded::integer` cannot be used to express the size of a bit-field.

# Reference

Visit http://doublewise.net/c++/bounded/reference/
