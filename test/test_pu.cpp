/*
Copyright © 2019,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "units/units.hpp"

#include "test.hpp"

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

TEST(PU, Example1)
{
    EXPECT_EQ(convert(1.0, puMW, ohm, 10000.0, 100.0), 1.0);
    EXPECT_NEAR(convert(136.0, kV, puV, 500, 138000), 0.9855, test::tolerance * 100);
    // problem from text book
    auto basePower = 100000000;

    EXPECT_NEAR(convert(1.0, ohm, puOhm, basePower, 8000), 1.56, 0.01);
    EXPECT_NEAR(convert(24.0, ohm, puOhm, basePower, 80000), 0.375, 0.01);
    EXPECT_NEAR(convert(1.0, ohm, puOhm, basePower, 16000), 0.39, 0.01);
    EXPECT_NEAR(convert(1.0, puOhm, ohm, basePower, 8000), 0.64, 0.01);
    EXPECT_NEAR(convert(1.0, puOhm, ohm, basePower, 80000), 64, 0.01);
    EXPECT_NEAR(convert(1.0, puOhm, ohm, basePower, 16000), 2.56, 0.01);

    EXPECT_NEAR(convert(0.22, puA, A, 100000000, 80000), 275, 0.1);
}

TEST(PU, Conversions_just_pu)
{
    EXPECT_EQ(convert(1.0, pu, ohm, 5.0), 5.0);
    EXPECT_NEAR(convert(136.0, kV, pu, 500, 138000), 0.9855, test::tolerance * 100);
    EXPECT_EQ(convert(2.7, pu, puMW), 2.7);
}

TEST(PU, pu_base_assumptions)
{
    EXPECT_NEAR(convert(1.0, puHz, Hz), 60.0, 0.0001);
    EXPECT_NEAR(convert(60.05, Hz, puHz), 60.05 / 60.0, 0.0001);

    EXPECT_NEAR(convert(1.0, puMW, W), 100000000, 0.01);
    EXPECT_NEAR(convert(450.0, MW, puMW), 4.5, 0.01);
}

TEST(PU, machNumber)
{
    EXPECT_NEAR(convert(1.0, precise::special::mach, m / s), 341.25, 0.0001);
    EXPECT_NEAR(convert(350.0, m / s, precise::special::mach), 1.0256, 0.001);

    // TODO:: figure out how this might work
    // EXPECT_NEAR(convert(1.0, precise::special::mach, precise::special::fujita), 12.0, 0.01);
}
