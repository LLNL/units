/*
Copyright (c) 2019-2025,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "test.hpp"
#include "units/units.hpp"

#include <iostream>
#include <limits>
using namespace units;

TEST(allUnits, all)
{
    // this type difference is on purpose
    for (std::uint64_t ii = 0; ii <= std::numeric_limits<std::uint32_t>::max();
         ++ii) {
        detail::unit_data unitdata(nullptr);
        auto data = static_cast<std::uint32_t>(ii & 0xFFFFFFFF);
        memcpy(static_cast<void*>(&unitdata), &data, 4);
        auto startunit = unit(unitdata);
        auto str = to_string(startunit);
        auto resunit = unit_cast_from_string(str);
        EXPECT_EQ(startunit, resunit) << "round trip failed " << data;
        if (data % 0xFFFFFF == 1) {
            std::cout << static_cast<double>(ii >> 24) / 256.0
                      << "% complete\n";
        }
    }
}
