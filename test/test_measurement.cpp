/*
Copyright © 2019,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#include "test.hpp"
#include "units/units.hpp"
#include <type_traits>

using namespace units;
TEST(Measurement, ops)
{
    measurement d1(45.0, m);
    measurement d2(79, m);

    auto area = d1 * d2;
    EXPECT_EQ(area.value(), 45.0 * 79);
    EXPECT_TRUE(area.units() == m * m);

    EXPECT_TRUE(d1 * d2 == d2 * d1);

    auto sum = d1 + d2;
    EXPECT_EQ(sum.value(), 45.0 + 79.0);
    EXPECT_TRUE(sum.units() == m);
    EXPECT_TRUE(d1 + d2 == d2 + d1);

    auto diff = d2 - d1;
    EXPECT_EQ(diff.value(), 79.0 - 45);
    EXPECT_TRUE(diff.units() == m);

    auto rat = d1 / d2;
    EXPECT_EQ(rat.value(), 45.0 / 79);
    EXPECT_TRUE(rat.units() == ratio);
}

TEST(Measurement, help_constructors)
{
    auto d1 = 45.0 * m;
    auto d2 = m * 79.0;

    static_assert(std::is_same<decltype(d1), decltype(d2)>::value,
                  "Types are not producing same measurement type");

    auto area = d1 * d2;
    EXPECT_EQ(area.value(), 45.0 * 79);
    EXPECT_TRUE(area.units() == m * m);

    EXPECT_TRUE(d1 * d2 == d2 * d1);

    auto sum = d1 + d2;
    EXPECT_EQ(sum.value(), 45.0 + 79.0);
    EXPECT_TRUE(sum.units() == m);
    EXPECT_TRUE(d1 + d2 == d2 + d1);

    auto diff = d2 - d1;
    EXPECT_EQ(diff.value(), 79.0 - 45);
    EXPECT_TRUE(diff.units() == m);

    auto rat = d1 / d2;
    EXPECT_EQ(rat.value(), 45.0 / 79);
    EXPECT_TRUE(rat.units() == ratio);
}

TEST(Measurement, comparison)
{
    EXPECT_TRUE(1000.0 * m == 1 * km);
    EXPECT_FALSE(1000.0 * m != 1 * km);

    EXPECT_TRUE(1 * in > 2 * cm);
    EXPECT_FALSE(1 * in < 2 * cm);
    EXPECT_TRUE((1 * in) == (2.54 * cm));
}

using namespace units;
TEST(fixedMeasurement, ops)
{
    fixed_measurement_type<double> d1(45.0, m);
    fixed_measurement_type<double> d2(79, m);

    auto area = d1 * d2;
    EXPECT_EQ(area.value(), 45.0 * 79);
    EXPECT_TRUE(area.units() == m * m);

    EXPECT_TRUE(d1 * d2 == d2 * d1);

    auto sum = d1 + d2;
    EXPECT_EQ(sum.value(), 45.0 + 79.0);
    EXPECT_TRUE(sum.units() == m);
    EXPECT_TRUE(d1 + d2 == d2 + d1);

    auto diff = d2 - d1;
    EXPECT_EQ(diff.value(), 79.0 - 45);
    EXPECT_TRUE(diff.units() == m);

    auto rat = d1 / d2;
    EXPECT_EQ(rat.value(), 45.0 / 79);
    EXPECT_TRUE(rat.units() == ratio);
}
