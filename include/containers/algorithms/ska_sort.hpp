// Copyright David Stone 2019, Malte Skarupke 2016.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#pragma once

#include <bounded/detail/copy_cv_ref.hpp>
#include <bounded/detail/forward.hpp>
#include <containers/algorithms/count.hpp>
#include <containers/algorithms/minmax_element.hpp>
#include <containers/algorithms/negate.hpp>
#include <containers/algorithms/partition.hpp>
#include <containers/algorithms/sort.hpp>
#include <containers/algorithms/unique.hpp>
#include <containers/erase.hpp>
#include <containers/legacy_iterator.hpp>
#include <containers/is_range.hpp>
#include <containers/is_iterator_sentinel.hpp>

#include <cassert>
#include <climits>
#include <cstdint>
#include <cstring>
#include <algorithm>
#include <type_traits>
#include <tuple>
#include <utility>

namespace containers {

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
		// TODO: implement with std::bit_cast to allow constexpr evaluation
		static_assert(std::numeric_limits<T>::is_iec559);
		struct unknown_floating_point;
		using unsigned_t =
			std::conditional_t<sizeof(T) == 4, std::uint32_t,
			std::conditional_t<sizeof(T) == 8, std::uint64_t,
			unknown_floating_point
		>>;
		static_assert(sizeof(T) == sizeof(unsigned_t));
		auto u = unsigned_t{};
		std::memcpy(std::addressof(u), std::addressof(value), sizeof(value));
		constexpr auto sign_bit_position = std::numeric_limits<unsigned_t>::digits - 1U;
		unsigned_t const sign_bit = -static_cast<std::make_signed_t<unsigned_t>>(u >> (sign_bit_position));
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

template<typename T> requires std::is_enum_v<T>
auto to_radix_sort_key(T const value) {
	return to_radix_sort_key(static_cast<std::underlying_type_t<T>>(value));
}

template<typename T>
auto to_radix_sort_key(T * ptr) {
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

template<typename It, typename ExtractKey>
using key_type = std::decay_t<decltype(std::declval<ExtractKey>()(*std::declval<It>()))>;

template<typename T>
concept indexable =
	requires(T && value) { BOUNDED_FORWARD(value)[0]; } or
	requires(T && value) { BOUNDED_FORWARD(value)[bounded::constant<0>]; };

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
using NextSort = void (&)(Iterator, Iterator, ExtractKey &, BaseListSortData *);

template<typename It, typename ExtractKey>
struct ListSortData : BaseListSortData {
	NextSort<It, ExtractKey> next_sort;
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

constexpr auto extract_key_to_compare = [](auto extract_key) {
	return [extract_key = std::move(extract_key)](auto const & lhs, auto const & rhs) {
		return extract_key(lhs) < extract_key(rhs);
	};
};

template<typename ExtractKey>
using extract_key_to_compare_t = decltype(extract_key_to_compare(std::declval<ExtractKey>()));

template<typename It, typename ExtractKey>
constexpr void StdSortFallback(It begin, It end, ExtractKey extract_key) {
	sort(begin, end, extract_key_to_compare(std::move(extract_key)));
}

struct PartitionCounts {
	std::array<PartitionInfo, 256> partitions;
	std::array<uint8_t, 256> remaining = {};
	int number = 0;
};

template<std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey, size_t NumBytes>
struct UnsignedInplaceSorter {
	// Must have this exact signature, no defaulted arguments
	template<typename It, typename ExtractKey>
	static constexpr void sort(It begin, It end, ExtractKey & extract_key, NextSort<It, ExtractKey> next_sort, BaseListSortData * sort_data) {
		sort_selector(begin, end, extract_key, next_sort, sort_data, 0U);
	}
private:

	template<typename T>
	constexpr static auto current_byte(T const & elem, BaseListSortData * sort_data, std::size_t const offset) {
		auto const shift_amount = (NumBytes - 1U - offset) * 8U;
		return static_cast<std::uint8_t>(CurrentSubKey::sub_key(elem, sort_data) >> shift_amount);
	}

	template<typename Iterator, typename ExtractKey>
	static constexpr auto partition_counts(Iterator first, Iterator last, ExtractKey & extract_key, BaseListSortData * sort_data, std::size_t const offset) {
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

	template<typename It, typename ExtractKey>
	static constexpr void sort_selector(It begin, It end, ExtractKey & extract_key, NextSort<It, ExtractKey> next_sort, BaseListSortData * sort_data, std::size_t const offset) {
		if (NumBytes == offset) {
			next_sort(begin, end, extract_key, sort_data);
		} else if (end - begin < bounded::constant<AmericanFlagSortThreshold>) {
			american_flag_sort(begin, end, extract_key, next_sort, sort_data, offset);
		} else {
			ska_byte_sort(begin, end, extract_key, next_sort, sort_data, offset);
		}
	}
	template<typename It, typename ExtractKey>
	static constexpr void american_flag_sort(It begin, It end, ExtractKey & extract_key, NextSort<It, ExtractKey> next_sort, BaseListSortData * sort_data, std::size_t const offset) {
		auto partitions = partition_counts(begin, end, extract_key, sort_data, offset);
		[&]{
			if (partitions.number > 1) {
				uint8_t * current_block_ptr = partitions.remaining.data();
				PartitionInfo * current_block = partitions.partitions.data() + *current_block_ptr;
				uint8_t * last_block = partitions.remaining.data() + partitions.number - 1;
				It it = begin;
				It block_end = begin + current_block->next_offset;
				It last_element = end - 1;
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

							it = begin + current_block->offset;
							block_end = begin + current_block->next_offset;
						}
					} else {
						size_t partition_offset = block->offset++;
						// TODO: ADL
						containers::swap(*it, *(begin + partition_offset));
					}
				}
			}
		}();
		size_t start_offset = 0;
		It partition_begin = begin;
		for (uint8_t * it = partitions.remaining.data(), * remaining_end = partitions.remaining.data() + partitions.number; it != remaining_end; ++it) {
			size_t end_offset = partitions.partitions[*it].next_offset;
			It partition_end = begin + end_offset;
			if (partition_end - partition_begin > bounded::constant<1>) {
				sort_selector(partition_begin, partition_end, extract_key, next_sort, sort_data, offset + 1U);
			}
			start_offset = end_offset;
			partition_begin = partition_end;
		}
	}

	template<typename It, typename ExtractKey>
	static constexpr void ska_byte_sort(It begin, It end, ExtractKey & extract_key, NextSort<It, ExtractKey> next_sort, BaseListSortData * sort_data, std::size_t const offset) {
		auto partitions = partition_counts(begin, end, extract_key, sort_data, offset);
		for (uint8_t * last_remaining = partitions.remaining.data() + partitions.number, * end_partition = partitions.remaining.data() + 1; last_remaining > end_partition;) {
			last_remaining = containers::partition(partitions.remaining.data(), last_remaining, [&](uint8_t partition) {
				size_t & begin_offset = partitions.partitions[partition].offset;
				size_t & end_offset = partitions.partitions[partition].next_offset;
				if (begin_offset == end_offset)
					return false;

				// TODO: This can sometimes perform a self-swap. If this could
				// be restructured to avoid that, it would also be a
				// performance gain.
				for (auto it = begin + begin_offset; it != begin + end_offset; ++it) {
					uint8_t this_partition = current_byte(extract_key(*it), sort_data, offset);
					size_t partition_offset = partitions.partitions[this_partition].offset++;
					// TODO: ADL
					containers::swap(*it, *(begin + partition_offset));
				}
				return begin_offset != end_offset;
			});
		}
		for (uint8_t * it = partitions.remaining.data() + partitions.number; it != partitions.remaining.data(); --it) {
			uint8_t partition = it[-1];
			size_t start_offset = (partition == 0 ? 0 : partitions.partitions[partition - 1].next_offset);
			size_t end_offset = partitions.partitions[partition].next_offset;
			It partition_begin = begin + start_offset;
			It partition_end = begin + end_offset;
			if (partition_end - partition_begin > bounded::constant<1>) {
				sort_selector(partition_begin, partition_end, extract_key, next_sort, sort_data, offset + 1U);
			}
		}
	}
};

template<std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey, typename SubKeyType>
struct ListInplaceSorter;

template<std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey, typename It, typename ExtractKey>
constexpr void inplace_sort(It begin, It end, ExtractKey & extract_key, NextSort<It, ExtractKey> next_sort, BaseListSortData * sort_data) {
	using SubKeyType = decltype(CurrentSubKey::sub_key(extract_key(*begin), sort_data));
	if constexpr (std::is_same_v<SubKeyType, bool>) {
		It middle = containers::partition(begin, end, [&](auto && a){ return !CurrentSubKey::sub_key(extract_key(a), sort_data); });
		next_sort(begin, middle, extract_key, sort_data);
		next_sort(middle, end, extract_key, sort_data);
	} else if constexpr (std::is_unsigned_v<SubKeyType>) {
		UnsignedInplaceSorter<AmericanFlagSortThreshold, CurrentSubKey, sizeof(SubKeyType)>::sort(begin, end, extract_key, next_sort, sort_data);
	} else {
		ListInplaceSorter<AmericanFlagSortThreshold, CurrentSubKey, SubKeyType>::sort(begin, end, extract_key, next_sort, sort_data);
	}
}

template<typename It, typename ExtractKey, typename ElementKey>
constexpr size_t common_prefix(It first, It last, size_t start_index, ExtractKey && extract_key, ElementKey && element_key) {
	assert(first != last);
	auto const & first_range = extract_key(*first);
	auto largest_match = end(first_range) - (begin(first_range) + start_index);
	for (auto it = containers::next(first); it != last; ++it) {
		auto const & current_range = extract_key(*it);
		largest_match = std::min(
			largest_match,
			std::mismatch(
				begin(first_range) + start_index,
				end(first_range),
				begin(current_range) + start_index,
				end(current_range),
				[&](auto const & lhs, auto const & rhs) {
					return element_key(lhs) == element_key(rhs);
				}
			).first - (begin(first_range) + start_index)
		);
	}
	return static_cast<std::size_t>(largest_match);
}

template<std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey, typename ListType>
struct ListInplaceSorter {
	template<typename It, typename ExtractKey>
	static constexpr void sort(It begin, It end, ExtractKey & extract_key, NextSort<It, ExtractKey> next_sort, BaseListSortData * next_sort_data) {
		constexpr auto current_index = 0U;
		constexpr auto recursion_limit = 16U;
		auto const offset = ListSortData<It, ExtractKey>{
			{
				current_index,
				recursion_limit,
				next_sort_data,
			},
			next_sort,
		};
		sort(begin, end, extract_key, offset);
	}

private:
	struct ElementSubKey {
		using base = SubKey<typename std::decay<decltype(std::declval<ListType>()[0])>::type>;

		using next = ElementSubKey;

		template<typename U>
		static constexpr decltype(auto) sub_key(U const & value, BaseListSortData * sort_data) {
			auto const & list = CurrentSubKey::sub_key(value, sort_data->next_sort_data);
			return base::sub_key(list[sort_data->current_index], sort_data->next_sort_data);
		}
	};

	template<typename It, typename ExtractKey>
	static constexpr void sort(It begin, It end, ExtractKey & extract_key, ListSortData<It, ExtractKey> sort_data) {
		auto current_key = [&](auto const & elem) -> decltype(auto) {
			return CurrentSubKey::sub_key(extract_key(elem), sort_data.next_sort_data);
		};
		auto element_key = [&](auto && elem) -> decltype(auto) {
			return ElementSubKey::base::sub_key(elem, std::addressof(sort_data));
		};
		sort_data.current_index += common_prefix(begin, end, sort_data.current_index, current_key, element_key);
		It end_of_shorter_ones = containers::partition(begin, end, [&](auto const & elem) {
			return current_key(elem).size() <= sort_data.current_index;
		});
		if (end_of_shorter_ones - begin > 1) {
			sort_data.next_sort(begin, end_of_shorter_ones, extract_key, sort_data.next_sort_data);
		}
		if (end - end_of_shorter_ones > bounded::constant<1>) {
			inplace_sort<AmericanFlagSortThreshold, ElementSubKey>(
				end_of_shorter_ones,
				end,
				extract_key,
				static_cast<NextSort<It, ExtractKey>>(sort_from_recursion),
				std::addressof(sort_data)
			);
		}
	}

	template<typename It, typename ExtractKey>
	static constexpr void sort_from_recursion(It begin, It end, ExtractKey & extract_key, BaseListSortData * next_sort_data) {
		auto offset = *static_cast<ListSortData<It, ExtractKey> *>(next_sort_data);
		++offset.current_index;
		--offset.recursion_limit;
		if (offset.recursion_limit == 0) {
			StdSortFallback(begin, end, extract_key);
		} else {
			sort(begin, end, extract_key, offset);
		}
	}
};

template<std::ptrdiff_t AmericanFlagSortThreshold, typename CurrentSubKey, typename It, typename ExtractKey>
constexpr void sort_starter(It begin, It end, ExtractKey & extract_key, BaseListSortData * next_sort_data) {
	if constexpr (!std::is_same_v<CurrentSubKey, SubKey<void>>) {
		if (end - begin <= bounded::constant<1>) {
			return;
		}

		inplace_sort<AmericanFlagSortThreshold, CurrentSubKey>(
			begin,
			end,
			extract_key,
			static_cast<NextSort<It, ExtractKey>>(sort_starter<AmericanFlagSortThreshold, typename CurrentSubKey::next>),
			next_sort_data
		);
	}
}

template<std::ptrdiff_t AmericanFlagSortThreshold, typename It, typename ExtractKey>
constexpr void inplace_radix_sort(It begin_, It end_, ExtractKey & extract_key) {
	auto begin = make_legacy_iterator(begin_);
	auto end = make_legacy_iterator(end_);
	using SubKey = SubKey<decltype(extract_key(*begin))>;
	sort_starter<AmericanFlagSortThreshold, SubKey>(begin, end, extract_key, nullptr);
}

inline constexpr auto identity = [](auto && value) -> auto && {
	return BOUNDED_FORWARD(value);
};
using identity_t = std::decay_t<decltype(identity)>;

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

} // namespace detail

struct ska_sort_t {
	template<iterator Iterator, typename ExtractKey>
	constexpr void operator()(Iterator begin, sentinel_for<Iterator> auto end, ExtractKey && extract_key) const {
		detail::inplace_radix_sort<1024>(begin, end, extract_key);
	}

	template<iterator Iterator>
	constexpr void operator()(Iterator begin, sentinel_for<Iterator> auto end) const {
		operator()(begin, end, detail::identity);
	}

	template<typename ExtractKey>
	constexpr void operator()(range auto && range, ExtractKey && extract_key) const {
		operator()(begin(range), end(range), BOUNDED_FORWARD(extract_key));
	}
	template<range Range>
	constexpr void operator()(Range && range) const {
		operator()(BOUNDED_FORWARD(range), detail::identity);
	}
} inline constexpr ska_sort;

struct ska_sort_copy_t {
	template<iterator SourceIterator, iterator BufferIterator, typename ExtractKey>
	constexpr bool operator()(SourceIterator begin_, sentinel_for<SourceIterator> auto end_, BufferIterator buffer_begin_, ExtractKey && extract_key) const {
		auto begin = make_legacy_iterator(begin_);
		auto end = make_legacy_iterator(end_);
		auto buffer_begin = make_legacy_iterator(buffer_begin_);

		using key_t = detail::key_type<SourceIterator, ExtractKey>;
		if constexpr (std::is_same_v<key_t, bool>) {
			auto false_position = buffer_begin;
			auto true_position = std::next(
				buffer_begin,
				containers::count_if(begin, end, containers::negate(extract_key)).value()
			);
			for (; begin != end; ++begin) {
				auto & position = extract_key(*begin) ? true_position : false_position;
				*position = std::move(*begin);
				++position;
			}
			return true;
		} else if constexpr (std::is_arithmetic_v<key_t>) {
			return numeric_sort_impl(begin, end, buffer_begin, extract_key);
		} else if constexpr (containers::range<key_t>) {
			if (begin == end) {
				return false;
			}
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
							range_view(begin, end),
							[](auto const & range) { return size(range); }
						)
					);
#endif
				}
			}();
			bool which = false;
			auto buffer_end = buffer_begin + (end - begin);
			for (size_t index = max_size; index > 0; --index) {
				auto extract_n = [&, index = index - 1](auto && o) {
					return extract_key(o)[index];
				};
				if (which) {
					which = !operator()(buffer_begin, buffer_end, begin, extract_n);
				} else {
					which = operator()(begin, end, buffer_begin, extract_n);
				}
			}
			return which;
		} else if constexpr (detail::tuple_like<key_t>) {
			return tuple_sort_impl<0U>(begin, end, buffer_begin, extract_key);
		} else {
			return operator()(begin, end, buffer_begin, [&](auto && a) -> decltype(auto) {
				return to_radix_sort_key(extract_key(a));
			});
		}
	}

	template<iterator SourceIterator, iterator BufferIterator>
	constexpr auto operator()(SourceIterator source_begin, sentinel_for<SourceIterator> auto source_end, BufferIterator buffer_begin) const -> bool {
		return operator()(source_begin, source_end, buffer_begin, detail::identity);
	}

	template<range SourceRange, range BufferRange, typename ExtractKey>
	constexpr auto operator()(SourceRange && source_range, BufferRange && buffer_range, ExtractKey && extract_key) const -> bool {
		return operator()(begin(source_range), end(source_range), begin(buffer_range), BOUNDED_FORWARD(extract_key));
	}
	template<range SourceRange, range BufferRange>
	constexpr auto operator()(SourceRange && source_range, BufferRange && buffer_range) const -> bool {
		return operator()(source_range, buffer_range, detail::identity);
	}
private:
	template<typename It, typename OutIt, typename ExtractKey>
	static constexpr bool numeric_sort_impl(It begin, It end, OutIt out_begin, ExtractKey && extract_key) {
		auto const out_end = out_begin + (end - begin);
		constexpr auto size = sizeof(detail::key_type<It, ExtractKey>);
		std::array<std::array<std::size_t, 256>, size> counts = {};

		for (It it = begin; it != end; ++it) {
			auto key = to_radix_sort_key(extract_key(*it));
			for (std::size_t index = 0U; index != size; ++index) {
				auto const inner_index = (key >> (index * 8U)) & 0xFF;
				++counts[index][inner_index];
			}
		}
		std::array<std::size_t, size> total = {};
		for (std::size_t index = 0U; index != size; ++index) {
			auto & indexed_total = total[index];
			auto & count = counts[index];
			for (int i = 0; i < 256; ++i) {
				indexed_total += std::exchange(count[i], indexed_total);
			}
		}
		static_assert(size == 1U or size % 2 == 0);
		for (std::size_t index = 0U; index != size; ) {
			for (It it = begin; it != end; ++it) {
				std::uint8_t const key = to_radix_sort_key(extract_key(*it)) >> (index * 8U);
				out_begin[counts[index][key]++] = std::move(*it);
			}
			++index;
			if constexpr (size != 1U) {
				for (OutIt it = out_begin; it != out_end; ++it) {
					std::uint8_t const key = to_radix_sort_key(extract_key(*it)) >> ((index) * 8U);
					begin[counts[index][key]++] = std::move(*it);
				}
				++index;
			}
		}
		return size == 1U;
	}

	template<std::size_t index, typename It, typename OutIt, typename ExtractKey>
	static constexpr bool tuple_sort_impl(It begin, It end, OutIt out_begin, ExtractKey && extract_key) {
		using key_t = detail::key_type<It, ExtractKey>;
		if constexpr (index == std::tuple_size_v<key_t>) {
			return false;
		} else {
			auto const out_end = out_begin + (end - begin);
			bool which = tuple_sort_impl<index + 1U>(begin, end, out_begin, extract_key);
			using std::get;
			auto extract_i = [&](auto && o) -> detail::extract_return_type<index, ExtractKey, decltype(o)> {
				return get<index>(extract_key(o));
			};
			if (which) {
				return !ska_sort_copy_t{}(out_begin, out_end, begin, extract_i);
			} else {
				return ska_sort_copy_t{}(begin, end, out_begin, extract_i);
			}
		}
	}
} inline constexpr ska_sort_copy;

constexpr inline struct unique_ska_sort_t {
	template<range Range, typename ExtractKey>
	constexpr auto operator()(Range & range, ExtractKey extract_key) const {
		ska_sort(range, extract_key);
		auto const equal = [&](auto const & lhs, auto const & rhs) {
			return extract_key(lhs) == extract_key(rhs);
		};
		return ::containers::erase(
			range,
			::containers::unique(begin(range), end(range), equal),
			end(range)
		);
	}
	template<range Range>
	constexpr auto operator()(Range && range) const {
		return operator()(range, detail::identity);
	}
} unique_ska_sort;

} // namespace containers