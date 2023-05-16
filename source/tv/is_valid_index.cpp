// Copyright David Stone 2018.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module tv.is_valid_index;

import bounded;
import std_module;

namespace tv {

template<typename Index, typename... Ts>
concept matches_exactly_one_type_impl = (bounded::constant<0> + ... + bounded::integer(std::same_as<Index, Ts>)) == bounded::constant<1>;

export template<typename Index, typename... Ts>
concept matches_exactly_one_type = matches_exactly_one_type_impl<std::decay_t<Index>, Ts...>;

export template<typename Index, std::size_t size>
concept variant_integer_index = bounded::bounded_integer<Index> and Index::value() < size;

export template<typename Index, typename... Ts>
concept unique_type_identifier = matches_exactly_one_type_impl<typename Index::type, Ts...> or variant_integer_index<Index, sizeof...(Ts)>;

} // namespace tv
