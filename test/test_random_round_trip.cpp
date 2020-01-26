/*
Copyright (c) 2019-2020,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "test.hpp"
#include "units/units.hpp"

#include <limits>
#include <random>

using namespace units;

TEST(randomRoundTrip, basic)
{
    std::default_random_engine generator;
    std::uniform_int_distribution<unsigned int> distribution(
        0, std::numeric_limits<unsigned int>::max());

    for (auto ii = 0; ii < 6000; ++ii) {
        auto start = distribution(generator);
        detail::unit_data unitdata(nullptr);
        memcpy(static_cast<void*>(&unitdata), &start, 4);
        auto startunit = unit(unitdata);
        auto str = to_string(startunit);
        auto resunit = unit_cast(unit_from_string(str));
        EXPECT_EQ(startunit, resunit) << "round trip failed " << start;
    }
}

struct rtrip : public ::testing::TestWithParam<unsigned int> {
};

TEST_P(rtrip, testConversions)
{
    unsigned int start = GetParam();
    detail::unit_data unitdata(nullptr);
    memcpy(static_cast<void*>(&unitdata), &start, 4);
    auto startunit = unit(unitdata);
    auto str = to_string(startunit);
    auto resunit = unit_cast(unit_from_string(str));
    EXPECT_EQ(startunit, resunit) << "round trip failed " << start;
}

const std::vector<unsigned int> customList{0, 545404204, 297480282, 1504872254};

INSTANTIATE_TEST_SUITE_P(spotChecks, rtrip, ::testing::ValuesIn(customList));
