// Copyright David Stone 2025.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.index_sequence_struct;

import bounded.integer;
import bounded.normalize;

import std_module;

namespace bounded {

export template<std::size_t size>
struct index_sequence_struct {
	index_sequence_struct() = default;
	template<auto size_> requires (size_ == size)
	constexpr explicit index_sequence_struct(bounded::constant_t<size_>) {}
	template<std::size_t index>
	static constexpr auto get() {
		return bounded::constant<index>;
	}
};

template<auto size>
index_sequence_struct(bounded::constant_t<size>) -> index_sequence_struct<size>;

} // namespace bounded

namespace std {

template<std::size_t size>
struct tuple_size<bounded::index_sequence_struct<size>> : std::integral_constant<std::size_t, size> {
};

template<std::size_t index, std::size_t size>
struct tuple_element<index, bounded::index_sequence_struct<size>> {
	using type = bounded::constant_t<bounded::normalize<index>>;
};

} // namespace std
