/*
Copyright © 2019,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "units/units.hpp"

#include "test.hpp"

using namespace units;

TEST(logUnits, nonEquality)
{
    auto u1 = precise::log::bel;
    EXPECT_FALSE(u1 == precise::log::dB);
    EXPECT_FALSE(u1 == precise::log::neglog10);
    EXPECT_FALSE(u1 == precise::log::logbase2);
    EXPECT_FALSE(u1 == precise::log::neglog100);
    EXPECT_FALSE(u1 == precise::log::neglog1000);
    EXPECT_FALSE(u1 == precise::log::neglog50000);
    EXPECT_FALSE(u1 == precise::log::neper);

    EXPECT_FALSE(precise::log::dB == precise::log::neglog10);
    EXPECT_FALSE(precise::log::dB == precise::log::logbase2);
    EXPECT_FALSE(precise::log::dB == precise::log::neglog100);
    EXPECT_FALSE(precise::log::dB == precise::log::neglog1000);
    EXPECT_FALSE(precise::log::dB == precise::log::neglog50000);
    EXPECT_FALSE(precise::log::dB == precise::log::neper);

    EXPECT_FALSE(precise::log::neglog10 == precise::log::logbase2);
    EXPECT_FALSE(precise::log::neglog10 == precise::log::neglog100);
    EXPECT_FALSE(precise::log::neglog10 == precise::log::neglog1000);
    EXPECT_FALSE(precise::log::neglog10 == precise::log::neglog50000);
    EXPECT_FALSE(precise::log::neglog10 == precise::log::neper);

    EXPECT_FALSE(precise::log::logbase2 == precise::log::neglog100);
    EXPECT_FALSE(precise::log::logbase2 == precise::log::neglog1000);
    EXPECT_FALSE(precise::log::logbase2 == precise::log::neglog50000);
    EXPECT_FALSE(precise::log::logbase2 == precise::log::neper);

    EXPECT_FALSE(precise::log::neglog100 == precise::log::neglog1000);
    EXPECT_FALSE(precise::log::neglog100 == precise::log::neglog50000);
    EXPECT_FALSE(precise::log::neglog100 == precise::log::neper);

    EXPECT_FALSE(precise::log::neglog1000 == precise::log::neglog50000);
    EXPECT_FALSE(precise::log::neglog1000 == precise::log::neper);

    EXPECT_FALSE(precise::log::neper == precise::log::neglog50000);
}

TEST(logUnits, base10)
{
    EXPECT_EQ(convert(precise::ten, precise::log::bel), 2.0);
    EXPECT_EQ(convert(precise::kilo, precise::log::bel), 6.0);
    EXPECT_EQ(convert(precise::milli, precise::log::bel), -6.0);
    EXPECT_EQ(convert(precise::exa, precise::log::bel), 36.0);
    EXPECT_EQ(convert(precise::femto, precise::log::bel), -30.0);
    EXPECT_EQ(convert(2.0, precise::log::bel, precise::ten), 1.0);
    EXPECT_EQ(convert(6.0, precise::log::bel, precise::kilo), 1.0);
    EXPECT_DOUBLE_EQ(convert(-6.0, precise::log::bel, precise::milli), 1.0);
    EXPECT_DOUBLE_EQ(convert(36.0, precise::log::bel, precise::exa), 1.0);
    EXPECT_NEAR(convert(-30.0, precise::log::bel, precise::femto), 1.0, test::precise_tolerance);

    EXPECT_EQ(convert(precise::ten, precise::log::belP), 1.0);
    EXPECT_EQ(convert(precise::kilo, precise::log::belP), 3.0);
    EXPECT_EQ(convert(precise::milli, precise::log::belP), -3.0);
    EXPECT_EQ(convert(precise::exa, precise::log::belP), 18.0);
    EXPECT_EQ(convert(precise::femto, precise::log::belP), -15.0);
    EXPECT_EQ(convert(1.0, precise::log::belP, precise::ten), 1.0);
    EXPECT_EQ(convert(3.0, precise::log::belP, precise::kilo), 1.0);
    EXPECT_DOUBLE_EQ(convert(-3.0, precise::log::belP, precise::milli), 1.0);
    EXPECT_DOUBLE_EQ(convert(18.0, precise::log::belP, precise::exa), 1.0);
    EXPECT_NEAR(convert(-15.0, precise::log::belP, precise::femto), 1.0, test::precise_tolerance);
}

TEST(logUnits, negbase10)
{
    EXPECT_EQ(convert(precise::ten, precise::log::neglog10), -1.0);
    EXPECT_EQ(convert(precise::kilo, precise::log::neglog10), -3.0);
    EXPECT_EQ(convert(precise::milli, precise::log::neglog10), 3.0);
    EXPECT_EQ(convert(precise::exa, precise::log::neglog10), -18.0);
    EXPECT_EQ(convert(precise::femto, precise::log::neglog10), 15.0);
    EXPECT_EQ(convert(-1.0, precise::log::neglog10, precise::ten), 1.0);
    EXPECT_EQ(convert(-3.0, precise::log::neglog10, precise::kilo), 1.0);
    EXPECT_DOUBLE_EQ(convert(3.0, precise::log::neglog10, precise::milli), 1.0);
    EXPECT_DOUBLE_EQ(convert(-18.0, precise::log::neglog10, precise::exa), 1.0);
    EXPECT_NEAR(convert(15.0, precise::log::neglog10, precise::femto), 1.0, test::precise_tolerance);
}

TEST(logUnits, dB)
{
    EXPECT_EQ(convert(precise::ten, precise::log::dB), 20.0);
    EXPECT_EQ(convert(precise::kilo, precise::log::dB), 60.0);
    EXPECT_EQ(convert(precise::milli, precise::log::dB), -60.0);
    EXPECT_EQ(convert(precise::exa, precise::log::dB), 360.0);
    EXPECT_EQ(convert(precise::femto, precise::log::dB), -300.0);
    EXPECT_EQ(convert(20.0, precise::log::dB, precise::ten), 1.0);
    EXPECT_EQ(convert(60.0, precise::log::dB, precise::kilo), 1.0);
    EXPECT_DOUBLE_EQ(convert(-60.0, precise::log::dB, precise::milli), 1.0);
    EXPECT_DOUBLE_EQ(convert(360.0, precise::log::dB, precise::exa), 1.0);
    EXPECT_NEAR(convert(-300.0, precise::log::dB, precise::femto), 1.0, test::precise_tolerance);
}

TEST(logUnits, negbase100)
{
    EXPECT_DOUBLE_EQ(convert(precise::hundred, precise::log::neglog100), -1.0);
    EXPECT_DOUBLE_EQ(convert(precise::mega, precise::log::neglog100), -3.0);
    EXPECT_DOUBLE_EQ(convert(precise::micro, precise::log::neglog100), 3.0);
    EXPECT_DOUBLE_EQ(convert(precise::exa, precise::log::neglog100), -9.0);
    EXPECT_DOUBLE_EQ(convert(precise::atto, precise::log::neglog100), 9.0);
    EXPECT_DOUBLE_EQ(convert(-1.0, precise::log::neglog100, precise::hundred), 1.0);
    EXPECT_DOUBLE_EQ(convert(-3.0, precise::log::neglog100, precise::mega), 1.0);
    EXPECT_DOUBLE_EQ(convert(3.0, precise::log::neglog100, precise::micro), 1.0);
    EXPECT_DOUBLE_EQ(convert(-9.0, precise::log::neglog100, precise::exa), 1.0);
    EXPECT_DOUBLE_EQ(convert(6.0, precise::log::neglog100, precise::pico), 1.0);
}

TEST(logUnits, negbase1000)
{
    EXPECT_EQ(convert(precise::kilo, precise::log::neglog1000), -1.0);
    EXPECT_EQ(convert(precise::milli, precise::log::neglog1000), 1.0);
    EXPECT_EQ(convert(precise::exa, precise::log::neglog1000), -6.0);
    EXPECT_EQ(convert(precise::femto, precise::log::neglog1000), 5.0);
    EXPECT_EQ(convert(-1.0, precise::log::neglog1000, precise::kilo), 1.0);
    EXPECT_EQ(convert(1.0, precise::log::neglog1000, precise::milli), 1.0);
    EXPECT_EQ(convert(-6.0, precise::log::neglog1000, precise::exa), 1.0);
    EXPECT_EQ(convert(5.0, precise::log::neglog1000, precise::femto), 1.0);
}

TEST(logUnits, negbase50000)
{
    EXPECT_EQ(convert(50000.0, precise::one, precise::log::neglog50000), -1.0);
    EXPECT_EQ(convert(50000.0 * 50000.0, precise::one, precise::log::neglog50000), -2.0);
    EXPECT_EQ(convert(1.0 / 50000.0, precise::one, precise::log::neglog50000), 1.0);
    EXPECT_EQ(convert(1.0 / (50000.0 * 50000.0), precise::one, precise::log::neglog50000), 2.0);
    EXPECT_EQ(convert(-1.0, precise::log::neglog50000, precise::one), 50000.0);
    EXPECT_EQ(convert(1.0, precise::log::neglog50000, precise::one), 1.0 / 50000.0);
    EXPECT_EQ(convert(-2.0, precise::log::neglog50000, precise::one), (50000.0 * 50000.0));
    EXPECT_DOUBLE_EQ(convert(2.0, precise::log::neglog50000, precise::one), 1.0 / (50000.0 * 50000.0));
}

TEST(logUnits, neper)
{
    EXPECT_EQ(convert(exp(1.0), precise::one, precise::log::neper), 1.0);
    EXPECT_EQ(convert(exp(3.0), precise::one, precise::log::neper), 3.0);
    EXPECT_EQ(convert(exp(-1.0), precise::one, precise::log::neper), -1.0);
    EXPECT_EQ(convert(exp(-3.65), precise::one, precise::log::neper), -3.65);
    EXPECT_EQ(convert(1.0, precise::log::neper, precise::one), exp(1));
    EXPECT_EQ(convert(-3.685, precise::log::neper, precise::one), exp(-3.685));
    EXPECT_EQ(convert(-2.0, precise::log::neper, precise::one), exp(-2));
    EXPECT_EQ(convert(2.0, precise::log::neper, precise::one), exp(2));
}

TEST(logUnits, log2)
{
    EXPECT_EQ(convert(4, precise::one, precise::log::logbase2), 2.0);
    EXPECT_EQ(convert(1024, precise::one, precise::log::logbase2), 10.0);
    EXPECT_EQ(convert(0.5, precise::one, precise::log::logbase2), -1.0);
    EXPECT_EQ(convert(1.0 / pow(2.0, 30), precise::one, precise::log::logbase2), -30);
    EXPECT_EQ(convert(2.0, precise::log::logbase2, precise::one), 4.0);
    EXPECT_EQ(convert(10, precise::log::logbase2, precise::one), 1024.0);
    EXPECT_EQ(convert(-2.0, precise::log::logbase2, precise::one), 0.25);
    EXPECT_EQ(convert(-40.0, precise::log::logbase2, precise::one), 1.0 / pow(2.0, 40));
}

TEST(logUnits, dBNeperConversions)
{
    EXPECT_EQ(convert(1.0, precise::log::bel, precise::log::dB), 10.0);
    EXPECT_NEAR(convert(1.0, precise::log::bel, precise::log::neper), 1.1512925465, test::tolerance);
    EXPECT_NEAR(convert(10.0, precise::log::dB, precise::log::neper), 1.1512925465, test::tolerance);

    EXPECT_EQ(convert(10.0, precise::log::bel, precise::log::dB), 100.0);
    EXPECT_NEAR(convert(10.0, precise::log::bel, precise::log::neper), 11.512925465, test::tolerance);
    EXPECT_NEAR(convert(100.0, precise::log::dB, precise::log::neper), 11.512925465, test::tolerance);

    EXPECT_NEAR(convert(2.0, precise::log::dB, precise::log::neper), 0.2302585093, test::tolerance);

    EXPECT_NEAR(convert(5.0, precise::log::neper, precise::log::dB), 43.4294481903, test::tolerance);
    EXPECT_NEAR(convert(5.0, precise::log::neper, precise::log::bel), 4.342944819, test::tolerance);
}

TEST(logUnits, pH)
{
    EXPECT_NEAR(convert(0.0025, precise::laboratory::molarity, precise::laboratory::pH), 2.6, 0.005);
    EXPECT_NEAR(convert(8.34, precise::laboratory::pH, precise::laboratory::molarity), 4.57e-9, 0.005);
    EXPECT_NEAR(convert(4.82e-5, precise::laboratory::molarity, precise::laboratory::pH), 4.32, 0.005);
}

TEST(otherUnits, prism_diopter)
{
    EXPECT_NEAR(convert(1, precise::deg, precise::clinical::prism_diopter), 1.75, 0.005);
    EXPECT_NEAR(convert(1.75, precise::clinical::prism_diopter, precise::deg), 1.0, 0.005);
}
