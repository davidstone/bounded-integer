// Copyright David Stone 2021, Malte Skarupke 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

// This is an implementation of ska_sort, as described in
// https://probablydance.com/2016/12/27/i-wrote-a-faster-sorting-algorithm/ and
// https://probablydance.com/2017/01/17/faster-sorting-algorithm-part-2/
// https://github.com/skarupke/ska_sort is the original implementation. The
// majority of this file is a reimplementation of the original algorithm, but it
// still follows the overall structure of the original.

module;

#include <operators/forward.hpp>

export module containers.algorithms.sort.inplace_radix_sort;

import containers.algorithms.sort.common_prefix;
import containers.algorithms.sort.sort;
import containers.algorithms.sort.to_radix_sort_key;

import containers.algorithms.advance;
import containers.algorithms.partition;
import containers.array;
import containers.at;
import containers.begin_end;
import containers.extract_key_to_less;
import containers.front_back;
import containers.index_type;
import containers.is_range;
import containers.iter_difference_t;
import containers.range_view;
import containers.size;

import bounded;
import std_module;
import tv;

using namespace bounded::literal;

namespace containers {

template<typename T>
concept view = is_range_view<T>;

struct PartitionInfo {
	constexpr PartitionInfo()
		: count(0)
	{
	}

	union {
		std::size_t count;
		std::size_t offset;
	};
	std::size_t next_offset = 0;
};

struct BaseListSortData {
	std::size_t current_index;
	std::size_t recursion_limit;
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
	// TODO: Should this use the user-provided `extract_key`?
	using base = SubKey<decltype(to_radix_sort_key(bounded::declval<T>()))>;
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

template<typename T> requires bounded::unsigned_builtin<T> or std::same_as<T, bool>
struct SubKey<T> {
	static constexpr auto sub_key(T const value, BaseListSortData *) {
		return value;
	}

	using next = SubKey<void>;
};

template<>
struct SubKey<void>;

template<std::size_t index, typename Current, typename... More>
struct TupleSubKey;

template<std::size_t index, typename Next, typename... More>
struct NextTupleSubKey {
	using type = TupleSubKey<index, Next, More...>;
};
template<std::size_t index, typename... More>
struct NextTupleSubKey<index, SubKey<void>, More...> {
	using type = TupleSubKey<index + 1, More...>;
};
template<std::size_t index>
struct NextTupleSubKey<index, SubKey<void>> {
	using type = SubKey<void>;
};

template<std::size_t index, typename Current, typename... More>
struct TupleSubKey {
	static constexpr decltype(auto) sub_key(auto && value, BaseListSortData * sort_data) {
		using std::get;
		return Current::sub_key(get<index>(OPERATORS_FORWARD(value)), sort_data);
	}

	using next = typename NextTupleSubKey<index, typename Current::next, More...>::type;
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
		return value;
	}

	using next = SubKey<void>;
};

struct PartitionCounts {
	std::array<PartitionInfo, 256> partitions;
	std::array<std::uint8_t, 256> remaining = {};
	int number = 0;
};

template<std::ptrdiff_t std_sort_threshold, std::ptrdiff_t american_flag_sort_threshold, typename CurrentSubKey, std::size_t number_of_bytes>
struct UnsignedInplaceSorter {
	// Must have this exact signature, no defaulted arguments
	template<view View, typename ExtractKey>
	static constexpr void sort(View to_sort, ExtractKey const & extract_key, NextSort<View, ExtractKey> next_sort, BaseListSortData * sort_data) {
		sort_selector(to_sort, extract_key, next_sort, sort_data, 0U);
	}
private:

	constexpr static auto current_byte(auto const & elem, BaseListSortData * sort_data, std::size_t const offset) {
		auto const shift_amount = (number_of_bytes - 1U - offset) * 8U;
		return static_cast<std::uint8_t>(CurrentSubKey::sub_key(elem, sort_data) >> shift_amount);
	}

	static constexpr auto partition_counts(view auto to_sort, auto const & extract_key, BaseListSortData * sort_data, std::size_t const offset) {
		auto result = PartitionCounts();
		for (auto const & value : to_sort) {
			++result.partitions[current_byte(extract_key(value), sort_data, offset)].count;
		}
		std::size_t total = 0;
		for (std::size_t i = 0; i < 256; ++i) {
			auto & result_partition = result.partitions[i];
			std::size_t count = result_partition.count;
			result_partition.offset = total;
			total += count;
			result_partition.next_offset = total;
			if (count) {
				result.remaining[static_cast<std::size_t>(result.number)] = static_cast<std::uint8_t>(i);
				++result.number;
			}
		}
		return result;
	}

	template<view View, typename ExtractKey>
	static constexpr void sort_selector(View to_sort, ExtractKey const & extract_key, NextSort<View, ExtractKey> next_sort, BaseListSortData * sort_data, std::size_t const offset) {
		if (number_of_bytes == offset) {
			next_sort(to_sort, extract_key, sort_data);
		} else if (containers::size(to_sort) <= bounded::constant<std_sort_threshold>) {
			containers::sort(to_sort, extract_key_to_less(extract_key));
		} else if (containers::size(to_sort) < bounded::constant<american_flag_sort_threshold>) {
			american_flag_sort(to_sort, extract_key, next_sort, sort_data, offset);
		} else {
			ska_byte_sort(to_sort, extract_key, next_sort, sort_data, offset);
		}
	}
	template<view View, typename ExtractKey>
	static constexpr void american_flag_sort(View to_sort, ExtractKey const & extract_key, NextSort<View, ExtractKey> next_sort, BaseListSortData * sort_data, std::size_t const offset) {
		auto partitions = partition_counts(to_sort, extract_key, sort_data, offset);
		auto const first = containers::begin(to_sort);
		using difference_type = iter_difference_t<decltype(first)>;
		[&]{
			if (partitions.number > 1) {
				std::uint8_t * current_block_ptr = partitions.remaining.data();
				PartitionInfo * current_block = partitions.partitions.data() + *current_block_ptr;
				std::uint8_t * last_block = partitions.remaining.data() + partitions.number - 1;
				auto it = first;
				auto block_end = first + ::bounded::assume_in_range<difference_type>(current_block->next_offset);
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

							it = first + ::bounded::assume_in_range<difference_type>(current_block->offset);
							block_end = first + ::bounded::assume_in_range<difference_type>(current_block->next_offset);
						}
					} else {
						auto const partition_offset = ::bounded::assume_in_range<containers::index_type<View>>(block->offset++);
						using std::swap;
						swap(*it, to_sort[partition_offset]);
					}
				}
			}
		}();
		auto partition_begin = first;
		for (std::uint8_t * it = partitions.remaining.data(), * remaining_end = partitions.remaining.data() + partitions.number; it != remaining_end; ++it) {
			auto const end_offset = ::bounded::assume_in_range<difference_type>(partitions.partitions[*it].next_offset);
			auto const partition_end = first + end_offset;
			if (partition_end - partition_begin > 1_bi) {
				sort_selector(
					range_view(partition_begin, partition_end),
					extract_key,
					next_sort,
					sort_data,
					offset + 1U
				);
			}
			partition_begin = partition_end;
		}
	}

	template<view View, typename ExtractKey>
	static constexpr void ska_byte_sort(View to_sort, ExtractKey const & extract_key, NextSort<View, ExtractKey> next_sort, BaseListSortData * sort_data, std::size_t const offset) {
		auto partitions = partition_counts(to_sort, extract_key, sort_data, offset);
		auto const first = containers::begin(to_sort);
		using difference_type = iter_difference_t<decltype(first)>;
		for (std::uint8_t * last_remaining = partitions.remaining.data() + partitions.number, * end_partition = partitions.remaining.data() + 1; last_remaining > end_partition;) {
			last_remaining = containers::partition(partitions.remaining.data(), last_remaining, [&](std::uint8_t index) {
				auto const begin_offset = ::bounded::assume_in_range<difference_type>(partitions.partitions[index].offset);
				auto const end_offset = ::bounded::assume_in_range<difference_type>(partitions.partitions[index].next_offset);
				if (begin_offset == end_offset)
					return false;

				for (auto it = first + begin_offset; it != first + end_offset; ++it) {
					std::uint8_t this_partition = current_byte(extract_key(*it), sort_data, offset);
					auto const partition_offset = ::bounded::assume_in_range<difference_type>(partitions.partitions[this_partition].offset++);
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
		for (std::uint8_t * it = partitions.remaining.data() + partitions.number; it != partitions.remaining.data(); --it) {
			std::uint8_t partition_index = it[-1];
			auto const start_offset = ::bounded::assume_in_range<difference_type>(partition_index == 0 ? 0 : partitions.partitions[partition_index - 1].next_offset);
			auto const end_offset = ::bounded::assume_in_range<difference_type>(partitions.partitions[partition_index].next_offset);
			auto partition_begin = first + start_offset;
			auto partition_end = first + end_offset;
			if (partition_end - partition_begin > 1_bi) {
				sort_selector(
					range_view(partition_begin, partition_end),
					extract_key,
					next_sort,
					sort_data,
					offset + 1U
				);
			}
		}
	}
};

template<std::ptrdiff_t std_sort_threshold, std::ptrdiff_t american_flag_sort_threshold, typename CurrentSubKey, typename SubKeyType>
struct ListInplaceSorter;

template<std::ptrdiff_t std_sort_threshold, std::ptrdiff_t american_flag_sort_threshold, typename CurrentSubKey, view View, typename ExtractKey>
constexpr void inplace_sort(View to_sort, ExtractKey const & extract_key, NextSort<View, ExtractKey> next_sort, BaseListSortData * sort_data) {
	using SubKeyType = decltype(CurrentSubKey::sub_key(extract_key(containers::front(to_sort)), sort_data));
	if constexpr (std::same_as<SubKeyType, bool>) {
		auto middle = containers::partition(to_sort, [&](auto && a){ return !CurrentSubKey::sub_key(extract_key(a), sort_data); });
		next_sort(
			range_view(containers::begin(to_sort), middle),
			extract_key,
			sort_data
		);
		next_sort(
			range_view(middle, containers::end(to_sort)),
			extract_key,
			sort_data
		);
	} else if constexpr (bounded::unsigned_builtin<SubKeyType>) {
		UnsignedInplaceSorter<std_sort_threshold, american_flag_sort_threshold, CurrentSubKey, sizeof(SubKeyType)>::sort(to_sort, extract_key, next_sort, sort_data);
	} else {
		ListInplaceSorter<std_sort_threshold, american_flag_sort_threshold, CurrentSubKey, SubKeyType>::sort(to_sort, extract_key, next_sort, sort_data);
	}
}

template<std::ptrdiff_t std_sort_threshold, std::ptrdiff_t american_flag_sort_threshold, typename CurrentSubKey, typename ListType>
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
		using base = SubKey<std::decay_t<decltype(containers::at(bounded::declval<ListType const &>(), 0))>>;

		using next = ElementSubKey;

		static constexpr decltype(auto) sub_key(auto && value, BaseListSortData * sort_data) {
			auto && list = CurrentSubKey::sub_key(OPERATORS_FORWARD(value), sort_data->next_sort_data);
			return base::sub_key(
				containers::at(OPERATORS_FORWARD(list), sort_data->current_index),
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
			::bounded::assume_in_range<iter_difference_t<decltype(containers::begin(current_key(containers::front(to_sort))))>>(sort_data.current_index)
		);
		auto end_of_shorter_ones = containers::partition(to_sort, [&](auto const & elem) {
			return containers::size(current_key(elem)) <= sort_data.current_index;
		});
		auto const first = containers::begin(to_sort);
		auto const last = containers::end(to_sort);
		if (end_of_shorter_ones - first > 1) {
			sort_data.next_sort(
				range_view(first, end_of_shorter_ones),
				extract_key,
				sort_data.next_sort_data
			);
		}
		if (last - end_of_shorter_ones > 1_bi) {
			inplace_sort<std_sort_threshold, american_flag_sort_threshold, ElementSubKey>(
				range_view(end_of_shorter_ones, last),
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

template<std::ptrdiff_t std_sort_threshold, std::ptrdiff_t american_flag_sort_threshold, typename CurrentSubKey, view View, typename ExtractKey>
constexpr void sort_starter(View to_sort, ExtractKey const & extract_key, BaseListSortData * next_sort_data) {
	if constexpr (!std::same_as<CurrentSubKey, SubKey<void>>) {
		if (containers::size(to_sort) <= 1_bi) {
			return;
		}

		inplace_sort<std_sort_threshold, american_flag_sort_threshold, CurrentSubKey>(
			to_sort,
			extract_key,
			static_cast<NextSort<View, ExtractKey>>(sort_starter<std_sort_threshold, american_flag_sort_threshold, typename CurrentSubKey::next>),
			next_sort_data
		);
	}
}

export template<std::ptrdiff_t std_sort_threshold, std::ptrdiff_t american_flag_sort_threshold>
constexpr void inplace_radix_sort(view auto to_sort, auto const & extract_key) {
	using SubKey = SubKey<decltype(extract_key(containers::front(to_sort)))>;
	sort_starter<std_sort_threshold, american_flag_sort_threshold, SubKey>(to_sort, extract_key, nullptr);
}

} // namespace containers
