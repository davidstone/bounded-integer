// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort.sort_exactly_4;

import containers.algorithms.sort.cheaply_sortable;
import containers.algorithms.sort.low_high_ref;
import containers.algorithms.sort.relocate_in_order;
import containers.algorithms.sort.rotate_one;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

// Stable
// Average compares: 4.67 (weight 112)
// Max compares: 5
// Sorted compares: 4
// Reversed compares: 4
export template<typename T>
constexpr auto sort_exactly_n_in_place_impl(
	T & x0,
	T & x1,
	T & x2,
	T & x3,
	auto const compare
) -> void {
	if (compare(x1, x0)) {
		if (compare(x2, x1)) {
			if (compare(x3, x1)) {
				if (compare(x3, x2)) {
					std::ranges::swap(x0, x3);
					std::ranges::swap(x1, x2);
				} else {
					::containers::rotate_one(x0, x2, x1, x3);
				}
			} else {
				if (compare(x3, x0)) {
					::containers::rotate_one(x0, x2, x3);
				} else {
					std::ranges::swap(x0, x2);
				}
			}
		} else {
			if (compare(x2, x0)) {
				if (compare(x3, x2)) {
					if (compare(x3, x1)) {
						std::ranges::swap(x0, x3);
					} else {
						::containers::rotate_one(x0, x1, x3);
					}
				} else {
					if (compare(x3, x0)) {
						::containers::rotate_one(x0, x1, x2, x3);
					} else {
						::containers::rotate_one(x0, x1, x2);
					}
				}
			} else {
				if (compare(x3, x0)) {
					if (compare(x3, x1)) {
						::containers::rotate_one(x0, x3, x2);
					} else {
						::containers::rotate_one(x0, x1, x3, x2);
					}
				} else {
					if (compare(x3, x2)) {
						std::ranges::swap(x0, x1);
						std::ranges::swap(x2, x3);
					} else {
						std::ranges::swap(x0, x1);
					}
				}
			}
		}
	} else {
		if (compare(x2, x1)) {
			if (compare(x2, x0)) {
				if (compare(x3, x0)) {
					if (compare(x3, x2)) {
						::containers::rotate_one(x0, x3, x1, x2);
					} else {
						std::ranges::swap(x0, x2);
						std::ranges::swap(x1, x3);
					}
				} else {
					if (compare(x3, x1)) {
						::containers::rotate_one(x0, x2, x3, x1);
					} else {
						::containers::rotate_one(x0, x2, x1);
					}
				}
			} else {
				if (compare(x3, x2)) {
					if (compare(x3, x0)) {
						::containers::rotate_one(x0, x3, x1);
					} else {
						std::ranges::swap(x1, x3);
					}
				} else {
					if (compare(x3, x1)) {
						::containers::rotate_one(x1, x2, x3);
					} else {
						std::ranges::swap(x1, x2);
					}
				}
			}
		} else {
			if (compare(x3, x1)) {
				if (compare(x3, x0)) {
					::containers::rotate_one(x0, x3, x2, x1);
				} else {
					::containers::rotate_one(x1, x3, x2);
				}
			} else {
				if (compare(x3, x2)) {
					std::ranges::swap(x2, x3);
				} else {
				}
			}
		}
	}
}

// Stable
// Average compares: 5 (weight 120)
// Max compares: 5
// Sorted compares: 5
// Reversed compares: 5
template<typename T, typename Compare>
constexpr auto sort_exactly_n_relocate_branchless(
	T & x0,
	T & x1,
	T & x2,
	T & x3,
	auto out,
	Compare const compare
) -> void {
	auto const first_half_in_order = !compare(x1, x0);
	auto const first_half = first_half_in_order ? low_high_ref(x0, x1) : low_high_ref(x1, x0);
	auto const second_half_in_order = !compare(x3, x2);
	auto const second_half = second_half_in_order ? low_high_ref(x2, x3) : low_high_ref(x3, x2);
	auto const lowest_in_first = !compare(second_half.low, first_half.low);
	auto const low_half = lowest_in_first ? low_high_ref(first_half.low, second_half.low) : low_high_ref(second_half.low, first_half.low);
	auto const highest_in_second = !compare(second_half.high, first_half.high);
	auto const high_half = highest_in_second ? low_high_ref(first_half.high, second_half.high) : low_high_ref(second_half.high, first_half.high);
	auto const middle_in_order = !compare(high_half.low, low_half.high);
	auto const middle_half = middle_in_order ? low_high_ref(low_half.high, high_half.low) : low_high_ref(high_half.low, low_half.high);
	::containers::relocate_in_order(
		std::move(out),
		low_half.low,
		middle_half.low,
		middle_half.high,
		high_half.high
	);
}

// Stable
// Average compares: 4.67 (weight 112)
// Max compares: 5
// Sorted compares: 4
// Reversed compares: 4
template<typename T, typename Compare>
constexpr auto sort_exactly_n_relocate_min_compares(
	T & x0,
	T & x1,
	T & x2,
	T & x3,
	auto out,
	Compare const compare
) -> void {
	if (compare(x1, x0)) {
		if (compare(x2, x1)) {
			if (compare(x3, x1)) {
				if (compare(x3, x2)) {
					::containers::relocate_in_order(std::move(out), x3, x2, x1, x0);
				} else {
					::containers::relocate_in_order(std::move(out), x2, x3, x1, x0);
				}
			} else {
				if (compare(x3, x0)) {
					::containers::relocate_in_order(std::move(out), x2, x1, x3, x0);
				} else {
					::containers::relocate_in_order(std::move(out), x2, x1, x0, x3);
				}
			}
		} else {
			if (compare(x2, x0)) {
				if (compare(x3, x2)) {
					if (compare(x3, x1)) {
						::containers::relocate_in_order(std::move(out), x3, x1, x2, x0);
					} else {
						::containers::relocate_in_order(std::move(out), x1, x3, x2, x0);
					}
				} else {
					if (compare(x3, x0)) {
						::containers::relocate_in_order(std::move(out), x1, x2, x3, x0);
					} else {
						::containers::relocate_in_order(std::move(out), x1, x2, x0, x3);
					}
				}
			} else {
				if (compare(x3, x0)) {
					if (compare(x3, x1)) {
						::containers::relocate_in_order(std::move(out), x3, x1, x0, x2);
					} else {
						::containers::relocate_in_order(std::move(out), x1, x3, x0, x2);
					}
				} else {
					if (compare(x3, x2)) {
						::containers::relocate_in_order(std::move(out), x1, x0, x3, x2);
					} else {
						::containers::relocate_in_order(std::move(out), x1, x0, x2, x3);
					}
				}
			}
		}
	} else {
		if (compare(x2, x1)) {
			if (compare(x2, x0)) {
				if (compare(x3, x0)) {
					if (compare(x3, x2)) {
						::containers::relocate_in_order(std::move(out), x3, x2, x0, x1);
					} else {
						::containers::relocate_in_order(std::move(out), x2, x3, x0, x1);
					}
				} else {
					if (compare(x3, x1)) {
						::containers::relocate_in_order(std::move(out), x2, x0, x3, x1);
					} else {
						::containers::relocate_in_order(std::move(out), x2, x0, x1, x3);
					}
				}
			} else {
				if (compare(x3, x2)) {
					if (compare(x3, x0)) {
						::containers::relocate_in_order(std::move(out), x3, x0, x2, x1);
					} else {
						::containers::relocate_in_order(std::move(out), x0, x3, x2, x1);
					}
				} else {
					if (compare(x3, x1)) {
						::containers::relocate_in_order(std::move(out), x0, x2, x3, x1);
					} else {
						::containers::relocate_in_order(std::move(out), x0, x2, x1, x3);
					}
				}
			}
		} else {
			if (compare(x3, x1)) {
				if (compare(x3, x0)) {
					::containers::relocate_in_order(std::move(out), x3, x0, x1, x2);
				} else {
					::containers::relocate_in_order(std::move(out), x0, x3, x1, x2);
				}
			} else {
				if (compare(x3, x2)) {
					::containers::relocate_in_order(std::move(out), x0, x1, x3, x2);
				} else {
					::containers::relocate_in_order(std::move(out), x0, x1, x2, x3);
				}
			}
		}
	}
}

// Stable
export template<typename T, typename Compare>
constexpr auto sort_exactly_n_relocate_impl(
	T & x0,
	T & x1,
	T & x2,
	T & x3,
	auto out,
	Compare const compare
) -> void {
	if constexpr (cheaply_sortable<T, Compare>) {
		sort_exactly_n_relocate_branchless(x0, x1, x2, x3, std::move(out), compare);
	} else {
		sort_exactly_n_relocate_min_compares(x0, x1, x2, x3, std::move(out), compare);
	}
}

} // namespace containers
