/*
Copyright (c) 2019-2025,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "test.hpp"
#include "units/units.hpp"

#include <memory>
#include <string>

using namespace units;
TEST(uncertainOps, construction)
{
    uncertain_measurement um1(5.0F, 0.01F, m);

    EXPECT_FLOAT_EQ(um1.value(), 5.0F);
    EXPECT_FLOAT_EQ(um1.uncertainty(), 0.01F);
    EXPECT_EQ(um1.units(), m);

    measurement m1(7.0, in);
    uncertain_measurement um2(m1, 0.03F);
    EXPECT_FLOAT_EQ(um2.value(), 7.0F);
    EXPECT_FLOAT_EQ(um2.uncertainty(), 0.03F);
    EXPECT_EQ(um2.units(), in);

    measurement mk(10.0, km);
    measurement mku(10.0, m);
    uncertain_measurement umk(mk, mku);

    EXPECT_FLOAT_EQ(umk.value(), 10.0F);
    EXPECT_FLOAT_EQ(umk.uncertainty(), 0.01F);
    EXPECT_EQ(umk.units(), km);

    uncertain_measurement um3(um1);
    EXPECT_DOUBLE_EQ(um1.value(), um3.value());
    EXPECT_DOUBLE_EQ(um1.uncertainty(), um3.uncertainty());
    EXPECT_EQ(um1.units(), um3.units());

    uncertain_measurement um4 = um2;
    EXPECT_DOUBLE_EQ(um2.value(), um4.value());
    EXPECT_DOUBLE_EQ(um2.uncertainty(), um4.uncertainty());
    EXPECT_EQ(um2.units(), um4.units());

    uncertain_measurement um6(7.0, 0.05, kg);

    EXPECT_FLOAT_EQ(um6.value(), 7.0F);
    EXPECT_FLOAT_EQ(um6.uncertainty(), 0.05F);
    EXPECT_EQ(um6.units(), kg);

    uncertain_measurement um7(m1, 0.1);

    EXPECT_FLOAT_EQ(um7.value(), m1.value());
    EXPECT_FLOAT_EQ(um7.uncertainty(), 0.1F);
    EXPECT_EQ(um7.units(), m1.units());

    uncertain_measurement um8(2.3F, kg);
    EXPECT_FLOAT_EQ(um8.value(), 2.3F);
    EXPECT_FLOAT_EQ(um8.uncertainty(), 0.0F);
    EXPECT_EQ(um8.units(), kg);

    uncertain_measurement um9(2.3, kg);
    EXPECT_FLOAT_EQ(um9.value(), 2.3F);
    EXPECT_FLOAT_EQ(um9.uncertainty(), 0.0F);
    EXPECT_EQ(um9.units(), kg);

    uncertain_measurement um10;
    EXPECT_FLOAT_EQ(um10.value(), 0.0F);
    EXPECT_FLOAT_EQ(um10.uncertainty(), 0.0F);
    EXPECT_EQ(um10.units(), one);
    EXPECT_TRUE(is_valid(um10));
}

// from
// https://www2.southeastern.edu/Academics/Faculty/rallain/plab194/error.html
TEST(uncertainOps, equality)
{
    uncertain_measurement um1(0.86, 0.02, s);

    uncertain_measurement um2(980.0, 20.0, ms);

    EXPECT_FALSE(um1 == um2);
    EXPECT_TRUE(um1 != um2);

    EXPECT_FALSE(um2 == um1);
    EXPECT_TRUE(um2 != um1);

    uncertain_measurement um3(0.86, 0.08, s);

    uncertain_measurement um4(980.0, 80.0, ms);

    EXPECT_TRUE(um3 == um4);
    EXPECT_FALSE(um3 != um4);
    EXPECT_TRUE(um4 == um3);
    EXPECT_FALSE(um4 != um3);
}

// Example Calculations from
// http://www.geol.lsu.edu/jlorenzo/geophysics/uncertainties/Uncertaintiespart2.html

TEST(uncertainOps, addsubtract)
{
    uncertain_measurement x(2.0, 0.2, cm);
    uncertain_measurement y(3.0, 0.6, cm);
    uncertain_measurement w(4.52, 0.02, cm);

    auto z = x.simple_add(y).simple_subtract(w);
    EXPECT_NEAR(z.value(), 0.5, 0.05);
    EXPECT_NEAR(z.uncertainty(), 0.8, 0.05);

    auto zs = x + y - w;
    EXPECT_NEAR(zs.value(), 0.5F, 0.05);
    EXPECT_NEAR(zs.uncertainty(), 0.6, 0.05);
}

TEST(uncertainOps, unaryOps)
{
    uncertain_measurement x(2.0, 0.2, cm);

    auto z = -x;
    auto y = +x;
    EXPECT_EQ(y, x);
    EXPECT_EQ(z.value(), -x.value());
    EXPECT_EQ(z.uncertainty(), x.uncertainty());
    EXPECT_EQ(z.units(), x.units());
}

TEST(uncertainOps, multConst)
{
    uncertain_measurement x(3.0, 0.2, cm);
    double Ck = 2.0 * constants::pi;

    auto z = Ck * x;
    EXPECT_NEAR(z.value(), 18.8, 0.05);
    EXPECT_NEAR(z.uncertainty(), 1.3, 0.05);

    auto z2 = x * Ck;
    EXPECT_NEAR(z2.value(), 18.8, 0.05);
    EXPECT_NEAR(z2.uncertainty(), 1.3, 0.05);

    auto zf = 10.0F * x;
    EXPECT_FLOAT_EQ(zf.value(), 30.0F);
    EXPECT_FLOAT_EQ(zf.uncertainty(), 2.0F);

    auto zf2 = x * 10.0F;
    EXPECT_FLOAT_EQ(zf2.value(), 30.0F);
    EXPECT_FLOAT_EQ(zf2.uncertainty(), 2.0F);

    auto zd = 10.0 * x;
    EXPECT_FLOAT_EQ(zd.value(), 30.0F);
    EXPECT_FLOAT_EQ(zd.uncertainty(), 2.0F);

    auto zd2 = x * 10.0;
    EXPECT_FLOAT_EQ(zd2.value(), 30.0F);
    EXPECT_FLOAT_EQ(zd2.uncertainty(), 2.0F);
}

TEST(uncertainOps, mult)
{
    uncertain_measurement w(4.52, 0.02, cm);
    uncertain_measurement x(2.0, 0.2, cm);

    auto zs = w.simple_product(x);
    EXPECT_NEAR(zs.value(), 9.04, 0.005);
    EXPECT_NEAR(zs.uncertainty(), 0.944, 0.0005);
    EXPECT_EQ(zs.units(), cm.pow(2));

    auto z = w = w * x;
    EXPECT_NEAR(z.value(), 9.04, 0.005);
    EXPECT_NEAR(z.uncertainty(), 0.905, 0.0005);
    EXPECT_EQ(z.units(), cm.pow(2));

    auto z2 = x * cm;
    EXPECT_FLOAT_EQ(z2.value(), 2.0F);
    EXPECT_FLOAT_EQ(z2.uncertainty(), 0.2F);
    EXPECT_EQ(z2.units(), cm.pow(2));
}

TEST(uncertainOps, divConst)
{
    uncertain_measurement x(3.0, 0.2, cm);

    auto zf = x / 10.0F;
    EXPECT_FLOAT_EQ(zf.value(), 0.3F);
    EXPECT_FLOAT_EQ(zf.uncertainty(), 0.02F);

    auto zf2 = 6.0F / x;
    EXPECT_FLOAT_EQ(zf2.value(), 2.0F);
    EXPECT_FLOAT_EQ(zf2.uncertainty(), 0.4F / 3.0F);
    EXPECT_TRUE(zf2.units() == cm.inv());

    auto zd = x / 10.0;
    EXPECT_FLOAT_EQ(zd.value(), 0.3F);
    EXPECT_FLOAT_EQ(zd.uncertainty(), 0.02F);

    auto zd2 = 6.0 / x;
    EXPECT_FLOAT_EQ(zd2.value(), 2.0F);
    EXPECT_FLOAT_EQ(zd2.uncertainty(), 0.4F / 3.0F);
    EXPECT_TRUE(zd2.units() == cm.inv());
}

TEST(uncertainOps, div)
{
    uncertain_measurement w(4.8, 0.02, cm);
    measurement x(2.0, cm);

    auto z = w / x;
    EXPECT_FLOAT_EQ(z.value(), 2.4F);
    EXPECT_FLOAT_EQ(z.uncertainty(), 0.01F);
    EXPECT_EQ(z.units(), one);

    auto z2 = w / cm;
    EXPECT_FLOAT_EQ(z2.value(), 4.8F);
    EXPECT_FLOAT_EQ(z2.uncertainty(), 0.02F);
    EXPECT_EQ(z2.units(), one);

    uncertain_measurement wb(10.0, 1.0, cm);
    measurement xb(1.0, cm);

    auto z3 = xb / wb;

    EXPECT_FLOAT_EQ(z3.value(), 0.1F);
    EXPECT_FLOAT_EQ(z3.uncertainty(), 0.01F);
    EXPECT_EQ(z3.units(), one);
}

TEST(uncertainOps, measAddSub)
{
    uncertain_measurement w(4.8, 0.02, cm);
    measurement x(0.02, m);

    auto z = w + x;
    EXPECT_FLOAT_EQ(z.value(), 6.8F);
    EXPECT_FLOAT_EQ(z.uncertainty(), 0.02F);
    EXPECT_EQ(z.units(), cm);

    auto z2 = w - x;
    EXPECT_FLOAT_EQ(z2.value(), 2.8F);
    EXPECT_FLOAT_EQ(z2.uncertainty(), 0.02F);
    EXPECT_EQ(z2.units(), cm);

    auto z3 = x + w;
    EXPECT_FLOAT_EQ(z3.value(), 0.068F);
    EXPECT_FLOAT_EQ(z3.uncertainty(), 0.0002F);
    EXPECT_EQ(z3.units(), m);

    auto z4 = x - w;
    EXPECT_FLOAT_EQ(z4.value(), -0.028F);
    EXPECT_FLOAT_EQ(z4.uncertainty(), 0.0002F);
    EXPECT_EQ(z4.units(), m);
}

TEST(uncertainOps, conversion)
{
    uncertain_measurement w(4.8, 0.02, cm);
    auto wc = w.convert_to(m);

    EXPECT_FLOAT_EQ(wc.value(), 0.048F);
    EXPECT_FLOAT_EQ(wc.uncertainty(), 0.0002F);
    EXPECT_EQ(wc.units(), m);
}

#ifndef UNITS_HEADER_ONLY
// http://www.geol.lsu.edu/jlorenzo/geophysics/uncertainties/Uncertaintiespart2.html
// example c as a note the example on this page is just wrong, the calculations
// don't match the numbers given
TEST(uncertainOps, pow1)
{
    uncertain_measurement w(4.52, 0.02, cm);
    uncertain_measurement y(3.0, 0.6, cm);
    uncertain_measurement Av(2.0, 0.2, cm.pow(2));

    auto z = w * pow(y, 2) / root(Av, 2);
    EXPECT_NEAR(z.value(), 29, 0.5);
    EXPECT_NEAR(z.uncertainty(), 12, 0.5);

    auto z2 = w * pow(y, 2) / sqrt(Av);
    EXPECT_NEAR(z2.value(), 29, 0.5);
    EXPECT_NEAR(z2.uncertainty(), 12, 0.5);

    auto zs = w.simple_product(pow(y, 2)).simple_divide(root(Av, 2));
    EXPECT_NEAR(zs.value(), 28.765, 0.0005);
    EXPECT_NEAR(zs.uncertainty(), 13.07, 0.005);
}
#endif

// examples from http://lectureonline.cl.msu.edu/~mmp/labs/error/e2.htm

TEST(uncertainOps, example1)
{
    uncertain_measurement x1(9.3, 0.2, m);
    uncertain_measurement x2(14.4, 0.3, m);

    auto z = x2 - x1;
    EXPECT_NEAR(z.value(), 5.1, 0.05);
    EXPECT_NEAR(z.uncertainty(), 0.36, 0.005);

    auto zs = x2.simple_subtract(x1);
    EXPECT_NEAR(zs.value(), 5.1, 0.05);
}

TEST(uncertainOps, example2)
{
    uncertain_measurement x(5.1, 0.4, m);
    uncertain_measurement t(0.4, 0.1, s);

    auto v = x / t;
    EXPECT_NEAR(v.value(), 12.75, 0.005);
    EXPECT_NEAR(v.uncertainty(), 3.34, 0.005);

    auto vs = x.simple_divide(t);
    EXPECT_NEAR(vs.value(), 12.75, 0.005);
}

// next two examples from
// https://chem.libretexts.org/Bookshelves/Analytical_Chemistry/Supplemental_Modules_(Analytical_Chemistry)/Quantifying_Nature/Significant_Digits/Propagation_of_Error
TEST(uncertainOps, chemExample1)
{
    uncertain_measurement conc(13.7, 0.3, mol / L);
    uncertain_measurement path(1.0, 0.1, cm);
    uncertain_measurement absorb(0.172807, 0.000008, one);

    auto eps = absorb / (conc * path);
    EXPECT_NEAR(eps.value(), 0.013, 0.005);
    EXPECT_NEAR(eps.uncertainty(), 0.001, 0.0005);
}

TEST(uncertainOps, testuncertaintySetters)
{
    uncertain_measurement um1(13.71, 0.05, ton);

    // this tests chaining
    um1.uncertainty(0.08).uncertainty(0.07F);
    EXPECT_FLOAT_EQ(um1.uncertainty(), 0.07F);
    EXPECT_FLOAT_EQ(um1.uncertainty_as(ton), 0.07F);
    um1.uncertainty(25 * kg);
    EXPECT_FLOAT_EQ(um1.uncertainty_as(kg), 25.0F);
}

TEST(uncertainOps, testcomparison)
{
    uncertain_measurement um1(13.71, ton);

    auto m1 = ton * 13.71;
    EXPECT_TRUE(m1 == um1);
    EXPECT_TRUE(um1 == m1);

    EXPECT_FALSE(m1 != um1);
    EXPECT_FALSE(um1 != um1);

    // the multiplier is purposeful to get in the range float can represent
    // but below the tolerance of measurement
    m1 = m1 + 0.000001 * ton;

    EXPECT_TRUE(m1 == um1);
    EXPECT_TRUE(um1 == m1);

    EXPECT_TRUE(m1 > um1);
    EXPECT_FALSE(um1 > m1);

    EXPECT_FALSE(m1 < um1);
    EXPECT_TRUE(um1 < m1);

    EXPECT_TRUE(m1 >= um1);
    EXPECT_TRUE(um1 >= m1);

    EXPECT_TRUE(um1 <= m1);
    EXPECT_TRUE(m1 <= um1);

    uncertain_measurement um2(m1, 0.0000005);
    EXPECT_FALSE(um2 == um1);
    EXPECT_FALSE(um1 == um2);

    EXPECT_TRUE(um2 > um1);
    EXPECT_FALSE(um1 > um2);

    EXPECT_FALSE(um2 < um1);
    EXPECT_TRUE(um1 < um2);

    EXPECT_TRUE(um2 >= um1);
    EXPECT_FALSE(um1 >= um2);

    EXPECT_TRUE(um1 <= um2);
    EXPECT_FALSE(um2 <= um1);
}

// Next 2 Examples from
// http://ipl.physics.harvard.edu/wp-uploads/2013/03/PS3_Error_Propagation_sp13.pdf

TEST(uncertainOps, testInv)
{
    uncertain_measurement Tp(0.2, 0.01, s);
    auto f = 1 / Tp;
    EXPECT_NEAR(f.uncertainty(), 0.25, 0.005);
    EXPECT_NEAR(f.value(), 5.0, 0.05);
    EXPECT_EQ(f.units(), s.inv());
}

TEST(uncertainOps, testHeight)
{
    uncertain_measurement v0(4.0, 0.2, m / s);
    uncertain_measurement t(0.6, 0.06, s);
    measurement gc = 9.8 * m / s.pow(2);

    auto y = v0.simple_product(t).simple_subtract(0.5 * gc * pow(t, 2));

    auto ys = v0 * t - 0.5 * gc * pow(t, 2);

    EXPECT_NEAR(y.uncertainty(), 0.712, 0.005);
    EXPECT_NEAR(y.value(), 0.636, 0.0005);
    EXPECT_EQ(y.units(), m);

    EXPECT_NEAR(ys.uncertainty(), 0.44, 0.005);
    EXPECT_NEAR(ys.value(), 0.636, 0.0005);
    EXPECT_EQ(ys.units(), m);
}

#ifndef UNITS_HEADER_ONLY
TEST(uncertainStrings, fromString)
{
    auto um1 = uncertain_measurement_from_string("12+/-3 m");
    EXPECT_EQ(um1.value(), 12.0);
    EXPECT_EQ(um1.uncertainty(), 3.0);
    EXPECT_EQ(um1.units(), m);

    auto um2 = uncertain_measurement_from_string("12m +-3 in");
    EXPECT_EQ(um2.value(), 12.0);
    EXPECT_TRUE(um2.uncertainty_measurement() == 3.0 * in);
    EXPECT_EQ(um2.units(), m);

    auto um3 = uncertain_measurement_from_string("12 m +-3");
    EXPECT_EQ(um3.value(), 12.0);
    EXPECT_TRUE(um3.uncertainty_measurement() == 3.0 * m);
    EXPECT_EQ(um3.units(), m);

    std::string ustr = "12\xB1 3 m";
    auto um4 = uncertain_measurement_from_string(ustr);
    EXPECT_EQ(um4.value(), 12.0);
    EXPECT_TRUE(um4.uncertainty_measurement() == 3.0 * m);
    EXPECT_EQ(um4.units(), m);

    auto um5 = uncertain_measurement_from_string("12.8kg");
    EXPECT_FLOAT_EQ(um5.value(), 12.8F);
    EXPECT_EQ(um5.uncertainty(), 0.0);
    EXPECT_EQ(um5.units(), kg);
    EXPECT_TRUE(isnormal(um5));

    uncertain_measurement um6;
    EXPECT_NO_THROW(um6 = uncertain_measurement_from_string(""));
    EXPECT_EQ(um6.uncertainty(), 0.0);
    EXPECT_EQ(um6.value(), 0.0);
}

TEST(uncertainStrings, fromStringConcise)
{
    auto um5 = uncertain_measurement_from_string("4.563(4) m");
    EXPECT_FLOAT_EQ(um5.value(), 4.563F);
    EXPECT_EQ(um5.uncertainty(), 0.004F);
    EXPECT_EQ(um5.units(), m);

    auto um6 = uncertain_measurement_from_string("4.56323(45)x10^-12 kg");
    EXPECT_FLOAT_EQ(um6.value(), 4.56323e-12F);
    EXPECT_EQ(um6.uncertainty(), 0.00045e-12F);
    EXPECT_EQ(um6.units(), kg);

    auto um7 = uncertain_measurement_from_string("9.1093837015(28)x10-31 kg");
    EXPECT_EQ(um7, constants::uncertain::me);
    EXPECT_DOUBLE_EQ(um7.uncertainty(), constants::uncertain::me.uncertainty());
}
TEST(uncertainStrings, toString)
{
    uncertain_measurement um1(10.0, 0.4, m);
    auto str = to_string(um1);
    EXPECT_EQ(str, "10+/-0.4 m");
}

TEST(uncertainStrings, toStringReduce)
{
    uncertain_measurement um1(10.897, 0.4, m);
    auto str = to_string(um1);
    EXPECT_EQ(str, "10.9+/-0.4 m");

    uncertain_measurement um2(10.848, 0.04, m);
    str = to_string(um2);
    EXPECT_EQ(str, "10.85+/-0.04 m");

    uncertain_measurement um3(10.848e-7, 0.041e-7, m);
    str = to_string(um3);
    EXPECT_EQ(str, "1.085e-06+/-4.1e-09 m");

    uncertain_measurement um4(335.0, 100.0, m);
    str = to_string(um4);
    EXPECT_EQ(str, "3.4e+02+/-1e+02 m");
}
#endif

TEST(uncertainOps, fractionalUncertainty)
{
    uncertain_measurement v0(10.0, 1.0, V);

    EXPECT_FLOAT_EQ(v0.fractional_uncertainty(), 0.1F);

    uncertain_measurement v1(-10.0, 1.0, V);

    EXPECT_FLOAT_EQ(v1.fractional_uncertainty(), 0.1F);
}

TEST(uncertainOps, conversions)
{
    uncertain_measurement v0(10.0, 1.0, V);

    precise_measurement vm(v0);
    EXPECT_DOUBLE_EQ(vm.value(), 10.0);
    EXPECT_TRUE(vm.units() == precise::V);
}

TEST(uncertainOps, invalid)
{
    uncertain_measurement iv1(1.2, 0.3, invalid);
    EXPECT_FALSE(is_valid(iv1));
    EXPECT_FALSE(isnormal(iv1));

    uncertain_measurement iv2(constants::invalid_conversion, 0.4, m);
    EXPECT_FALSE(is_valid(iv2));
    EXPECT_FALSE(isnormal(iv2));

    uncertain_measurement iv3(2.7, constants::invalid_conversion, m);
    EXPECT_FALSE(is_valid(iv3));
    EXPECT_FALSE(isnormal(iv3));

    uncertain_measurement iv4(constants::infinity, 0.5, m);
    EXPECT_TRUE(is_valid(iv4));
    EXPECT_FALSE(isnormal(iv4));

    uncertain_measurement iv5(1e-41, 0.5, m);
    EXPECT_TRUE(is_valid(iv5));
    EXPECT_FALSE(isnormal(iv5));

    uncertain_measurement iv6(0, 1e-23, m);
    EXPECT_TRUE(is_valid(iv6));
    EXPECT_TRUE(isnormal(iv6));

    uncertain_measurement iv7(1e-21, constants::infinity, m);
    EXPECT_TRUE(is_valid(iv7));
    EXPECT_FALSE(isnormal(iv7));

    uncertain_measurement iv8(1e-21, 1e-41, m);
    EXPECT_TRUE(is_valid(iv8));
    EXPECT_FALSE(isnormal(iv8));
}

TEST(uncertainOps, cast)
{
    uncertain_measurement v0(10.0, 1.0, V);
    EXPECT_TRUE(measurement_cast(v0) == 10.0 * V);
    static_assert(
        std::is_same<decltype(measurement_cast(v0)), measurement>::value,
        "uncertain measurement cast not working properly");

    auto* v1 = new uncertain_measurement(10.0F, V);
    EXPECT_TRUE(*v1 == 10.0 * V);
    delete v1;

    auto* v2 = new uncertain_measurement(10.0, V);
    EXPECT_TRUE(*v2 == 10.0 * V);
    delete v2;
}
