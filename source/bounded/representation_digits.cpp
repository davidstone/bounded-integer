// Copyright David Stone 2019.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <bounded/representation_digits.hpp>

#include <bounded/detail/class.hpp>
#include <bounded/detail/comparison_mixed.hpp>
#include <bounded/detail/literal.hpp>

namespace {
using namespace bounded::literal;

using sixty_three = bounded::constant_t<63>;
using sixty_four = bounded::constant_t<64>;
using sixty_five = bounded::constant_t<65>;

using negative_sixty_three_to_twenty = bounded::integer<-63, 20>;
using negative_sixty_four_to_twenty = bounded::integer<-64, 20>;
using negative_sixty_five_to_twenty = bounded::integer<-65, 20>;
using negative_one_to_nine = bounded::integer<-1, 9>;
using negative_one_to_ten = bounded::integer<-1, 10>;
using negative_one_to_ninety_nine = bounded::integer<-1, 99>;

enum class sixty_three_enum : std::uint16_t { value = 63 };

} // namespace
namespace bounded {

template<>
constexpr auto min_value<sixty_three_enum> = sixty_three_enum::value;
template<>
constexpr auto max_value<sixty_three_enum> = sixty_three_enum::value;

} // namespace bounded
namespace {

static_assert(bounded::representation_bits<sixty_three> == 6_bi);
static_assert(bounded::digits_in_string_representation<sixty_three>(2_bi) == 6_bi);
static_assert(bounded::digits_in_string_representation<sixty_three>(10_bi) == 2_bi);

static_assert(bounded::representation_bits<sixty_four> == 7_bi);
static_assert(bounded::digits_in_string_representation<sixty_four>(2_bi) == 7_bi);
static_assert(bounded::digits_in_string_representation<sixty_four>(10_bi) == 2_bi);

static_assert(bounded::representation_bits<sixty_five> == 7_bi);
static_assert(bounded::digits_in_string_representation<sixty_five>(2_bi) == 7_bi);
static_assert(bounded::digits_in_string_representation<sixty_five>(10_bi) == 2_bi);

static_assert(bounded::representation_bits<negative_sixty_three_to_twenty> == 7_bi);
static_assert(bounded::digits_in_string_representation<negative_sixty_three_to_twenty>(2_bi) == 7_bi);
static_assert(bounded::digits_in_string_representation<negative_sixty_three_to_twenty>(10_bi) == 3_bi);

static_assert(bounded::representation_bits<negative_sixty_four_to_twenty> == 8_bi);
static_assert(bounded::digits_in_string_representation<negative_sixty_four_to_twenty>(2_bi) == 8_bi);
static_assert(bounded::digits_in_string_representation<negative_sixty_four_to_twenty>(10_bi) == 3_bi);

static_assert(bounded::representation_bits<negative_sixty_five_to_twenty> == 8_bi);
static_assert(bounded::digits_in_string_representation<negative_sixty_five_to_twenty>(2_bi) == 8_bi);
static_assert(bounded::digits_in_string_representation<negative_sixty_five_to_twenty>(10_bi) == 3_bi);

static_assert(bounded::representation_bits<negative_one_to_nine> == 5_bi);
static_assert(bounded::digits_in_string_representation<negative_one_to_nine>(2_bi) == 4_bi);
static_assert(bounded::digits_in_string_representation<negative_one_to_nine>(10_bi) == 2_bi);

static_assert(bounded::representation_bits<negative_one_to_ten> == 5_bi);
static_assert(bounded::digits_in_string_representation<negative_one_to_ten>(2_bi) == 4_bi);
static_assert(bounded::digits_in_string_representation<negative_one_to_ten>(10_bi) == 2_bi);

static_assert(bounded::representation_bits<negative_one_to_ninety_nine> == 8_bi);
static_assert(bounded::digits_in_string_representation<negative_one_to_ninety_nine>(2_bi) == 7_bi);
static_assert(bounded::digits_in_string_representation<negative_one_to_ninety_nine>(10_bi) == 2_bi);

static_assert(bounded::representation_bits<sixty_three_enum> == 6_bi);
static_assert(bounded::digits_in_string_representation<sixty_three_enum>(2_bi) == 6_bi);
static_assert(bounded::digits_in_string_representation<sixty_three_enum>(10_bi) == 2_bi);

} // namespace