/*
Copyright (c) 2019-2020,
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

    EXPECT_TRUE(2.0 / m == measurement(2.0, m.inv()));
    EXPECT_TRUE(m / 2.0 == measurement(0.5, m));
    EXPECT_TRUE(m * 2.0 == measurement(2.0, m));

    //equivalent to asking how much is left over if you divide a 2 m object into  6 inch chunks
    auto fd11 = (2.0 * m) % (6 * in);
    EXPECT_LT(fd11, (6 * in));
}

TEST(Measurement, doubleOps)
{
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

    auto fd11 = (27.0 * m) % 6;
    EXPECT_DOUBLE_EQ(fd11.value(), 3.0);
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

TEST(measurement, powroot)
{
    measurement m1(2.0, m);

    auto v1 = pow(m1, 3);
    EXPECT_EQ(v1.value(), 8.0);
    EXPECT_EQ(v1.units(), m.pow(3));
#ifndef UNITS_HEADER_ONLY
    auto m2 = root(v1, 3);
    EXPECT_TRUE(m2 == m1);

    auto m0 = root(v1, 0);
    EXPECT_EQ(m0.value(), 1.0);
    EXPECT_EQ(m0.units(), one);

    measurement m4(16.0, m.pow(2));
    EXPECT_EQ(sqrt(m4), measurement(4.0, m));
#endif
}

using namespace units;
TEST(fixedMeasurement, ops)
{
    fixed_measurement d1(45.0, m);
    fixed_measurement d2(79, m);
    fixed_measurement d3(79 * m);

    auto area = d1 * d2;

    EXPECT_TRUE(d2 == d3);
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

    auto m1 = d2 / s;
    fixed_measurement spd(m1);
    auto m3 = spd * s;

    EXPECT_TRUE(d2 == m3);
    EXPECT_TRUE(m3 == d2);

    fixed_measurement fm3(2.0, m);
    fm3 *= 2.0;
    EXPECT_EQ(fm3.value(), 4.0);
    fm3 /= 4.0;
    EXPECT_EQ(fm3.value(), 1.0);

    auto v = fm3 *= 2.0;
    EXPECT_EQ(v.value(), 2.0);
}

TEST(fixedMeasurement, methods)
{
    fixed_measurement size(1.2, m);
    auto f2 = size.convert_to(in);
    EXPECT_TRUE(f2 == size);

    measurement m3(1.0, f2.as_unit());
    EXPECT_DOUBLE_EQ(m3.value(), 1.0);
    EXPECT_TRUE(m3 == f2);

    EXPECT_FLOAT_EQ(static_cast<float>(f2.value_as(m)), 1.2F);
    EXPECT_FLOAT_EQ(static_cast<float>(size.value_as(f2.as_unit())), 1.0F);

    size += 0.1;
    EXPECT_TRUE(size > f2);
    EXPECT_TRUE(size > m3);
    EXPECT_TRUE(f2 < size);
    EXPECT_TRUE(m3 < size);
    EXPECT_TRUE(size > 1.2);
    EXPECT_TRUE(1.2 < size);

    EXPECT_TRUE(size >= f2);
    EXPECT_TRUE(size >= m3);
    EXPECT_TRUE(f2 <= size);
    EXPECT_TRUE(m3 <= size);
    EXPECT_TRUE(size >= 1.2);
    EXPECT_TRUE(1.2 <= size);

    size -= 0.1;
    EXPECT_TRUE(size == f2);
    EXPECT_TRUE(size == m3);
    EXPECT_TRUE(f2 == size);
    EXPECT_TRUE(m3 == size);
    EXPECT_TRUE(size == 1.2);
    EXPECT_TRUE(1.2 == size);

    EXPECT_FALSE(size != 1.2);
    EXPECT_FALSE(1.2 != size);

    EXPECT_TRUE(size >= f2);
    EXPECT_TRUE(size >= m3);
    EXPECT_TRUE(f2 >= size);
    EXPECT_TRUE(m3 >= size);
    EXPECT_TRUE(size >= 1.2);
    EXPECT_TRUE(1.2 >= size);

    EXPECT_TRUE(size <= f2);
    EXPECT_TRUE(size <= m3);
    EXPECT_TRUE(f2 <= size);
    EXPECT_TRUE(m3 <= size);
    EXPECT_TRUE(size <= 1.2);
    EXPECT_TRUE(1.2 <= size);
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

    fixed_measurement y(2.0 * m);
    EXPECT_DOUBLE_EQ(y.value(), 2.0);
    y = 5.0 * m;
    EXPECT_DOUBLE_EQ(y.value(), 5.0);
    y = 7.0;
    EXPECT_DOUBLE_EQ(y.value(), 7.0);
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

TEST(fixedMeasurement, powroot)
{
    fixed_measurement m1(2.0, m);

    auto v1 = pow(m1, 3);
    EXPECT_EQ(v1.value(), 8.0);
    EXPECT_EQ(v1.units(), m.pow(3));
#ifndef UNITS_HEADER_ONLY
    auto m2 = root(v1, 3);
    EXPECT_TRUE(m2 == m1);

    fixed_measurement m4(16.0, m.pow(2));
    EXPECT_TRUE(sqrt(m4) == fixed_measurement(4.0, m));
#endif
}

TEST(PreciseMeasurement, ops)
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

TEST(PreciseMeasurement, doubleOps)
{
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

TEST(PreciseMeasurement, help_constructors)
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

TEST(PreciseMeasurement, conversions)
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

TEST(PreciseMeasurement, comparison)
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

TEST(PreciseMeasurement, powroot)
{
    precise_measurement m1(2.0, precise::m);

    auto v1 = pow(m1, 3);
    EXPECT_EQ(v1.value(), 8.0);
    EXPECT_EQ(v1.units(), precise::m.pow(3));
#ifndef UNITS_HEADER_ONLY
    auto m2 = root(v1, 3);
    EXPECT_TRUE(m2 == m1);
    precise_measurement m4(16.0, precise::m.pow(2));
    EXPECT_EQ(sqrt(m4), precise_measurement(4.0, precise::m));
#endif
}

using namespace units;
TEST(fixedPreciseMeasurement, ops)
{
    fixed_precise_measurement d1(45.0, precise::m);
    fixed_precise_measurement d2(79, precise::m);

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

    fixed_precise_measurement y(2.0 * precise::m);
    EXPECT_DOUBLE_EQ(y.value(), 2.0);
    y = 5.0 * precise::m;
    EXPECT_DOUBLE_EQ(y.value(), 5.0);
    y = 7.0;
    EXPECT_DOUBLE_EQ(y.value(), 7.0);
}

TEST(fixedPreciseMeasurement, powroot)
{
    fixed_precise_measurement m1(2.0, precise::m);

    auto v1 = pow(m1, 3);
    EXPECT_EQ(v1.value(), 8.0);
    EXPECT_EQ(v1.units(), precise::m.pow(3));
#ifndef UNITS_HEADER_ONLY
    auto m2 = root(v1, 3);
    EXPECT_TRUE(m2 == m1);

    fixed_precise_measurement m4(16.0, precise::m.pow(2));
    EXPECT_TRUE(sqrt(m4) == fixed_precise_measurement(4.0, precise::m));
#endif
}
