/*
Copyright (c) 2019-2025,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "test.hpp"
#include "units/units_math.hpp"

using namespace units;
TEST(mathOps, typeTraits)
{
    EXPECT_TRUE(is_measurement<uncertain_measurement>::value);
    EXPECT_TRUE(is_measurement<measurement>::value);
    EXPECT_TRUE(is_measurement<precise_measurement>::value);
    EXPECT_TRUE(is_measurement<fixed_measurement>::value);
    EXPECT_TRUE(is_measurement<fixed_precise_measurement>::value);
}

TEST(mathOps, uncertainMeas)
{
    uncertain_measurement um1(5.69F, 0.01F, m.pow(3));

    auto um2 = cbrt(um1);
    EXPECT_EQ(um2, root(um1, 3));

    auto um3 = floor(um1);
    EXPECT_FLOAT_EQ(um3.value_f(), 5.0F);

    auto um4 = trunc(um1);
    EXPECT_FLOAT_EQ(um4.value_f(), 5.0F);

    auto um5 = ceil(um1);
    EXPECT_FLOAT_EQ(um5.value_f(), 6.0F);

    auto um6 = round(um1);
    EXPECT_FLOAT_EQ(um6.value_f(), 6.0F);
}

TEST(mathOps, meas)
{
    measurement um1(5.69, m.pow(3));

    auto um2 = cbrt(um1);
    EXPECT_EQ(um2, root(um1, 3));

    auto um3 = floor(um1);
    EXPECT_DOUBLE_EQ(um3.value(), 5.0);

    auto um4 = trunc(um1);
    EXPECT_DOUBLE_EQ(um4.value(), 5.0);

    auto um5 = ceil(um1);
    EXPECT_DOUBLE_EQ(um5.value(), 6.0);

    auto um6 = round(um1);
    EXPECT_DOUBLE_EQ(um6.value(), 6.0);
}

TEST(mathOps, fixedMeas)
{
    fixed_measurement um1(5.69, m.pow(3));

    auto um2 = cbrt(um1);
    EXPECT_EQ(um2, root(um1, 3));

    auto um3 = floor(um1);
    EXPECT_FLOAT_EQ(um3.value(), 5.0F);

    auto um4 = trunc(um1);
    EXPECT_FLOAT_EQ(um4.value(), 5.0F);

    auto um5 = ceil(um1);
    EXPECT_FLOAT_EQ(um5.value(), 6.0F);

    auto um6 = round(um1);
    EXPECT_FLOAT_EQ(um6.value(), 6.0F);
}

TEST(mathOps, preciseMeas)
{
    precise_measurement um1(5.69, precise::m.pow(3));

    auto um2 = cbrt(um1);
    EXPECT_EQ(um2, root(um1, 3));

    auto um3 = floor(um1);
    EXPECT_FLOAT_EQ(um3.value(), 5.0F);

    auto um4 = trunc(um1);
    EXPECT_FLOAT_EQ(um4.value(), 5.0F);

    auto um5 = ceil(um1);
    EXPECT_FLOAT_EQ(um5.value(), 6.0F);

    auto um6 = round(um1);
    EXPECT_FLOAT_EQ(um6.value(), 6.0F);
}

TEST(mathOps, preciseFixedMeas)
{
    fixed_precise_measurement um1(5.69, precise::m.pow(3));

    auto um2 = cbrt(um1);
    EXPECT_EQ(um2, root(um1, 3));

    auto um3 = floor(um1);
    EXPECT_FLOAT_EQ(um3.value(), 5.0F);

    auto um4 = trunc(um1);
    EXPECT_FLOAT_EQ(um4.value(), 5.0F);

    auto um5 = ceil(um1);
    EXPECT_FLOAT_EQ(um5.value(), 6.0F);

    auto um6 = round(um1);
    EXPECT_FLOAT_EQ(um6.value(), 6.0F);
}

TEST(mathOps, trigSin)
{
    measurement m1(constants::pi / 4.0, rad);
    EXPECT_FLOAT_EQ(sin(m1), sin(constants::pi / 4.0));

    measurement m2(45.0, deg);
    EXPECT_FLOAT_EQ(sin(m2), sin(constants::pi / 4.0));

    precise_measurement m3(constants::pi / 4.0, precise::rad);
    EXPECT_DOUBLE_EQ(sin(m3), sin(constants::pi / 4.0));

    precise_measurement m4(45.0, precise::deg);
    EXPECT_DOUBLE_EQ(sin(m4), sin(constants::pi / 4.0));
}

TEST(mathOps, trigCos)
{
    measurement m1(constants::pi / 4.0, rad);
    EXPECT_FLOAT_EQ(cos(m1), cos(constants::pi / 4.0));

    measurement m2(45.0, deg);
    EXPECT_FLOAT_EQ(cos(m2), cos(constants::pi / 4.0));

    precise_measurement m3(constants::pi / 4.0, precise::rad);
    EXPECT_DOUBLE_EQ(cos(m3), cos(constants::pi / 4.0));

    precise_measurement m4(45.0, precise::deg);
    EXPECT_DOUBLE_EQ(cos(m4), cos(constants::pi / 4.0));
}

TEST(mathOps, trigTan)
{
    measurement m1(constants::pi / 6.0, rad);
    EXPECT_FLOAT_EQ(tan(m1), tan(constants::pi / 6.0));

    measurement m2(30.0, deg);
    EXPECT_FLOAT_EQ(tan(m2), tan(constants::pi / 6.0));

    precise_measurement m3(constants::pi / 6.0, precise::rad);
    EXPECT_DOUBLE_EQ(tan(m3), tan(constants::pi / 6.0));

    precise_measurement m4(30.0, precise::deg);
    EXPECT_DOUBLE_EQ(tan(m4), tan(constants::pi / 6.0));
}

TEST(mathOps, hypot)
{
    measurement m1(5.0, m);
    measurement m2(5.4, in);

    auto res = hypot(m1, m2);
    EXPECT_GT(res, m1);
    EXPECT_GT(res, m2);

    auto res2 = hypot(m2, m1);
    EXPECT_EQ(res, res2);
}

TEST(mathOps, hypot3)
{
    precise_measurement m1(5.0, precise::m);
    precise_measurement m2(5.4, precise::in);
    precise_measurement m3(0.01, precise::km);
    auto res = hypot(m1, m2, m3);
    EXPECT_GT(res, m1);
    EXPECT_GT(res, m2);
    EXPECT_GT(res, m3);
    auto res2 = hypot(m3, m1, m2);
    auto res3 = hypot(m2, m3, m1);
    EXPECT_EQ(res, res2);
    EXPECT_EQ(res2, res3);
}

TEST(mathOps, multiplies)
{
    precise_measurement m1(0.0001, precise::m * precise::pu);

    precise_measurement m2(5.4, precise::in);
    precise_measurement m3(0.01, precise::km);

    precise_measurement mkg(3.0, precise::kg);

    auto m4 = multiplies(m2, m3);
    auto m5 = m2 * m3;
    EXPECT_EQ(m4, m5);

    EXPECT_EQ(multiplies(m1, m3).units(), precise::km);
    EXPECT_EQ(multiplies(m3, m1).units(), precise::km);

    EXPECT_EQ(multiplies(m2, m1).units(), precise::in);
    EXPECT_EQ(multiplies(m1, m2).units(), precise::in);
    EXPECT_EQ(multiplies(m3, m1), multiplies(m1, m3));

    EXPECT_TRUE(multiplies(m1, mkg).units().is_per_unit());

    // from user guide example
    measurement deltaLength = 0.0001 * pu * m;
    measurement meas2 = 6.5 * m;

    auto res = multiplies(meas2, deltaLength);

    EXPECT_EQ(to_string(res), "0.00065 m");
}

TEST(mathOps, divides)
{
    precise_measurement m1(0.0001, precise::m * precise::N);

    precise_measurement m2(5.4, precise::in);
    precise_measurement m3(0.01, precise::km);

    auto m4 = divides(m1, m2);
    auto m5 = m1 / m2;
    EXPECT_EQ(m4, m5);

    EXPECT_EQ(divides(m2, m3).units(), precise::km * precise::pu);
    EXPECT_EQ(divides(m3, m2).units(), precise::in * precise::pu);

    measurement change = 0.0001 * m;
    measurement length = 10.0 * m;

    auto res = divides(change, length);

    EXPECT_EQ(to_string(res), "1e-05 strain");
}

TEST(mathOps, multDivReciprocity)
{
    measurement kg1(0.02, g);
    measurement kgT(3.4, ton);
    measurement nMass(247.5, lb);

    auto nm1 = (kg1 / kgT) * nMass;
    auto nm2 = (nMass / kgT * kg1);
    auto nm3 = kg1 * nMass / kgT;
    auto nm4 = nMass * (kg1 / kgT);

    EXPECT_EQ(nm1, nm2);
    EXPECT_EQ(nm1, nm3);
    EXPECT_EQ(nm2, nm4);

    auto nm1b = multiplies(divides(kg1, kgT), nMass);
    auto nm2b = multiplies(divides(nMass, kgT), kg1);
    auto nm2b2 = multiplies(kg1, divides(nMass, kgT));
    auto nm3b = divides(multiplies(kg1, nMass), kgT);
    auto nm4b = multiplies(nMass, divides(kg1, kgT));

    EXPECT_EQ(nm1b, nm2b);
    EXPECT_EQ(nm2b, nm2b2);
    EXPECT_EQ(nm1b, nm3b);
    EXPECT_EQ(nm2b, nm4b);

    EXPECT_EQ(nm1, nm1b);
    EXPECT_EQ(nm2, nm2b);
    EXPECT_EQ(nm3, nm3b);
    EXPECT_EQ(nm4, nm4b);
}

TEST(strain, example1)
{
    measurement deltaLength = 0.00001 * m;
    measurement length = 1 * m;

    auto strain = deltaLength / length;

    EXPECT_EQ(to_string(strain), "1e-05");

    // applied to a 10 ft bar
    auto distortion = strain * (10 * ft);
    EXPECT_EQ(to_string(distortion), "0.0001 ft");
}

TEST(strain, example2)
{
    precise_measurement strain = 1e-05 * default_unit("strain");
    EXPECT_EQ(to_string(strain), "1e-05 strain");

    // applied to a 10 ft bar
    auto distortion = multiplies(strain, (10 * ft));
    EXPECT_EQ(to_string(distortion), "0.0001 ft");
}

TEST(strain, example3)
{
    measurement deltaLength = 0.00001 * m;
    measurement length = 1 * m;

    auto strain = divides(deltaLength, length);
    EXPECT_EQ(to_string(strain), "1e-05 strain");

    // applied to a 10 ft bar
    auto distortion = multiplies(strain, (10 * ft));
    EXPECT_EQ(to_string(distortion), "0.0001 ft");
}

TEST(strain, example4)
{
    precise_unit ustrain(1e-6, eflag);  // microstrain

    addUserDefinedUnit("ustrain", ustrain);
    precise_measurement strain = 45.7 * ustrain;
    EXPECT_EQ(to_string(strain), "45.7 ustrain");

    // applied to a 10 m bar
    auto distortion = strain * (10 * m);
    EXPECT_FLOAT_EQ(distortion.value_as(mm), 0.457F);

    clearUserDefinedUnits();
}
