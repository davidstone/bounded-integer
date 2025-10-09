// Copyright David Stone 2021.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.uninitialized_array;

import containers.c_array;
import containers.maximum_array_size;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

export template<typename T, array_size_type<T> size_>
struct uninitialized_array {
	constexpr uninitialized_array() = default;
	constexpr auto data() const noexcept -> T const * {
		return reinterpret_cast<T const *>(m_storage);
	}
	constexpr auto data() noexcept -> T * {
		return reinterpret_cast<T *>(m_storage);
	}
	static constexpr auto size() noexcept {
		return bounded::constant<size_>;
	}
private:
	alignas(T) [[no_unique_address]] c_array<std::byte, sizeof(T) * static_cast<std::size_t>(size_)> m_storage;
};

// reinterpret_cast is not allowed in a constexpr function, so this avoids
// storage that requires a reinterpret_cast if possible.
template<typename T>
concept sufficiently_trivial = (bounded::trivially_default_constructible<T> or bounded::tombstone_traits<T>::spare_representations != 0_bi) and bounded::trivially_destructible<T>;

template<sufficiently_trivial T, array_size_type<T> size_> requires(size_ != 0_bi)
struct uninitialized_array<T, size_> {
	constexpr uninitialized_array() requires bounded::trivially_default_constructible<T> = default;
	constexpr uninitialized_array() noexcept:
		uninitialized_array(bounded::make_index_sequence(size()))
	{
	}
	constexpr auto data() const noexcept -> T const * {
		return m_storage;
	}
	constexpr auto data() noexcept -> T * {
		return m_storage;
	}
	static constexpr auto size() noexcept {
		return bounded::constant<size_>;
	}
private:
	template<std::size_t... indexes>
	constexpr explicit uninitialized_array(std::index_sequence<indexes...>) noexcept:
		m_storage{(static_cast<void>(indexes), bounded::tombstone_traits<T>::make(0_bi))...}
	{
	}
	[[no_unique_address]] c_array<T, static_cast<std::size_t>(size_)> m_storage;
};

template<typename T, array_size_type<T> size_> requires(size_ == 0_bi)
struct uninitialized_array<T, size_> {
	constexpr auto data() const noexcept -> T const * {
		return nullptr;
	}
	constexpr auto data() noexcept -> T * {
		return nullptr;
	}
	static constexpr auto size() noexcept {
		return 0_bi;
	}
};

} // namespace containers

using container = containers::uninitialized_array<int, 5_bi>;

static_assert(container().data() != container().data());

struct non_trivial {
	constexpr explicit non_trivial(int const x):
		m(x)
	{
	}
	int m;
};
template<>
struct bounded::tombstone<non_trivial> {
	using index_t = bounded::integer<-1, 0>;
	static constexpr auto make(bounded::constant_t<0>) noexcept -> non_trivial {
		return non_trivial(17);
	}
	static constexpr auto index(non_trivial const & value) noexcept -> index_t {
		return value.m == 17 ? index_t(-1_bi) : index_t(0_bi);
	}
};

static_assert(containers::uninitialized_array<non_trivial, 3_bi>().data() != containers::uninitialized_array<non_trivial, 3_bi>().data());