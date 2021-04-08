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
#include <containers/integer_range.hpp>
#include <containers/is_iterator_sentinel.hpp>
#include <containers/algorithms/reverse_iterator.hpp>
#include <containers/size.hpp>
#include <containers/to_radix_sort_key.hpp>

#include <bounded/detail/copy_cv_ref.hpp>
#include <operators/forward.hpp>

#include <algorithm>
#include <array>
#include <cstdint>
#include <cstddef>
#include <iterator>
#include <memory>
#include <type_traits>
#include <tuple>
#include <utility>

namespace containers {
namespace detail {

template<typename>
constexpr inline auto is_view = false;

template<typename Iterator, typename Sentinel>
constexpr inline auto is_view<range_view<Iterator, Sentinel>> = true;

template<typename T>
concept view = is_view<T>;

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

template<view View, typename ExtractKey>
using NextSort = void (&)(View, ExtractKey const &, BaseListSortData *);

template<view View, typename ExtractKey>
struct ListSortData : BaseListSortData {
	NextSort<View, ExtractKey> next_sort;
};


template<typename T>
struct SubKey {
	using base = SubKey<decltype(to_radix_sort_key(std::declval<T>()))>;
	static constexpr decltype(auto) sub_key(auto && value, BaseListSortData * data) {
		return base::sub_key(to_radix_sort_key(OPERATORS_FORWARD(value)), data);
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

template<typename T> requires(std::is_unsigned_v<T>)
struct SubKey<T> {
	static constexpr auto sub_key(T const value, BaseListSortData *) {
		return value;
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
	static constexpr decltype(auto) sub_key(auto && value, BaseListSortData * sort_data) {
		using std::get;
		return Current::sub_key(get<Index>(OPERATORS_FORWARD(value)), sort_data);
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

template<indexable_range T>
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

struct PartitionCounts {
	std::array<PartitionInfo, 256> partitions;
	std::array<uint8_t, 256> remaining = {};
	int number = 0;
};

template<std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey, size_t NumBytes>
struct UnsignedInplaceSorter {
	// Must have this exact signature, no defaulted arguments
	template<view View, typename ExtractKey>
	static constexpr void sort(View to_sort, ExtractKey const & extract_key, NextSort<View, ExtractKey> next_sort, BaseListSortData * sort_data) {
		sort_selector(to_sort, extract_key, next_sort, sort_data, 0U);
	}
private:

	constexpr static auto current_byte(auto const & elem, BaseListSortData * sort_data, std::size_t const offset) {
		auto const shift_amount = (NumBytes - 1U - offset) * 8U;
		return static_cast<std::uint8_t>(CurrentSubKey::sub_key(elem, sort_data) >> shift_amount);
	}

	static constexpr auto partition_counts(view auto to_sort, auto const & extract_key, BaseListSortData * sort_data, std::size_t const offset) {
		auto result = PartitionCounts();
		for (auto const & value : to_sort) {
			++result.partitions[current_byte(extract_key(value), sort_data, offset)].count;
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

	template<view View, typename ExtractKey>
	static constexpr void sort_selector(View to_sort, ExtractKey const & extract_key, NextSort<View, ExtractKey> next_sort, BaseListSortData * sort_data, std::size_t const offset) {
		if (NumBytes == offset) {
			next_sort(to_sort, extract_key, sort_data);
		} else if (containers::size(to_sort) < bounded::constant<AmericanFlagSortThreshold>) {
			american_flag_sort(to_sort, extract_key, next_sort, sort_data, offset);
		} else {
			ska_byte_sort(to_sort, extract_key, next_sort, sort_data, offset);
		}
	}
	template<view View, typename ExtractKey>
	static constexpr void american_flag_sort(View to_sort, ExtractKey const & extract_key, NextSort<View, ExtractKey> next_sort, BaseListSortData * sort_data, std::size_t const offset) {
		auto partitions = partition_counts(to_sort, extract_key, sort_data, offset);
		auto const first = containers::begin(to_sort);
		using difference_type = typename std::iterator_traits<std::remove_const_t<decltype(first)>>::difference_type;
		[&]{
			if (partitions.number > 1) {
				uint8_t * current_block_ptr = partitions.remaining.data();
				PartitionInfo * current_block = partitions.partitions.data() + *current_block_ptr;
				uint8_t * last_block = partitions.remaining.data() + partitions.number - 1;
				auto it = first;
				auto block_end = first + static_cast<difference_type>(current_block->next_offset);
				auto last_element = containers::prev(containers::end(to_sort));
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

							it = first + static_cast<difference_type>(current_block->offset);
							block_end = first + static_cast<difference_type>(current_block->next_offset);
						}
					} else {
						auto const partition_offset = static_cast<containers::index_type<View>>(block->offset++);
						using std::swap;
						swap(*it, to_sort[partition_offset]);
					}
				}
			}
		}();
		auto partition_begin = first;
		for (uint8_t * it = partitions.remaining.data(), * remaining_end = partitions.remaining.data() + partitions.number; it != remaining_end; ++it) {
			auto const end_offset = static_cast<difference_type>(partitions.partitions[*it].next_offset);
			auto const partition_end = first + end_offset;
			if (partition_end - partition_begin > bounded::constant<1>) {
				sort_selector(containers::range_view(partition_begin, partition_end), extract_key, next_sort, sort_data, offset + 1U);
			}
			partition_begin = partition_end;
		}
	}

	template<view View, typename ExtractKey>
	static constexpr void ska_byte_sort(View to_sort, ExtractKey const & extract_key, NextSort<View, ExtractKey> next_sort, BaseListSortData * sort_data, std::size_t const offset) {
		auto partitions = partition_counts(to_sort, extract_key, sort_data, offset);
		auto const first = containers::begin(to_sort);
		using difference_type = typename std::iterator_traits<std::remove_const_t<decltype(first)>>::difference_type;
		for (uint8_t * last_remaining = partitions.remaining.data() + partitions.number, * end_partition = partitions.remaining.data() + 1; last_remaining > end_partition;) {
			last_remaining = containers::partition(partitions.remaining.data(), last_remaining, [&](uint8_t partition) {
				auto const begin_offset = static_cast<difference_type>(partitions.partitions[partition].offset);
				auto const end_offset = static_cast<difference_type>(partitions.partitions[partition].next_offset);
				if (begin_offset == end_offset)
					return false;

				for (auto it = first + begin_offset; it != first + end_offset; ++it) {
					uint8_t this_partition = current_byte(extract_key(*it), sort_data, offset);
					auto const partition_offset = static_cast<difference_type>(partitions.partitions[this_partition].offset++);
					auto const other = first + partition_offset;
					// Is there a better way to avoid self swap?
					if (it != other) {
						using std::swap;
						swap(*it, *(first + partition_offset));
					}
				}
				return begin_offset != end_offset;
			});
		}
		for (uint8_t * it = partitions.remaining.data() + partitions.number; it != partitions.remaining.data(); --it) {
			uint8_t partition = it[-1];
			auto const start_offset = static_cast<difference_type>(partition == 0 ? 0 : partitions.partitions[partition - 1].next_offset);
			auto const end_offset = static_cast<difference_type>(partitions.partitions[partition].next_offset);
			auto partition_begin = first + start_offset;
			auto partition_end = first + end_offset;
			if (partition_end - partition_begin > bounded::constant<1>) {
				sort_selector(containers::range_view(partition_begin, partition_end), extract_key, next_sort, sort_data, offset + 1U);
			}
		}
	}
};

template<std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey, typename SubKeyType>
struct ListInplaceSorter;

template<std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey, view View, typename ExtractKey>
constexpr void inplace_sort(View to_sort, ExtractKey const & extract_key, NextSort<View, ExtractKey> next_sort, BaseListSortData * sort_data) {
	using SubKeyType = decltype(CurrentSubKey::sub_key(extract_key(containers::front(to_sort)), sort_data));
	if constexpr (std::is_same_v<SubKeyType, bool>) {
		auto middle = containers::partition(to_sort, [&](auto && a){ return !CurrentSubKey::sub_key(extract_key(a), sort_data); });
		next_sort(containers::range_view(containers::begin(to_sort), middle), extract_key, sort_data);
		next_sort(containers::range_view(middle, containers::end(to_sort)), extract_key, sort_data);
	} else if constexpr (std::is_unsigned_v<SubKeyType>) {
		UnsignedInplaceSorter<AmericanFlagSortThreshold, CurrentSubKey, sizeof(SubKeyType)>::sort(to_sort, extract_key, next_sort, sort_data);
	} else {
		ListInplaceSorter<AmericanFlagSortThreshold, CurrentSubKey, SubKeyType>::sort(to_sort, extract_key, next_sort, sort_data);
	}
}

constexpr size_t common_prefix(
	view auto to_sort,
	auto const & extract_key,
	auto && element_key,
	typename std::iterator_traits<decltype(containers::begin(extract_key(containers::front(to_sort))))>::difference_type const start_index
) {
	if (containers::is_empty(to_sort)) {
		return 0;
	}
	auto const first = containers::begin(to_sort);
	auto const last = containers::end(to_sort);
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
	template<view View, typename ExtractKey>
	static constexpr void sort(View to_sort, ExtractKey const & extract_key, NextSort<View, ExtractKey> next_sort, BaseListSortData * next_sort_data) {
		constexpr auto current_index = 0U;
		constexpr auto recursion_limit = 16U;
		auto const offset = ListSortData<View, ExtractKey>{
			{
				current_index,
				recursion_limit,
				next_sort_data,
			},
			next_sort,
		};
		sort(to_sort, extract_key, offset);
	}

private:
	struct ElementSubKey {
		using base = SubKey<std::decay_t<decltype(containers::at(std::declval<ListType const &>(), 0, bounded::non_check))>>;

		using next = ElementSubKey;

		static constexpr decltype(auto) sub_key(auto && value, BaseListSortData * sort_data) {
			auto && list = CurrentSubKey::sub_key(OPERATORS_FORWARD(value), sort_data->next_sort_data);
			return base::sub_key(
				containers::at(OPERATORS_FORWARD(list), sort_data->current_index, bounded::non_check),
				sort_data->next_sort_data
			);
		}
	};

	template<view View, typename ExtractKey>
	static constexpr void sort(View to_sort, ExtractKey const & extract_key, ListSortData<View, ExtractKey> sort_data) {
		auto current_key = [&](auto const & elem) -> decltype(auto) {
			return CurrentSubKey::sub_key(extract_key(elem), sort_data.next_sort_data);
		};
		auto element_key = [&](auto && elem) -> decltype(auto) {
			return ElementSubKey::base::sub_key(elem, std::addressof(sort_data));
		};
		sort_data.current_index += common_prefix(
			to_sort,
			current_key,
			element_key,
			typename std::iterator_traits<decltype(containers::begin(current_key(containers::front(to_sort))))>::difference_type(sort_data.current_index)
		);
		auto end_of_shorter_ones = containers::partition(to_sort, [&](auto const & elem) {
			return containers::size(current_key(elem)) <= sort_data.current_index;
		});
		auto const first = containers::begin(to_sort);
		auto const last = containers::end(to_sort);
		if (end_of_shorter_ones - first > 1) {
			sort_data.next_sort(containers::range_view(first, end_of_shorter_ones), extract_key, sort_data.next_sort_data);
		}
		if (last - end_of_shorter_ones > bounded::constant<1>) {
			inplace_sort<AmericanFlagSortThreshold, ElementSubKey>(
				containers::range_view(end_of_shorter_ones, last),
				extract_key,
				static_cast<NextSort<View, ExtractKey>>(sort_from_recursion),
				std::addressof(sort_data)
			);
		}
	}

	template<view View, typename ExtractKey>
	static constexpr void sort_from_recursion(View to_sort, ExtractKey const & extract_key, BaseListSortData * next_sort_data) {
		auto offset = *static_cast<ListSortData<View, ExtractKey> *>(next_sort_data);
		++offset.current_index;
		--offset.recursion_limit;
		if (offset.recursion_limit == 0) {
			containers::sort(to_sort, extract_key_to_less(extract_key));
		} else {
			sort(to_sort, extract_key, offset);
		}
	}
};

template<std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey, view View, typename ExtractKey>
constexpr void sort_starter(View to_sort, ExtractKey const & extract_key, BaseListSortData * next_sort_data) {
	if constexpr (!std::is_same_v<CurrentSubKey, SubKey<void>>) {
		if (containers::size(to_sort) <= bounded::constant<1>) {
			return;
		}

		inplace_sort<AmericanFlagSortThreshold, CurrentSubKey>(
			to_sort,
			extract_key,
			static_cast<NextSort<View, ExtractKey>>(sort_starter<AmericanFlagSortThreshold, typename CurrentSubKey::next>),
			next_sort_data
		);
	}
}

template<std::ptrdiff_t AmericanFlagSortThreshold>
constexpr void inplace_radix_sort(view auto to_sort, auto const & extract_key) {
	using SubKey = SubKey<decltype(extract_key(containers::front(to_sort)))>;
	sort_starter<AmericanFlagSortThreshold, SubKey>(to_sort, extract_key, nullptr);
}

} // namespace detail

struct ska_sort_t {
	constexpr void operator()(range auto && to_sort, auto const & extract_key) const {
		containers::detail::inplace_radix_sort<1024>(containers::range_view(to_sort), extract_key);
	}
	constexpr void operator()(range auto && to_sort) const {
		operator()(to_sort, to_radix_sort_key);
	}
} inline constexpr ska_sort;

constexpr inline struct unique_ska_sort_t {
	constexpr void operator()(range auto & to_sort, auto const & extract_key) const {
		ska_sort(to_sort, extract_key);
		auto const equal = [&](auto const & lhs, auto const & rhs) {
			return extract_key(lhs) == extract_key(rhs);
		};
		::containers::erase(
			to_sort,
			::containers::unique(containers::begin(to_sort), containers::end(to_sort), equal),
			containers::end(to_sort)
		);
	}
	constexpr void operator()(range auto & to_sort) const {
		operator()(to_sort, to_radix_sort_key);
	}
} unique_ska_sort;

} // namespace containers