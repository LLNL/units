/*
Copyright (c) 2019-2020,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "test.hpp"
#include "units/unit_definitions.hpp"
#include "units/units_decl.hpp"

#include <memory>

using namespace units;
TEST(unitOps, Simple)
{
    EXPECT_EQ(m, m);
    EXPECT_EQ(m * m, m * m);
    EXPECT_NE(m * m, s * s);
    EXPECT_EQ(V / km, mV / m);
    static_assert((V / km).is_exactly_the_same(mV / m), "units not static equivalent");
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
    static_assert((m.inv()).is_exactly_the_same(one / m), "inverse units not static equivalent");
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
    auto m4 = pow(m, 4); //use the free function form
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
    EXPECT_FALSE(is_valid(unit(invalid.base_units(), std::numeric_limits<double>::quiet_NaN())));
    EXPECT_FALSE(
        is_valid(unit(invalid.base_units(), std::numeric_limits<double>::signaling_NaN())));
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
        "Error flag should be constexpr and allowable in compile time constants");
    static_assert(
        error.has_e_flag(),
        "Error flag should be constexpr and allowable in compile time constants");

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
    EXPECT_EQ(precise::V / precise::km, precise::mV / precise::m);
}

TEST(preciseUnitOps, Hash)
{
    auto h1 = std::hash<precise_unit>()(precise::N);
    auto u2 = precise::one / precise::N;
    auto h2 = std::hash<precise_unit>()(u2.inv());
    EXPECT_EQ(h1, h2);
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
    auto u1 = precise::kW / precise::gal;
    auto u2 = u1 / precise::kW;
    auto u3 = u2.inv();
    EXPECT_EQ(u3, precise::gal);
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
TEST(preciseUnitOps, root)
{
    auto m1 = precise::m.pow(1);
    EXPECT_EQ(precise::m, root(m1, 1));
    EXPECT_EQ(precise::m.inv(), root(m1, -1));
    auto m2 = pow(precise::m, 2); //use the alternate free function form
    EXPECT_EQ(precise::m, root(m2, 2));
    EXPECT_EQ(precise::m, sqrt(m2));
    auto m4 = precise::m.pow(4);
    EXPECT_EQ(precise::m * precise::m, root(m4, 2));
    EXPECT_EQ(precise::m, root(m4, 4));

    EXPECT_EQ(root(precise::ft, 0), precise::one);
    auto ft1 = precise::ft.pow(1);
    EXPECT_EQ(precise::ft, root(ft1, 1));
    EXPECT_EQ(precise::ft.inv(), root(ft1, -1));

    auto ft2 = precise::ft.pow(2);
    EXPECT_EQ(precise::ft, root(ft2, 2));
    EXPECT_EQ(precise::ft.inv(), root(ft2, -2));
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
    EXPECT_TRUE(is_error(precise_unit(constants::invalid_conversion, precise::V)));
}

TEST(preciseUnitOps, normal)
{
    EXPECT_FALSE(isnormal(precise::invalid));
    EXPECT_FALSE(isnormal(precise::defunit));
    EXPECT_TRUE(isnormal(precise::V));
    auto zunit = precise_unit(0.0, m);
    auto nunit = precise::kg / zunit;
    EXPECT_FALSE(isnormal(nunit));
    EXPECT_FALSE(isnormal(precise_unit(std::numeric_limits<double>::infinity(), precise::m)));
    EXPECT_FALSE(isnormal(precise_unit(-std::numeric_limits<double>::infinity(), precise::m)));
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
    EXPECT_TRUE(is_valid(precise_unit(std::numeric_limits<double>::quiet_NaN(), precise::m)));
    EXPECT_FALSE(is_valid(
        precise_unit(precise::invalid.base_units(), std::numeric_limits<double>::quiet_NaN())));
    EXPECT_FALSE(is_valid(
        precise_unit(precise::invalid.base_units(), std::numeric_limits<double>::signaling_NaN())));
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
        "Error flag should be constexpr and allowable in compile time constants");
    static_assert(
        precise::error.has_e_flag(),
        "Error flag should be constexpr and allowable in compile time constants");

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

TEST(preciseunitOps, subnormal_test)
{
    precise_unit u1(2.3456e-306, precise::m);
    precise_unit u2(2.3457e-306, precise::m);
    //these are equal to within a normal precision floating point.
    EXPECT_TRUE(u1 == u2);
    EXPECT_FALSE(u1 != u2);
    EXPECT_TRUE(u2 == u1);

    precise_unit u3(2.3456e-300, precise::m);
    precise_unit u4(2.3457e-300, precise::m);
    //these are not equal.
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

    //EXPECT_EQ(rh.pow(3).pow(2), Hz.pow(3));
    //EXPECT_EQ(rh.pow(4), Hz.pow(2));
    //EXPECT_EQ(rh.pow(6), Hz.pow(3));
    //EXPECT_EQ(rh.pow(-2), s);
    //EXPECT_EQ(rh.pow(-4), s.pow(2));
    //EXPECT_EQ(rh.pow(-6), s.pow(3));
}

TEST(customUnits, definition)
{
    auto cunit1 = precise::generate_custom_unit(4);
    auto cunit2 = precise::generate_custom_unit(7);
    EXPECT_FALSE(cunit1 == cunit2);
}

TEST(customUnits, testAllInv)
{
    for (unsigned short ii = 0; ii < 1024; ++ii) {
        auto cunit1 = precise::generate_custom_unit(ii);
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit1.base_units()));
        EXPECT_FALSE(precise::custom::is_custom_unit_inverted(cunit1.base_units()));

        EXPECT_FALSE(precise::custom::is_custom_count_unit(cunit1.base_units()));
        auto cunit2 = cunit1.inv();
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit2.base_units()));
        EXPECT_TRUE(precise::custom::is_custom_unit_inverted(cunit2.base_units()));
        EXPECT_FALSE(precise::custom::is_custom_count_unit(cunit2.base_units()));
        auto cunit3 = cunit2.inv();
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit3.base_units()));
        EXPECT_FALSE(precise::custom::is_custom_unit_inverted(cunit3.base_units()));

        EXPECT_FALSE(cunit1 == cunit2) << "Error with false comparison 1 index " << ii;
        EXPECT_FALSE(cunit2 == cunit3) << "Error with false comparison 2 index " << ii;
        EXPECT_TRUE(cunit1 == cunit3) << "Error with inversion " << ii;
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit1.base_units()))
            << "Error with custom unit detection " << ii;
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit2.base_units()))
            << "Error with custom unit detection of inverse " << ii;
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit3.base_units()))
            << "Error with custom unit detection inv inv" << ii;
    }
}

TEST(customUnits, uniqueness)
{
    for (unsigned short ii = 0; ii < 1024; ++ii) {
        auto cunit1 = precise::generate_custom_unit(ii);
        auto cunit1inv = cunit1.inv();

        EXPECT_EQ(precise::custom::custom_unit_number(cunit1.base_units()), ii);
        EXPECT_EQ(precise::custom::custom_unit_number(cunit1.base_units()), ii);
        for (unsigned short jj = 0; jj < 1024; ++jj) {
            if (ii == jj) {
                continue;
            }
            auto cunit2 = precise::generate_custom_unit(jj);
            EXPECT_FALSE(cunit1 == cunit2)
                << "Error with false comparison 1 index " << ii << "," << jj;
            EXPECT_FALSE(cunit1 == cunit2.inv())
                << "Error with false comparison 1 inv index " << ii << "," << jj;
            EXPECT_FALSE(cunit1inv == cunit2)
                << "Error with false inv comparison 2 index " << ii << "," << jj;
            EXPECT_FALSE(cunit1inv == cunit2.inv())
                << "Error with false inv comparison 2 inv index " << ii << "," << jj;
        }
    }
}

TEST(customUnits, uniquenesspermeter)
{
    for (unsigned short ii = 0; ii < 1024; ++ii) {
        auto cunit1 = precise::generate_custom_unit(ii);
        auto cunit1adj = cunit1 / precise::meter;

        EXPECT_EQ(precise::custom::custom_unit_number(cunit1adj.base_units()), ii);
        EXPECT_EQ(precise::custom::custom_unit_number(cunit1adj.inv().base_units()), ii);
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit1.base_units()))
            << "Error with custom unit/meter detection " << ii;
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit1adj.base_units()))
            << "Error with custom unit/meter detection of inverse " << ii;
        for (unsigned short jj = 0; jj < 1024; ++jj) {
            if (ii == jj) {
                continue;
            }
            auto cunit2 = precise::generate_custom_unit(jj);
            auto cunit2adj = cunit2 / precise::m;
            EXPECT_FALSE(cunit1 == cunit2adj)
                << "Error with false per meter comparison 1 inv index " << ii << "," << jj;
            EXPECT_FALSE(cunit1adj == cunit2)
                << "Error with false per meter comparison 2 index " << ii << "," << jj;
            EXPECT_FALSE(cunit1adj == cunit2adj)
                << "Error with false per meter comparison 2 per meter index " << ii << "," << jj;
        }
        EXPECT_TRUE(cunit1adj * precise::m == cunit1) << "Error with true per meter multiply" << ii;
    }
}

TEST(customUnits, uniquenesspermeter2)
{
    for (unsigned short ii = 0; ii < 1024; ++ii) {
        auto cunit1 = precise::generate_custom_unit(ii);
        auto cunit1adj = cunit1 / precise::meter.pow(2);
        EXPECT_EQ(precise::custom::custom_unit_number(cunit1adj.base_units()), ii);
        EXPECT_EQ(precise::custom::custom_unit_number(cunit1adj.inv().base_units()), ii);

        EXPECT_TRUE(precise::custom::is_custom_unit(cunit1.base_units()))
            << "Error with custom unit/meter2 detection " << ii;
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit1adj.base_units()))
            << "Error with custom unit/meter2 detection of inverse " << ii;
        for (unsigned short jj = 0; jj < 1024; ++jj) {
            if (ii == jj) {
                continue;
            }
            auto cunit2 = precise::generate_custom_unit(jj);
            auto cunit2adj = cunit2 / precise::m.pow(2);
            EXPECT_FALSE(cunit1 == cunit2adj)
                << "Error with false per meter2 comparison 1 inv index " << ii << "," << jj;
            EXPECT_FALSE(cunit1adj == cunit2)
                << "Error with false per meter2 comparison 2 index " << ii << "," << jj;
            EXPECT_FALSE(cunit1adj == cunit2adj)
                << "Error with false per meter2 comparison 2 per meter2 index " << ii << "," << jj;
        }
        EXPECT_TRUE(cunit1adj * precise::m * precise::m == cunit1)
            << "Error with true per meter2 multiply" << ii;
    }
}

TEST(customUnits, uniquenesspermeter3)
{
    for (unsigned short ii = 0; ii < 1024; ++ii) {
        auto cunit1 = precise::generate_custom_unit(ii);
        auto cunit1adj = cunit1 / precise::meter.pow(3);
        EXPECT_EQ(precise::custom::custom_unit_number(cunit1adj.base_units()), ii);
        EXPECT_EQ(precise::custom::custom_unit_number(cunit1adj.inv().base_units()), ii);
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit1.base_units()))
            << "Error with custom unit/meter3 detection " << ii;
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit1adj.base_units()))
            << "Error with custom unit/meter3 detection of inverse " << ii;
        for (unsigned short jj = 0; jj < 1024; ++jj) {
            if (ii == jj) {
                continue;
            }
            auto cunit2 = precise::generate_custom_unit(jj);
            auto cunit2adj = cunit2 / precise::m.pow(3);
            EXPECT_FALSE(cunit1 == cunit2adj)
                << "Error with false per meter3 comparison 1 inv index " << ii << "," << jj;
            EXPECT_FALSE(cunit1adj == cunit2)
                << "Error with false per meter3 comparison 2 index " << ii << "," << jj;
            EXPECT_FALSE(cunit1adj == cunit2adj)
                << "Error with false per meter3 comparison 2 per meter3 index " << ii << "," << jj;
        }
        EXPECT_TRUE(cunit1adj * precise::m.pow(3) == cunit1)
            << "Error with true per meter3 multiply" << ii;
    }
}

TEST(customUnits, uniquenessperkg)
{
    for (unsigned short ii = 0; ii < 1024; ++ii) {
        auto cunit1 = precise::generate_custom_unit(ii);
        auto cunit1adj = cunit1 / precise::kg;
        EXPECT_EQ(precise::custom::custom_unit_number(cunit1adj.base_units()), ii);
        EXPECT_EQ(precise::custom::custom_unit_number(cunit1adj.inv().base_units()), ii);
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit1.base_units()))
            << "Error with custom unit/kg detection " << ii;
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit1adj.base_units()))
            << "Error with custom unit/kg detection of inverse " << ii;
        for (unsigned short jj = 0; jj < 1024; ++jj) {
            if (ii == jj) {
                continue;
            }
            auto cunit2 = precise::generate_custom_unit(jj);
            auto cunit2adj = cunit2 / precise::kg;
            EXPECT_FALSE(cunit1 == cunit2adj)
                << "Error with false per kg comparison 1 inv index " << ii << "," << jj;
            EXPECT_FALSE(cunit1adj == cunit2)
                << "Error with false per kg comparison 2 index " << ii << "," << jj;
            EXPECT_FALSE(cunit1adj == cunit2adj)
                << "Error with false per kg comparison 2 per kg index " << ii << "," << jj;
        }
        EXPECT_TRUE(cunit1adj * precise::kg == cunit1) << "Error with true per kg multiply" << ii;
    }
}

TEST(customUnits, uniquenesspersecond)
{
    for (unsigned short ii = 0; ii < 1024; ++ii) {
        auto cunit1 = precise::generate_custom_unit(ii);
        auto cunit1adj = cunit1 / precise::s;
        EXPECT_EQ(precise::custom::custom_unit_number(cunit1adj.base_units()), ii);
        EXPECT_EQ(precise::custom::custom_unit_number(cunit1adj.inv().base_units()), ii);
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit1.base_units()))
            << "Error with custom unit/s detection " << ii;
        EXPECT_TRUE(precise::custom::is_custom_unit(cunit1adj.base_units()))
            << "Error with custom unit/s detection of inverse " << ii;
        for (unsigned short jj = 0; jj < 1024; ++jj) {
            if (ii == jj) {
                continue;
            }
            auto cunit2 = precise::generate_custom_unit(jj);
            auto cunit2adj = cunit2 / precise::s;
            EXPECT_FALSE(cunit1 == cunit2adj)
                << "Error with false per sec comparison 1 inv index " << ii << "," << jj;
            EXPECT_FALSE(cunit1adj == cunit2)
                << "Error with false per sec comparison 2 index " << ii << "," << jj;
            EXPECT_FALSE(cunit1adj == cunit2adj)
                << "Error with false per sec comparison 2 per sec index " << ii << "," << jj;
        }
        EXPECT_TRUE(cunit1adj * precise::s == cunit1) << "Error with true per sec multiply" << ii;
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
    for (unsigned short ii = 0; ii < 16; ++ii) {
        auto cunit1 = precise::generate_custom_count_unit(ii);
        EXPECT_TRUE(precise::custom::is_custom_count_unit(cunit1.base_units()));
        EXPECT_FALSE(precise::custom::is_custom_count_unit_inverted(cunit1.base_units()));

        EXPECT_FALSE(precise::custom::is_custom_unit(cunit1.base_units()));
        auto cunit2 = cunit1.inv();
        EXPECT_TRUE(precise::custom::is_custom_count_unit(cunit2.base_units()));
        EXPECT_TRUE(precise::custom::is_custom_count_unit_inverted(cunit2.base_units()));
        EXPECT_FALSE(precise::custom::is_custom_unit(cunit2.base_units()));
        auto cunit3 = cunit2.inv();
        EXPECT_TRUE(precise::custom::is_custom_count_unit(cunit3.base_units()));
        EXPECT_FALSE(precise::custom::is_custom_count_unit_inverted(cunit3.base_units()));

        EXPECT_FALSE(cunit1 == cunit2) << "Error with false comparison 1 index " << ii;
        EXPECT_FALSE(cunit2 == cunit3) << "Error with false comparison 2 index " << ii;
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
    for (unsigned short ii = 0; ii < 16; ++ii) {
        auto cunit1 = precise::generate_custom_count_unit(ii);
        auto cunit1inv = cunit1.inv();

        EXPECT_EQ(precise::custom::custom_count_unit_number(cunit1.base_units()), ii);
        EXPECT_EQ(precise::custom::custom_count_unit_number(cunit1.base_units()), ii);
        for (unsigned short jj = 0; jj < 16; ++jj) {
            if (ii == jj) {
                continue;
            }
            auto cunit2 = precise::generate_custom_count_unit(jj);
            EXPECT_FALSE(cunit1 == cunit2)
                << "Error with false comparison 1 index " << ii << "," << jj;
            EXPECT_FALSE(cunit1 == cunit2.inv())
                << "Error with false comparison 1 inv index " << ii << "," << jj;
            EXPECT_FALSE(cunit1inv == cunit2)
                << "Error with false inv comparison 2 index " << ii << "," << jj;
            EXPECT_FALSE(cunit1inv == cunit2.inv())
                << "Error with false inv comparison 2 inv index " << ii << "," << jj;
        }
    }
}
