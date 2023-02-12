# Libraries

This repository contains two libraries, one built using the other.

## bounded::integer

You can read the [documentation](bounded-readme.md) and view the [headers](include/bounded). This library handles integer types with compile-time bounds on the range -- `bounded::integer<1, 10>` is an integer between 1 and 10. Arithmetic operations automatically expand these bounds, so programs without explicit casts or assignment are guaranteed to have no integer overflows.

## containers

You can read the [documentation](containers-readme.md) and view the [headers](include/containers). This library implements many container types and algorithms. It follows the principles of the C++ standard library, but does not try to be perfectly backward compatible in most cases. There are some containers that have backward compatibility as a goal in `include/containers/std`. The focus is on maximum efficiency and fully decoupling data structures from algorithms.

## Getting the code

* `git submodule update --init` will check out any necessary dependencies
* Configure with cmake
* Build with clang 16+