/*
Copyright (c) 2019-2025,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "test.hpp"
#include "units/units.hpp"

static const double neg_forty_C = -40.0;
static const double neg_forty_C_in_F = -40.0;
static const double neg_forty_C_in_K = 233.15;
static const double zero_C = 0.0;
static const double zero_C_in_F = 32.0;
static const double zero_C_in_K = 273.15;
static const double five_C = 5.0;
static const double five_C_in_F = 41.0;
static const double five_C_in_K = 278.15;
static const double hundred_C = 100.0;
static const double hundred_C_in_F = 212.0;
static const double hundred_C_in_K = 373.15;

TEST(Temperature, ConversionsUnit)
{
    using namespace units;
    EXPECT_NEAR(
        convert(neg_forty_C, degC, degF),
        neg_forty_C_in_F,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(neg_forty_C, degC, K),
        neg_forty_C_in_K,
        test::precise_tolerance);

    EXPECT_NEAR(
        convert(neg_forty_C_in_F, degF, degC),
        neg_forty_C,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(neg_forty_C_in_K, K, degC),
        neg_forty_C,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(neg_forty_C_in_K, K, degF),
        neg_forty_C_in_F,
        test::precise_tolerance);

    EXPECT_NEAR(
        convert(zero_C, degC, degF), zero_C_in_F, test::precise_tolerance);
    EXPECT_NEAR(convert(zero_C, degC, K), zero_C_in_K, test::precise_tolerance);

    EXPECT_NEAR(
        convert(zero_C_in_F, degF, degC), zero_C, test::precise_tolerance);
    EXPECT_NEAR(convert(zero_C_in_K, K, degC), zero_C, test::precise_tolerance);
    EXPECT_NEAR(
        convert(zero_C_in_K, K, degF), zero_C_in_F, test::precise_tolerance);

    EXPECT_NEAR(
        convert(five_C, degC, degF), five_C_in_F, test::precise_tolerance);
    EXPECT_NEAR(convert(five_C, degC, K), five_C_in_K, test::precise_tolerance);

    EXPECT_NEAR(
        convert(five_C_in_F, degF, degC), five_C, test::precise_tolerance);
    EXPECT_NEAR(convert(five_C_in_K, K, degC), five_C, test::precise_tolerance);
    EXPECT_NEAR(
        convert(five_C_in_K, K, degF), five_C_in_F, test::precise_tolerance);

    EXPECT_NEAR(
        convert(hundred_C, degC, degF),
        hundred_C_in_F,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(hundred_C, degC, K), hundred_C_in_K, test::precise_tolerance);

    EXPECT_NEAR(
        convert(hundred_C_in_F, degF, degC),
        hundred_C,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(hundred_C_in_K, K, degC), hundred_C, test::precise_tolerance);
    EXPECT_NEAR(
        convert(hundred_C_in_K, K, degF),
        hundred_C_in_F,
        test::precise_tolerance);
}

TEST(Temperature, ConversionsUnitPrecise)
{
    using namespace units::precise;

    EXPECT_NEAR(
        convert(neg_forty_C, degC, degF),
        neg_forty_C_in_F,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(neg_forty_C, degC, K),
        neg_forty_C_in_K,
        test::precise_tolerance);

    EXPECT_NEAR(
        convert(neg_forty_C_in_F, degF, degC),
        neg_forty_C,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(neg_forty_C_in_K, K, degC),
        neg_forty_C,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(neg_forty_C_in_K, K, degF),
        neg_forty_C_in_F,
        test::precise_tolerance);

    EXPECT_NEAR(
        convert(zero_C, degC, degF), zero_C_in_F, test::precise_tolerance);
    EXPECT_NEAR(convert(zero_C, degC, K), zero_C_in_K, test::precise_tolerance);

    EXPECT_NEAR(
        convert(zero_C_in_F, degF, degC), zero_C, test::precise_tolerance);
    EXPECT_NEAR(convert(zero_C_in_K, K, degC), zero_C, test::precise_tolerance);
    EXPECT_NEAR(
        convert(zero_C_in_K, K, degF), zero_C_in_F, test::precise_tolerance);

    EXPECT_NEAR(
        convert(five_C, degC, degF), five_C_in_F, test::precise_tolerance);
    EXPECT_NEAR(convert(five_C, degC, K), five_C_in_K, test::precise_tolerance);

    EXPECT_NEAR(
        convert(five_C_in_F, degF, degC), five_C, test::precise_tolerance);
    EXPECT_NEAR(convert(five_C_in_K, K, degC), five_C, test::precise_tolerance);
    EXPECT_NEAR(
        convert(five_C_in_K, K, degF), five_C_in_F, test::precise_tolerance);

    EXPECT_NEAR(
        convert(hundred_C, degC, degF),
        hundred_C_in_F,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(hundred_C, degC, K), hundred_C_in_K, test::precise_tolerance);

    EXPECT_NEAR(
        convert(hundred_C_in_F, degF, degC),
        hundred_C,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(hundred_C_in_K, K, degC), hundred_C, test::precise_tolerance);
    EXPECT_NEAR(
        convert(hundred_C_in_K, K, degF),
        hundred_C_in_F,
        test::precise_tolerance);
}

TEST(Temperature, ConversionsUnitPreciseUnit2Precise)
{
    using namespace units;
    EXPECT_NEAR(
        convert(neg_forty_C, degC, precise::degF),
        neg_forty_C_in_F,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(neg_forty_C, degC, precise::K),
        neg_forty_C_in_K,
        test::precise_tolerance);

    EXPECT_NEAR(
        convert(neg_forty_C_in_F, degF, precise::degC),
        neg_forty_C,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(neg_forty_C_in_K, K, precise::degC),
        neg_forty_C,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(neg_forty_C_in_K, K, precise::degF),
        neg_forty_C_in_F,
        test::precise_tolerance);

    EXPECT_NEAR(
        convert(zero_C, degC, precise::degF),
        zero_C_in_F,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(zero_C, degC, precise::K),
        zero_C_in_K,
        test::precise_tolerance);

    EXPECT_NEAR(
        convert(zero_C_in_F, degF, precise::degC),
        zero_C,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(zero_C_in_K, K, precise::degC),
        zero_C,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(zero_C_in_K, K, precise::degF),
        zero_C_in_F,
        test::precise_tolerance);

    EXPECT_NEAR(
        convert(five_C, degC, precise::degF),
        five_C_in_F,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(five_C, degC, precise::K),
        five_C_in_K,
        test::precise_tolerance);

    EXPECT_NEAR(
        convert(five_C_in_F, degF, precise::degC),
        five_C,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(five_C_in_K, K, precise::degC),
        five_C,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(five_C_in_K, K, precise::degF),
        five_C_in_F,
        test::precise_tolerance);

    EXPECT_NEAR(
        convert(hundred_C, degC, precise::degF),
        hundred_C_in_F,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(hundred_C, degC, precise::K),
        hundred_C_in_K,
        test::precise_tolerance);

    EXPECT_NEAR(
        convert(hundred_C_in_F, degF, precise::degC),
        hundred_C,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(hundred_C_in_K, K, precise::degC),
        hundred_C,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(hundred_C_in_K, K, precise::degF),
        hundred_C_in_F,
        test::precise_tolerance);
}

TEST(Temperature, ConversionsUnitPrecise2Unit)
{
    using namespace units;
    EXPECT_NEAR(
        convert(neg_forty_C, precise::degC, degF),
        neg_forty_C_in_F,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(neg_forty_C, precise::degC, K),
        neg_forty_C_in_K,
        test::precise_tolerance);

    EXPECT_NEAR(
        convert(neg_forty_C_in_F, precise::degF, degC),
        neg_forty_C,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(neg_forty_C_in_K, precise::K, degC),
        neg_forty_C,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(neg_forty_C_in_K, precise::K, degF),
        neg_forty_C_in_F,
        test::precise_tolerance);

    EXPECT_NEAR(
        convert(zero_C, precise::degC, degF),
        zero_C_in_F,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(zero_C, precise::degC, K),
        zero_C_in_K,
        test::precise_tolerance);

    EXPECT_NEAR(
        convert(zero_C_in_F, precise::degF, degC),
        zero_C,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(zero_C_in_K, precise::K, degC),
        zero_C,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(zero_C_in_K, precise::K, degF),
        zero_C_in_F,
        test::precise_tolerance);

    EXPECT_NEAR(
        convert(five_C, precise::degC, degF),
        five_C_in_F,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(five_C, precise::degC, K),
        five_C_in_K,
        test::precise_tolerance);

    EXPECT_NEAR(
        convert(five_C_in_F, precise::degF, degC),
        five_C,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(five_C_in_K, precise::K, degC),
        five_C,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(five_C_in_K, precise::K, degF),
        five_C_in_F,
        test::precise_tolerance);

    EXPECT_NEAR(
        convert(hundred_C, precise::degC, degF),
        hundred_C_in_F,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(hundred_C, precise::degC, K),
        hundred_C_in_K,
        test::precise_tolerance);

    EXPECT_NEAR(
        convert(hundred_C_in_F, precise::degF, degC),
        hundred_C,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(hundred_C_in_K, precise::K, degC),
        hundred_C,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(hundred_C_in_K, precise::K, degF),
        hundred_C_in_F,
        test::precise_tolerance);
}
TEST(Temperature, Detection)
{
    using namespace units;
    EXPECT_TRUE(is_temperature(degF));
    EXPECT_TRUE(is_temperature(precise::degF));
    EXPECT_TRUE(is_temperature(degC));
    EXPECT_TRUE(is_temperature(precise::degC));
    EXPECT_FALSE(is_temperature(K));
    EXPECT_FALSE(is_temperature(precise::K));

    EXPECT_FALSE(is_temperature(degF.inv()));
    EXPECT_FALSE(is_temperature(precise::degF.inv()));
    // this is correct since 1 C != 1 K in absolute temperature
    // but per deg C is equal to per Kelvin so this is the desired behavior
    EXPECT_FALSE(degC == K);
    EXPECT_FALSE(precise::degC == precise::K);
    EXPECT_TRUE(precise::degC.inv().inv() == precise::degC);
}

TEST(Temperature, Rankine)
{
    using namespace units;
    auto rankine = precise::temperature::rankine;

    EXPECT_FALSE(is_temperature(rankine));
    EXPECT_NEAR(convert(0.0, K, rankine), 0.0, test::tolerance);
    EXPECT_NEAR(
        convert(459.67, rankine, precise::degC), -17.7777777, test::tolerance);
    EXPECT_NEAR(convert(0.0, precise::degC, rankine), 491.67, test::tolerance);
    EXPECT_NEAR(convert(100, precise::degC, rankine), 671.67, test::tolerance);
    EXPECT_NEAR(convert(0, rankine, degF), -459.67, test::tolerance);
}

TEST(Temperature, Reaumur)
{
    using namespace units;
    auto re = precise::temperature::reaumur;

    EXPECT_TRUE(is_temperature(re));
    EXPECT_NEAR(convert(313.15, K, re), 32.0, test::tolerance);
    EXPECT_NEAR(convert(32.0, re, precise::degC), 40.0, test::tolerance);
    EXPECT_NEAR(convert(40.0, precise::degC, re), 32.0, test::tolerance);
    EXPECT_NEAR(
        convert(563.67, precise::temperature::rankine, re),
        32.0,
        test::tolerance);
    EXPECT_NEAR(convert(32.0, re, degF), 104.0, test::tolerance);
}

TEST(TimeConversions, Correctness)
{
    using namespace units;
    double one_min_in_sec = 60.0;
    double one_hour_in_min = 60.0;
    double one_day_in_hour = 24.0;
    double one_week_in_day = 7.0;
    double one_day_in_sec = 86400.0;

    EXPECT_NEAR(convert(minute, s), one_min_in_sec, test::tolerance);
    EXPECT_NEAR(convert(hr, minute), one_hour_in_min, test::tolerance);
    EXPECT_NEAR(
        convert(unit_cast(precise::time::day), hr),
        one_day_in_hour,
        test::tolerance);
    EXPECT_NEAR(
        convert(unit_cast(precise::time::week), unit_cast(precise::time::day)),
        one_week_in_day,
        test::tolerance);
    EXPECT_NEAR(
        convert(unit_cast(precise::time::day), s),
        one_day_in_sec,
        test::tolerance);
    EXPECT_NEAR(
        convert(unit_cast(precise::time::yr), unit_cast(precise::time::day)),
        365.0,
        test::tolerance);
}

TEST(TimeConversionsPrecise, Correctness)
{
    using namespace units::precise::time;
    double one_min_in_sec = 60.0;
    double one_hour_in_min = 60.0;
    double one_day_in_hour = 24.0;
    double one_week_in_day = 7.0;
    double one_day_in_sec = 86400.0;

    EXPECT_NEAR(
        convert(minute, units::precise::s),
        one_min_in_sec,
        test::precise_tolerance);
    EXPECT_NEAR(convert(hr, minute), one_hour_in_min, test::precise_tolerance);
    EXPECT_NEAR(convert(day, hr), one_day_in_hour, test::precise_tolerance);
    EXPECT_NEAR(convert(week, day), one_week_in_day, test::precise_tolerance);
    EXPECT_NEAR(
        convert(day, units::precise::s),
        one_day_in_sec,
        test::precise_tolerance * one_day_in_sec);
    EXPECT_NEAR(convert(yr, day), 365.0, test::precise_tolerance * 365.0);
}

TEST(countConversions, rotations)
{
    using namespace units;
    EXPECT_NEAR(convert(Hz, rad / s), 2.0 * constants::pi, test::tolerance);
    EXPECT_NEAR(
        convert(rad, one), 1.0 / (2.0 * constants::pi), test::tolerance);
    EXPECT_NEAR(
        convert(2.0 * constants::pi, rad / s, Hz), 1.0, test::tolerance);
    EXPECT_NEAR(
        convert(1.0 / (2.0 * constants::pi), one, rad), 1.0, test::tolerance);
    // failed conversion
    EXPECT_TRUE(std::isnan(convert(rad.pow(2), count)));
    EXPECT_TRUE(std::isnan(convert(rad.pow(3), one)));
}

TEST(countConversions, mols)
{
    using namespace units;
    EXPECT_NEAR(convert(mol, count), 6.02214076e23, test::precise_tolerance);
    EXPECT_NEAR(
        convert(mol.inv(), one), 1.0 / 6.02214076e23, test::precise_tolerance);
    EXPECT_NEAR(
        convert(count, mol), 1.0 / 6.02214076e23, test::precise_tolerance);
    EXPECT_NEAR(
        convert(one, mol.inv()), 6.02214076e23, test::precise_tolerance);
    // failed conversion
    EXPECT_TRUE(std::isnan(convert(mol.pow(2), count)));
    EXPECT_TRUE(std::isnan(convert(mol.pow(2), count.pow(2))));
}

TEST(ImperialTranslations, Correctness)
{
    using namespace units;
    double one_m = 1;
    double one_m_in_ft = 1.0 / 0.3048;

    double one_ft_in_in = 12.0;
    double one_yd_in_ft = 3.0;
    double one_mi_in_yd = 1760.0;
    EXPECT_NEAR(convert(one_m, m, ft), one_m_in_ft, test::tolerance);
    EXPECT_NEAR(convert(ft, m), 0.3048, test::tolerance);
    EXPECT_NEAR(
        convert(one_m, precise::m, precise::ft),
        one_m_in_ft,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(precise::ft, precise::m), 0.3048, test::precise_tolerance);

    EXPECT_NEAR(convert(one_ft_in_in, in, ft), one_m, test::tolerance);
    EXPECT_NEAR(convert(one_yd_in_ft, ft, yd), one_m, test::tolerance);
    EXPECT_NEAR(convert(one_mi_in_yd, yd, mile), one_m, test::tolerance);

    EXPECT_NEAR(
        convert(mile, yd), one_mi_in_yd, test::tolerance * one_mi_in_yd);
    EXPECT_NEAR(convert(yd, foot), one_yd_in_ft, test::tolerance);
    EXPECT_NEAR(convert(ft, in), one_ft_in_in, test::tolerance);
    EXPECT_NEAR(convert(in, cm), 2.54, test::tolerance);

    EXPECT_NEAR(
        convert(precise::mile, precise::yd),
        one_mi_in_yd,
        test::precise_tolerance * one_mi_in_yd);
    EXPECT_NEAR(
        convert(precise::yd, precise::ft),
        one_yd_in_ft,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(precise::ft, precise::in),
        one_ft_in_in,
        test::precise_tolerance);
    EXPECT_NEAR(
        convert(precise::in, precise::cm), 2.54, test::precise_tolerance);
}

TEST(ImperialTranslations, matching)
{
    using namespace units;
    auto ps = precise::lbf / precise::in.pow(2);
    EXPECT_TRUE(ps == precise::pressure::psi);
}

TEST(SIPrefix, Correctness)
{
    using namespace units;
    EXPECT_EQ(convert(precise::kilo, one), 1000.0);
    EXPECT_EQ(convert(precise::milli, one), 0.001);
    EXPECT_EQ(convert(precise::mega, count), 1e6);
    EXPECT_EQ(convert(precise::micro, count), 1e-6);
    EXPECT_EQ(convert(precise::nano, count), 1e-9);
    EXPECT_EQ(convert(precise::pico, one), 1e-12);
    EXPECT_EQ(convert(precise::femto, one), 1e-15);
    EXPECT_EQ(convert(precise::atto, one), 1e-18);
    EXPECT_NEAR(convert(one, precise::nano), 1e9, test::tolerance);
    EXPECT_EQ(convert(one, precise::giga), 1e-9);
    EXPECT_EQ(convert(precise::giga, one), 1e9);
    EXPECT_EQ(convert(precise::tera, one), 1e12);
    EXPECT_EQ(convert(precise::peta, one), 1e15);
    EXPECT_EQ(convert(precise::exa, one), 1e18);
}

TEST(SIPrefix, conversions)
{
    using namespace units;
    EXPECT_EQ(convert(precise::kilo, precise::mega), 1e-3);
    EXPECT_EQ(convert(precise::mega, precise::milli), 1e9);
    EXPECT_EQ(convert(precise::giga, precise::mega), 1e3);
    EXPECT_NEAR(
        convert(precise::micro, precise::nano), 1e3, test::precise_tolerance);

    EXPECT_EQ(convert(precise::mega, precise::kilo), 1e3);
    EXPECT_EQ(convert(precise::milli, precise::giga), 1e-12);
    EXPECT_EQ(convert(precise::mega, precise::giga), 1e-3);
    EXPECT_NEAR(
        convert(precise::nano, precise::micro), 1e-3, test::precise_tolerance);
}

TEST(UnitDefinitions, SIextended)
{
    using namespace units;

    unit s_2 = s * s;
    unit m_2 = m * m;

    // TODO(PT) check more of the possibilities
    // https://en.wikipedia.org/wiki/SI_derived_unit#Derived_units_with_special_names
    EXPECT_TRUE((one / s).has_same_base(Hz));
    EXPECT_TRUE((kg * m / s_2).has_same_base(N));
    EXPECT_TRUE((N / m_2).has_same_base(Pa));
    EXPECT_TRUE((N * m).has_same_base(J));
    EXPECT_TRUE((J / s).has_same_base(W));
    EXPECT_TRUE((s * A).has_same_base(C));
    EXPECT_TRUE((W / A).has_same_base(V));
    EXPECT_TRUE((C / V).has_same_base(F));
    EXPECT_TRUE((one / S).has_same_base(ohm));
    EXPECT_TRUE((one / ohm).has_same_base(S));
    EXPECT_TRUE((ohm.inv()).has_same_base(S));
    EXPECT_TRUE((J / A).has_same_base(Wb));
    EXPECT_TRUE((V * s / m_2).has_same_base(T));
    EXPECT_TRUE((V * s / A).has_same_base(H));
    EXPECT_TRUE((degC).has_same_base(K));
    EXPECT_TRUE((lm).equivalent_non_counting(cd));
    EXPECT_TRUE((lm / m_2).has_same_base(lx));
    EXPECT_TRUE((count / s).has_same_base(Bq));
    EXPECT_TRUE((J / kg).has_same_base(Gy));
    EXPECT_TRUE((J / kg).has_same_base(Sv));
    EXPECT_TRUE((mol / s).has_same_base(kat));
    EXPECT_TRUE((V / m).has_same_base(mV / yd));
}

TEST(UnitDefinitions, SIextendedPrecise)
{
    using namespace units::precise;

    auto s_2 = s * s;
    auto m_2 = m * m;

    // https://en.wikipedia.org/wiki/SI_derived_unit#Derived_units_with_special_names
    EXPECT_TRUE((one / s).has_same_base(Hz));
    EXPECT_TRUE((kg * m / s_2).has_same_base(N));
    EXPECT_TRUE((N / m_2).has_same_base(Pa));
    EXPECT_TRUE((N * m).has_same_base(J));
    EXPECT_TRUE((J / s).has_same_base(W));
    EXPECT_TRUE((s * A).has_same_base(C));
    EXPECT_TRUE((W / A).has_same_base(V));
    EXPECT_TRUE((C / V).has_same_base(F));
    EXPECT_TRUE((one / S).has_same_base(ohm));
    EXPECT_TRUE((one / ohm).has_same_base(S));
    EXPECT_TRUE((ohm.inv()).has_same_base(S));
    EXPECT_TRUE((J / A).has_same_base(Wb));
    EXPECT_TRUE((V * s / m_2).has_same_base(T));
    EXPECT_TRUE((V * s / A).has_same_base(H));
    EXPECT_TRUE((degC).has_same_base(K));
    EXPECT_TRUE((lm).equivalent_non_counting(cd));
    EXPECT_TRUE((lm / m_2).has_same_base(lx));
    EXPECT_TRUE((count / s).has_same_base(Bq));
    EXPECT_TRUE((J / kg).has_same_base(Gy));
    EXPECT_TRUE((J / kg).has_same_base(Sv));
    EXPECT_TRUE((mol / s).has_same_base(kat));
    EXPECT_TRUE((V / m).has_same_base(electrical::mV / yd));
    EXPECT_TRUE((mol / L).has_same_base(laboratory::pH));
}

TEST(UnitDefinitions, conversions)
{
    using namespace units;
    auto field = V / ft;
    double res = convert(field, V / m);

    EXPECT_NEAR(res, 3.28084, test::tolerance);
}

TEST(UnitDefinitions, inversionConversion)
{
    using namespace units;
    EXPECT_NEAR(convert(0.5, m, m.inv()), 2.0, test::tolerance);

    EXPECT_NEAR(convert(10, Hz, s), 0.1, test::tolerance);

    EXPECT_FLOAT_EQ(convert(1.0, kilo * Hz, milli * s), 1.0);
    EXPECT_FLOAT_EQ(convert(1.0, milli * s, Hz), 1000.0);
    EXPECT_FLOAT_EQ(convert(50.0, micro * s, kilo * Hz), 20.0);
    EXPECT_FLOAT_EQ(convert(20000, kilo * Hz, nano * s), 50.0);
}

TEST(quickConvert, simple)
{
    using namespace units;
    auto u1 = precise::cm;
    auto u2 = precise::in;
    EXPECT_EQ(quick_convert(u2, u1), 2.54);
    EXPECT_EQ(quick_convert(2.0, u2, u1), 2.0 * 2.54);
}

TEST(quickConvert, invalid)
{
    using namespace units;
    auto u1 = precise::cm;
    auto u2 = precise::lb;
    EXPECT_TRUE(std::isnan(quick_convert(u2, u1)));
    u1 = precise::cm * precise::log::bel;
    u2 = precise::cm * precise::log::bel;
    EXPECT_TRUE(std::isnan(quick_convert(u2, u1)));
}

TEST(quickConvert, constConstexpr)
{
    using namespace units;
    static_assert(
        quick_convert(precise::in, precise::cm) == 2.54,
        "results of quick_convert not correct");
    static_assert(
        quick_convert(2.0, precise::in, precise::cm) == 2.0 * 2.54,
        "results of quick_convert 2 not correct");
}

TEST(natCasUnits, psig)
{
    using namespace units;
    double val = convert(15.2, precise::pressure::psi, precise::pressure::psig);
    EXPECT_NEAR(val, 0.5, 0.01);

    val = convert(2.0, precise::pressure::atm, precise::pressure::psig, 14.8);
    EXPECT_NEAR(val, 14.6, 0.01);

    val = convert(0.5, precise::pressure::psig, precise::pressure::psi);
    EXPECT_NEAR(val, 15.2, 0.01);

    val = convert(14.6, precise::pressure::psig, precise::pressure::atm, 14.8);
    EXPECT_NEAR(val, 2.0, 0.01);

    val = convert(
        10,
        precise_unit(2.0, precise::pressure::psig),
        precise::pressure::psig);
    EXPECT_NEAR(val, 20.0, 0.01);
}

TEST(invalidConversions, invalid)
{
    using namespace units;
    double val = detail::extraValidConversions(2.3, precise::m, precise::lb);
    EXPECT_TRUE(std::isnan(val));
}

TEST(UnitDefinitions, mass2weight)
{
    using namespace units;
    EXPECT_NEAR(convert(1.0, N, kg), 0.101971621, test::tolerance);
    EXPECT_NEAR(convert(1.0, kg, N), constants::g0.value(), test::tolerance);

    EXPECT_NEAR(convert(1.0, precise::lbf, kg), 0.45359237, test::tolerance);
}

TEST(UnitDefinitions, unusualTemperature)
{
    using namespace units;
    precise_unit kC = precise::kilo * precise::temperature::degC;
    EXPECT_NEAR(convert(0.1, kC, precise::temperature::degF), 212.0, 0.1);
    EXPECT_NEAR(convert(212, precise::temperature::degF, kC), 0.1, 0.0001);

    EXPECT_NEAR(
        convert(3.0, unit_from_string("gas mark"), precise::temperature::degF),
        325.0,
        1.0);
}
