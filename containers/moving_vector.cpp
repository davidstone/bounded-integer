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

#include "moving_vector.hpp"
#include <algorithm>
#include <cassert>
#include <numeric>

using namespace containers;
using namespace smart_pointer;

namespace {
moving_vector<int> container_after_unique() {
	moving_vector<int> v;
	v.push_back(-3);
	for (int const value : {2, 5, 6}) {
		v.emplace_back(value);
	}
	return v;
}
}	// namespace

int main() {
	assert(moving_vector<int>({1, 2, 2, 3}) == moving_vector<int>({1, 2, 2, 3}));
	moving_vector<int> v({2, 5, 6, 2, -3});
	assert(std::accumulate(v.begin(), v.end(), 0) == 2 + 5 + 6 + 2 - 3);
	std::sort(v.begin(), v.end());
	assert(v == moving_vector<int>({-3, 2, 2, 5, 6}));
	auto const last = std::unique(v.begin(), v.end());
	v.erase(last, v.end());
	assert(v == container_after_unique());
	moving_vector<moving_vector<int>> inception(5);
	assert(inception.size() == 5);
	assert(inception.front().size() == 0);
	inception.front().assign(v.begin(), v.end());
	assert(inception.front() == v);

	v.emplace(v.begin(), 12);
	assert(v == moving_vector<int>({12, -3, 2, 5, 6}));
	v.emplace(v.end(), 12);
	assert(v == moving_vector<int>({12, -3, 2, 5, 6, 12}));
	v.emplace(v.begin() + 3, 15);
	assert(v == moving_vector<int>({12, -3, 2, 15, 5, 6, 12}));
}
