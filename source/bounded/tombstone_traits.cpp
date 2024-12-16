// Copyright David Stone 2020.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

export module bounded.tombstone_traits;

import bounded.arithmetic.operators;
import bounded.integer;
import bounded.normalize;

namespace bounded {

// Another alternative would be to implement this in terms of
// std::array<std::byte, sizeof(T)>. That approach is more correct because it
// does not claim that there is a T object somewhere, but it would not work with
// constexpr. This means all of the users of tombstone_traits would have to have
// a separate code path guarded by `if consteval` to avoid the small object
// optimization somehow.

export template<typename T>
struct tombstone_traits {
	static constexpr auto spare_representations = constant<0>;

	static auto make(auto index) noexcept -> T = delete;
	// Returns constant<-1> if there is an object present
	static auto index(T const &) noexcept = delete;
};

export struct tombstone_tag {};

export template<auto>
struct tombstone_traits_composer;

template<typename Outer, typename Inner, Inner Outer::* pointer>
struct tombstone_traits_composer<pointer> {
	static constexpr auto spare_representations = tombstone_traits<Inner>::spare_representations;

	static constexpr auto make(auto const index) noexcept -> Outer {
		return Outer(
			tombstone_tag(),
			[=] noexcept { return tombstone_traits<Inner>::make(index); }
		);
	}
	static constexpr auto index(Outer const & value) noexcept -> bounded::integer<
		-1,
		bounded::normalize<spare_representations - bounded::constant<1>>
	> {
		return tombstone_traits<Inner>::index(value.*pointer);
	}
};

} // namespace bounded