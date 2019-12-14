/*
Copyright (c) 2019,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#include "test.hpp"
#include "units/units.hpp"

#include <type_traits>

using namespace units;
TEST(Measurement, ops)
{
    measurement d1(45.0, m);
    measurement d2(79, m);

    auto area = d1 * d2;
    EXPECT_EQ(area.value(), 45.0 * 79);
    EXPECT_TRUE(area.units() == m * m);

    EXPECT_TRUE(d1 * d2 == d2 * d1);

    auto sum = d1 + d2;
    EXPECT_EQ(sum.value(), 45.0 + 79.0);
    EXPECT_TRUE(sum.units() == m);
    EXPECT_TRUE(d1 + d2 == d2 + d1);

    auto diff = d2 - d1;
    EXPECT_EQ(diff.value(), 79.0 - 45);
    EXPECT_TRUE(diff.units() == m);

    auto rat = d1 / d2;
    EXPECT_EQ(rat.value(), 45.0 / 79);
    EXPECT_TRUE(rat.units() == ratio);
}

TEST(Measurement, doubleOps)
{
    measurement d1(45.0, s);

    auto freq = 9.0 / s;
    EXPECT_EQ(freq.units(), one / s);
    auto freq2 = 9.0 * Hz;
    EXPECT_TRUE(freq == freq2);

    auto freq3 = Hz / (1.0 / 9.0);
    EXPECT_TRUE(freq3 == freq2);

    auto fd3 = freq / 3;
    EXPECT_DOUBLE_EQ(fd3.value(), 3.0);

    auto fd9 = freq * 3;
    EXPECT_DOUBLE_EQ(fd9.value(), 27.0);

    auto fd10 = 3.0 * freq;
    EXPECT_DOUBLE_EQ(fd10.value(), 27.0);
    auto fd2 = 27.0 / freq;
    EXPECT_DOUBLE_EQ(fd2.value(), 3.0);
    EXPECT_EQ(fd2.units(), s);
}

TEST(Measurement, help_constructors)
{
    auto d1 = 45.0 * m;
    auto d2 = m * 79.0;

    static_assert(
        std::is_same<decltype(d1), decltype(d2)>::value,
        "Types are not producing same measurement type");

    auto area = d1 * d2;
    EXPECT_EQ(area.value(), 45.0 * 79);
    EXPECT_TRUE(area.units() == m * m);

    EXPECT_TRUE(d1 * d2 == d2 * d1);

    auto sum = d1 + d2;
    EXPECT_EQ(sum.value(), 45.0 + 79.0);
    EXPECT_TRUE(sum.units() == m);
    EXPECT_TRUE(d1 + d2 == d2 + d1);

    auto diff = d2 - d1;
    EXPECT_EQ(diff.value(), 79.0 - 45);
    EXPECT_TRUE(diff.units() == m);

    auto rat = d1 / d2;
    EXPECT_EQ(rat.value(), 45.0 / 79);
    EXPECT_TRUE(rat.units() == ratio);
}

TEST(Measurement, comparison)
{
    EXPECT_TRUE(1000.0 * m == 1 * km);
    EXPECT_FALSE(1000.0 * m != 1 * km);

    EXPECT_TRUE(1 * in > 2 * cm);
    EXPECT_FALSE(1 * in < 2 * cm);
    EXPECT_TRUE((1 * in) == (2.54 * cm));

    EXPECT_TRUE((1 * in) >= (2.54 * cm));
    EXPECT_TRUE((1 * in) <= (2.54 * cm));
    EXPECT_FALSE((1 * in) >= (2.541 * cm));
    EXPECT_TRUE((1 * in) <= (2.54001 * cm));
    EXPECT_FALSE((1 * in) <= (2.0 * cm));
}

TEST(Measurement, conversions)
{
    auto d1 = 45.0 * ft;
    auto d2 = d1.convert_to_base();
    EXPECT_EQ(d2.units(), m);

    auto d3 = d1.convert_to(in);
    EXPECT_EQ(d3.units(), in);

    EXPECT_EQ(d1, d2);
    EXPECT_EQ(d2, d3);
    EXPECT_EQ(d3, d1);
    EXPECT_EQ(d1, d3);

    auto ud4 = d1.as_unit();
    auto d4 = d1.convert_to(ud4);
    EXPECT_FLOAT_EQ(static_cast<float>(d4.value()), 1.0f);
}

using namespace units;
TEST(fixedMeasurement, ops)
{
    fixed_measurement_type<double> d1(45.0, m);
    fixed_measurement_type<double> d2(79, m);

    auto area = d1 * d2;
    EXPECT_EQ(area.value(), 45.0 * 79);
    EXPECT_TRUE(area.units() == m * m);

    EXPECT_TRUE(d1 * d2 == d2 * d1);

    auto sum = d1 + d2;
    EXPECT_EQ(sum.value(), 45.0 + 79.0);
    EXPECT_TRUE(sum.units() == m);
    EXPECT_TRUE(d1 + d2 == d2 + d1);

    auto diff = d2 - d1;
    EXPECT_EQ(diff.value(), 79.0 - 45);
    EXPECT_TRUE(diff.units() == m);

    auto rat = d1 / d2;
    EXPECT_EQ(rat.value(), 45.0 / 79);
    EXPECT_TRUE(rat.units() == ratio);
}

TEST(fixedMeasurement, ops_v2)
{
    fixed_measurement d1(45.0, m);
    fixed_measurement d2(79, m);

    auto area = d1 * d2;
    EXPECT_EQ(area.value(), 45.0 * 79);
    EXPECT_TRUE(area.units() == m * m);

    EXPECT_TRUE(d1 * d2 == d2 * d1);

    auto sum = d1 + d2;
    EXPECT_EQ(sum.value(), 45.0 + 79.0);
    EXPECT_TRUE(sum.units() == m);
    EXPECT_TRUE(d1 + d2 == d2 + d1);

    auto diff = d2 - d1;
    EXPECT_EQ(diff.value(), 79.0 - 45);
    EXPECT_TRUE(diff.units() == m);

    auto rat = d1 / d2;
    EXPECT_EQ(rat.value(), 45.0 / 79);
    EXPECT_TRUE(rat.units() == ratio);
}

TEST(fixedMeasurement, doubleOps)
{
    fixed_measurement freq(9.0, Hz);
    EXPECT_EQ(freq.units(), one / s);
    auto freq2 = 2.0 * freq;
    EXPECT_FLOAT_EQ(static_cast<float>(freq2.value()), 18.0);
    bool res = std::is_same<decltype(freq), decltype(freq2)>::value;
    EXPECT_TRUE(res);

    auto f4 = freq * 3;
    EXPECT_FLOAT_EQ(static_cast<float>(f4.value()), 27.0);

    auto f3 = freq / 3;
    EXPECT_FLOAT_EQ(static_cast<float>(f3.value()), 3.0);

    auto f1 = 9.0 / freq;
    EXPECT_TRUE(f1 == (1.0 * s));
    EXPECT_TRUE((1.0 * s) == f1);

    auto fp1 = freq + 3.0;
    EXPECT_FLOAT_EQ(static_cast<float>(fp1.value()), 12.0);

    auto fp2 = 3.0 + freq;
    EXPECT_FLOAT_EQ(static_cast<float>(fp2.value()), 12.0);

    auto fp3 = freq - 3.0;
    EXPECT_FLOAT_EQ(static_cast<float>(fp3.value()), 6.0);

    auto fp4 = 12.0 - freq;
    EXPECT_FLOAT_EQ(static_cast<float>(fp4.value()), 3.0);
}

TEST(fixedMeasurement, comparison)
{
    fixed_measurement d1(45.0, m);
    fixed_measurement d2(79, m);
    fixed_measurement d3(d2);

    EXPECT_TRUE(d1 < d2);
    EXPECT_TRUE(d2 == d3);
    EXPECT_FALSE(d2 != d3);
    EXPECT_FALSE(d1 == d2);

    EXPECT_TRUE(d2 == 79.0);
    EXPECT_TRUE(79.0 == d2);

    EXPECT_FALSE(d1 == 79.0);
    EXPECT_FALSE(79.0 == d1);

    EXPECT_TRUE((1 * in) >= (2.54 * cm));
    EXPECT_TRUE((1 * in) <= (2.54 * cm));
    EXPECT_FALSE((1 * in) >= (2.541 * cm));
    EXPECT_TRUE((1 * in) <= (2.54001 * cm));
    EXPECT_FALSE((1 * in) <= (2.0 * cm));
}

TEST(PrecisionMeasurement, ops)
{
    precise_measurement d1(45.0, precise::m);
    precise_measurement d2(79, precise::m);

    auto area = d1 * d2;
    EXPECT_EQ(area.value(), 45.0 * 79);
    EXPECT_TRUE(area.units() == precise::m * precise::m);

    EXPECT_TRUE(d1 * d2 == d2 * d1);

    auto sum = d1 + d2;
    EXPECT_EQ(sum.value(), 45.0 + 79.0);
    EXPECT_TRUE(sum.units() == precise::m);
    EXPECT_TRUE(d1 + d2 == d2 + d1);

    auto diff = d2 - d1;
    EXPECT_EQ(diff.value(), 79.0 - 45);
    EXPECT_TRUE(diff.units() == precise::m);

    auto rat = d1 / d2;
    EXPECT_EQ(rat.value(), 45.0 / 79);
    EXPECT_TRUE(rat.units() == ratio);
}

TEST(PrecisionMeasurement, doubleOps)
{
    precise_measurement d1(45.0, precise::s);

    auto freq = 9.0 / precise::s;
    EXPECT_EQ(freq.units(), precise::one / precise::s);
    auto freq2 = 9.0 * precise::Hz;
    EXPECT_TRUE(freq == freq2);

    auto freq3 = precise::Hz / (1.0 / 9.0);
    EXPECT_TRUE(freq3 == freq2);

    auto fd3 = freq / 3;
    EXPECT_DOUBLE_EQ(fd3.value(), 3.0);

    auto fd9 = freq * 3;
    EXPECT_DOUBLE_EQ(fd9.value(), 27.0);

    auto fd10 = 3.0 * freq;
    EXPECT_DOUBLE_EQ(fd10.value(), 27.0);
    auto fd2 = 27.0 / freq;
    EXPECT_DOUBLE_EQ(fd2.value(), 3.0);
    EXPECT_EQ(fd2.units(), precise::s);
}

TEST(PrecisionMeasurement, help_constructors)
{
    auto d1 = 45.0 * precise::m;
    auto d2 = precise::m * 79.0;

    static_assert(
        std::is_same<decltype(d1), decltype(d2)>::value,
        "Types are not producing same measurement type");

    auto area = d1 * d2;
    EXPECT_EQ(area.value(), 45.0 * 79);
    EXPECT_TRUE(area.units() == precise::m * precise::m);

    EXPECT_TRUE(d1 * d2 == d2 * d1);

    auto sum = d1 + d2;
    EXPECT_EQ(sum.value(), 45.0 + 79.0);
    EXPECT_TRUE(sum.units() == precise::m);
    EXPECT_TRUE(d1 + d2 == d2 + d1);

    auto diff = d2 - d1;
    EXPECT_EQ(diff.value(), 79.0 - 45);
    EXPECT_TRUE(diff.units() == precise::m);

    auto rat = d1 / d2;
    EXPECT_EQ(rat.value(), 45.0 / 79);
    EXPECT_TRUE(rat.units() == ratio);
}

TEST(PrecisionMeasurement, conversions)
{
    auto d1 = 45.0 * precise::ft;
    auto d2 = d1.convert_to_base();
    EXPECT_EQ(d2.units(), precise::m);

    auto d3 = d1.convert_to(precise::in);
    EXPECT_EQ(d3.units(), precise::in);

    EXPECT_EQ(d1, d2);
    EXPECT_EQ(d2, d3);
    EXPECT_EQ(d3, d1);
    EXPECT_EQ(d1, d3);

    auto ud4 = d1.as_unit();
    auto d4 = d1.convert_to(ud4);
    EXPECT_EQ(d4.value(), 1.0);
}

TEST(PrecisionMeasurement, comparison)
{
    EXPECT_TRUE(1000.0 * precise::m == 1 * precise::km);
    EXPECT_FALSE(1000.0 * precise::m != 1 * precise::km);

    EXPECT_TRUE(1 * precise::in > 2 * precise::cm);
    EXPECT_FALSE(1 * precise::in < 2 * precise::cm);
    EXPECT_TRUE((1 * precise::in) == (2.54 * precise::cm));

    EXPECT_TRUE((1 * precise::in) >= (2.54 * precise::cm));
    EXPECT_TRUE((1 * precise::in) <= (2.54 * precise::cm));
    EXPECT_FALSE((1 * precise::in) >= (2.541 * precise::cm));
    EXPECT_TRUE((1 * precise::in) <= (2.54001 * precise::cm));
    EXPECT_FALSE((1 * precise::in) <= (2.0 * precise::cm));
}

using namespace units;
TEST(fixedPrecisionMeasurement, ops)
{
    fixed_precision_measurement d1(45.0, precise::m);
    fixed_precision_measurement d2(79, precise::m);

    auto area = d1 * d2;
    EXPECT_EQ(area.value(), 45.0 * 79);
    EXPECT_TRUE(area.units() == m * m);

    EXPECT_TRUE(d1 * d2 == d2 * d1);

    auto sum = d1 + d2;
    EXPECT_EQ(sum.value(), 45.0 + 79.0);
    EXPECT_TRUE(sum.units() == m);
    EXPECT_TRUE(d1 + d2 == d2 + d1);

    auto diff = d2 - d1;
    EXPECT_EQ(diff.value(), 79.0 - 45);
    EXPECT_TRUE(diff.units() == m);

    auto rat = d1 / d2;
    EXPECT_EQ(rat.value(), 45.0 / 79);
    EXPECT_TRUE(rat.units() == ratio);
}
