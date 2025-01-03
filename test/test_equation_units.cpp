/*
Copyright (c) 2019-2025,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "test.hpp"
#include "units/units.hpp"

#include <vector>

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
    EXPECT_DOUBLE_EQ(convert(-6.0, precise::log::belA, precise::milli), 1.0);
    EXPECT_DOUBLE_EQ(convert(-3.0, precise::log::belP, precise::milli), 1.0);
    EXPECT_DOUBLE_EQ(convert(36.0, precise::log::bel, precise::exa), 1.0);
    EXPECT_NEAR(
        convert(-30.0, precise::log::bel, precise::femto),
        1.0,
        test::precise_tolerance);

    EXPECT_EQ(convert(precise::ten, precise::log::belP), 1.0);
    EXPECT_EQ(convert(precise::ten, precise::log::belA), 2.0);
    EXPECT_EQ(convert(precise::kilo, precise::log::belP), 3.0);
    EXPECT_EQ(convert(precise::milli, precise::log::belP), -3.0);
    EXPECT_EQ(convert(precise::exa, precise::log::belP), 18.0);
    EXPECT_EQ(convert(precise::femto, precise::log::belP), -15.0);
    EXPECT_EQ(convert(1.0, precise::log::belP, precise::ten), 1.0);
    EXPECT_EQ(convert(3.0, precise::log::belP, precise::kilo), 1.0);
    EXPECT_DOUBLE_EQ(convert(-3.0, precise::log::belP, precise::milli), 1.0);
    EXPECT_DOUBLE_EQ(convert(18.0, precise::log::belP, precise::exa), 1.0);
    EXPECT_NEAR(
        convert(-15.0, precise::log::belP, precise::femto),
        1.0,
        test::precise_tolerance);
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
    EXPECT_NEAR(
        convert(15.0, precise::log::neglog10, precise::femto),
        1.0,
        test::precise_tolerance);
}

TEST(logUnits, dB)
{
    EXPECT_EQ(convert(precise::ten, precise::log::dB), 20.0);
    EXPECT_EQ(convert(precise::ten, precise::log::dBA), 20.0);
    EXPECT_EQ(convert(precise::ten, precise::log::dBP), 10.0);
    EXPECT_EQ(convert(precise::kilo, precise::log::dB), 60.0);
    EXPECT_EQ(convert(precise::milli, precise::log::dB), -60.0);
    EXPECT_EQ(convert(precise::exa, precise::log::dB), 360.0);
    EXPECT_EQ(convert(precise::femto, precise::log::dB), -300.0);
    EXPECT_EQ(convert(20.0, precise::log::dB, precise::ten), 1.0);
    EXPECT_EQ(convert(60.0, precise::log::dB, precise::kilo), 1.0);
    EXPECT_EQ(convert(20.0, precise::log::dBA, precise::ten), 1.0);
    EXPECT_EQ(convert(60.0, precise::log::dBA, precise::kilo), 1.0);
    EXPECT_EQ(convert(10.0, precise::log::dBP, precise::ten), 1.0);
    EXPECT_EQ(convert(30.0, precise::log::dBP, precise::kilo), 1.0);
    EXPECT_DOUBLE_EQ(convert(-60.0, precise::log::dB, precise::milli), 1.0);
    EXPECT_DOUBLE_EQ(convert(360.0, precise::log::dB, precise::exa), 1.0);
    EXPECT_NEAR(
        convert(-300.0, precise::log::dB, precise::femto),
        1.0,
        test::precise_tolerance);
}

TEST(logUnits, negbase100)
{
    EXPECT_DOUBLE_EQ(convert(precise::hundred, precise::log::neglog100), -1.0);
    EXPECT_DOUBLE_EQ(convert(precise::mega, precise::log::neglog100), -3.0);
    EXPECT_DOUBLE_EQ(convert(precise::micro, precise::log::neglog100), 3.0);
    EXPECT_DOUBLE_EQ(convert(precise::exa, precise::log::neglog100), -9.0);
    EXPECT_DOUBLE_EQ(convert(precise::atto, precise::log::neglog100), 9.0);
    EXPECT_DOUBLE_EQ(
        convert(-1.0, precise::log::neglog100, precise::hundred), 1.0);
    EXPECT_DOUBLE_EQ(
        convert(-3.0, precise::log::neglog100, precise::mega), 1.0);
    EXPECT_DOUBLE_EQ(
        convert(3.0, precise::log::neglog100, precise::micro), 1.0);
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
    EXPECT_EQ(
        convert(50000.0 * 50000.0, precise::one, precise::log::neglog50000),
        -2.0);
    EXPECT_EQ(
        convert(1.0 / 50000.0, precise::one, precise::log::neglog50000), 1.0);
    EXPECT_EQ(
        convert(
            1.0 / (50000.0 * 50000.0), precise::one, precise::log::neglog50000),
        2.0);
    EXPECT_EQ(convert(-1.0, precise::log::neglog50000, precise::one), 50000.0);
    EXPECT_EQ(
        convert(1.0, precise::log::neglog50000, precise::one), 1.0 / 50000.0);
    EXPECT_EQ(
        convert(-2.0, precise::log::neglog50000, precise::one),
        (50000.0 * 50000.0));
    EXPECT_DOUBLE_EQ(
        convert(2.0, precise::log::neglog50000, precise::one),
        1.0 / (50000.0 * 50000.0));
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

    EXPECT_EQ(convert(exp(1.0), precise::one, precise::log::neperA), 1.0);
    EXPECT_EQ(convert(exp(3.0), precise::one, precise::log::neperA), 3.0);
    EXPECT_EQ(convert(exp(-1.0), precise::one, precise::log::neperA), -1.0);
    EXPECT_EQ(convert(exp(-3.65), precise::one, precise::log::neperA), -3.65);
    EXPECT_EQ(convert(1.0, precise::log::neperA, precise::one), exp(1));
    EXPECT_EQ(convert(-3.685, precise::log::neperA, precise::one), exp(-3.685));
    EXPECT_EQ(convert(-2.0, precise::log::neperA, precise::one), exp(-2));
    EXPECT_EQ(convert(2.0, precise::log::neperA, precise::one), exp(2));
}

TEST(logUnits, log2)
{
    EXPECT_EQ(convert(4, precise::one, precise::log::logbase2), 2.0);
    EXPECT_EQ(convert(1024, precise::one, precise::log::logbase2), 10.0);
    EXPECT_EQ(convert(0.5, precise::one, precise::log::logbase2), -1.0);
    EXPECT_EQ(
        convert(1.0 / pow(2.0, 30), precise::one, precise::log::logbase2), -30);
    EXPECT_EQ(convert(2.0, precise::log::logbase2, precise::one), 4.0);
    EXPECT_EQ(convert(10, precise::log::logbase2, precise::one), 1024.0);
    EXPECT_EQ(convert(-2.0, precise::log::logbase2, precise::one), 0.25);
    EXPECT_EQ(
        convert(-40.0, precise::log::logbase2, precise::one),
        1.0 / pow(2.0, 40));
}

TEST(logUnits, dBNeperConversions)
{
    EXPECT_EQ(convert(1.0, precise::log::bel, precise::log::dB), 10.0);
    EXPECT_NEAR(
        convert(1.0, precise::log::bel, precise::log::neper),
        1.1512925465,
        test::tolerance);
    EXPECT_NEAR(
        convert(10.0, precise::log::dB, precise::log::neper),
        1.1512925465,
        test::tolerance);

    EXPECT_NEAR(
        convert(
            10.0,
            precise::log::dB * precise::V,
            precise::log::neper * precise::V),
        1.1512925465,
        test::tolerance);
    EXPECT_NEAR(
        convert(
            10.0,
            precise::log::dB * precise::W,
            precise::log::neper * precise::W),
        1.1512925465,
        test::tolerance);

    EXPECT_NEAR(
        convert(
            10.0,
            precise::log::dB * precise::V,
            precise::log::neperA * precise::V),
        1.1512925465,
        test::tolerance);
    EXPECT_NEAR(
        convert(
            10.0,
            precise::log::dB * precise::W,
            precise::log::neperP * precise::W),
        1.1512925465,
        test::tolerance);

    EXPECT_NEAR(
        convert(
            1.1512925465,
            precise::log::neper * precise::V,
            precise::log::dB * precise::V),
        10.0,
        test::tolerance);
    EXPECT_NEAR(
        convert(
            1.1512925465,
            precise::log::neper * precise::W,
            precise::log::dB * precise::W),
        10.0,
        test::tolerance);

    EXPECT_NEAR(
        convert(
            1.1512925465,
            precise::log::neperA * precise::V,
            precise::log::dB * precise::V),
        10.0,
        test::tolerance);
    EXPECT_NEAR(
        convert(
            1.1512925465,
            precise::log::neperP * precise::W,
            precise::log::dB * precise::W),
        10.0,
        test::tolerance);

    EXPECT_EQ(convert(10.0, precise::log::bel, precise::log::dB), 100.0);
    EXPECT_NEAR(
        convert(10.0, precise::log::bel, precise::log::neper),
        11.512925465,
        test::tolerance);
    EXPECT_NEAR(
        convert(100.0, precise::log::dB, precise::log::neper),
        11.512925465,
        test::tolerance);

    EXPECT_NEAR(
        convert(2.0, precise::log::dB, precise::log::neper),
        0.2302585093,
        test::tolerance);

    EXPECT_NEAR(
        convert(5.0, precise::log::neper, precise::log::dB),
        43.4294481903,
        test::tolerance);
    EXPECT_NEAR(
        convert(5.0, precise::log::neper, precise::log::bel),
        4.342944819,
        test::tolerance);
}

TEST(logUnits, pH)
{
    EXPECT_NEAR(
        convert(0.0025, precise::laboratory::molarity, precise::laboratory::pH),
        2.6,
        0.005);
    EXPECT_NEAR(
        convert(8.34, precise::laboratory::pH, precise::laboratory::molarity),
        4.57e-9,
        0.005);
    EXPECT_NEAR(
        convert(
            4.82e-5, precise::laboratory::molarity, precise::laboratory::pH),
        4.32,
        0.005);
}

TEST(logUnits, general)
{
    double res = convert(
        20.0,
        precise::log::dBA * precise::m / precise::s,
        precise::m / precise::s);
    EXPECT_DOUBLE_EQ(res, 10.0);
    res = convert(
        100.0,
        precise::m / precise::s,
        precise::log::dBA * precise::m / precise::s);
    EXPECT_DOUBLE_EQ(res, 40.0);

    res = convert(
        10.0,
        precise::log::dBP * precise::km / precise::hr,
        precise::m / precise::s);
    EXPECT_DOUBLE_EQ(res, 10000.0 / 3600.0);
    res = convert(
        100000.0 / 3600.0,
        precise::m / precise::s,
        precise::log::dBP * precise::km / precise::hr);
    EXPECT_DOUBLE_EQ(res, 20.0);
}

TEST(logUnits, error)
{
    EXPECT_TRUE(std::isnan(convert(-20.0, precise::one, precise::log::bel)));
    EXPECT_TRUE(std::isnan(convert(
        20.0, precise::log::dBA * precise::m / precise::s, precise::m)));
}

TEST(otherEqUnits, prismDiopter)
{
    EXPECT_NEAR(
        convert(1, precise::deg, precise::clinical::prism_diopter),
        1.75,
        0.005);
    EXPECT_NEAR(
        convert(1.75, precise::clinical::prism_diopter, precise::deg),
        1.0,
        0.005);
}

TEST(otherEqUnits, saffirSimpson)
{
    EXPECT_EQ(
        std::floor(
            convert(44.0, precise::m / precise::s, precise::special::sshws)),
        2.0);
    EXPECT_EQ(
        std::floor(convert(77.0, precise::mph, precise::special::sshws)), 1.0);

    EXPECT_EQ(
        std::floor(
            convert(268.0, precise::km / precise::hr, precise::special::sshws)),
        5.0);
    EXPECT_EQ(
        std::floor(
            convert(116.0, precise::nautical::knot, precise::special::sshws)),
        4.0);

    EXPECT_EQ(
        std::floor(convert(44.0, precise::mph, precise::special::sshws)), 0.0);
    EXPECT_EQ(
        std::floor(
            convert(56.0, precise::m / precise::s, precise::special::sshws)),
        3.0);
}

TEST(otherEqUnits, saffirSimpson2Speed)
{
    EXPECT_NEAR(
        convert(3.0, precise::special::sshws, precise::m / precise::s),
        50.0,
        1.0);
    EXPECT_NEAR(convert(2.0, precise::special::sshws, precise::mph), 96.0, 1.0);
    EXPECT_NEAR(
        convert(1.0, precise::special::sshws, precise::km / precise::hr),
        119.0,
        1.0);
    EXPECT_NEAR(
        convert(5.0, precise::special::sshws, precise::nautical::knot),
        135.0,
        1.0);
    EXPECT_NEAR(
        convert(0.5, precise::special::sshws, precise::m / precise::s),
        26.0,
        1.0);
    EXPECT_NEAR(convert(0.0, precise::special::sshws, precise::mph), 39.0, 1.0);
}

class beaufort : public ::testing::TestWithParam<std::pair<double, double>> {};

TEST_P(beaufort, beaufortTests)
{
    auto p = GetParam();
    auto bnumber = p.first;
    auto wspeed = p.second;

    auto conv = convert(wspeed, precise::mph, precise::special::beaufort);

    EXPECT_EQ(std::round(conv), std::floor(bnumber));
    EXPECT_NEAR(
        convert(conv, precise::special::beaufort, precise::mph), wspeed, 0.5);
}

static const std::vector<std::pair<double, double>> testBValues{
    {0.0, 0.0},
    {1.5, 2.0},
    {2.0, 4.0},
    {3.0, 8.0},
    {4.0, 13.0},
    {5.0, 19.0},
    {6.0, 25.0},
    {7.0, 32.0},
    {8.0, 39.0},
    {9.0, 47.0},
    {10.0, 55.0},
    {11.0, 64.0},
    {12.0, 73.0},
};

INSTANTIATE_TEST_SUITE_P(
    beaufortConversionTests,
    beaufort,
    ::testing::ValuesIn(testBValues));

TEST(otherEqUnits, saffirSimpson2Sbeaufort)
{
    EXPECT_NEAR(
        convert(12.1, precise::special::beaufort, precise::special::sshws),
        1.05,
        0.05);
    EXPECT_NEAR(
        convert(0.0, precise::special::sshws, precise::special::beaufort),
        8.0,
        0.05);  // tropical storm
}

class fujita : public ::testing::TestWithParam<std::pair<double, double>> {};

TEST_P(fujita, fujitaTests)
{
    auto p = GetParam();
    auto fnumber = p.first;
    auto wspeed = p.second;

    auto conv = convert(wspeed, precise::mph, precise::special::fujita);

    EXPECT_EQ(std::round(conv), std::floor(fnumber));
    EXPECT_NEAR(
        convert(conv, precise::special::fujita, precise::mph), wspeed, 0.5);
}

static const std::vector<std::pair<double, double>> testFValues{
    {0.0, 40.0},
    {1.0, 73.0},
    {2.0, 113},
    {3.0, 158.0},
    {4.0, 207.0},
    {5.0, 261},
};

INSTANTIATE_TEST_SUITE_P(
    fujitaConversionTests,
    fujita,
    ::testing::ValuesIn(testFValues));

TEST(otherEqUnits, saffirSimpson2Sfujita)
{
    EXPECT_NEAR(
        convert(1.0, precise::special::fujita, precise::special::sshws),
        1.00,
        0.05);
    EXPECT_NEAR(
        convert(1.0, precise::special::sshws, precise::special::fujita),
        1.0,
        0.05);
}

TEST(otherEqUnits, trits)
{
    EXPECT_NEAR(
        convert(1.0, precise::data::trit, precise::data::bit_s),
        1.58496,
        0.00001);
    EXPECT_NEAR(
        convert(6.0, precise::data::trit, precise::data::bit_s), 9.5, 0.01);
    EXPECT_NEAR(
        convert(20.19, precise::data::trit, precise::data::bit_s), 32.0, 0.01);
    EXPECT_NEAR(
        convert(40.38, precise::data::trit, precise::data::digits),
        19.27,
        0.01);

    EXPECT_NEAR(
        convert(1.58496, precise::data::bit_s, precise::data::trit),
        1.0,
        0.00001);
    EXPECT_NEAR(
        convert(9.5, precise::data::bit_s, precise::data::trit), 6.0, 0.01);
    EXPECT_NEAR(
        convert(9, precise::data::digits, precise::data::trit), 18.86, 0.01);
}

TEST(otherEqUnits, digits)
{
    EXPECT_NEAR(
        convert(12.0, precise::data::digits, precise::data::bit_s),
        39.86,
        0.01);
    EXPECT_NEAR(
        convert(6.0, precise::data::digits, precise::data::bit_s), 19.93, 0.01);
    EXPECT_NEAR(convert(1.0, precise::data::digits, precise::one), 10.0, 0.01);
}

TEST(otherEqUnits, Richter)
{
    auto conv5 = convert(
        5.0, precise::special::moment_magnitude, precise::N * precise::m);
    EXPECT_FALSE(std::isnan(conv5));

    EXPECT_NEAR(
        convert(
            conv5, precise::N * precise::m, precise::special::moment_magnitude),
        5.0,
        0.0001);

    auto conv7 = convert(
        7.0, precise::special::moment_magnitude, precise::N * precise::m);
    EXPECT_FALSE(std::isnan(conv7));

    EXPECT_NEAR(
        convert(
            conv7, precise::N * precise::m, precise::special::moment_magnitude),
        7.0,
        0.0001);

    EXPECT_NEAR(conv7 / conv5, 1000.0, 10.0);
}

TEST(otherEqUnits, momentEnergy)
{
    auto conv5 = convert(5.0, precise::special::moment_energy, precise::J);
    EXPECT_FALSE(std::isnan(conv5));

    EXPECT_NEAR(
        convert(conv5, precise::J, precise::special::moment_energy),
        5.0,
        0.0001);

    auto conv7 = convert(7.0, precise::special::moment_energy, precise::J);
    EXPECT_FALSE(std::isnan(conv7));

    EXPECT_NEAR(
        convert(conv7, precise::J, precise::special::moment_energy),
        7.0,
        0.0001);

    EXPECT_NEAR(conv7 / conv5, 1000.0, 10.0);
}

TEST(otherEqUnits, unknownEQ)
{
    auto eq20 = precise_unit(precise::custom::equation_unit(21));
    auto eq21 = precise_unit(precise::custom::equation_unit(22));
    EXPECT_EQ(convert(1.92, eq20, precise::one), 1.92);

    EXPECT_EQ(convert(1.92, precise::one, eq20), 1.92);
    auto conv7 = convert(7.0, eq21, eq20 * precise::W);
    EXPECT_TRUE(std::isnan(conv7));

    EXPECT_EQ(convert(1.927, eq20 * precise::W, eq20 * precise::W), 1.927);
}

TEST(otherEqUnits, APIgravity)
{
    auto apiG = precise::special::degreeAPI;
    // gasoline
    EXPECT_NEAR(convert(56.98, apiG, precise::g / precise::mL), 0.7508, 0.0001);
    EXPECT_NEAR(
        convert(0.750743, precise::g / precise::mL, apiG), 56.98, 0.001);
    // kerosene
    EXPECT_NEAR(
        convert(50.9, apiG, precise::g / precise::mL), 0.775768, 0.0001);
    EXPECT_NEAR(convert(0.775768, precise::g / precise::mL, apiG), 50.9, 0.001);
}

TEST(otherEqUnits, degreeBaume)
{
    auto degBL = precise::special::degreeBaumeLight;
    EXPECT_NEAR(convert(10, degBL, precise::g / precise::mL), 1.0000, 0.0001);
    EXPECT_NEAR(convert(1.0000, precise::g / precise::mL, degBL), 10.0, 0.001);
    EXPECT_NEAR(convert(26.075, degBL, precise::g / precise::mL), 0.897, 0.001);
    EXPECT_NEAR(convert(0.897, precise::g / precise::mL, degBL), 26.075, 0.001);
    EXPECT_NEAR(convert(43.91, degBL, precise::g / precise::mL), 0.805, 0.001);
    EXPECT_NEAR(convert(0.805, precise::g / precise::mL, degBL), 43.91, 0.01);

    auto degBH = precise::special::degreeBaumeHeavy;
    EXPECT_NEAR(convert(0, degBH, precise::g / precise::mL), 1.0000, 0.0001);
    EXPECT_NEAR(convert(1.0000, precise::g / precise::mL, degBH), 0.0, 0.001);
    EXPECT_NEAR(convert(15.0, degBH, precise::g / precise::mL), 1.115, 0.001);
    EXPECT_NEAR(convert(1.115, precise::g / precise::mL, degBH), 14.955, 0.001);
    EXPECT_NEAR(convert(69.0, degBH, precise::g / precise::mL), 1.908, 0.001);
    EXPECT_NEAR(convert(1.908, precise::g / precise::mL, degBH), 69.0, 0.005);
}

#ifndef UNITS_HEADER_ONLY

TEST(otherEqUnits, strings)
{
    precise_unit eq18(1.36, precise::custom::equation_unit(18));

    auto str = units::to_string(eq18);
    EXPECT_EQ(str, "1.36*EQXUN[18]");

    auto nunit = unit_from_string(str);
    EXPECT_EQ(nunit, eq18);
}

#endif
