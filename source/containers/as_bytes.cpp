// Copyright David Stone 2026.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.as_bytes;

import containers.contiguous_range;
import containers.data;
import containers.range_reference_t;
import containers.range_value_t;
import containers.size;
import containers.span;

import bounded;
import std_module;

namespace containers {

export template<typename T, contiguous_range Range>
auto as_bytes_impl(Range && r) -> span<T> {
	return span<T>(
		reinterpret_cast<T *>(containers::data(r)),
		containers::size(r) * bounded::size_of<range_value_t<Range>>
	);
}

export auto as_bytes(contiguous_range auto const & r) -> span<std::byte const> {
	return as_bytes_impl<std::byte const>(r);
}

export template<contiguous_range Range>
auto as_writeable_bytes(Range && r) -> span<std::byte> {
	static_assert(!std::is_const_v<std::remove_reference_t<range_reference_t<Range>>>);
	return as_bytes_impl<std::byte>(r);
}

} // namespace containers
