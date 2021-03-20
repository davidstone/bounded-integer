// Copyright David Stone 2019, Malte Skarupke 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <containers/algorithms/count.hpp>
#include <containers/algorithms/erase.hpp>
#include <containers/algorithms/minmax_element.hpp>
#include <containers/algorithms/negate.hpp>
#include <containers/algorithms/partition.hpp>
#include <containers/algorithms/sort.hpp>
#include <containers/algorithms/unique.hpp>
#include <containers/at.hpp>
#include <containers/begin_end.hpp>
#include <containers/legacy_iterator.hpp>
#include <containers/is_range.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/size.hpp>

#include <bounded/detail/copy_cv_ref.hpp>
#include <operators/forward.hpp>
#include <bounded/identity.hpp>

#include <climits>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <type_traits>
#include <tuple>
#include <utility>

namespace containers {
namespace detail {

struct unknown_floating_point;
template<typename T>
using float_to_unsigned =
	std::conditional_t<sizeof(T) == 4, std::uint32_t,
	std::conditional_t<sizeof(T) == 8, std::uint64_t,
	unknown_floating_point
>>;

} // namespace detail

// TODO: Delete bool overload, currently relying on implicit conversions
constexpr auto to_radix_sort_key(bool const value) {
	return value;
}
template<typename T> requires std::is_arithmetic_v<T>
constexpr auto to_radix_sort_key(T const value) {
	constexpr auto is_character_type =
		std::is_same_v<T, char> or
		std::is_same_v<T, char8_t> or
		std::is_same_v<T, char16_t> or
		std::is_same_v<T, char32_t> or
		std::is_same_v<T, wchar_t>;

	if constexpr (std::is_floating_point_v<T>) {
		static_assert(std::numeric_limits<T>::is_iec559);
		using unsigned_t = detail::float_to_unsigned<T>;
		static_assert(sizeof(T) == sizeof(unsigned_t));
		auto const u = __builtin_bit_cast(unsigned_t, value);
		constexpr auto sign_bit_position = std::numeric_limits<unsigned_t>::digits - 1U;
		auto const sign_bit = static_cast<unsigned_t>(-static_cast<std::make_signed_t<unsigned_t>>(u >> (sign_bit_position)));
		return u ^ (sign_bit | (static_cast<unsigned_t>(1U) << sign_bit_position));
	} else if constexpr (is_character_type) {
		return static_cast<std::make_unsigned_t<T>>(value);
	} else if constexpr (std::is_unsigned_v<T>) {
		return value;
	} else {
		static_assert(std::is_signed_v<T>);
		using unsigned_t = std::make_unsigned_t<T>;
		return static_cast<unsigned_t>(static_cast<unsigned_t>(value) + static_cast<unsigned_t>(bounded::min_value<T>));
	}
}

constexpr auto to_radix_sort_key(bounded::bounded_integer auto const value) {
	return to_radix_sort_key(value.value());
}

template<typename T> requires std::is_enum_v<T>
constexpr auto to_radix_sort_key(T const value) {
	return to_radix_sort_key(bounded::integer(value));
}

auto to_radix_sort_key(auto * ptr) {
	return reinterpret_cast<std::uintptr_t>(ptr);
}

namespace detail {

// TODO: Also validate get and tuple_element
template<typename T>
concept tuple_like = requires{
	std::tuple_size<T>::value;
};

template<typename T>
concept is_boolean = std::is_arithmetic_v<T> and std::is_same_v<T, bool>;

template<typename T>
concept indexable =
	requires(T && value) { OPERATORS_FORWARD(value)[0]; } or
	requires(T && value) { OPERATORS_FORWARD(value)[bounded::constant<0>]; };

struct PartitionInfo {
	constexpr PartitionInfo()
		: count(0)
	{
	}

	union {
		size_t count;
		size_t offset;
	};
	size_t next_offset = 0;
};

struct BaseListSortData {
	size_t current_index;
	size_t recursion_limit;
	BaseListSortData * next_sort_data;
};

template<typename Iterator, typename ExtractKey>
using NextSort = void (&)(Iterator, Iterator, ExtractKey const &, BaseListSortData *);

template<typename Iterator, typename ExtractKey>
struct ListSortData : BaseListSortData {
	NextSort<Iterator, ExtractKey> next_sort;
};


template<typename T>
struct SubKey {
	using base = SubKey<decltype(to_radix_sort_key(std::declval<T>()))>;
	static constexpr decltype(auto) sub_key(T const & value, BaseListSortData * data) {
		return base::sub_key(to_radix_sort_key(value), data);
	}

	using next = typename base::next;
};

template<typename T>
struct SubKey<T const> : SubKey<T> {
};
template<typename T>
struct SubKey<T &> : SubKey<T> {
};
template<typename T>
struct SubKey<T &&> : SubKey<T> {
};

template<typename T> requires(std::is_pointer_v<T> or std::is_unsigned_v<T>)
struct SubKey<T> {
	static constexpr auto sub_key(T const value, BaseListSortData *) {
		return to_radix_sort_key(value);
	}

	using next = SubKey<void>;
};

template<>
struct SubKey<void>;

template<size_t Index, typename Current, typename... More>
struct TupleSubKey;

template<size_t Index, typename Next, typename... More>
struct NextTupleSubKey {
	using type = TupleSubKey<Index, Next, More...>;
};
template<size_t Index, typename... More>
struct NextTupleSubKey<Index, SubKey<void>, More...> {
	using type = TupleSubKey<Index + 1, More...>;
};
template<size_t Index>
struct NextTupleSubKey<Index, SubKey<void>> {
	using type = SubKey<void>;
};

template<size_t Index, typename Current, typename... More>
struct TupleSubKey {
	template<typename Tuple>
	static constexpr decltype(auto) sub_key(const Tuple & value, BaseListSortData * sort_data) {
		using std::get;
		return Current::sub_key(get<Index>(value), sort_data);
	}

	using next = typename NextTupleSubKey<Index, typename Current::next, More...>::type;
};

template<typename Tuple, typename Indexes>
struct TupleSubKeyWrapper;

template<typename Tuple, std::size_t... indexes>
struct TupleSubKeyWrapper<Tuple, std::index_sequence<indexes...>> :
	TupleSubKey<
		0U,
		SubKey<std::tuple_element_t<indexes, Tuple>>...
	>
{
};

template<typename Tuple> requires(tuple_like<Tuple> and !range<Tuple>)
struct SubKey<Tuple> : TupleSubKeyWrapper<
	Tuple,
	std::make_index_sequence<std::tuple_size_v<Tuple>>
> {
};

template<typename T> requires (!std::is_pointer_v<T> and indexable<T> and range<T>)
struct SubKey<T> {
	static constexpr const T & sub_key(T const & value, BaseListSortData *) {
		return value;
	}
	static constexpr T sub_key(T && value, BaseListSortData *) {
		// TODO: This std::move is not needed in C++20
		return std::move(value);
	}

	using next = SubKey<void>;
};

template<typename ExtractKey>
struct extract_key_to_less {
	extract_key_to_less() = default;
	explicit constexpr extract_key_to_less(ExtractKey extract_key):
		m_extract_key(std::move(extract_key))
	{
	}

	constexpr auto operator()(auto const & lhs, auto const & rhs) const {
		return m_extract_key(lhs) < m_extract_key(rhs);
	}

private:
	[[no_unique_address]] ExtractKey m_extract_key;
};

template<iterator Iterator>
constexpr void StdSortFallback(Iterator first, sentinel_for<Iterator> auto last, auto extract_key) {
	sort(first, last, extract_key_to_less(std::move(extract_key)));
}

struct PartitionCounts {
	std::array<PartitionInfo, 256> partitions;
	std::array<uint8_t, 256> remaining = {};
	int number = 0;
};

template<std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey, size_t NumBytes>
struct UnsignedInplaceSorter {
	// Must have this exact signature, no defaulted arguments
	template<iterator Iterator, typename ExtractKey>
	static constexpr void sort(Iterator first, sentinel_for<Iterator> auto last, ExtractKey const & extract_key, NextSort<Iterator, ExtractKey> next_sort, BaseListSortData * sort_data) {
		sort_selector(first, last, extract_key, next_sort, sort_data, 0U);
	}
private:

	constexpr static auto current_byte(auto const & elem, BaseListSortData * sort_data, std::size_t const offset) {
		auto const shift_amount = (NumBytes - 1U - offset) * 8U;
		return static_cast<std::uint8_t>(CurrentSubKey::sub_key(elem, sort_data) >> shift_amount);
	}

	template<iterator Iterator>
	static constexpr auto partition_counts(Iterator first, sentinel_for<Iterator> auto last, auto const & extract_key, BaseListSortData * sort_data, std::size_t const offset) {
		auto result = PartitionCounts();
		for (; first != last; ++first) {
			++result.partitions[current_byte(extract_key(*first), sort_data, offset)].count;
		}
		size_t total = 0;
		for (std::size_t i = 0; i < 256; ++i) {
			auto & partition = result.partitions[i];
			size_t count = partition.count;
			partition.offset = total;
			total += count;
			partition.next_offset = total;
			if (count) {
				result.remaining[static_cast<std::size_t>(result.number)] = static_cast<std::uint8_t>(i);
				++result.number;
			}
		}
		return result;
	}

	template<iterator Iterator, typename ExtractKey>
	static constexpr void sort_selector(Iterator first, sentinel_for<Iterator> auto last, ExtractKey const & extract_key, NextSort<Iterator, ExtractKey> next_sort, BaseListSortData * sort_data, std::size_t const offset) {
		if (NumBytes == offset) {
			next_sort(first, last, extract_key, sort_data);
		} else if (last - first < bounded::constant<AmericanFlagSortThreshold>) {
			american_flag_sort(first, last, extract_key, next_sort, sort_data, offset);
		} else {
			ska_byte_sort(first, last, extract_key, next_sort, sort_data, offset);
		}
	}
	template<typename Iterator, typename ExtractKey>
	static constexpr void american_flag_sort(Iterator first, sentinel_for<Iterator> auto last, ExtractKey const & extract_key, NextSort<Iterator, ExtractKey> next_sort, BaseListSortData * sort_data, std::size_t const offset) {
		auto partitions = partition_counts(first, last, extract_key, sort_data, offset);
		[&]{
			if (partitions.number > 1) {
				uint8_t * current_block_ptr = partitions.remaining.data();
				PartitionInfo * current_block = partitions.partitions.data() + *current_block_ptr;
				uint8_t * last_block = partitions.remaining.data() + partitions.number - 1;
				auto it = first;
				auto block_end = first + current_block->next_offset;
				auto last_element = last - 1;
				for (;;) {
					PartitionInfo * block = partitions.partitions.data() + current_byte(extract_key(*it), sort_data, offset);
					if (block == current_block) {
						++it;
						if (it == last_element)
							break;
						else if (it == block_end) {
							for (;;) {
								++current_block_ptr;
								if (current_block_ptr == last_block)
									return;
								current_block = partitions.partitions.data() + *current_block_ptr;
								if (current_block->offset != current_block->next_offset)
									break;
							}

							it = first + current_block->offset;
							block_end = first + current_block->next_offset;
						}
					} else {
						size_t partition_offset = block->offset++;
						// TODO: ADL
						containers::swap(*it, *(first + partition_offset));
					}
				}
			}
		}();
		size_t start_offset = 0;
		auto partition_begin = first;
		for (uint8_t * it = partitions.remaining.data(), * remaining_end = partitions.remaining.data() + partitions.number; it != remaining_end; ++it) {
			size_t end_offset = partitions.partitions[*it].next_offset;
			auto partition_end = first + end_offset;
			if (partition_end - partition_begin > bounded::constant<1>) {
				sort_selector(partition_begin, partition_end, extract_key, next_sort, sort_data, offset + 1U);
			}
			start_offset = end_offset;
			partition_begin = partition_end;
		}
	}

	template<typename Iterator, typename ExtractKey>
	static constexpr void ska_byte_sort(Iterator first, sentinel_for<Iterator> auto last, ExtractKey const & extract_key, NextSort<Iterator, ExtractKey> next_sort, BaseListSortData * sort_data, std::size_t const offset) {
		auto partitions = partition_counts(first, last, extract_key, sort_data, offset);
		for (uint8_t * last_remaining = partitions.remaining.data() + partitions.number, * end_partition = partitions.remaining.data() + 1; last_remaining > end_partition;) {
			last_remaining = containers::partition(partitions.remaining.data(), last_remaining, [&](uint8_t partition) {
				size_t & begin_offset = partitions.partitions[partition].offset;
				size_t & end_offset = partitions.partitions[partition].next_offset;
				if (begin_offset == end_offset)
					return false;

				// TODO: This can sometimes perform a self-swap. If this could
				// be restructured to avoid that, it would also be a
				// performance gain.
				for (auto it = first + begin_offset; it != first + end_offset; ++it) {
					uint8_t this_partition = current_byte(extract_key(*it), sort_data, offset);
					size_t partition_offset = partitions.partitions[this_partition].offset++;
					// TODO: ADL
					containers::swap(*it, *(first + partition_offset));
				}
				return begin_offset != end_offset;
			});
		}
		for (uint8_t * it = partitions.remaining.data() + partitions.number; it != partitions.remaining.data(); --it) {
			uint8_t partition = it[-1];
			size_t start_offset = (partition == 0 ? 0 : partitions.partitions[partition - 1].next_offset);
			size_t end_offset = partitions.partitions[partition].next_offset;
			auto partition_begin = first + start_offset;
			auto partition_end = first + end_offset;
			if (partition_end - partition_begin > bounded::constant<1>) {
				sort_selector(partition_begin, partition_end, extract_key, next_sort, sort_data, offset + 1U);
			}
		}
	}
};

template<std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey, typename SubKeyType>
struct ListInplaceSorter;

template<std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey, typename Iterator, typename ExtractKey>
constexpr void inplace_sort(Iterator first, sentinel_for<Iterator> auto last, ExtractKey const & extract_key, NextSort<Iterator, ExtractKey> next_sort, BaseListSortData * sort_data) {
	using SubKeyType = decltype(CurrentSubKey::sub_key(extract_key(*first), sort_data));
	if constexpr (std::is_same_v<SubKeyType, bool>) {
		auto middle = containers::partition(first, last, [&](auto && a){ return !CurrentSubKey::sub_key(extract_key(a), sort_data); });
		next_sort(first, middle, extract_key, sort_data);
		next_sort(middle, last, extract_key, sort_data);
	} else if constexpr (std::is_unsigned_v<SubKeyType>) {
		UnsignedInplaceSorter<AmericanFlagSortThreshold, CurrentSubKey, sizeof(SubKeyType)>::sort(first, last, extract_key, next_sort, sort_data);
	} else {
		ListInplaceSorter<AmericanFlagSortThreshold, CurrentSubKey, SubKeyType>::sort(first, last, extract_key, next_sort, sort_data);
	}
}

template<typename Iterator>
constexpr size_t common_prefix(
	Iterator first,
	sentinel_for<Iterator> auto last,
	auto const & extract_key,
	auto && element_key,
	typename std::iterator_traits<decltype(containers::begin(extract_key(*first)))>::difference_type const start_index
) {
	BOUNDED_ASSERT(first != last);
	auto const & first_range = extract_key(*first);
	auto largest_match = containers::end(first_range) - (containers::begin(first_range) + start_index);
	for (auto it = containers::next(first); it != last; ++it) {
		auto const & current_range = extract_key(*it);
		largest_match = std::min(
			largest_match,
			std::mismatch(
				containers::begin(first_range) + start_index,
				containers::end(first_range),
				containers::begin(current_range) + start_index,
				containers::end(current_range),
				[&](auto const & lhs, auto const & rhs) {
					return element_key(lhs) == element_key(rhs);
				}
			).first - (containers::begin(first_range) + start_index)
		);
	}
	return static_cast<std::size_t>(largest_match);
}

template<std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey, typename ListType>
struct ListInplaceSorter {
	template<typename Iterator, typename ExtractKey>
	static constexpr void sort(Iterator first, sentinel_for<Iterator> auto last, ExtractKey const & extract_key, NextSort<Iterator, ExtractKey> next_sort, BaseListSortData * next_sort_data) {
		constexpr auto current_index = 0U;
		constexpr auto recursion_limit = 16U;
		auto const offset = ListSortData<Iterator, ExtractKey>{
			{
				current_index,
				recursion_limit,
				next_sort_data,
			},
			next_sort,
		};
		sort(first, last, extract_key, offset);
	}

private:
	struct ElementSubKey {
		using base = SubKey<std::decay_t<decltype(containers::at(std::declval<ListType const &>(), 0, bounded::non_check))>>;

		using next = ElementSubKey;

		static constexpr decltype(auto) sub_key(auto const & value, BaseListSortData * sort_data) {
			auto const & list = CurrentSubKey::sub_key(value, sort_data->next_sort_data);
			return base::sub_key(
				containers::at(list, sort_data->current_index, bounded::non_check),
				sort_data->next_sort_data
			);
		}
	};

	template<typename Iterator, typename ExtractKey>
	static constexpr void sort(Iterator first, sentinel_for<Iterator> auto last, ExtractKey const & extract_key, ListSortData<Iterator, ExtractKey> sort_data) {
		auto current_key = [&](auto const & elem) -> decltype(auto) {
			return CurrentSubKey::sub_key(extract_key(elem), sort_data.next_sort_data);
		};
		auto element_key = [&](auto && elem) -> decltype(auto) {
			return ElementSubKey::base::sub_key(elem, std::addressof(sort_data));
		};
		sort_data.current_index += common_prefix(
			first,
			last,
			current_key,
			element_key,
			typename std::iterator_traits<decltype(containers::begin(current_key(*first)))>::difference_type(sort_data.current_index)
		);
		auto end_of_shorter_ones = containers::partition(first, last, [&](auto const & elem) {
			return containers::size(current_key(elem)) <= sort_data.current_index;
		});
		if (end_of_shorter_ones - first > 1) {
			sort_data.next_sort(first, end_of_shorter_ones, extract_key, sort_data.next_sort_data);
		}
		if (last - end_of_shorter_ones > bounded::constant<1>) {
			inplace_sort<AmericanFlagSortThreshold, ElementSubKey>(
				end_of_shorter_ones,
				last,
				extract_key,
				static_cast<NextSort<Iterator, ExtractKey>>(sort_from_recursion),
				std::addressof(sort_data)
			);
		}
	}

	template<typename Iterator, typename ExtractKey>
	static constexpr void sort_from_recursion(Iterator first, sentinel_for<Iterator> auto last, ExtractKey const & extract_key, BaseListSortData * next_sort_data) {
		auto offset = *static_cast<ListSortData<Iterator, ExtractKey> *>(next_sort_data);
		++offset.current_index;
		--offset.recursion_limit;
		if (offset.recursion_limit == 0) {
			StdSortFallback(first, last, extract_key);
		} else {
			sort(first, last, extract_key, offset);
		}
	}
};

template<std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey, typename Iterator, typename ExtractKey>
constexpr void sort_starter(Iterator first, sentinel_for<Iterator> auto last, ExtractKey const & extract_key, BaseListSortData * next_sort_data) {
	if constexpr (!std::is_same_v<CurrentSubKey, SubKey<void>>) {
		if (last - first <= bounded::constant<1>) {
			return;
		}

		inplace_sort<AmericanFlagSortThreshold, CurrentSubKey>(
			first,
			last,
			extract_key,
			static_cast<NextSort<Iterator, ExtractKey>>(sort_starter<AmericanFlagSortThreshold, typename CurrentSubKey::next>),
			next_sort_data
		);
	}
}

template<std::ptrdiff_t AmericanFlagSortThreshold, typename Iterator>
constexpr void inplace_radix_sort(Iterator first_, sentinel_for<Iterator> auto last_, auto const & extract_key) {
	auto first = make_legacy_iterator(first_);
	auto last = make_legacy_iterator(last_);
	using SubKey = SubKey<decltype(extract_key(*first))>;
	sort_starter<AmericanFlagSortThreshold, SubKey>(first, last, extract_key, nullptr);
}

namespace extract {

using std::get;

template<std::size_t index, typename ExtractKey, typename T>
struct return_type_impl {
private:
	using tuple = decltype(std::declval<ExtractKey &>()(std::declval<T &>()));
	using reference_type = decltype(get<index>(std::declval<tuple>()));
	using value_type = bounded::detail::copy_cv_ref<tuple, std::tuple_element_t<index, std::decay_t<tuple>>>;
public:
	using type = std::conditional_t<std::is_reference_v<tuple>, reference_type, value_type>;
};

} // namespace extract

template<std::size_t index, typename ExtractKey, typename T>
using extract_return_type = typename extract::return_type_impl<index, ExtractKey, T>::type;


constexpr void bool_sort_copy(auto & source, auto & output, auto const & extract_key) {
	// As an optimization, we could copy all initial false values and all
	// trailing true values, then only do this algorithm on the middle section.
	// I'm not sure if the increased code size is worth the benefit of iterating
	// over those values only once.
	auto false_position = containers::begin(output);
	auto true_position = containers::next(
		false_position,
		containers::count_if(source, containers::negate(extract_key))
	);
	for (auto && value : source) {
		auto & position = extract_key(value) ? true_position : false_position;
		*position = std::move(value);
		++position;
	}
}


template<range Source, range Buffer, typename ExtractKey>
constexpr bool double_buffered_numeric_sort(Source & source, Buffer & buffer, ExtractKey const & extract_key) {
	constexpr auto size = sizeof(std::decay_t<decltype(extract_key(containers::front(source)))>);
	auto counts = std::array<std::array<std::size_t, 256>, size>();

	for (auto const & value : source) {
		auto key = to_radix_sort_key(extract_key(value));
		for (std::size_t index = 0U; index != size; ++index) {
			auto const inner_index = (key >> (index * 8U)) & 0xFFU;
			++counts[index][inner_index];
		}
	}
	auto total = std::array<std::size_t, size>();
	for (std::size_t index = 0U; index != size; ++index) {
		auto & indexed_total = total[index];
		auto & count = counts[index];
		for (std::size_t i = 0; i < 256; ++i) {
			indexed_total += std::exchange(count[i], indexed_total);
		}
	}
	for (std::size_t index = 0U; index != size; ) {
		auto sort_segment_copy = [&](auto & current, auto & next) {
			for (auto && value : current) {
				auto const key = static_cast<std::uint8_t>(to_radix_sort_key(extract_key(value)) >> (index * 8U));
				next[containers::index_type<Buffer>(counts[index][key]++)] = std::move(value);
			}
			++index;
		};
		sort_segment_copy(source, buffer);
		if constexpr (size % 2U == 0U) {
			sort_segment_copy(buffer, source);
		}
	}
	return size % 2U == 1U;
}

template<typename ExtractKey>
constexpr auto double_buffered_sort_impl(range auto & source, range auto & buffer, ExtractKey const & extract_key) -> bool;

template<std::size_t index, typename ExtractKey>
constexpr bool double_buffered_tuple_sort(range auto & source, range auto & buffer, ExtractKey const & extract_key) {
	using key_t = std::decay_t<decltype(extract_key(containers::front(source)))>;
	if constexpr (index == std::tuple_size_v<key_t>) {
		return false;
	} else {
		bool const which = containers::detail::double_buffered_tuple_sort<index + 1U>(source, buffer, extract_key);
		using std::get;
		auto extract_i = [&](auto && o) -> extract_return_type<index, ExtractKey, decltype(o)> {
			return get<index>(extract_key(o));
		};
		if (which) {
			return !containers::detail::double_buffered_sort_impl(buffer, source, extract_i);
		} else {
			return containers::detail::double_buffered_sort_impl(source, buffer, extract_i);
		}
	}
}

template<typename ExtractKey>
constexpr auto double_buffered_sort_impl(range auto & source, range auto & buffer, ExtractKey const & extract_key) -> bool {
	using key_t = std::decay_t<decltype(extract_key(containers::front(source)))>;
	if constexpr (std::is_same_v<key_t, bool>) {
		containers::detail::bool_sort_copy(source, buffer, extract_key);
		return true;
	} else if constexpr (std::is_arithmetic_v<key_t>) {
		return containers::detail::double_buffered_numeric_sort(source, buffer, extract_key);
	} else if constexpr (containers::range<key_t>) {
		// Currently just sized ranges
		auto const max_size = [&]{
			if constexpr (requires { key_t::size(); }) {
				return key_t::size();
			} else if constexpr (requires { std::tuple_size<key_t>::value; }) {
				return std::tuple_size<key_t>::value;
			} else {
				// TODO: Support variable-sized ranges
#if 0
				return *containers::max_element(
					containers::transform(
						source,
						[](auto const & range) { return containers::size(range); }
					)
				);
#endif
			}
		}();
		bool which = false;
		for (size_t index = max_size; index > 0; --index) {
			auto extract_n = [&, index = index - 1](auto && o) {
				return extract_key(o)[index];
			};
			if (which) {
				which = !containers::detail::double_buffered_sort_impl(buffer, source, extract_n);
			} else {
				which = containers::detail::double_buffered_sort_impl(source, buffer, extract_n);
			}
		}
		return which;
	} else if constexpr (tuple_like<key_t>) {
		return containers::detail::double_buffered_tuple_sort<0U>(source, buffer, extract_key);
	} else {
		return containers::detail::double_buffered_sort_impl(source, buffer, [&](auto && a) -> decltype(auto) {
			return to_radix_sort_key(extract_key(a));
		});
	}
}

} // namespace detail

struct ska_sort_t {
	template<iterator Iterator>
	constexpr void operator()(Iterator first, sentinel_for<Iterator> auto last, auto const & extract_key) const {
		detail::inplace_radix_sort<1024>(first, last, extract_key);
	}

	template<iterator Iterator>
	constexpr void operator()(Iterator first, sentinel_for<Iterator> auto last) const {
		operator()(first, last, bounded::identity);
	}

	constexpr void operator()(range auto && to_sort, auto const & extract_key) const {
		operator()(containers::begin(to_sort), containers::end(to_sort), OPERATORS_FORWARD(extract_key));
	}
	constexpr void operator()(range auto && to_sort) const {
		operator()(OPERATORS_FORWARD(to_sort), bounded::identity);
	}
} inline constexpr ska_sort;

struct double_buffered_ska_sort_t {
	constexpr auto operator()(range auto && source, range auto && buffer, auto const & extract_key) const -> bool {
		// TODO: Allow buffer to be larger, return range_view instead of bool
		BOUNDED_ASSERT(containers::size(source) == containers::size(buffer));
		// This delegates to an implementation function to collapse lvalue and
		// rvalue code into a single instantiation and to let the tuple version
		// be recursive with the main function. We need to accept rvalues so
		// that users can easily pass in view types.
		return detail::double_buffered_sort_impl(source, buffer, extract_key);
	}

	constexpr auto operator()(range auto && source, range auto && buffer) const -> bool {
		return operator()(source, buffer, bounded::identity);
	}
} inline constexpr double_buffered_ska_sort;

constexpr inline struct unique_ska_sort_t {
	constexpr auto operator()(range auto & to_sort, auto const & extract_key) const {
		ska_sort(to_sort, extract_key);
		auto const equal = [&](auto const & lhs, auto const & rhs) {
			return extract_key(lhs) == extract_key(rhs);
		};
		return ::containers::erase(
			to_sort,
			::containers::unique(containers::begin(to_sort), containers::end(to_sort), equal),
			containers::end(to_sort)
		);
	}
	constexpr auto operator()(range auto && to_sort) const {
		return operator()(to_sort, bounded::identity);
	}
} unique_ska_sort;

} // namespace containers