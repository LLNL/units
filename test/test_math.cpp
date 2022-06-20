/*
Copyright (c) 2019-2022,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "test.hpp"
#include "units/units_math.hpp"

using namespace units;
TEST(mathOps, type_traits)
{
    EXPECT_TRUE(is_measurement<uncertain_measurement>::value);
    EXPECT_TRUE(is_measurement<measurement>::value);
    EXPECT_TRUE(is_measurement<precise_measurement>::value);
    EXPECT_TRUE(is_measurement<fixed_measurement>::value);
    EXPECT_TRUE(is_measurement<fixed_precise_measurement>::value);
}

TEST(mathOps, uncertain_meas)
{
    uncertain_measurement um1(5.69F, 0.01F, m.pow(3));

    auto um2 = cbrt(um1);
    EXPECT_EQ(um2, root(um1, 3));

    auto um3 = floor(um1);
    EXPECT_FLOAT_EQ(um3.value(), 5.0F);

    auto um4 = trunc(um1);
    EXPECT_FLOAT_EQ(um4.value(), 5.0F);

    auto um5 = ceil(um1);
    EXPECT_FLOAT_EQ(um5.value(), 6.0F);

    auto um6 = round(um1);
    EXPECT_FLOAT_EQ(um6.value(), 6.0F);
}

TEST(mathOps, meas)
{
    measurement um1(5.69F, m.pow(3));

    auto um2 = cbrt(um1);
    EXPECT_EQ(um2, root(um1, 3));

    auto um3 = floor(um1);
    EXPECT_FLOAT_EQ(um3.value(), 5.0F);

    auto um4 = trunc(um1);
    EXPECT_FLOAT_EQ(um4.value(), 5.0F);

    auto um5 = ceil(um1);
    EXPECT_FLOAT_EQ(um5.value(), 6.0F);

    auto um6 = round(um1);
    EXPECT_FLOAT_EQ(um6.value(), 6.0F);
}

TEST(mathOps, fixed_meas)
{
    fixed_measurement um1(5.69F, m.pow(3));

    auto um2 = cbrt(um1);
    EXPECT_EQ(um2, root(um1, 3));

    auto um3 = floor(um1);
    EXPECT_FLOAT_EQ(um3.value(), 5.0F);

    auto um4 = trunc(um1);
    EXPECT_FLOAT_EQ(um4.value(), 5.0F);

    auto um5 = ceil(um1);
    EXPECT_FLOAT_EQ(um5.value(), 6.0F);

    auto um6 = round(um1);
    EXPECT_FLOAT_EQ(um6.value(), 6.0F);
}

TEST(mathOps, precise_meas)
{
    precise_measurement um1(5.69F, precise::m.pow(3));

    auto um2 = cbrt(um1);
    EXPECT_EQ(um2, root(um1, 3));

    auto um3 = floor(um1);
    EXPECT_FLOAT_EQ(um3.value(), 5.0F);

    auto um4 = trunc(um1);
    EXPECT_FLOAT_EQ(um4.value(), 5.0F);

    auto um5 = ceil(um1);
    EXPECT_FLOAT_EQ(um5.value(), 6.0F);

    auto um6 = round(um1);
    EXPECT_FLOAT_EQ(um6.value(), 6.0F);
}

TEST(mathOps, precise_fixed_meas)
{
    fixed_precise_measurement um1(5.69F, precise::m.pow(3));

    auto um2 = cbrt(um1);
    EXPECT_EQ(um2, root(um1, 3));

    auto um3 = floor(um1);
    EXPECT_FLOAT_EQ(um3.value(), 5.0F);

    auto um4 = trunc(um1);
    EXPECT_FLOAT_EQ(um4.value(), 5.0F);

    auto um5 = ceil(um1);
    EXPECT_FLOAT_EQ(um5.value(), 6.0F);

    auto um6 = round(um1);
    EXPECT_FLOAT_EQ(um6.value(), 6.0F);
}

TEST(mathOps, trig_sin)
{
    measurement m1(constants::pi / 4.0, rad);
    EXPECT_FLOAT_EQ(sin(m1), sin(constants::pi / 4.0));

    measurement m2(45.0, deg);
    EXPECT_FLOAT_EQ(sin(m2), sin(constants::pi / 4.0));

    precise_measurement m3(constants::pi / 4.0, precise::rad);
    EXPECT_DOUBLE_EQ(sin(m3), sin(constants::pi / 4.0));

    precise_measurement m4(45.0, precise::deg);
    EXPECT_DOUBLE_EQ(sin(m4), sin(constants::pi / 4.0));
}

TEST(mathOps, trig_cos)
{
    measurement m1(constants::pi / 4.0, rad);
    EXPECT_FLOAT_EQ(cos(m1), cos(constants::pi / 4.0));

    measurement m2(45.0, deg);
    EXPECT_FLOAT_EQ(cos(m2), cos(constants::pi / 4.0));

    precise_measurement m3(constants::pi / 4.0, precise::rad);
    EXPECT_DOUBLE_EQ(cos(m3), cos(constants::pi / 4.0));

    precise_measurement m4(45.0, precise::deg);
    EXPECT_DOUBLE_EQ(cos(m4), cos(constants::pi / 4.0));
}

TEST(mathOps, trig_tan)
{
    measurement m1(constants::pi / 6.0, rad);
    EXPECT_FLOAT_EQ(tan(m1), tan(constants::pi / 6.0));

    measurement m2(30.0, deg);
    EXPECT_FLOAT_EQ(tan(m2), tan(constants::pi / 6.0));

    precise_measurement m3(constants::pi / 6.0, precise::rad);
    EXPECT_DOUBLE_EQ(tan(m3), tan(constants::pi / 6.0));

    precise_measurement m4(30.0, precise::deg);
    EXPECT_DOUBLE_EQ(tan(m4), tan(constants::pi / 6.0));
}

TEST(mathOps, hypot)
{
    measurement m1(5.0, m);
    measurement m2(5.4, in);

    auto res = hypot(m1, m2);
    EXPECT_GT(res, m1);
    EXPECT_GT(res, m2);

    auto res2 = hypot(m2, m1);
    EXPECT_EQ(res, res2);
}

TEST(mathOps, hypot3)
{
    precise_measurement m1(5.0, precise::m);
    precise_measurement m2(5.4, precise::in);
    precise_measurement m3(0.01, precise::km);
    auto res = hypot(m1, m2, m3);
    EXPECT_GT(res, m1);
    EXPECT_GT(res, m2);
    EXPECT_GT(res, m3);
    auto res2 = hypot(m3, m1, m2);
    auto res3 = hypot(m2, m3, m1);
    EXPECT_EQ(res, res2);
    EXPECT_EQ(res2, res3);
}
