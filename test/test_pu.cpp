/*
Copyright (c) 2019-2025,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "test.hpp"
#include "units/units.hpp"

using namespace units;

TEST(PU, basic)
{
    EXPECT_EQ(pu * V, puV);
    EXPECT_EQ(puHz, pu * Hz);
    EXPECT_TRUE(puHz.is_per_unit());
}

TEST(PU, Ops)
{
    EXPECT_EQ(puV * puA, pu * W);
    EXPECT_EQ(puV / puA, puOhm);
}
TEST(PU, baseGeneration)
{
    EXPECT_EQ(puconversion::generate_base(W.base_units(), 100.0, 34.7), 100.0);
    EXPECT_TRUE(
        std::isnan(puconversion::generate_base(m.base_units(), 45.8, 99.34)));
}

TEST(PU, Example1)
{
    EXPECT_EQ(convert(1.0, pu * watt, ohm, 10000.0, 100.0), 1.0);
    EXPECT_NEAR(
        convert(136.0, kV, puV, 500, 138000), 0.9855, test::tolerance * 100);
    // problem from text book
    auto basePower = 100000000.0;
    auto baseVoltage = 80000.0;

    EXPECT_NEAR(convert(1.0, ohm, puOhm, basePower, 8000), 1.56, 0.01);
    EXPECT_NEAR(convert(24.0, ohm, puOhm, basePower, baseVoltage), 0.375, 0.01);
    EXPECT_NEAR(convert(1.0, ohm, puOhm, basePower, 16000), 0.39, 0.01);
    EXPECT_NEAR(convert(1.0, puOhm, ohm, basePower, 8000), 0.64, 0.01);
    EXPECT_NEAR(convert(1.0, puOhm, ohm, basePower, baseVoltage), 64, 0.01);
    EXPECT_NEAR(convert(1.0, puOhm, ohm, basePower, 16000), 2.56, 0.01);
    EXPECT_NEAR(convert(1.0, pu * S, S, basePower, 16000), 1.0 / 2.56, 0.01);
    EXPECT_NEAR(convert(0.22, puA, A, basePower, baseVoltage), 275, 0.1);
    EXPECT_NEAR(
        convert(2.5, puMW, A, 100, baseVoltage),
        2.5 * basePower / baseVoltage,
        0.1);
    EXPECT_NEAR(
        convert(24.0, A, puMW, 100, baseVoltage),
        24.0 * baseVoltage / 100000000,
        0.000001);
}

TEST(PU, twoBaseValues)
{
    EXPECT_EQ(convert(1.47, ohm, defunit, 24.7, 92.3), 1.47);
    EXPECT_NEAR(convert(1.0, in, cm, 5.7, 19.36), 2.54, test::tolerance);

    EXPECT_NEAR(convert(4.5, puMW, puA, 100, 80000), 4.5, 0.00001);

    EXPECT_NEAR(convert(4.5, puMW, puMW, 100, 80), 4.5, 0.00001);

    EXPECT_NEAR(
        convert(4.5, pu * m, pu * m, 100, 80), 4.5 * 10.0 / 8.0, 0.00001);

    EXPECT_NEAR(convert(0.1, pu * m, pu * mm, 1.0, 10), 10.0, 0.00001);
}

TEST(PU, ConversionsJustPu)
{
    EXPECT_EQ(convert(1.0, pu, ohm, 5.0), 5.0);
    EXPECT_NEAR(
        convert(136.0, kV, pu, 500, 138000), 0.9855, test::tolerance * 100);
    EXPECT_EQ(convert(2.7, pu, puMW), 2.7);

    EXPECT_NEAR(convert(3.0, pu, MW, 100, 25000), 300, test::tolerance * 300);
}

TEST(PU, puBaseAssumptions)
{
    EXPECT_NEAR(convert(1.0, puHz, Hz), 60.0, 0.0001);
    EXPECT_NEAR(convert(60.05, Hz, puHz), 60.05 / 60.0, 0.0001);

    EXPECT_NEAR(convert(1.0, puMW, W), 100000000, 0.01);
    EXPECT_NEAR(convert(450.0, MW, puMW), 4.5, 0.01);

    EXPECT_NEAR(convert(0.2, puOhm, puMW), 5.0, 0.0001);
    EXPECT_NEAR(convert(0.2, puOhm, puA), 5.0, 0.0001);
    EXPECT_NEAR(convert(5.0, puMW, puOhm), 0.2, 0.0001);
    EXPECT_NEAR(convert(4.5, puMW, puA), 4.5, 0.00001);
    EXPECT_NEAR(convert(4.5, puA, puMW), 4.5, 0.00001);
    EXPECT_NEAR(convert(10.0, puA, puOhm), 0.1, 0.00001);
}

TEST(PU, machNumber)
{
    EXPECT_NEAR(convert(1.0, precise::special::mach, m / s), 341.25, 0.0001);
    EXPECT_NEAR(convert(350.0, m / s, precise::special::mach), 1.0256, 0.001);

    // TODO(PT) :: figure out how this might work
    // EXPECT_NEAR(convert(1.0, precise::special::mach,
    // precise::special::fujita), 12.0, 0.01);
}

TEST(PU, failures)
{
    EXPECT_TRUE(std::isnan(convert(0.76, precise::special::mach, puMW)));
    EXPECT_TRUE(std::isnan(convert(0.262, puV, puMW)));
    EXPECT_TRUE(std::isnan(convert(1.3, puV, ft)));
    EXPECT_TRUE(std::isnan(convert(1.0, puV, ft, 10000)));
}

TEST(PU, conversions)
{
    EXPECT_NEAR(convert(1.0, puMW, kilo * pu * W, 100.0), 1000.0, 0.0001);
    EXPECT_NEAR(convert(1.0, puMW, defunit, 100.0), 1.0, 0.0001);
    // the 100 has nothing to do with this just testing the function call
    EXPECT_NEAR(convert(1.0, inch, cm, 100.0), 2.54, 0.0001);

    EXPECT_NEAR(convert(0.2, puOhm, puMW, 100.0), 5.0, 0.0001);
}
