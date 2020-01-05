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