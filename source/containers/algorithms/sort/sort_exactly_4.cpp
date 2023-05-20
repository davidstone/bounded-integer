// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort.sort_exactly_4;

import containers.algorithms.sort.cheaply_sortable;
import containers.algorithms.sort.dereference_all;
import containers.algorithms.sort.relocate_in_order;
import containers.algorithms.sort.rotate_one;
import containers.algorithms.sort.sort_exactly_3;

import bounded;
import std_module;

using namespace bounded::literal;

namespace containers {

// stable, 4-5 compares (average 4.67), 0-6 relocates (average 4.08)
export constexpr auto sort_exactly_n_impl(auto & x0, auto & x1, auto & x2, auto & x3, auto const compare) -> void {
	using T = std::remove_reference_t<decltype(x0)>;
	if constexpr (cheaply_sortable<T>) {
		::containers::sort_exactly_n_impl(x0, x1, x2, compare);
		if (compare(x3, x2)) {
			if (compare(x3, x1)) {
				if (compare(x3, x0)) {
					::containers::rotate_one(x3, x2, x1, x0);
				} else {
					::containers::rotate_one(x3, x2, x1);
				}
			} else {
				std::ranges::swap(x3, x2);
			}
		} else {
		}
	} else {
		if (compare(x1, x0)) {
			if (compare(x2, x0)) {
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
}

export constexpr auto sort_exactly_n(auto it, bounded::constant_t<4> const size, auto const compare) -> void {
	auto [x0, x1, x2, x3, last] = ::containers::dereference_all(std::move(it), size);
	sort_exactly_n_impl(x0, x1, x2, x3, compare);
}

#if 1
// stable, 4-5 compares (average 4.67), 0-8 relocates (average 6.25)
export constexpr auto sort_exactly_n_relocate(auto it, bounded::constant_t<4> const size, auto out, auto const compare) {
	auto [x0, x1, x2, x3, last] = ::containers::dereference_all(std::move(it), size);
	if (compare(x1, x0)) {
		if (compare(x2, x0)) {
			if (compare(x2, x1)) {
				if (compare(x3, x1)) {
					if (compare(x3, x2)) {
						relocate_in_order(std::move(out), x3, x2, x1, x0);
					} else {
						relocate_in_order(std::move(out), x2, x3, x1, x0);
					}
				} else {
					if (compare(x3, x0)) {
						relocate_in_order(std::move(out), x2, x1, x3, x0);
					} else {
						relocate_in_order(std::move(out), x2, x1, x0, x3);
					}
				}
			} else {
				if (compare(x3, x2)) {
					if (compare(x3, x1)) {
						relocate_in_order(std::move(out), x3, x1, x2, x0);
					} else {
						relocate_in_order(std::move(out), x1, x3, x2, x0);
					}
				} else {
					if (compare(x3, x0)) {
						relocate_in_order(std::move(out), x1, x2, x3, x0);
					} else {
						relocate_in_order(std::move(out), x1, x2, x0, x3);
					}
				}
			}
		} else {
			if (compare(x3, x0)) {
				if (compare(x3, x1)) {
					relocate_in_order(std::move(out), x3, x1, x0, x2);
				} else {
					relocate_in_order(std::move(out), x1, x3, x0, x2);
				}
			} else {
				if (compare(x3, x2)) {
					relocate_in_order(std::move(out), x1, x0, x3, x2);
				} else {
					relocate_in_order(std::move(out), x1, x0, x2, x3);
				}
			}
		}
	} else {
		if (compare(x2, x1)) {
			if (compare(x2, x0)) {
				if (compare(x3, x0)) {
					if (compare(x3, x2)) {
						relocate_in_order(std::move(out), x3, x2, x0, x1);
					} else {
						relocate_in_order(std::move(out), x2, x3, x0, x1);
					}
				} else {
					if (compare(x3, x1)) {
						relocate_in_order(std::move(out), x2, x0, x3, x1);
					} else {
						relocate_in_order(std::move(out), x2, x0, x1, x3);
					}
				}
			} else {
				if (compare(x3, x2)) {
					if (compare(x3, x0)) {
						relocate_in_order(std::move(out), x3, x0, x2, x1);
					} else {
						relocate_in_order(std::move(out), x0, x3, x2, x1);
					}
				} else {
					if (compare(x3, x1)) {
						relocate_in_order(std::move(out), x0, x2, x3, x1);
					} else {
						relocate_in_order(std::move(out), x0, x2, x1, x3);
					}
				}
			}
		} else {
			if (compare(x3, x1)) {
				if (compare(x3, x0)) {
					relocate_in_order(std::move(out), x3, x0, x1, x2);
				} else {
					relocate_in_order(std::move(out), x0, x3, x1, x2);
				}
			} else {
				if (compare(x3, x2)) {
					relocate_in_order(std::move(out), x0, x1, x3, x2);
				} else {
					relocate_in_order(std::move(out), x0, x1, x2, x3);
				}
			}
		}
	}
	return last;
}

#else

template<typename T>
struct low_high_ref {
	constexpr low_high_ref(T & low_, T & high_):
		low(low_),
		high(high_)
	{
	}

	T & low;
	T & high;
};

// stable, 4-5 compares (average 4.67), 0-8 relocates (average 6.25)
export constexpr auto sort_exactly_n_relocate(auto it, bounded::constant_t<4>, auto const out, auto const compare) {
	auto [x0, x1, x2, x3, last] = ::containers::dereference_all(std::move(it), size);

	auto const first_half_in_order = !compare(x1, x0);
	auto const first_half = first_half_in_order ? low_high_ref(x0, x1) : low_high_ref(x1, x0);
	auto const second_half_in_order = !compare(x3, x2);
	auto const second_half = second_half_in_order ? low_high_ref(x2, x3) : low_high_ref(x3, x2);
	auto const lowest_in_first = !compare(second_half.low, first_half.low);
	auto const low_half = lowest_in_first ? low_high_ref(first_half.low, second_half.low) : low_high_ref(second_half.low, first_half.low);
	auto const highest_in_second = !compare(second_half.high, first_half.high);
	auto const high_half = highest_in_second ? low_high_ref(first_half.high, second_half.high) : low_high_ref(second_half.high, first_half.high);
	auto const middle_in_order = !compare(high_half.low, low_half.high);
	if (first_half_in_order and second_half_in_order and middle_in_order and std::addressof(low_half.high) == std::addressof(x1) and std::addressof(high_half.low) == std::addressof(x2)) {
		return last;
	}
	auto const middle_half = middle_in_order ? low_high_ref(low_half.high, high_half.low) : low_high_ref(high_half.low, low_half.high);
	relocate_in_order(out, low_half.low, middle_half.low, middle_half.high, high_half.high);
	return last;
}

#endif

} // namespace containers
