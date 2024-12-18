module;

#include <bounded/conditional.hpp>
#include <operators/forward.hpp>

export module containers.trivial_inplace_function;

import containers.array;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {
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

	[[no_unique_address]] containers::array<std::byte, bounded::constant<capacity>> data;
	[[no_unique_address]] maybe<short> short_;
	[[no_unique_address]] maybe<int> int_;
	[[no_unique_address]] maybe<long> long_;
	[[no_unique_address]] maybe<long long> long_long_;
	[[no_unique_address]] maybe<void *> void_star_;
	[[no_unique_address]] maybe<void(*)()> function_ptr_;
	[[no_unique_address]] maybe<double_holder> double_holder_;
	[[no_unique_address]] maybe<containers::array<double, 4_bi>> array_double_;
	[[no_unique_address]] maybe<long double> long_double_;
};

template<std::size_t capacity>
constexpr auto alignment_for_capacity = alignof(aligned_storage_helper<capacity>);

template<typename Function>
concept function_pointer = std::is_function_v<std::remove_pointer_t<Function>>;

template<typename Function, std::size_t capacity, std::size_t alignment>
concept trivially_storable_class = 
	(capacity == 0 ? std::is_empty_v<Function> : sizeof(Function) <= capacity) and
	alignof(Function) <= alignment and
	std::is_trivially_copyable_v<Function>;

template<typename Function, std::size_t capacity, std::size_t alignment, typename R, typename... Args>
concept trivially_storable =
	std::same_as<std::invoke_result_t<Function, Args...>, R> and
	(function_pointer<Function> or trivially_storable_class<Function, capacity, alignment>);

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
	using padding_type = containers::array<std::byte, bounded::constant<size - (std::is_empty_v<T> ? 0 : sizeof(T))>>;
	[[no_unique_address]] T value;
	[[no_unique_address]] padding_type padding;
};

template<typename T>
concept function_signature = std::is_function_v<T>;

template<bool is_const, std::size_t capacity, std::size_t alignment, typename R, typename... Args>
struct trivial_inplace_function_impl {
private:
	static constexpr auto use_single_indirection = capacity == 0 and alignment == alignment_for_capacity<0>;
	using storage_type = containers::array<std::byte, bounded::constant<capacity>>;
	using function_ptr = std::conditional_t<
		use_single_indirection,
		R(*)(Args...),
		R(*)(std::conditional_t<is_const, storage_type const &, storage_type &>, Args...)
	>;

	[[no_unique_address]] alignas(alignment) storage_type m_storage;
	function_ptr m_function_indirection;

	template<typename Function>
	static constexpr auto create_function_indirection(Function & function) -> function_ptr {
		if constexpr (use_single_indirection) {
			if constexpr (bounded::convertible_to<Function, function_ptr>) {
				return function;
			} else {
				return [](Args... args) -> R {
					return Function()(OPERATORS_FORWARD(args)...);
				};
			}
		} else {
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
	}

	template<typename Function>
	static constexpr auto make_storage(Function function) -> storage_type {
		if constexpr (use_single_indirection) {
			static_assert(
				bounded::convertible_to<Function, function_ptr> or
				bounded::trivially_default_constructible<Function>
			);
			return storage_type();
		} else {
			return std::bit_cast<storage_type>(
				padded(function, bounded::constant<capacity>)
			);
		}
	}

	constexpr explicit trivial_inplace_function_impl(bounded::tombstone_tag):
		m_function_indirection(nullptr)
	{
	}

	friend bounded::tombstone<trivial_inplace_function_impl>;
public:

	constexpr trivial_inplace_function_impl(trivially_storable<capacity, alignment, R, Args...> auto function):
		m_storage(make_storage(function)),
		m_function_indirection(create_function_indirection(function))
	{
	}

	constexpr auto operator()(Args... args) const -> R requires(is_const) {
		if constexpr (use_single_indirection) {
			return m_function_indirection(OPERATORS_FORWARD(args)...);
		} else {
			return m_function_indirection(m_storage, OPERATORS_FORWARD(args)...);
		}
	}
	constexpr auto operator()(Args... args) -> R {
		if constexpr (use_single_indirection) {
			return m_function_indirection(OPERATORS_FORWARD(args)...);
		} else {
			return m_function_indirection(m_storage, OPERATORS_FORWARD(args)...);
		}
	}
};

} // namespace containers

template<bool is_const, std::size_t capacity, std::size_t alignment, typename R, typename... Args>
struct bounded::tombstone<containers::trivial_inplace_function_impl<is_const, capacity, alignment, R, Args...>> {
private:
	using T = containers::trivial_inplace_function_impl<is_const, capacity, alignment, R, Args...>;
public:
	static constexpr auto make(bounded::constant_t<0>) noexcept {
		return T(bounded::tombstone_tag());
	}
	static constexpr auto index(T const & value) noexcept {
		return BOUNDED_CONDITIONAL(value.m_function_indirection, -1_bi, 0_bi);
	}
};

namespace containers {

// If your function type is convertible to a function pointer, calling through
// the function must have the same semantics as calling through the function
// pointer.
//
// If `capacity` is 0 and `alignment` is the default, your type must be
// trivially default constructible or convertible to a function pointer.

export template<function_signature, std::size_t capacity, std::size_t alignment = alignment_for_capacity<capacity>>
struct trivial_inplace_function;

template<typename R, typename... Args, std::size_t capacity, std::size_t alignment>
struct trivial_inplace_function<R(Args...), capacity, alignment> :
	trivial_inplace_function_impl<false, capacity, alignment, R, Args...>
{
private:
	using base = trivial_inplace_function_impl<false, capacity, alignment, R, Args...>;
	friend bounded::tombstone<trivial_inplace_function>;
public:
	using base::base;
	constexpr explicit trivial_inplace_function(base && b) noexcept:
		base(std::move(b))
	{
	}
	using base::operator();
};

template<typename R, typename... Args, std::size_t capacity, std::size_t alignment>
struct trivial_inplace_function<R(Args...) const, capacity, alignment> :
	trivial_inplace_function_impl<true, capacity, alignment, R, Args...>
{
private:
	using base = trivial_inplace_function_impl<true, capacity, alignment, R, Args...>;
	friend bounded::tombstone<trivial_inplace_function>;
public:
	using base::base;
	constexpr explicit trivial_inplace_function(base && b) noexcept:
		base(std::move(b))
	{
	}
	using base::operator();
};

} // namespace containers

template<containers::function_signature Signature, std::size_t capacity, std::size_t alignment>
struct bounded::tombstone<containers::trivial_inplace_function<Signature, capacity, alignment>> {
private:
	using T = containers::trivial_inplace_function<Signature, capacity, alignment>;
	using base = tombstone<typename T::base>;
public:
	static constexpr auto make(bounded::constant_t<0>) noexcept {
		return T(base::make(0_bi));
	}
	static constexpr auto index(T const & value) noexcept {
		return base::index(value);
	}
};
