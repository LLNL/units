/*
Copyright (c) 2019-2025,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "test.hpp"
#include "units/unit_definitions.hpp"
#include "units/units_decl.hpp"
#include "units/units_util.hpp"

#include <limits>
#include <memory>

using namespace units;

TEST(unitOps, maxNeg)
{
    EXPECT_EQ(units::detail::maxNeg(4), -8);
    EXPECT_EQ(units::detail::maxNeg(3), -4);
    EXPECT_EQ(units::detail::maxNeg(2), -2);
    static_assert(
        units::detail::maxNeg(4) == -8, "maxNeg not computing correct result");
}

TEST(unitOps, Simple)
{
    EXPECT_EQ(m, m);
    EXPECT_EQ(m * m, m * m);
    EXPECT_NE(m * m, s * s);
    EXPECT_EQ(V / km, mV / m);
    static_assert(
        (V / km).is_exactly_the_same(mV / m), "units not static equivalent");
}

TEST(unitOps, hash)
{
    auto h1 = std::hash<unit>()(N);
    auto u2 = one / N;
    auto h2 = std::hash<unit>()(u2.inv());
    EXPECT_EQ(h1, h2);
}

TEST(unitOps, Inv)
{
    EXPECT_EQ(m.inv(), one / m);
    static_assert(
        (m.inv()).is_exactly_the_same(one / m),
        "inverse units not static equivalent");
    EXPECT_EQ(m.inv().inv(), m);
    EXPECT_EQ(N.inv(), one / N);

    EXPECT_EQ(gal.inv().inv(), gal);
}

TEST(unitOps, MultipleOps)
{
    auto u1 = kW / gal;
    auto u2 = u1 / kW;
    auto u3 = u2.inv();
    EXPECT_EQ(u3, gal);
}

TEST(unitOps, power)
{
    auto m2 = m.pow(2);
    EXPECT_EQ(m * m, m2);
    auto m4 = pow(m, 4);  // use the free function form
    EXPECT_EQ(m * m * m * m, m4);
    auto m4_b = m2.pow(2);
    EXPECT_EQ(m4_b, m * m * m * m);
    EXPECT_EQ(m4_b, m2 * m2);

    EXPECT_EQ(m.inv(), m.pow(-1));
    EXPECT_EQ(m.inv().inv(), m.pow(-1).pow(-1));
}

#ifndef UNITS_HEADER_ONLY
TEST(unitOps, root)
{
    EXPECT_EQ(root(m, 0), one);
    auto m1 = m.pow(1);
    EXPECT_EQ(m, root(m1, 1));
    EXPECT_EQ(m.inv(), root(m1, -1));
    auto m2 = pow(m, 2);
    EXPECT_EQ(m, root(m2, 2));

    EXPECT_EQ(m, sqrt(m2));

    auto m4 = m.pow(4);
    EXPECT_EQ(m * m, root(m4, 2));
    EXPECT_EQ(m, root(m4, 4));

    auto ft1 = ft.pow(1);
    EXPECT_EQ(ft, root(ft1, 1));
    EXPECT_EQ(ft.inv(), root(ft1, -1));

    auto ft2 = ft.pow(2);
    EXPECT_EQ(ft, root(ft2, 2));
    EXPECT_EQ(ft.inv(), root(ft2, -2));
    auto ft3 = ft.pow(3);
    EXPECT_EQ(ft, root(ft3, 3));
    EXPECT_EQ(ft.inv(), root(ft3, -3));
    auto ft4 = ft.pow(4);
    EXPECT_EQ(ft * ft, root(ft4, 2));
    EXPECT_EQ(ft, root(ft4, 4));
    EXPECT_EQ(ft.inv(), root(ft4, -4));

    auto ft5 = ft.pow(5);
    EXPECT_EQ(ft, root(ft5, 5));
    EXPECT_EQ(ft.inv(), root(ft5, -5));

    EXPECT_EQ(root(unit(-4.5, m), 2), error);
}

TEST(unitOps, rootPow1)
{
    EXPECT_EQ(root(count, 1), count);
    EXPECT_EQ(root(cd, 1), cd);
    EXPECT_EQ(root(mol, 1), mol);
    EXPECT_EQ(root(currency, 1), currency);

    EXPECT_EQ(root(count, -1), count.inv());
    EXPECT_EQ(root(cd, -1), cd.inv());
    EXPECT_EQ(root(mol, -1), mol.inv());
    EXPECT_EQ(root(currency, -1), currency.inv());
}

TEST(unitOps, wideRoots)
{
    if (units::detail::bitwidth::base_size > 4) {
        EXPECT_EQ(sqrt(mol * mol), mol);
        EXPECT_EQ(sqrt(count * count), count);
    }
}

#endif

TEST(unitOps, nan)
{
    EXPECT_TRUE(isnan(invalid));
    EXPECT_FALSE(isnan(defunit));
    auto zunit = unit(0.0, m);
    auto zunit2 = unit(0.0, kg);
    auto nunit = zunit2 / zunit;
    EXPECT_TRUE(isnan(nunit));
    EXPECT_TRUE(isnan(unit_cast(precise::invalid)));
}

TEST(unitOps, inf)
{
    EXPECT_FALSE(isinf(invalid));
    EXPECT_FALSE(isinf(defunit));
    EXPECT_FALSE(isinf(V));
    auto zunit = unit(0.0, m);
    auto nunit = kg / zunit;
    EXPECT_TRUE(isinf(nunit));
    EXPECT_TRUE(isinf(unit_cast(precise_unit(nunit))));
    EXPECT_TRUE(isinf(unit(std::numeric_limits<double>::infinity(), m)));
}

TEST(unitOps, valid)
{
    EXPECT_FALSE(is_valid(invalid));
    EXPECT_TRUE(is_valid(defunit));
    EXPECT_TRUE(is_valid(V));
    EXPECT_TRUE(is_valid(unit(std::numeric_limits<double>::quiet_NaN(), m)));
    EXPECT_FALSE(is_valid(
        unit(std::numeric_limits<double>::quiet_NaN(), invalid.base_units())));
    EXPECT_FALSE(is_valid(unit(
        std::numeric_limits<double>::signaling_NaN(), invalid.base_units())));
}

TEST(unitOps, normal)
{
    EXPECT_FALSE(isnormal(invalid));
    EXPECT_FALSE(isnormal(defunit));
    EXPECT_FALSE(isnormal(error));
    EXPECT_TRUE(isnormal(V));
    auto zunit = unit(0.0, m);
    auto nunit = kg / zunit;
    EXPECT_FALSE(isnormal(nunit));
    EXPECT_FALSE(isnormal(unit_cast(precise_unit(nunit))));
    EXPECT_FALSE(isnormal(unit(std::numeric_limits<double>::infinity(), m)));
    EXPECT_FALSE(isnormal(unit(-std::numeric_limits<double>::infinity(), m)));
    auto zunit2 = unit(0.0, kg);
    auto nunit2 = zunit2 / zunit;
    EXPECT_FALSE(isnormal(nunit2));
    EXPECT_FALSE(isnormal(unit(-0.25, kg)));
    EXPECT_FALSE(isnormal(unit(1.4e-42, kg * m)));
    EXPECT_TRUE(isnormal(unit(1.4e-36, kg * m)));
    EXPECT_FALSE(isnormal(zunit2));
    EXPECT_FALSE(isnormal(unit_cast(precise::invalid)));

    EXPECT_TRUE(isnormal(m));
    EXPECT_TRUE(isnormal(m * milli));
}

TEST(unitOps, error)
{
    EXPECT_TRUE(is_error(invalid));
    EXPECT_FALSE(is_error(defunit));
    EXPECT_FALSE(is_error(one));
    EXPECT_TRUE(is_error(error));
    EXPECT_FALSE(is_error(V));
    EXPECT_TRUE(is_error(unit(constants::invalid_conversion, V)));
}

TEST(unitOps, finite)
{
    EXPECT_FALSE(isfinite(invalid));
    EXPECT_TRUE(isfinite(defunit));
    EXPECT_FALSE(isfinite(infinite));
    EXPECT_TRUE(isfinite(error));
    EXPECT_TRUE(isfinite(V));
    EXPECT_FALSE(isfinite(unit(constants::infinity, V)));
}

TEST(unitOps, cast)
{
    EXPECT_EQ(ft, unit_cast(precise::ft));
    EXPECT_EQ(gal, unit_cast(precise::gal));
    EXPECT_TRUE(is_unit_cast_lossless(precise::m));
    EXPECT_FALSE(is_unit_cast_lossless(precise::gal));
}

TEST(unitOps, assignment)
{
    unit U1;

    EXPECT_NE(U1, ft);
    U1 = ft;
    EXPECT_EQ(U1, ft);
    auto U2 = std::make_shared<unit>();
    EXPECT_NE(*U2, mile);
    *U2 = mile;
    EXPECT_EQ(*U2, mile);
}

TEST(unitOps, equality1)
{
    int eqFailPos = 0;
    int eqFailNeg = 0;
    double start = 1.0;
    while (start < (1.0 + 2e-6)) {
        double diff = 0.0;
        while (diff < 5e-7) {
            auto u1 = unit(start, V);
            auto u2 = unit(start + diff, V);
            auto u3 = unit(start - diff, V);
            if (u1 != u2) {
                ++eqFailPos;
            }
            if (u1 != u3) {
                ++eqFailNeg;
            }
            diff += 1e-9;
        }
        start += 1e-9;
    }
    EXPECT_EQ(eqFailPos, 0);
    EXPECT_EQ(eqFailNeg, 0);
}

TEST(unitOps, flags)
{
    auto e1 = error;
    EXPECT_TRUE(e1.has_i_flag());
    EXPECT_TRUE(e1.has_e_flag());
    static_assert(
        error.has_i_flag(),
        "i flag should be constexpr and allowable in compile time constants");
    static_assert(
        error.has_e_flag(),
        "e flag should be constexpr and allowable in compile time constants");

    e1.clear_flags();
    EXPECT_FALSE(e1.has_i_flag());
    EXPECT_FALSE(e1.has_e_flag());
}

TEST(unitOps, equivalency)
{
    unit m0;
    unit m1 = ft;
    m0 = m1;
    EXPECT_TRUE(m1.is_exactly_the_same(m0));
    EXPECT_TRUE(m0.is_exactly_the_same(m1));
    unit m2(1.0000001, ft);

    EXPECT_TRUE(m2 == m1);
    EXPECT_FALSE(m1.is_exactly_the_same(m2));
    EXPECT_EQ(m1.unit_type_count(), 1);

    auto m4 = m1.add_e_flag();
    EXPECT_FALSE(m1.is_exactly_the_same(m4));
    EXPECT_TRUE(m1.equivalent_non_counting(m4));
    EXPECT_TRUE(m1.equivalent_non_counting(m4.base_units()));

    auto m5 = m1.add_i_flag();
    EXPECT_FALSE(m1.is_exactly_the_same(m5));

    auto m6 = m1.add_per_unit();
    EXPECT_FALSE(m1.is_exactly_the_same(m6));
}

TEST(unitOps, flagClearSetPerUnit)
{
    unit m1(0.245, m);
    auto m2 = m1.add_per_unit();
    EXPECT_TRUE(m2.is_per_unit());
    EXPECT_FALSE(m1.is_exactly_the_same(m2));
    auto m3 = m2.clear_per_unit();
    EXPECT_FALSE(m3.is_per_unit());
    EXPECT_TRUE(m1.is_exactly_the_same(m3));
    m2.clear_flags();
    EXPECT_TRUE(m2.is_exactly_the_same(m1));
}

TEST(unitOps, flagClearSetIflag)
{
    unit m1(0.245, m);
    auto m2 = m1.add_i_flag();
    EXPECT_TRUE(m2.has_i_flag());
    EXPECT_FALSE(m1.is_exactly_the_same(m2));
    auto m3 = m2.clear_i_flag();
    EXPECT_FALSE(m3.has_i_flag());
    EXPECT_TRUE(m1.is_exactly_the_same(m3));
    m2.clear_flags();
    EXPECT_TRUE(m2.is_exactly_the_same(m1));
}

TEST(unitOps, flagClearSetEflag)
{
    unit m1(0.245, m);
    auto m2 = m1.add_e_flag();
    EXPECT_TRUE(m2.has_e_flag());
    EXPECT_FALSE(m1.is_exactly_the_same(m2));
    auto m3 = m2.clear_e_flag();
    EXPECT_FALSE(m3.has_e_flag());
    EXPECT_TRUE(m1.is_exactly_the_same(m3));
    m2.clear_flags();
    EXPECT_TRUE(m2.is_exactly_the_same(m1));
}

TEST(unitOps, flagClearSet)
{
    unit m1(0.245, m);
    m1.set_flags(false, true, false);
    EXPECT_FALSE(m1.is_per_unit());
    EXPECT_TRUE(m1.has_i_flag());
    EXPECT_FALSE(m1.has_e_flag());

    m1.set_flags(true, false, true);
    EXPECT_TRUE(m1.is_per_unit());
    EXPECT_FALSE(m1.has_i_flag());
    EXPECT_TRUE(m1.has_e_flag());

    m1.clear_flags();
    EXPECT_FALSE(m1.is_per_unit());
    EXPECT_FALSE(m1.has_i_flag());
    EXPECT_FALSE(m1.has_e_flag());
}

TEST(unitOps, convertible)
{
    unit m1{m};
    EXPECT_TRUE(m1.is_convertible(ft));
    EXPECT_TRUE(m1.is_convertible(mile));
    EXPECT_TRUE(m1.is_convertible(nm.base_units()));
    EXPECT_FALSE(m1.is_convertible(farad));
    EXPECT_FALSE(m1.is_convertible(lb));
    EXPECT_FALSE(m1.is_convertible(V.base_units()));
}

/*
TEST(unitOps, equalitytest)
{
int eqFailPos = 0;
int eqFailNeg = 0;
double start = 0.1;

double diff = 0.0;
while (diff < 2.75e-6)
{
    auto u1 = unit(start, V);
    auto u2 = unit(start + diff, V);
    auto u3 = unit(start - diff, V);

    std::cout << "diff (" << diff << ") compares "
              << ((u1 == u2) ? std::string("equal") : std::string("not equal"))
<< std::endl; std::cout << "diff (-" << diff << ") compares "
              << ((u1 == u2) ? std::string("equal") : std::string("not equal"))
<< std::endl;

    diff += 1e-8;
}
}
*/
TEST(unitOps, inequality1)
{
    int eqFailPos = 0;
    int eqFailNeg = 0;
    double start = 1.0;
    while (start < (1.0 + 2e-6)) {
        double diff = 1e-5;
        while (diff > 2.501e-6) {
            auto u1 = unit(start, V);
            auto u2 = unit(start + diff, V);
            auto u3 = unit(start - diff, V);
            if (u1 == u2) {
                ++eqFailPos;
            }
            if (u1 == u3) {
                ++eqFailNeg;
            }
            diff -= 1e-8;
        }
        start += 1e-9;
    }
    EXPECT_EQ(eqFailPos, 0);
    EXPECT_EQ(eqFailNeg, 0);
}

TEST(preciseUnitOps, Simple)
{
    EXPECT_EQ(precise::m, precise::m);
    EXPECT_EQ(precise::m * precise::m, precise::m * precise::m);
    EXPECT_NE(precise::m * precise::m, precise::s * precise::s);
    EXPECT_EQ(precise::V / precise::km, precise::electrical::mV / precise::m);
}

TEST(preciseUnitOps, Hash)
{
    auto h1 = std::hash<precise_unit>()(precise::N);
    auto u2 = precise::one / precise::N;
    auto h2 = std::hash<precise_unit>()(u2.inv());
    EXPECT_EQ(h1, h2);
}

TEST(preciseUnitOps, HashCoversFullUnitDataWidth)
{
    auto h1 = std::hash<precise_unit>()(precise::m);
    auto h2 = std::hash<precise_unit>()(precise::m * precise::count);
    EXPECT_NE(h1, h2);
}

TEST(preciseUnitOps, Inv)
{
    EXPECT_EQ(precise::m.inv(), precise::one / precise::m);
    EXPECT_EQ(precise::m.inv().inv(), precise::m);
    EXPECT_EQ(precise::N.inv(), precise::one / precise::N);

    EXPECT_EQ(precise::gal.inv().inv(), precise::gal);
}

TEST(preciseUnitOps, MultipleOps)
{
    auto u1 = precise::electrical::kW / precise::gal;
    auto u2 = u1 / precise::electrical::kW;
    auto u3 = u2.inv();
    EXPECT_EQ(u3, precise::gal);

    EXPECT_EQ(precise::m * precise::W, precise::m * W);
}

TEST(preciseUnitOps, Power)
{
    auto m2 = precise::m.pow(2);
    EXPECT_EQ(precise::m * precise::m, m2);
    auto m4 = pow(m, 4);
    EXPECT_EQ(precise::m * precise::m * precise::m * precise::m, m4);
    auto m4_b = m2.pow(2);
    EXPECT_EQ(m4_b, precise::m * precise::m * precise::m * precise::m);
    EXPECT_EQ(m4_b, m2 * m2);
}

#ifndef UNITS_HEADER_ONLY
TEST(preciseUnitOps, rootMeter)
{
    auto m1 = precise::m.pow(1);
    EXPECT_EQ(precise::m, root(m1, 1));
    EXPECT_EQ(precise::m.inv(), root(m1, -1));
    auto m2 = pow(precise::m, 2);  // use the alternate free function form
    EXPECT_EQ(precise::m, root(m2, 2));
    EXPECT_EQ(precise::m, sqrt(m2));
    auto m4 = precise::m.pow(4);
    EXPECT_EQ(precise::m * precise::m, root(m4, 2));
    EXPECT_EQ(precise::m, root(m4, 4));
}

TEST(preciseUnitOps, rootFoot)
{
    EXPECT_EQ(root(precise::ft, 0), precise::one);
    auto ft1 = precise::ft.pow(1);
    EXPECT_EQ(precise::ft, root(ft1, 1));
    EXPECT_EQ(precise::ft.inv(), root(ft1, -1));

    auto ft2 = precise::ft.pow(2);
    EXPECT_EQ(precise::ft, root(ft2, 2));
    EXPECT_EQ(precise::ft.inv(), root(ft2, -2));
}

TEST(preciseUnitOps, rootFoot345)
{
    auto ft3 = precise::ft.pow(3);
    EXPECT_EQ(precise::ft, root(ft3, 3));
    EXPECT_EQ(precise::ft.inv(), root(ft3, -3));
    auto ft4 = precise::ft.pow(4);
    EXPECT_EQ(precise::ft * precise::ft, root(ft4, 2));
    EXPECT_EQ(precise::ft, root(ft4, 4));
    EXPECT_EQ(precise::ft.inv(), root(ft4, -4));

    auto ft5 = precise::ft.pow(5);
    EXPECT_EQ(precise::ft, root(ft5, 5));
    EXPECT_EQ(precise::ft.inv(), root(ft5, -5));

    if (units::detail::bitwidth::base_size == 8) {
        EXPECT_EQ(precise::ft, root(precise::ft.pow(25), 25));
    }
}

TEST(preciseUnitOps, rootError)
{
    EXPECT_TRUE(is_error(root(precise_unit(-4.5, m), 2)));
}
#endif

TEST(preciseUnitOps, nan)
{
    EXPECT_TRUE(isnan(precise::invalid));
    EXPECT_FALSE(isnan(precise::defunit));
    auto zunit = precise_unit(0.0, m);
    auto zunit2 = precise_unit(0.0, kg);
    auto nunit = zunit2 / zunit;
    EXPECT_TRUE(isnan(nunit));

    EXPECT_TRUE(isnan(precise_unit(invalid)));
}

TEST(preciseUnitOps, inf)
{
    EXPECT_FALSE(isinf(precise::invalid));
    EXPECT_FALSE(isinf(precise::defunit));
    EXPECT_FALSE(isinf(V));
    auto zunit = precise_unit(0.0, precise::m);
    auto nunit = precise::kg / zunit;
    EXPECT_TRUE(isinf(nunit));
    EXPECT_TRUE(isinf(precise_unit(nunit)));
}

TEST(preciseUnitOps, error)
{
    EXPECT_TRUE(is_error(precise::invalid));
    EXPECT_FALSE(is_error(precise::defunit));
    EXPECT_FALSE(is_error(precise::one));
    EXPECT_TRUE(is_error(precise::error));
    EXPECT_FALSE(is_error(precise::V));
    EXPECT_TRUE(
        is_error(precise_unit(constants::invalid_conversion, precise::V)));
}

TEST(preciseUnitOps, normal)
{
    EXPECT_FALSE(isnormal(precise::invalid));
    EXPECT_FALSE(isnormal(precise::defunit));
    EXPECT_TRUE(isnormal(precise::V));
    auto zunit = precise_unit(0.0, m);
    auto nunit = precise::kg / zunit;
    EXPECT_FALSE(isnormal(nunit));
    EXPECT_FALSE(isnormal(
        precise_unit(std::numeric_limits<double>::infinity(), precise::m)));
    EXPECT_FALSE(isnormal(
        precise_unit(-std::numeric_limits<double>::infinity(), precise::m)));
    auto zunit2 = precise_unit(0.0, precise::kg);
    auto nunit2 = zunit2 / zunit;
    EXPECT_FALSE(isnormal(nunit2));
    EXPECT_FALSE(isnormal(precise_unit(-0.25, precise::kg)));
    EXPECT_FALSE(isnormal(precise_unit(1.4e-320, precise::kg * precise::m)));
    EXPECT_TRUE(isnormal(precise_unit(1.4e-306, precise::kg * precise::m)));
    EXPECT_FALSE(isnormal(zunit2));

    EXPECT_TRUE(isnormal(precise::m));
    EXPECT_TRUE(isnormal(precise::m * precise::milli));
}

TEST(preciseUnitOps, valid)
{
    EXPECT_FALSE(is_valid(precise::invalid));
    EXPECT_TRUE(is_valid(precise::defunit));
    EXPECT_TRUE(is_valid(precise::V));
    EXPECT_TRUE(is_valid(
        precise_unit(std::numeric_limits<double>::quiet_NaN(), precise::m)));
    EXPECT_FALSE(is_valid(precise_unit(
        std::numeric_limits<double>::quiet_NaN(),
        precise::invalid.base_units())));
    EXPECT_FALSE(is_valid(precise_unit(
        std::numeric_limits<double>::signaling_NaN(),
        precise::invalid.base_units())));
}

TEST(preciseUnitOps, cast)
{
    EXPECT_NE(precise_unit(ft), precise::ft);
    EXPECT_EQ(precise_unit(m), precise::m);
}

TEST(preciseUnitOps, assignment)
{
    precise_unit U1;

    EXPECT_NE(U1, precise::ft);
    U1 = precise::ft;
    EXPECT_EQ(U1, precise::ft);
    auto U2 = std::make_shared<precise_unit>();
    EXPECT_NE(*U2, precise::mile);
    *U2 = precise::mile;
    EXPECT_EQ(*U2, precise::mile);
}

TEST(preciseunitOps, equivalency)
{
    precise_unit m0;
    precise_unit m1 = precise::ft;
    m0 = m1;
    EXPECT_TRUE(m1.is_exactly_the_same(m0));
    EXPECT_TRUE(m0.is_exactly_the_same(m1));
    precise_unit m2(1.0 + 1e-14, precise::ft);

    EXPECT_TRUE(m2 == m1);
    EXPECT_FALSE(m1.is_exactly_the_same(m2));
    EXPECT_EQ(m1.unit_type_count(), 1);

    auto m4 = m1.add_e_flag();
    EXPECT_FALSE(m1.is_exactly_the_same(m4));
    EXPECT_TRUE(m1.equivalent_non_counting(m4));
    EXPECT_TRUE(m1.equivalent_non_counting(m4.base_units()));

    auto m5 = m1.add_i_flag();
    EXPECT_FALSE(m1.is_exactly_the_same(m5));

    auto m6 = m1.add_per_unit();
    EXPECT_FALSE(m1.is_exactly_the_same(m6));

    EXPECT_TRUE(precise::m.is_exactly_the_same(m));
    EXPECT_FALSE(precise::ft.is_exactly_the_same(ft));
}

TEST(preciseunitOps, flagClearSetPerUnit)
{
    precise_unit m1(0.245, precise::yd);
    auto m2 = m1.add_per_unit();
    EXPECT_TRUE(m2.is_per_unit());
    EXPECT_FALSE(m1.is_exactly_the_same(m2));
    auto m3 = m2.clear_per_unit();
    EXPECT_FALSE(m3.is_per_unit());
    EXPECT_TRUE(m1.is_exactly_the_same(m3));
    m2.clear_flags();
    EXPECT_TRUE(m2.is_exactly_the_same(m1));
}

TEST(preciseunitOps, flagClearSetIflag)
{
    precise_unit m1(0.245, precise::yd);
    auto m2 = m1.add_i_flag();
    EXPECT_TRUE(m2.has_i_flag());
    EXPECT_FALSE(m1.is_exactly_the_same(m2));
    auto m3 = m2.clear_i_flag();
    EXPECT_FALSE(m3.has_i_flag());
    EXPECT_TRUE(m1.is_exactly_the_same(m3));
    m2.clear_flags();
    EXPECT_TRUE(m2.is_exactly_the_same(m1));
}

TEST(preciseunitOps, flagClearSetEflag)
{
    precise_unit m1(0.245, precise::yd);
    auto m2 = m1.add_e_flag();
    EXPECT_TRUE(m2.has_e_flag());
    EXPECT_FALSE(m1.is_exactly_the_same(m2));
    auto m3 = m2.clear_e_flag();
    EXPECT_FALSE(m3.has_e_flag());
    EXPECT_TRUE(m1.is_exactly_the_same(m3));
    m2.clear_flags();
    EXPECT_TRUE(m2.is_exactly_the_same(m1));
}

TEST(preciseunitOps, flagClearSet)
{
    precise_unit m1(0.245, precise::yd);
    m1.set_flags(false, true, false);
    EXPECT_FALSE(m1.is_per_unit());
    EXPECT_TRUE(m1.has_i_flag());
    EXPECT_FALSE(m1.has_e_flag());

    m1.set_flags(true, false, true);
    EXPECT_TRUE(m1.is_per_unit());
    EXPECT_FALSE(m1.has_i_flag());
    EXPECT_TRUE(m1.has_e_flag());

    m1.clear_flags();
    EXPECT_FALSE(m1.is_per_unit());
    EXPECT_FALSE(m1.has_i_flag());
    EXPECT_FALSE(m1.has_e_flag());
}

TEST(preciseunitOps, equality1)
{
    int eqFailPos = 0;
    int eqFailNeg = 0;
    double start = 1.0;
    while (start < (1.0 + 2e-12)) {
        double diff = 0.0;
        while (diff < 5e-13) {
            auto u1 = precise_unit(start, precise::V);
            auto u2 = precise_unit(start + diff, precise::V);
            auto u3 = precise_unit(start - diff, precise::V);
            if (u1 != u2) {
                ++eqFailPos;
            }
            if (u1 != u3) {
                ++eqFailNeg;
            }
            diff += 1e-15;
        }
        start += 1e-15;
    }
    EXPECT_EQ(eqFailPos, 0);
    EXPECT_EQ(eqFailNeg, 0);
}

TEST(preciseunitOps, flags)
{
    auto e1 = precise::error;
    EXPECT_TRUE(e1.has_i_flag());
    EXPECT_TRUE(e1.has_e_flag());
    static_assert(
        precise::error.has_i_flag(),
        "i flag should be constexpr and allowable in compile time constants");
    static_assert(
        precise::error.has_e_flag(),
        "e flag should be constexpr and allowable in compile time constants");

    e1.clear_flags();
    EXPECT_FALSE(e1.has_i_flag());
    EXPECT_FALSE(e1.has_e_flag());
}

/*
TEST(unitOps, equalitytest)
{
int eqFailPos = 0;
int eqFailNeg = 0;
double start = 0.1;

double diff = 0.0;
while (diff < 2.75e-6)
{
auto u1 = unit(start, V);
auto u2 = unit(start + diff, V);
auto u3 = unit(start - diff, V);

std::cout << "diff (" << diff << ") compares "
<< ((u1 == u2) ? std::string("equal") : std::string("not equal")) << std::endl;
std::cout << "diff (-" << diff << ") compares "
<< ((u1 == u2) ? std::string("equal") : std::string("not equal")) << std::endl;

diff += 1e-8;
}
}
*/
TEST(preciseunitOps, inequality1)
{
    int eqFailPos = 0;
    int eqFailNeg = 0;
    double start = 1.0;
    while (start < (1.0 + 2e-12)) {
        double diff = 1e-11;
        while (diff > 1.501e-12) {
            auto u1 = precise_unit(start, precise::V);
            auto u2 = precise_unit(start + diff, precise::V);
            auto u3 = precise_unit(start - diff, precise::V);
            if (u1 == u2) {
                ++eqFailPos;
            }
            if (u1 == u3) {
                ++eqFailNeg;
            }
            diff -= 1e-14;
        }
        start += 1e-15;
    }
    EXPECT_EQ(eqFailPos, 0);
    EXPECT_EQ(eqFailNeg, 0);
}

TEST(preciseunitOps, subnormal)
{
    precise_unit u1(2.3456e-306, precise::m);
    precise_unit u2(2.3457e-306, precise::m);
    // these are equal to within a normal precision floating point.
    EXPECT_TRUE(u1 == u2);
    EXPECT_FALSE(u1 != u2);
    EXPECT_TRUE(u2 == u1);

    precise_unit u3(2.3456e-300, precise::m);
    precise_unit u4(2.3457e-300, precise::m);
    // these are not equal.
    EXPECT_FALSE(u3 == u4);
    EXPECT_TRUE(u3 != u4);
    EXPECT_FALSE(u4 == u3);
}

TEST(invalidOps, saturate)
{
    for (int ii = -8; ii < 8; ++ii) {
        auto nunit = precise::s.pow(ii);
        auto nunit2 = nunit.pow(2);
        EXPECT_EQ(nunit2.base_units().kg(), 0);
        EXPECT_EQ(nunit2.base_units().meter(), 0);
    }
}

TEST(specialOps, rootHertz)
{
    auto res = precise::special::ASD.pow(2);
    EXPECT_EQ(res, precise::m.pow(2) / precise::s.pow(4) / precise::Hz);
    EXPECT_FALSE(is_error(precise::special::ASD));

    auto rh = precise::special::rootHertz;

    EXPECT_EQ(rh.pow(1), rh);
    EXPECT_EQ(rh.pow(0), precise::one);
    EXPECT_EQ(rh.pow(2), Hz);
    EXPECT_EQ(rh.pow(-2), s);

    EXPECT_EQ(rh.pow(4), Hz.pow(2));
    EXPECT_EQ(rh.pow(-4), s.pow(2));

    auto rhinv = rh.inv();
    EXPECT_EQ(rhinv.pow(2), s);

    // EXPECT_EQ(rh.pow(3).pow(2), Hz.pow(3));
    // EXPECT_EQ(rh.pow(4), Hz.pow(2));
    // EXPECT_EQ(rh.pow(6), Hz.pow(3));
    // EXPECT_EQ(rh.pow(-2), s);
    // EXPECT_EQ(rh.pow(-4), s.pow(2));
    // EXPECT_EQ(rh.pow(-6), s.pow(3));
}

TEST(specialOps, degC)
{
    auto res = precise::degC.pow(2);
    EXPECT_EQ(root(res, 2), precise::degC);
}

TEST(customUnits, definition)
{
    auto cunit1 = precise::generate_custom_unit(4);
    auto cunit2 = precise::generate_custom_unit(7);
    EXPECT_FALSE(cunit1 == cunit2);
}

TEST(customUnits, testCustomInv)
{
    for (std::uint16_t ii = 0; ii < 1024; ++ii) {
        auto cunit1 = precise::generate_custom_unit(ii);
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit1.base_units()))
            << "Error with custom unit detection " << ii;
        EXPECT_FALSE(
            precise::custom::is_custom_unit_inverted(cunit1.base_units()));
        EXPECT_FALSE(
            precise::custom::is_custom_count_unit(cunit1.base_units()));

        auto cunit2 = cunit1.inv();
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit2.base_units()))
            << "Error with custom unit detection of inverse " << ii;
        EXPECT_TRUE(
            precise::custom::is_custom_unit_inverted(cunit2.base_units()));
        EXPECT_FALSE(
            precise::custom::is_custom_count_unit(cunit2.base_units()));

        auto cunit3 = cunit2.inv();

        EXPECT_FALSE(cunit1 == cunit2)
            << "Error with false comparison 1 index " << ii;
        EXPECT_FALSE(cunit2 == cunit3)
            << "Error with false comparison 2 index " << ii;
        EXPECT_TRUE(cunit1 == cunit3) << "Error with inversion " << ii;
    }
}

TEST(customUnits, uniqueness)
{
    for (std::uint16_t ii = 0; ii < 1024; ++ii) {
        auto cunit1 = precise::generate_custom_unit(ii);
        auto cunit1inv = cunit1.inv();

        EXPECT_EQ(precise::custom::custom_unit_number(cunit1.base_units()), ii);
        EXPECT_EQ(precise::custom::custom_unit_number(cunit1.base_units()), ii);
        for (std::uint16_t jj = 0; jj < 1024; ++jj) {
            if (ii == jj) {
                continue;
            }
            auto cunit2 = precise::generate_custom_unit(jj);
            EXPECT_FALSE(cunit1 == cunit2)
                << "Error with false comparison 1 index " << ii << "," << jj;
            EXPECT_FALSE(cunit1 == cunit2.inv())
                << "Error with false comparison 1 inv index " << ii << ","
                << jj;
            EXPECT_FALSE(cunit1inv == cunit2)
                << "Error with false inv comparison 2 index " << ii << ","
                << jj;
            EXPECT_FALSE(cunit1inv == cunit2.inv())
                << "Error with false inv comparison 2 inv index " << ii << ","
                << jj;
        }
    }
}

TEST(customUnits, uniquenesspermeter)
{
    for (std::uint16_t ii = 0; ii < 1024; ++ii) {
        auto cunit1 = precise::generate_custom_unit(ii);
        auto cunit1adj = cunit1 / precise::meter;

        EXPECT_EQ(
            precise::custom::custom_unit_number(cunit1adj.base_units()), ii);
        EXPECT_EQ(
            precise::custom::custom_unit_number(cunit1adj.inv().base_units()),
            ii);
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit1.base_units()))
            << "Error with custom unit/meter detection " << ii;
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit1adj.base_units()))
            << "Error with custom unit/meter detection of inverse " << ii;
        for (std::uint16_t jj = 0; jj < 1024; ++jj) {
            if (ii == jj) {
                continue;
            }
            auto cunit2 = precise::generate_custom_unit(jj);
            auto cunit2adj = cunit2 / precise::m;
            EXPECT_FALSE(cunit1 == cunit2adj)
                << "Error with false per meter comparison 1 inv index " << ii
                << "," << jj;
            EXPECT_FALSE(cunit1adj == cunit2)
                << "Error with false per meter comparison 2 index " << ii << ","
                << jj;
            EXPECT_FALSE(cunit1adj == cunit2adj)
                << "Error with false per meter comparison 2 per meter index "
                << ii << "," << jj;
        }
        EXPECT_TRUE(cunit1adj * precise::m == cunit1)
            << "Error with true per meter multiply" << ii;
    }
}

TEST(customUnits, uniquenesspermeter2)
{
    for (std::uint16_t ii = 0; ii < 1024; ++ii) {
        auto cunit1 = precise::generate_custom_unit(ii);
        auto cunit1adj = cunit1 / precise::meter.pow(2);
        EXPECT_EQ(
            precise::custom::custom_unit_number(cunit1adj.base_units()), ii);
        EXPECT_EQ(
            precise::custom::custom_unit_number(cunit1adj.inv().base_units()),
            ii);

        EXPECT_TRUE(precise::custom::is_custom_unit(cunit1.base_units()))
            << "Error with custom unit/meter2 detection " << ii;
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit1adj.base_units()))
            << "Error with custom unit/meter2 detection of inverse " << ii;
        for (std::uint16_t jj = 0; jj < 1024; ++jj) {
            if (ii == jj) {
                continue;
            }
            auto cunit2 = precise::generate_custom_unit(jj);
            auto cunit2adj = cunit2 / precise::m.pow(2);
            EXPECT_FALSE(cunit1 == cunit2adj)
                << "Error with false per meter2 comparison 1 inv index " << ii
                << "," << jj;
            EXPECT_FALSE(cunit1adj == cunit2)
                << "Error with false per meter2 comparison 2 index " << ii
                << "," << jj;
            EXPECT_FALSE(cunit1adj == cunit2adj)
                << "Error with false per meter2 comparison 2 per meter2 index "
                << ii << "," << jj;
        }
        EXPECT_TRUE(cunit1adj * precise::m * precise::m == cunit1)
            << "Error with true per meter2 multiply" << ii;
    }
}

TEST(customUnits, uniquenesspermeter3)
{
    for (std::uint16_t ii = 0; ii < 1024; ++ii) {
        auto cunit1 = precise::generate_custom_unit(ii);
        auto cunit1adj = cunit1 / precise::meter.pow(3);
        EXPECT_EQ(
            precise::custom::custom_unit_number(cunit1adj.base_units()), ii);
        EXPECT_EQ(
            precise::custom::custom_unit_number(cunit1adj.inv().base_units()),
            ii);
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit1.base_units()))
            << "Error with custom unit/meter3 detection " << ii;
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit1adj.base_units()))
            << "Error with custom unit/meter3 detection of inverse " << ii;
        for (std::uint16_t jj = 0; jj < 1024; ++jj) {
            if (ii == jj) {
                continue;
            }
            auto cunit2 = precise::generate_custom_unit(jj);
            auto cunit2adj = cunit2 / precise::m.pow(3);
            EXPECT_FALSE(cunit1 == cunit2adj)
                << "Error with false per meter3 comparison 1 inv index " << ii
                << "," << jj;
            EXPECT_FALSE(cunit1adj == cunit2)
                << "Error with false per meter3 comparison 2 index " << ii
                << "," << jj;
            EXPECT_FALSE(cunit1adj == cunit2adj)
                << "Error with false per meter3 comparison 2 per meter3 index "
                << ii << "," << jj;
        }
        EXPECT_TRUE(cunit1adj * precise::m.pow(3) == cunit1)
            << "Error with true per meter3 multiply" << ii;
    }
}

TEST(customUnits, uniquenessperkg)
{
    for (std::uint16_t ii = 0; ii < 1024; ++ii) {
        auto cunit1 = precise::generate_custom_unit(ii);
        auto cunit1adj = cunit1 / precise::kg;
        EXPECT_EQ(
            precise::custom::custom_unit_number(cunit1adj.base_units()), ii);
        EXPECT_EQ(
            precise::custom::custom_unit_number(cunit1adj.inv().base_units()),
            ii);
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit1.base_units()))
            << "Error with custom unit/kg detection " << ii;
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit1adj.base_units()))
            << "Error with custom unit/kg detection of inverse " << ii;
        for (std::uint16_t jj = 0; jj < 1024; ++jj) {
            if (ii == jj) {
                continue;
            }
            auto cunit2 = precise::generate_custom_unit(jj);
            auto cunit2adj = cunit2 / precise::kg;
            EXPECT_FALSE(cunit1 == cunit2adj)
                << "Error with false per kg comparison 1 inv index " << ii
                << "," << jj;
            EXPECT_FALSE(cunit1adj == cunit2)
                << "Error with false per kg comparison 2 index " << ii << ","
                << jj;
            EXPECT_FALSE(cunit1adj == cunit2adj)
                << "Error with false per kg comparison 2 per kg index " << ii
                << "," << jj;
        }
        EXPECT_TRUE(cunit1adj * precise::kg == cunit1)
            << "Error with true per kg multiply" << ii;
    }
}

TEST(customUnits, uniquenesspersecond)
{
    for (std::uint16_t ii = 0; ii < 1024; ++ii) {
        auto cunit1 = precise::generate_custom_unit(ii);
        auto cunit1adj = cunit1 / precise::s;
        EXPECT_EQ(
            precise::custom::custom_unit_number(cunit1adj.base_units()), ii);
        EXPECT_EQ(
            precise::custom::custom_unit_number(cunit1adj.inv().base_units()),
            ii);
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit1.base_units()))
            << "Error with custom unit/s detection " << ii;
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit1adj.base_units()))
            << "Error with custom unit/s detection of inverse " << ii;
        for (std::uint16_t jj = 0; jj < 1024; ++jj) {
            if (ii == jj) {
                continue;
            }
            auto cunit2 = precise::generate_custom_unit(jj);
            auto cunit2adj = cunit2 / precise::s;
            EXPECT_FALSE(cunit1 == cunit2adj)
                << "Error with false per sec comparison 1 inv index " << ii
                << "," << jj;
            EXPECT_FALSE(cunit1adj == cunit2)
                << "Error with false per sec comparison 2 index " << ii << ","
                << jj;
            EXPECT_FALSE(cunit1adj == cunit2adj)
                << "Error with false per sec comparison 2 per sec index " << ii
                << "," << jj;
        }
        EXPECT_TRUE(cunit1adj * precise::s == cunit1)
            << "Error with true per sec multiply" << ii;
    }
}

TEST(customCountUnits, definition)
{
    auto cunit1 = precise::generate_custom_count_unit(4);
    auto cunit2 = precise::generate_custom_count_unit(7);
    EXPECT_FALSE(cunit1 == cunit2);
}

TEST(customCountUnits, testAllInv)
{
    for (std::uint16_t ii = 0; ii < 16; ++ii) {
        auto cunit1 = precise::generate_custom_count_unit(ii);
        EXPECT_TRUE(precise::custom::is_custom_count_unit(cunit1.base_units()));
        EXPECT_FALSE(precise::custom::is_custom_count_unit_inverted(
            cunit1.base_units()));

        EXPECT_FALSE(precise::custom::is_custom_unit(cunit1.base_units()));
        auto cunit2 = cunit1.inv();
        EXPECT_TRUE(precise::custom::is_custom_count_unit(cunit2.base_units()));
        EXPECT_TRUE(precise::custom::is_custom_count_unit_inverted(
            cunit2.base_units()));
        EXPECT_FALSE(precise::custom::is_custom_unit(cunit2.base_units()));
        auto cunit3 = cunit2.inv();
        EXPECT_TRUE(precise::custom::is_custom_count_unit(cunit3.base_units()));
        EXPECT_FALSE(precise::custom::is_custom_count_unit_inverted(
            cunit3.base_units()));

        EXPECT_FALSE(cunit1 == cunit2)
            << "Error with false comparison 1 index " << ii;
        EXPECT_FALSE(cunit2 == cunit3)
            << "Error with false comparison 2 index " << ii;
        EXPECT_TRUE(cunit1 == cunit3) << "Error with inversion " << ii;
        EXPECT_TRUE(precise::custom::is_custom_count_unit(cunit1.base_units()))
            << "Error with custom unit detection " << ii;
        EXPECT_TRUE(precise::custom::is_custom_count_unit(cunit2.base_units()))
            << "Error with custom unit detection of inverse " << ii;
        EXPECT_TRUE(precise::custom::is_custom_count_unit(cunit3.base_units()))
            << "Error with custom unit detection inv inv" << ii;
    }
}

TEST(customCountUnits, uniqueness)
{
    for (std::uint16_t ii = 0; ii < 16; ++ii) {
        auto cunit1 = precise::generate_custom_count_unit(ii);
        auto cunit1inv = cunit1.inv();

        EXPECT_EQ(
            precise::custom::custom_count_unit_number(cunit1.base_units()), ii);
        EXPECT_EQ(
            precise::custom::custom_count_unit_number(cunit1.base_units()), ii);
        for (std::uint16_t jj = 0; jj < 16; ++jj) {
            if (ii == jj) {
                continue;
            }
            auto cunit2 = precise::generate_custom_count_unit(jj);
            EXPECT_FALSE(cunit1 == cunit2)
                << "Error with false comparison 1 index " << ii << "," << jj;
            EXPECT_FALSE(cunit1 == cunit2.inv())
                << "Error with false comparison 1 inv index " << ii << ","
                << jj;
            EXPECT_FALSE(cunit1inv == cunit2)
                << "Error with false inv comparison 2 index " << ii << ","
                << jj;
            EXPECT_FALSE(cunit1inv == cunit2.inv())
                << "Error with false inv comparison 2 inv index " << ii << ","
                << jj;
        }
    }
}

TEST(UnitUtilTest, timesOverflowsEdge)
{
    const auto m1 = m;
    const auto m7 = m * m * m * m * m * m * m;
    const auto im8 = (one / m7) / m;
    const auto im1 = one / m1;
    // At lower or upper bound:
    // unchanged exponent
    EXPECT_FALSE(times_overflows(m7, one));
    EXPECT_FALSE(times_overflows(one, m7));
    EXPECT_FALSE(times_overflows(im8, one));
    EXPECT_FALSE(times_overflows(one, im8));
    // change by 1 *away* from bound
    EXPECT_FALSE(times_overflows(m7, im1));
    EXPECT_FALSE(times_overflows(im1, m7));
    EXPECT_FALSE(times_overflows(im8, m1));
    EXPECT_FALSE(times_overflows(m1, im8));
    // change by 1 *towards* from bound => overflow or underflow
    if (units::detail::bitwidth::base_size == 4) {
        EXPECT_TRUE(times_overflows(m7, m1));
        EXPECT_TRUE(times_overflows(m1, m7));
        EXPECT_TRUE(times_overflows(im8, im1));
        EXPECT_TRUE(times_overflows(im1, im8));
    } else {
        EXPECT_FALSE(times_overflows(m7, m1));
        EXPECT_FALSE(times_overflows(m1, m7));
        EXPECT_FALSE(times_overflows(im8, im1));
        EXPECT_FALSE(times_overflows(im1, im8));
    }
}

TEST(UnitUtilTest, timesOverflowsMid)
{
    const auto m3 = m * m * m;
    const auto m4 = m * m * m * m;
    const auto m5 = m * m * m * m * m;
    const auto im5 = one / m5;
    const auto im4 = one / m4;

    // Start far from bounds:
    EXPECT_FALSE(times_overflows(m3, m4));
    EXPECT_FALSE(times_overflows(m4, m3));

    EXPECT_FALSE(times_overflows(im4, im4));
    if (units::detail::bitwidth::base_size == 4) {
        EXPECT_TRUE(times_overflows(m4, m4));  // overflow
        EXPECT_TRUE(times_overflows(im4, im5));  // underflow
        EXPECT_TRUE(times_overflows(im5, im4));  // underflow
        EXPECT_TRUE(times_overflows(count, count));
    } else {
        EXPECT_FALSE(times_overflows(m4, m4));  // no overflow
        EXPECT_FALSE(times_overflows(im4, im5));  // no underflow
        EXPECT_FALSE(times_overflows(im5, im4));  // no underflow
        EXPECT_FALSE(times_overflows(count, count));
    }
}

TEST(UnitUtilTest, dividesOverflowsEdge)
{
    const auto m1 = m;
    const auto m7 = m * m * m * m * m * m * m;
    const auto im8 = (one / m7) / m;
    const auto im7 = one / m7;
    const auto im1 = one / m1;
    // At lower or upper bound:
    // unchanged exponent
    EXPECT_FALSE(divides_overflows(m7, one));
    EXPECT_FALSE(divides_overflows(im8, one));
    // change by 1 *away* from bound
    EXPECT_FALSE(divides_overflows(m7, m1));
    EXPECT_FALSE(divides_overflows(im8, im1));
    // change by 1 *towards* from bound => overflow or underflow
    if (units::detail::bitwidth::base_size == 4) {
        EXPECT_TRUE(divides_overflows(m7, im1));
        EXPECT_TRUE(divides_overflows(m1, im7));
        EXPECT_TRUE(divides_overflows(im8, m1));
        EXPECT_TRUE(divides_overflows(m1, im8));
    }
}

TEST(UnitUtilTest, dividesOverflows)
{
    const auto m3 = m * m * m;
    const auto m4 = m * m * m * m;
    const auto m5 = m * m * m * m * m;

    const auto im4 = one / m4;

    // Start far from bounds:
    EXPECT_FALSE(divides_overflows(m3, im4));
    EXPECT_FALSE(divides_overflows(im4, m3));
    if (units::detail::bitwidth::base_size == 4) {
        EXPECT_TRUE(divides_overflows(m4, im4));  // overflow
        EXPECT_FALSE(divides_overflows(im4, m4));
        EXPECT_TRUE(divides_overflows(im4, m5));  // underflow
        EXPECT_TRUE(divides_overflows(m5, im4));  // underflow
    }
}

TEST(UnitUtilTest, invOverflows)
{
    EXPECT_FALSE(inv_overflows(m));
    const auto inv_mol = one / mol;
    if (units::detail::bitwidth::base_size == 4) {
        EXPECT_TRUE(inv_overflows(inv_mol * inv_mol));
    } else {
        EXPECT_FALSE(inv_overflows(inv_mol * inv_mol));
    }
}

TEST(UnitUtilTest, powOverflows)
{
    EXPECT_FALSE(pow_overflows(m, -1));
    EXPECT_FALSE(pow_overflows(m, 0));
    EXPECT_FALSE(pow_overflows(m, 1));
    EXPECT_FALSE(pow_overflows(m, 2));
    EXPECT_FALSE(pow_overflows(m, 4));
    EXPECT_FALSE(pow_overflows(m, 7));
    if (units::detail::bitwidth::base_size == 4) {
        EXPECT_TRUE(pow_overflows(m, 8));
        EXPECT_TRUE(pow_overflows(m * m * m * m, 2));
    } else {
        EXPECT_FALSE(pow_overflows(m, 8));
        EXPECT_FALSE(pow_overflows(m * m * m * m, 2));
    }
}
