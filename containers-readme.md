# Design

This library implements many container types and algorithms. It follows the principles of the C++ standard library, but does not try to be perfectly backward compatible in most cases. There are some containers that have backward compatibility as a goal in `include/containers/std`. The focus is on maximum efficiency and fully decoupling data structures from algorithms.

## Containers implemented

* [uninitialized_array](include/containers/uninitialized_array.hpp)
* [uninitialized_dynamic_array](include/containers/uninitialized_dynamic_array.hpp)
* [array](include/containers/array.hpp)
* [dynamic_array](include/containers/dynamic_array.hpp)
* [small_buffer_optimized_vector](include/containers/small_buffer_optimized_vector.hpp)
* [stable_vector](include/containers/stable_vector.hpp)
* [static_vector](include/containers/static_vector.hpp)
* [vector](include/containers/vector.hpp)
* [bidirectional_linked_list](include/containers/bidirectional_linked_list.hpp)
* [flat_map](include/containers/flat_map.hpp)