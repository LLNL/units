/*
Copyright (c) 2019,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "test.hpp"
#include "units/units.hpp"

#include <memory>

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
}

// from https://www2.southeastern.edu/Academics/Faculty/rallain/plab194/error.html
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

// Example Calculations from http://www.geol.lsu.edu/jlorenzo/geophysics/uncertainties/Uncertaintiespart2.html

TEST(uncertainOps, addsubtract)
{
    uncertain_measurement x(2.0, 0.2, cm);
    uncertain_measurement y(3.0, 0.6, cm);
    uncertain_measurement w(4.52, 0.02, cm);

    auto z = x + y - w;
    EXPECT_NEAR(z.value(), 0.5, 0.05);
    EXPECT_NEAR(z.uncertainty(), 0.8, 0.05);

    auto zs = x.rss_add(y).rss_subtract(w);
    EXPECT_NEAR(zs.value(), 0.5F, 0.05);
    EXPECT_NEAR(zs.uncertainty(), 0.6, 0.05);
}

TEST(uncertainOps, multConst)
{
    uncertain_measurement x(3.0, 0.2, cm);
    double Ck = 2.0 * constants::pi;

    auto z = Ck * x;
    EXPECT_NEAR(z.value(), 18.8, 0.05);
    EXPECT_NEAR(z.uncertainty(), 1.3, 0.05);
}

TEST(uncertainOps, mult)
{
    uncertain_measurement w(4.52, 0.02, cm);
    uncertain_measurement x(2.0, 0.2, cm);

    auto z = w * x;
    EXPECT_NEAR(z.value(), 9.04, 0.005);
    EXPECT_NEAR(z.uncertainty(), 0.944, 0.0005);
    EXPECT_EQ(z.units(), cm.pow(2));

    auto zs = w.rss_product(x);
    EXPECT_NEAR(zs.value(), 9.04, 0.005);
    EXPECT_NEAR(zs.uncertainty(), 0.905, 0.0005);
    EXPECT_EQ(zs.units(), cm.pow(2));
}

// http://www.geol.lsu.edu/jlorenzo/geophysics/uncertainties/Uncertaintiespart2.html example c
// as a note the example on this page is just wrong, the calculations don't match the numbers given
TEST(uncertainOps, pow1)
{
    uncertain_measurement w(4.52, 0.02, cm);
    uncertain_measurement y(3.0, 0.6, cm);
    uncertain_measurement Av(2.0, 0.2, cm.pow(2));

    auto z = w * y.pow(2) / Av.root(2);
    EXPECT_NEAR(z.value(), 28.765, 0.0005);
    EXPECT_NEAR(z.uncertainty(), 13.07, 0.005);

    auto z2 = w * y.pow(2) / sqrt(Av);
    EXPECT_NEAR(z2.value(), 28.765, 0.0005);
    EXPECT_NEAR(z2.uncertainty(), 13.07, 0.005);

    auto zs = w.rss_product(y.pow(2)).rss_divide(Av.root(2));
    EXPECT_NEAR(zs.value(), 29, 0.5);
    EXPECT_NEAR(zs.uncertainty(), 12, 0.5);
}

// examples from http://lectureonline.cl.msu.edu/~mmp/labs/error/e2.htm

TEST(uncertainOps, example1)
{
    uncertain_measurement x1(9.3, 0.2, m);
    uncertain_measurement x2(14.4, 0.3, m);

    auto z = x2 - x1;
    EXPECT_NEAR(z.value(), 5.1, 0.05);

    auto zs = x2.rss_subtract(x1);
    EXPECT_NEAR(zs.value(), 5.1, 0.05);
    EXPECT_NEAR(zs.uncertainty(), 0.36, 0.005);
}

TEST(uncertainOps, example2)
{
    uncertain_measurement x(5.1, 0.4, m);
    uncertain_measurement t(0.4, 0.1, s);

    auto v = x / t;
    EXPECT_NEAR(v.value(), 12.75, 0.005);

    auto vs = x.rss_divide(t);
    EXPECT_NEAR(vs.value(), 12.75, 0.005);
    EXPECT_NEAR(vs.uncertainty(), 3.34, 0.005);
}

// next two examples from https://chem.libretexts.org/Bookshelves/Analytical_Chemistry/Supplemental_Modules_(Analytical_Chemistry)/Quantifying_Nature/Significant_Digits/Propagation_of_Error
TEST(uncertainOps, chemExample1)
{
    uncertain_measurement conc(13.7, 0.3, mol / L);
    uncertain_measurement path(1.0, 0.1, cm);
    uncertain_measurement absorb(0.172807, 0.000008, one);

    auto eps = absorb.rss_divide(conc.rss_product(path));
    EXPECT_NEAR(eps.value(), 0.013, 0.005);
    EXPECT_NEAR(eps.uncertainty(), 0.001, 0.0005);
}

TEST(uncertainOps, testuncertaintySetters)
{
    uncertain_measurement um1(13.71, 0.05, ton);

    //this tests chaining
    um1.uncertainty(0.08).uncertainty(0.07F);
    EXPECT_FLOAT_EQ(um1.uncertainty(), 0.07F);
    um1.uncertainty(25 * kg);
    EXPECT_FLOAT_EQ(um1.uncertainty_as(kg), 25.0);
}

TEST(uncertainOps, testcomparison)
{
    uncertain_measurement um1(13.71, ton);

    auto m1 = ton * 13.71;
    EXPECT_TRUE(m1 == um1);
    EXPECT_TRUE(um1 == m1);

    EXPECT_FALSE(m1 != um1);
    EXPECT_FALSE(um1 != um1);

    //the multiplier is purposeful to get in the range float can represent
    //but below the tolerance of measurement
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

// Next 2 Examples from http://ipl.physics.harvard.edu/wp-uploads/2013/03/PS3_Error_Propagation_sp13.pdf

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

    auto y = v0 * t - 0.5 * gc * t.pow(2);

    auto ys = v0.rss_product(t).rss_subtract(0.5 * gc * t.pow(2));

    EXPECT_NEAR(y.uncertainty(), 0.712, 0.005);
    EXPECT_NEAR(y.value(), 0.636, 0.0005);
    EXPECT_EQ(y.units(), m);

    EXPECT_NEAR(ys.uncertainty(), 0.44, 0.005);
    EXPECT_NEAR(ys.value(), 0.636, 0.0005);
    EXPECT_EQ(ys.units(), m);
}