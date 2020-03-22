/*
Copyright (c) 2019-2020,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "test.hpp"
#include "units/units.hpp"

#include <tuple>

using unitD = std::tuple<const char*, const char*, units::precise_unit>;

TEST(r20, order)
{
    std::size_t unit_count{0};
    const void* r20 = units::detail::testing::r20rawData(unit_count);
    auto* r20data = reinterpret_cast<const unitD*>(r20);
    for (size_t ii = 1; ii < unit_count; ++ii) {
        EXPECT_LT(
            std::string(std::get<0>(r20data[ii - 1])),
            std::string(std::get<0>(r20data[ii])));
    }
}

TEST(r20, conversions)
{
    std::size_t unit_count{0};
    const void* r20 = units::detail::testing::r20rawData(unit_count);
    auto* r20data = reinterpret_cast<const unitD*>(r20);
    int missed{0};
	int correct{ 0 };
	int skipped{ 0 };
    for (size_t ii = 1; ii < unit_count; ++ii) {
        std::string ustr = std::string(std::get<1>(r20data[ii]));
        auto unit = units::unit_from_string(ustr);
        if (is_valid(unit)) {
            EXPECT_EQ(unit, std::get<2>(r20data[ii]))
                << ' ' << ii << ' ' << ustr << " not converted properly";
            if (unit != std::get<2>(r20data[ii])) {
                ++missed;
            }
			else
			{
				++correct;
			}
        }
		else
		{
			++skipped;
		}
    }
    std::cout << missed << " r20 units not translated properly\n";
	std::cout << skipped << " r20 units skipped\n";
	std::cout << correct << " r20 units correctly translated\n";
}