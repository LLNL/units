/*
Copyright (c) 2019-2020,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#include "test.hpp"
#include "units/units.hpp"

#include <iostream>
#include <type_traits>

using namespace units;
TEST(MeasurementStrings, basic)
{
    auto pm = measurement_from_string("45 m");
    EXPECT_EQ(pm, 45.0 * precise::m);

    pm = measurement_from_string("9.0 * 5.0 m");
    EXPECT_EQ(pm, 45.0 * precise::m);

    pm = measurement_from_string("23.7 m/s");
    EXPECT_EQ(pm, 23.7 * precise::m / precise::s);
    pm = measurement_from_string("99.9 N * m");
    EXPECT_EQ(pm, 99.9 * precise::N * precise::m);

    EXPECT_NO_THROW(pm = measurement_from_string(""));
    EXPECT_EQ(pm.value(), 0.0);
}

TEST(MeasurementStrings, basic_cast)
{
    auto pm = measurement_cast_from_string("45 m");
    EXPECT_EQ(pm, 45.0 * m);

    pm = measurement_cast_from_string("9.0 * 5.0 m");
    EXPECT_EQ(pm, 45.0 * m);

    pm = measurement_cast_from_string("23.7 m/s");
    EXPECT_EQ(pm, 23.7 * m / s);
    EXPECT_EQ(pm, 23.7 * (m / s));
    pm = measurement_cast_from_string("99.9 N * m");
    EXPECT_EQ(pm, 99.9 * N * m);
    EXPECT_EQ(pm, 99.9 * (N * m));
}

TEST(MeasurementStrings, invalid)
{
    auto pm = measurement_from_string("345 blarg");
    EXPECT_FALSE(is_valid(pm.units()));
}

TEST(MeasurementStrings, currency)
{
    auto pm = measurement_from_string("$9.99");
    EXPECT_EQ(pm, 9.99 * precise::currency);

    pm = measurement_from_string("$ 9.99");
    EXPECT_EQ(pm, 9.99 * precise::currency);
}

TEST(MeasurementToString, simple)
{
    auto pm = precise_measurement(45.0, precise::m);
    EXPECT_EQ(to_string(pm), "45 m");
    auto meas = 45.0 * m;
    EXPECT_EQ(to_string(meas), "45 m");
}
