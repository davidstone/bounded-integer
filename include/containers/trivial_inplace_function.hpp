#pragma once

#include <containers/array/array.hpp>

#include <cstdint>
#include <type_traits>

namespace containers {
namespace detail {
// For a given size, we know the maximum natural alignment for any type that is
// no larger than that size. This allows us to get a better default alignment
// than the maximum possible alignment for smaller buffer sizes. This is the
// approach taken by
// https://github.com/WG21-SG14/SG14/blob/master/SG14/inplace_function.h and
// this implementation copies that approach.

template<std::size_t capacity>
union aligned_storage_helper {
	struct empty {};

	template<class T>
	using maybe = std::conditional_t<capacity >= sizeof(T), T, empty>;
	struct double_holder { double a; };

	[[no_unique_address]] containers::array<std::byte, capacity> data;
	[[no_unique_address]] maybe<short> short_;
	[[no_unique_address]] maybe<int> int_;
	[[no_unique_address]] maybe<long> long_;
	[[no_unique_address]] maybe<long long> long_long_;
	[[no_unique_address]] maybe<void *> void_star_;
	[[no_unique_address]] maybe<void(*)()> function_ptr_;
	[[no_unique_address]] maybe<double_holder> double_holder_;
	[[no_unique_address]] maybe<containers::array<double, 4>> array_double_;
	[[no_unique_address]] maybe<long double> long_double_;
};

template<std::size_t capacity>
inline constexpr auto alignment_for_capacity = alignof(aligned_storage_helper<capacity>);

template<typename Function, std::size_t capacity, std::size_t alignment, typename R, typename... Args>
concept trivially_storable =
	std::is_invocable_r_v<R, Function, Args...> and
	(capacity == 0 ? std::is_empty_v<Function> : sizeof(Function) <= capacity) and
	alignof(Function) <= alignment and
	std::is_trivially_copyable_v<Function>;

// bit_cast requires that the source and destination are exactly the same size.
// We use this struct to pad the function out to the size needed. We cannot use
// the more typical (and portable) union version of this because bit_cast is not
// constexpr if used on a union.
template<typename T, auto size>
struct padded {
	constexpr explicit padded(T value_, bounded::constant_t<size>):
		value(value_),
		padding()
	{
	}

	// If value is not declared [[no_unique_address]] but it is empty, the bytes
	// would not be initialized. This would lead to uninitialized bytes being
	// copied, which would be rejected by the constexpr evaluator.
	using padding_type = containers::array<std::byte, size - (std::is_empty_v<T> ? 0 : sizeof(T))>;
	[[no_unique_address]] T value;
	[[no_unique_address]] padding_type padding;
};

template<bool is_const, std::size_t capacity, std::size_t alignment, typename R, typename... Args>
struct trivial_inplace_function_impl {
private:
	using storage_type = containers::array<std::byte, capacity>;
	using function_ptr = R(*)(std::conditional_t<is_const, storage_type const &, storage_type &>, Args...);

	[[no_unique_address]] alignas(alignment) storage_type m_storage;
	function_ptr m_function_indirection;

	template<typename Function>
	static constexpr auto create_function_indirection() {
		if constexpr (is_const) {
			return [](storage_type const & storage, Args... args) -> R {
				return std::invoke(reinterpret_cast<Function const &>(storage), OPERATORS_FORWARD(args)...);
			};
		} else {
			return [](storage_type & storage, Args... args) -> R {
				return std::invoke(reinterpret_cast<Function &>(storage), OPERATORS_FORWARD(args)...);
			};
		}
	}

public:
	// I would prefer that this not be default constructible. However, that
	// would mean it could not be stored in a constexpr static_vector
	trivial_inplace_function_impl() = default;
	constexpr trivial_inplace_function_impl(trivially_storable<capacity, alignment, R, Args...> auto function):
		m_storage(__builtin_bit_cast(
			storage_type,
			padded(function, bounded::constant<capacity>)
		)),
		m_function_indirection(create_function_indirection<decltype(function)>())
	{
	}

	auto operator()(Args... args) const -> R requires(is_const) {
		return m_function_indirection(m_storage, OPERATORS_FORWARD(args)...);
	}
	auto operator()(Args... args) -> R {
		return m_function_indirection(m_storage, OPERATORS_FORWARD(args)...);
	}
};

} // namespace detail

template<typename, std::size_t capacity, std::size_t alignment = detail::alignment_for_capacity<capacity>>
struct trivial_inplace_function;

template<typename R, typename... Args, std::size_t capacity, std::size_t alignment>
struct trivial_inplace_function<R(Args...), capacity, alignment> :
	detail::trivial_inplace_function_impl<false, capacity, alignment, R, Args...>
{
private:
	using base = detail::trivial_inplace_function_impl<false, capacity, alignment, R, Args...>;
public:
	using base::base;
	using base::operator();
};

template<typename R, typename... Args, std::size_t capacity, std::size_t alignment>
struct trivial_inplace_function<R(Args...) const, capacity, alignment> :
	detail::trivial_inplace_function_impl<true, capacity, alignment, R, Args...>
{
private:
	using base = detail::trivial_inplace_function_impl<true, capacity, alignment, R, Args...>;
public:
	using base::base;
	using base::operator();
};

} // namespace containers