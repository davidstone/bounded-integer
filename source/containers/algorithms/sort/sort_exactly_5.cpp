// Copyright David Stone 2023.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module containers.algorithms.sort.sort_exactly_5;

import containers.algorithms.sort.relocate_in_order;
import containers.algorithms.sort.rotate_one;

import bounded;
import std_module;

namespace containers {
using namespace bounded::literal;

// Stable
// Average compares: 6.93 (weight 832)
// Max compares: 7
// Sorted compares: 6
// Reversed compares: 6
export template<typename T>
constexpr auto sort_exactly_n_in_place_impl(
	T & x0,
	T & x1,
	T & x2,
	T & x3,
	T & x4,
	auto const compare
) -> void {
	if (compare(x1, x0)) {
		if (compare(x3, x2)) {
			if (compare(x3, x1)) {
				if (compare(x4, x1)) {
					if (compare(x2, x1)) {
						if (compare(x4, x3)) {
							std::ranges::swap(x0, x4);
							std::ranges::swap(x1, x3);
						} else {
							if (compare(x4, x2)) {
								::containers::rotate_one(x0, x3, x1, x4);
							} else {
								::containers::rotate_one(x0, x3, x1, x2, x4);
							}
						}
					} else {
						if (compare(x2, x0)) {
							if (compare(x4, x3)) {
								std::ranges::swap(x0, x4);
								::containers::rotate_one(x1, x3, x2);
							} else {
								::containers::rotate_one(x0, x3, x2, x1, x4);
							}
						} else {
							if (compare(x4, x3)) {
								::containers::rotate_one(x0, x4, x2, x1, x3);
							} else {
								std::ranges::swap(x0, x3);
								::containers::rotate_one(x1, x4, x2);
							}
						}
					}
				} else {
					if (compare(x4, x0)) {
						if (compare(x4, x2)) {
							if (compare(x2, x0)) {
								::containers::rotate_one(x0, x3, x2, x4);
							} else {
								std::ranges::swap(x0, x3);
								std::ranges::swap(x2, x4);
							}
						} else {
							if (compare(x2, x1)) {
								::containers::rotate_one(x0, x3, x4);
								std::ranges::swap(x1, x2);
							} else {
								::containers::rotate_one(x0, x3, x4);
							}
						}
					} else {
						if (compare(x2, x0)) {
							if (compare(x2, x1)) {
								std::ranges::swap(x0, x3);
								std::ranges::swap(x1, x2);
							} else {
								std::ranges::swap(x0, x3);
							}
						} else {
							if (compare(x4, x2)) {
								::containers::rotate_one(x0, x3, x4, x2);
							} else {
								::containers::rotate_one(x0, x3, x2);
							}
						}
					}
				}
			} else {
				if (compare(x4, x3)) {
					if (compare(x3, x0)) {
						if (compare(x2, x0)) {
							if (compare(x4, x1)) {
								std::ranges::swap(x0, x4);
								std::ranges::swap(x2, x3);
							} else {
								::containers::rotate_one(x0, x1, x4);
								std::ranges::swap(x2, x3);
							}
						} else {
							if (compare(x4, x1)) {
								::containers::rotate_one(x0, x4, x2, x3);
							} else {
								::containers::rotate_one(x0, x1, x4, x2, x3);
							}
						}
					} else {
						if (compare(x4, x0)) {
							if (compare(x4, x1)) {
								::containers::rotate_one(x0, x4, x2);
							} else {
								::containers::rotate_one(x0, x1, x4, x2);
							}
						} else {
							std::ranges::swap(x0, x1);
							std::ranges::swap(x2, x4);
						}
					}
				} else {
					if (compare(x4, x2)) {
						if (compare(x4, x0)) {
							if (compare(x2, x0)) {
								::containers::rotate_one(x0, x1, x3, x2, x4);
							} else {
								::containers::rotate_one(x0, x1, x3);
								std::ranges::swap(x2, x4);
							}
						} else {
							if (compare(x3, x0)) {
								::containers::rotate_one(x0, x1, x3, x4, x2);
							} else {
								std::ranges::swap(x0, x1);
								::containers::rotate_one(x2, x3, x4);
							}
						}
					} else {
						if (compare(x2, x0)) {
							if (compare(x4, x0)) {
								::containers::rotate_one(x0, x1, x3, x4);
							} else {
								::containers::rotate_one(x0, x1, x3);
							}
						} else {
							if (compare(x3, x0)) {
								::containers::rotate_one(x0, x1, x3, x2);
							} else {
								std::ranges::swap(x0, x1);
								std::ranges::swap(x2, x3);
							}
						}
					}
				}
			}
		} else {
			if (compare(x2, x1)) {
				if (compare(x4, x1)) {
					if (compare(x3, x1)) {
						if (compare(x4, x2)) {
							std::ranges::swap(x0, x4);
							::containers::rotate_one(x1, x2, x3);
						} else {
							if (compare(x4, x3)) {
								::containers::rotate_one(x0, x2, x3, x1, x4);
							} else {
								::containers::rotate_one(x0, x2, x4);
								std::ranges::swap(x1, x3);
							}
						}
					} else {
						if (compare(x3, x0)) {
							if (compare(x4, x2)) {
								std::ranges::swap(x0, x4);
								std::ranges::swap(x1, x2);
							} else {
								::containers::rotate_one(x0, x2, x1, x4);
							}
						} else {
							if (compare(x4, x2)) {
								::containers::rotate_one(x0, x4, x3);
								std::ranges::swap(x1, x2);
							} else {
								::containers::rotate_one(x0, x2, x1, x4, x3);
							}
						}
					}
				} else {
					if (compare(x4, x0)) {
						if (compare(x4, x3)) {
							if (compare(x3, x0)) {
								::containers::rotate_one(x0, x2, x4);
							} else {
								::containers::rotate_one(x0, x2, x4, x3);
							}
						} else {
							if (compare(x3, x1)) {
								::containers::rotate_one(x0, x2, x1, x3, x4);
							} else {
								::containers::rotate_one(x0, x2, x3, x4);
							}
						}
					} else {
						if (compare(x3, x0)) {
							if (compare(x3, x1)) {
								::containers::rotate_one(x0, x2, x1, x3);
							} else {
								::containers::rotate_one(x0, x2, x3);
							}
						} else {
							if (compare(x4, x3)) {
								std::ranges::swap(x0, x2);
								std::ranges::swap(x3, x4);
							} else {
								std::ranges::swap(x0, x2);
							}
						}
					}
				}
			} else {
				if (compare(x4, x2)) {
					if (compare(x2, x0)) {
						if (compare(x3, x0)) {
							if (compare(x4, x1)) {
								std::ranges::swap(x0, x4);
							} else {
								::containers::rotate_one(x0, x1, x4);
							}
						} else {
							if (compare(x4, x1)) {
								::containers::rotate_one(x0, x4, x3);
							} else {
								::containers::rotate_one(x0, x1, x4, x3);
							}
						}
					} else {
						if (compare(x4, x0)) {
							if (compare(x4, x1)) {
								::containers::rotate_one(x0, x4, x3, x2);
							} else {
								::containers::rotate_one(x0, x1, x4, x3, x2);
							}
						} else {
							std::ranges::swap(x0, x1);
							::containers::rotate_one(x2, x4, x3);
						}
					}
				} else {
					if (compare(x4, x3)) {
						if (compare(x4, x0)) {
							if (compare(x3, x0)) {
								::containers::rotate_one(x0, x1, x2, x4);
							} else {
								::containers::rotate_one(x0, x1, x2, x4, x3);
							}
						} else {
							if (compare(x2, x0)) {
								::containers::rotate_one(x0, x1, x2);
								std::ranges::swap(x3, x4);
							} else {
								std::ranges::swap(x0, x1);
								std::ranges::swap(x3, x4);
							}
						}
					} else {
						if (compare(x3, x0)) {
							if (compare(x4, x0)) {
								::containers::rotate_one(x0, x1, x2, x3, x4);
							} else {
								::containers::rotate_one(x0, x1, x2, x3);
							}
						} else {
							if (compare(x2, x0)) {
								::containers::rotate_one(x0, x1, x2);
							} else {
								std::ranges::swap(x0, x1);
							}
						}
					}
				}
			}
		}
	} else {
		if (compare(x3, x2)) {
			if (compare(x2, x1)) {
				if (compare(x4, x2)) {
					if (compare(x4, x3)) {
						if (compare(x3, x0)) {
							if (compare(x2, x0)) {
								::containers::rotate_one(x0, x4, x1, x3);
							} else {
								::containers::rotate_one(x0, x4, x1, x3, x2);
							}
						} else {
							if (compare(x4, x0)) {
								::containers::rotate_one(x0, x4, x1);
								std::ranges::swap(x2, x3);
							} else {
								std::ranges::swap(x1, x4);
								std::ranges::swap(x2, x3);
							}
						}
					} else {
						if (compare(x4, x0)) {
							if (compare(x2, x0)) {
								std::ranges::swap(x0, x3);
								std::ranges::swap(x1, x4);
							} else {
								::containers::rotate_one(x0, x3, x2);
								std::ranges::swap(x1, x4);
							}
						} else {
							if (compare(x3, x0)) {
								::containers::rotate_one(x0, x3, x2, x4, x1);
							} else {
								::containers::rotate_one(x1, x3, x2, x4);
							}
						}
					}
				} else {
					if (compare(x2, x0)) {
						if (compare(x4, x0)) {
							std::ranges::swap(x0, x3);
							::containers::rotate_one(x1, x2, x4);
						} else {
							if (compare(x4, x1)) {
								::containers::rotate_one(x0, x3, x4, x1, x2);
							} else {
								::containers::rotate_one(x0, x3, x1, x2);
							}
						}
					} else {
						if (compare(x3, x0)) {
							if (compare(x4, x1)) {
								::containers::rotate_one(x0, x3, x4, x1);
							} else {
								::containers::rotate_one(x0, x3, x1);
							}
						} else {
							if (compare(x4, x1)) {
								::containers::rotate_one(x1, x3, x4);
							} else {
								std::ranges::swap(x1, x3);
							}
						}
					}
				}
			} else {
				if (compare(x4, x1)) {
					if (compare(x4, x0)) {
						if (compare(x3, x0)) {
							if (compare(x4, x3)) {
								::containers::rotate_one(x0, x4, x2);
								std::ranges::swap(x1, x3);
							} else {
								::containers::rotate_one(x0, x3, x1, x4, x2);
							}
						} else {
							if (compare(x3, x1)) {
								::containers::rotate_one(x0, x4, x2, x3, x1);
							} else {
								::containers::rotate_one(x0, x4, x2, x1);
							}
						}
					} else {
						if (compare(x4, x3)) {
							if (compare(x3, x1)) {
								::containers::rotate_one(x1, x4, x2, x3);
							} else {
								::containers::rotate_one(x1, x4, x2);
							}
						} else {
							if (compare(x3, x0)) {
								::containers::rotate_one(x0, x3, x1);
								std::ranges::swap(x2, x4);
							} else {
								std::ranges::swap(x1, x3);
								std::ranges::swap(x2, x4);
							}
						}
					}
				} else {
					if (compare(x3, x1)) {
						if (compare(x3, x0)) {
							if (compare(x4, x2)) {
								::containers::rotate_one(x0, x3, x4, x2, x1);
							} else {
								::containers::rotate_one(x0, x3, x2, x1);
							}
						} else {
							if (compare(x4, x2)) {
								::containers::rotate_one(x1, x3, x4, x2);
							} else {
								::containers::rotate_one(x1, x3, x2);
							}
						}
					} else {
						if (compare(x4, x2)) {
							if (compare(x4, x3)) {
								std::ranges::swap(x2, x4);
							} else {
								::containers::rotate_one(x2, x3, x4);
							}
						} else {
							std::ranges::swap(x2, x3);
						}
					}
				}
			}
		} else {
			if (compare(x3, x1)) {
				if (compare(x4, x3)) {
					if (compare(x4, x2)) {
						if (compare(x2, x0)) {
							if (compare(x3, x0)) {
								::containers::rotate_one(x0, x4, x1, x2, x3);
							} else {
								::containers::rotate_one(x0, x4, x1, x2);
							}
						} else {
							if (compare(x4, x0)) {
								::containers::rotate_one(x0, x4, x1);
							} else {
								std::ranges::swap(x1, x4);
							}
						}
					} else {
						if (compare(x4, x0)) {
							if (compare(x3, x0)) {
								::containers::rotate_one(x0, x2, x3);
								std::ranges::swap(x1, x4);
							} else {
								std::ranges::swap(x0, x2);
								std::ranges::swap(x1, x4);
							}
						} else {
							if (compare(x2, x0)) {
								::containers::rotate_one(x0, x2, x4, x1);
							} else {
								::containers::rotate_one(x1, x2, x4);
							}
						}
					}
				} else {
					if (compare(x3, x0)) {
						if (compare(x4, x0)) {
							::containers::rotate_one(x0, x2, x4, x1, x3);
						} else {
							if (compare(x4, x1)) {
								std::ranges::swap(x0, x2);
								::containers::rotate_one(x1, x3, x4);
							} else {
								std::ranges::swap(x0, x2);
								std::ranges::swap(x1, x3);
							}
						}
					} else {
						if (compare(x2, x0)) {
							if (compare(x4, x1)) {
								::containers::rotate_one(x0, x2, x3, x4, x1);
							} else {
								::containers::rotate_one(x0, x2, x3, x1);
							}
						} else {
							if (compare(x4, x1)) {
								::containers::rotate_one(x1, x2, x3, x4);
							} else {
								::containers::rotate_one(x1, x2, x3);
							}
						}
					}
				}
			} else {
				if (compare(x4, x1)) {
					if (compare(x4, x0)) {
						if (compare(x2, x0)) {
							if (compare(x4, x2)) {
								::containers::rotate_one(x0, x4, x3, x1, x2);
							} else {
								std::ranges::swap(x0, x2);
								::containers::rotate_one(x1, x4, x3);
							}
						} else {
							if (compare(x2, x1)) {
								::containers::rotate_one(x0, x4, x3, x1);
							} else {
								::containers::rotate_one(x0, x4, x3, x2, x1);
							}
						}
					} else {
						if (compare(x4, x2)) {
							if (compare(x2, x1)) {
								::containers::rotate_one(x1, x4, x3);
							} else {
								::containers::rotate_one(x1, x4, x3, x2);
							}
						} else {
							if (compare(x2, x0)) {
								::containers::rotate_one(x0, x2, x4, x3, x1);
							} else {
								::containers::rotate_one(x1, x2, x4, x3);
							}
						}
					}
				} else {
					if (compare(x2, x1)) {
						if (compare(x2, x0)) {
							if (compare(x4, x3)) {
								::containers::rotate_one(x0, x2, x1);
								std::ranges::swap(x3, x4);
							} else {
								::containers::rotate_one(x0, x2, x1);
							}
						} else {
							if (compare(x4, x3)) {
								std::ranges::swap(x1, x2);
								std::ranges::swap(x3, x4);
							} else {
								std::ranges::swap(x1, x2);
							}
						}
					} else {
						if (compare(x4, x3)) {
							if (compare(x4, x2)) {
								::containers::rotate_one(x2, x4, x3);
							} else {
								std::ranges::swap(x3, x4);
							}
						} else {
						}
					}
				}
			}
		}
	}
}

} // namespace containers
