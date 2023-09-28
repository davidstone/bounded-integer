# Libraries

This repository contains three libraries. They are implemented primarily in C++ modules, so there are no header files except for those providing macro definitions.

## bounded::integer

You can read the [documentation](bounded-readme.md) and view the [headers](include/bounded) and [sources](source/bounded). This library handles integer types with compile-time bounds on the range -- `bounded::integer<1, 10>` is an integer between 1 and 10. Arithmetic operations automatically expand these bounds, so programs without explicit casts or assignment are guaranteed to have no integer overflows.

## tv

This provides `tv::tuple`, `tv::variant`, and `tv::optional`. You can view the [sources](source/tv). Documentation should be added eventually.

## containers

You can read the [documentation](containers-readme.md) and view the [sources](source/containers). This library implements many container types and algorithms. It follows the principles of the C++ standard library, but does not try to be perfectly backward compatible in most cases. The focus is on maximum efficiency and fully decoupling data structures from algorithms.

There are some [containers that have backward compatibility as a goal](source/containers/std/) in `namespace std_containers` -- currently `std_containers::vector` and `std_containers::list`.

## Getting the code

* `git submodule update --init` will check out any necessary dependencies
* Configure with cmake 3.27.6+
* Build with clang 17+