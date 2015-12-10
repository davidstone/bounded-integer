// A vector-like class that can take advantage of cheap moves
// Copyright (C) 2014 David Stone
//
// This program is free software: you can redistribute it and / or modify
// it under the terms of the GNU Affero General Public License as
// published by the Free Software Foundation, either version 3 of the
// License, or (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU Affero General Public License for more details.
//
// You should have received a copy of the GNU Affero General Public License
// along with this program. If not, see <http://www.gnu.org/licenses/>.

#include <containers/moving_vector/moving_vector.hpp>

#include <containers/algorithms/accumulate.hpp>
#include <containers/algorithms/unique.hpp>

#include <algorithm>
#include <cassert>
#include <numeric>

namespace {
using namespace containers;
using namespace smart_pointer;
using namespace bounded::literal;

moving_vector<int> container_after_unique() {
	moving_vector<int> v;
	push_back(v, -3);
	for (int const value : {2, 5, 6}) {
		v.emplace_back(value);
	}
	return v;
}
}	// namespace

int main() {
	assert(moving_vector<int>({1, 2, 2, 3}) == moving_vector<int>({1, 2, 2, 3}));
	moving_vector<int> v({2, 5, 6, 2, -3});
	assert((containers::accumulate<bounded::checked_integer<0, 100>>(v.begin(), v.end())) == 2_bi + 5_bi + 6_bi + 2_bi - 3_bi);
	std::sort(v.begin(), v.end());
	assert(v == moving_vector<int>({-3, 2, 2, 5, 6}));
	auto const last = containers::unique(v.begin(), v.end());
	erase(v, last, v.end());
	assert(v == container_after_unique());
	moving_vector<moving_vector<int>> inception(5_bi);
	assert(size(inception) == 5_bi);
	assert(size(front(inception)) == 0_bi);
	assign(front(inception), v.begin(), v.end());
	assert(front(inception) == v);

	v.emplace(v.begin(), 12);
	assert(v == moving_vector<int>({12, -3, 2, 5, 6}));
	v.emplace(v.end(), 12);
	assert(v == moving_vector<int>({12, -3, 2, 5, 6, 12}));
	v.emplace(v.begin() + 3, 15);
	assert(v == moving_vector<int>({12, -3, 2, 15, 5, 6, 12}));
}
