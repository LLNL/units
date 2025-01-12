/*
Copyright (c) 2019-2025,
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

    // equivalent to asking how much is left over if you divide a 2 m object
    // into  6 inch chunks
    auto fd11 = (2.0 * m) % (6 * in);
    EXPECT_LT(fd11, (6 * in));

    EXPECT_TRUE(is_valid(d1));
}

TEST(Measurement, unaryOps)
{
    measurement x(2.0, cm);

    auto z = -x;
    auto y = +x;
    EXPECT_EQ(y, x);
    EXPECT_EQ(z.value(), -x.value());
    EXPECT_EQ(z.units(), x.units());
}

TEST(Measurement, assignment)
{
    measurement m1;
    EXPECT_TRUE(is_valid(m1));
    measurement m2 = 10.5 * m;

    m1 = m2;
    EXPECT_EQ(m1, m2);
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

TEST(Measurement, helpConstructors)
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
    EXPECT_FLOAT_EQ(static_cast<float>(d4.value()), 1.0F);
}

TEST(Measurement, powroot)
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

    measurement mneg(-0.25, m.pow(6));
    EXPECT_FALSE(is_valid(root(mneg, 2)));
    EXPECT_FALSE(is_valid(root(mneg, -2)));
    EXPECT_FALSE(is_valid(root(mneg, 4)));
    EXPECT_FALSE(is_valid(root(mneg, -4)));
    EXPECT_FALSE(is_valid(root(mneg, 6)));
    EXPECT_FALSE(is_valid(root(mneg, -6)));
    EXPECT_TRUE(is_valid(root(mneg, 0)));
    EXPECT_TRUE(is_valid(root(mneg, 1)));
    EXPECT_TRUE(is_valid(root(mneg, -1)));
    EXPECT_TRUE(is_valid(root(mneg, 3)));
    EXPECT_TRUE(is_valid(root(mneg, -3)));
#endif
}

TEST(Measurement, invalid)
{
    measurement iv1(1.2, invalid);
    EXPECT_FALSE(is_valid(iv1));
    EXPECT_FALSE(isnormal(iv1));

    measurement iv2(constants::invalid_conversion, m);
    EXPECT_FALSE(is_valid(iv2));
    EXPECT_FALSE(isnormal(iv2));

    measurement iv3(constants::infinity, m);
    EXPECT_TRUE(is_valid(iv3));
    EXPECT_FALSE(isnormal(iv3));

    measurement iv4(1e-311, m);  // subnormal
    EXPECT_TRUE(is_valid(iv4));
    EXPECT_FALSE(isnormal(iv4));

    measurement iv5(0.0, m);
    EXPECT_TRUE(is_valid(iv5));
    EXPECT_TRUE(isnormal(iv5));
}

using namespace units;
TEST(fixedMeasurement, ops)
{
    fixed_measurement d1(45.0, m);
    fixed_measurement d2(79, m);
    fixed_measurement d3(79 * m);
    fixed_measurement d4(1.0, ft);
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

    d4 = d1;
    EXPECT_TRUE(d4 == d1);
    EXPECT_TRUE(d4.units() == ft);
}

TEST(fixedMeasurement, opsV2)
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

TEST(fixedMeasurement, unaryOps)
{
    fixed_measurement x(2.0, cm);

    auto z = -x;
    auto y = +x;
    EXPECT_EQ(y, x);
    EXPECT_EQ(z.value(), -x.value());
    EXPECT_EQ(z.units(), x.units());
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

    EXPECT_TRUE(is_valid(size));
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

    EXPECT_TRUE(79.0001 > d2);
    EXPECT_TRUE(d2 < 79.0001);

    EXPECT_FALSE(79.0001 < d2);
    EXPECT_FALSE(d2 > 79.0001);

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

TEST(fixedMeasurement, invalid)
{
    fixed_measurement iv1(1.2, invalid);
    EXPECT_FALSE(is_valid(iv1));
    EXPECT_FALSE(isnormal(iv1));

    fixed_measurement iv2(constants::invalid_conversion, m);
    EXPECT_FALSE(is_valid(iv2));
    EXPECT_FALSE(isnormal(iv2));

    fixed_measurement iv3(constants::infinity, m);
    EXPECT_TRUE(is_valid(iv3));
    EXPECT_FALSE(isnormal(iv3));

    fixed_measurement iv4(1e-311, m);  // subnormal
    EXPECT_TRUE(is_valid(iv4));
    EXPECT_FALSE(isnormal(iv4));

    fixed_measurement iv5(0.0, m);
    EXPECT_TRUE(is_valid(iv5));
    EXPECT_TRUE(isnormal(iv5));
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
    EXPECT_TRUE(is_valid(d1));
}

TEST(PreciseMeasurement, unaryOps)
{
    precise_measurement x(2.0, precise::cm);

    auto z = -x;
    auto y = +x;
    EXPECT_EQ(y, x);
    EXPECT_EQ(z.value(), -x.value());
    EXPECT_EQ(z.units(), x.units());
}

TEST(PreciseMeasurement, countAddition)
{
    auto m1 = 1.0 * precise::Hz;
    auto m2 = 1.0 * unit_from_string("baud");
    auto m3 = 1.0 * precise::Bq;

    auto m4 = m1 + m2 + m3;
    auto str = to_string(m4);
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

    auto fd6 = precise::Hz * 9.0;
    EXPECT_TRUE(fd6 == freq2);

    auto fd7 = 9.0 / precise::s;
    EXPECT_TRUE(fd7 == freq2);

    auto fd8 = precise::Hz / 0.5;
    EXPECT_DOUBLE_EQ(fd8.value(), 2.0);
}

TEST(PreciseMeasurement, helpConstructors)
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

    constexpr auto d5 = precise::ft * 3.0;
    // convert to base
    EXPECT_EQ(d5.convert_to_base().units(), precise::m);
    static_assert(
        (3.0 * precise::ft).convert_to_base().units().base_units() ==
            precise::m.base_units(),
        "constexpr convert_to_base not working");

    EXPECT_EQ(d5.as_unit(), precise::yd);
    EXPECT_DOUBLE_EQ(quick_convert(1.0, d5.as_unit(), precise::yd), 1.0);
    // static_assert(==1.0, "constexpr convert_to_base not working");
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

TEST(PreciseMeasurement, doubleOperators)
{
    EXPECT_EQ(precise::m * 7.0, 7.0 * precise::m);
    EXPECT_EQ(precise::m / 4.0, 0.25 * precise::m);
    EXPECT_EQ(4.0 / precise::m, 4.0 * precise::m.inv());
}

TEST(PreciseMeasurement, convert2base)
{
    precise_measurement m3 = 45.0 * precise::ft;

    auto m5 = m3.convert_to_base();
    EXPECT_EQ(m5.units(), precise::m);
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

TEST(PreciseMeasurement, invalid)
{
    precise_measurement iv1(1.2, precise::invalid);
    EXPECT_FALSE(is_valid(iv1));
    EXPECT_FALSE(isnormal(iv1));

    precise_measurement iv2(constants::invalid_conversion, precise::m);
    EXPECT_FALSE(is_valid(iv2));
    EXPECT_FALSE(isnormal(iv2));

    precise_measurement iv3(constants::infinity, precise::m);
    EXPECT_TRUE(is_valid(iv3));
    EXPECT_FALSE(isnormal(iv3));

    precise_measurement iv4(1e-311, precise::m);  // subnormal
    EXPECT_TRUE(is_valid(iv4));
    EXPECT_FALSE(isnormal(iv4));

    precise_measurement iv5(0.0, precise::m);
    EXPECT_TRUE(is_valid(iv5));
    EXPECT_TRUE(isnormal(iv5));
}

TEST(PreciseMeasurement, cast)
{
    precise_measurement m1(2.0, precise::m);
    auto m3 = measurement_cast(m1);

    auto m4 = measurement_cast(m3);
    static_assert(
        std::is_same<decltype(m3), measurement>::value,
        "measurement cast not working for precise_measurement");
    static_assert(
        std::is_same<decltype(m4), measurement>::value,
        "measurement cast not working for measurement");
}

using namespace units;
TEST(fixedPreciseMeasurement, ops)
{
    fixed_precise_measurement d1(45.0, precise::m);
    fixed_precise_measurement d2(79, precise::m);
    fixed_precise_measurement d4(1.0, precise::ft);
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
    EXPECT_EQ(rat.value(), 45.0 / 79.0);
    EXPECT_TRUE(rat.units() == precise::ratio);

    fixed_precise_measurement y(2.0 * precise::m);
    EXPECT_DOUBLE_EQ(y.value(), 2.0);
    y = 5.0 * precise::m;
    EXPECT_DOUBLE_EQ(y.value(), 5.0);
    y = 7.0;
    EXPECT_DOUBLE_EQ(y.value(), 7.0);
    EXPECT_TRUE(is_valid(d1));

    d4 = d1;
    EXPECT_TRUE(d4 == d1);
    EXPECT_TRUE(d4.units() == precise::ft);
}

TEST(fixedPreciseMeasurement, opsV2)
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

    auto m1 = d2 / precise::s;
    fixed_precise_measurement spd(m1);
    auto m3 = spd * precise::s;

    EXPECT_TRUE(d2 == m3);
    EXPECT_TRUE(m3 == d2);

    fixed_precise_measurement fm3(2.0, precise::m);
    fm3 *= 2.0;
    EXPECT_EQ(fm3.value(), 4.0);
    fm3 /= 4.0;
    EXPECT_EQ(fm3.value(), 1.0);

    auto v = fm3 *= 2.0;
    EXPECT_EQ(v.value(), 2.0);
}

TEST(fixedPreciseMeasurement, unaryOps)
{
    fixed_precise_measurement x(2.0, precise::cm);

    auto z = -x;
    auto y = +x;
    EXPECT_EQ(y, x);
    EXPECT_EQ(z.value(), -x.value());
    EXPECT_EQ(z.units(), x.units());
}

TEST(fixedPreciseMeasurement, methods)
{
    fixed_precise_measurement size(1.2, precise::m);
    auto f2 = size.convert_to(precise::in);
    EXPECT_TRUE(f2 == size);

    precise_measurement m3(1.0, f2.as_unit());
    EXPECT_DOUBLE_EQ(m3.value(), 1.0);
    EXPECT_TRUE(m3 == f2);

    EXPECT_DOUBLE_EQ(f2.value_as(precise::m), 1.2);
    EXPECT_DOUBLE_EQ(size.value_as(f2.as_unit()), 1.0);

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

    EXPECT_TRUE(is_valid(size));
}

TEST(fixedPreciseMeasurement, doubleOps)
{
    fixed_precise_measurement freq(9.0, precise::Hz);
    EXPECT_EQ(freq.units(), precise::one / precise::s);
    auto freq2 = 2.0 * freq;
    EXPECT_DOUBLE_EQ(freq2.value(), 18.0);
    bool res = std::is_same<decltype(freq), decltype(freq2)>::value;
    EXPECT_TRUE(res);

    auto f4 = freq * 3;
    EXPECT_DOUBLE_EQ(f4.value(), 27.0);

    auto f3 = freq / 3;
    EXPECT_DOUBLE_EQ(static_cast<float>(f3.value()), 3.0);

    auto f1 = 9.0 / freq;
    EXPECT_TRUE(f1 == (1.0 * precise::s));
    EXPECT_TRUE((1.0 * precise::s) == f1);

    auto fp1 = freq + 3.0;
    EXPECT_DOUBLE_EQ(fp1.value(), 12.0);

    auto fp2 = 3.0 + freq;
    EXPECT_DOUBLE_EQ(fp2.value(), 12.0);

    auto fp3 = freq - 3.0;
    EXPECT_DOUBLE_EQ(fp3.value(), 6.0);

    auto fp4 = 12.0 - freq;
    EXPECT_DOUBLE_EQ(fp4.value(), 3.0);

    fixed_precise_measurement y(2.0 * precise::m);
    EXPECT_DOUBLE_EQ(y.value(), 2.0);
    y = 5.0 * m;
    EXPECT_DOUBLE_EQ(y.value(), 5.0);
    y = 7.0;
    EXPECT_DOUBLE_EQ(y.value(), 7.0);
}

TEST(fixedPreciseMeasurement, comparison)
{
    fixed_precise_measurement d1(45.0, precise::m);
    fixed_precise_measurement d2(79, precise::m);
    fixed_precise_measurement d3(d2);

    EXPECT_TRUE(d1 < d2);
    EXPECT_TRUE(d2 == d3);
    EXPECT_FALSE(d2 != d3);
    EXPECT_FALSE(d1 == d2);

    EXPECT_TRUE(d2 == 79.0);
    EXPECT_TRUE(79.0 == d2);

    EXPECT_FALSE(d1 == 79.0);
    EXPECT_FALSE(79.0 == d1);

    EXPECT_TRUE(79.0001 > d2);
    EXPECT_TRUE(d2 < 79.0001);

    EXPECT_FALSE(79.0001 < d2);
    EXPECT_FALSE(d2 > 79.0001);

    EXPECT_TRUE((1 * in) >= (2.54 * cm));
    EXPECT_TRUE((1 * in) <= (2.54 * cm));
    EXPECT_FALSE((1 * in) >= (2.541 * cm));
    EXPECT_TRUE((1 * in) <= (2.54001 * cm));
    EXPECT_FALSE((1 * in) <= (2.0 * cm));
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

TEST(fixedPreciseMeasurement, invalid)
{
    fixed_precise_measurement iv1(1.2, precise::invalid);
    EXPECT_FALSE(is_valid(iv1));
    EXPECT_FALSE(isnormal(iv1));

    fixed_precise_measurement iv2(constants::invalid_conversion, precise::m);
    EXPECT_FALSE(is_valid(iv2));
    EXPECT_FALSE(isnormal(iv2));

    fixed_precise_measurement iv3(constants::infinity, precise::m);
    EXPECT_TRUE(is_valid(iv3));
    EXPECT_FALSE(isnormal(iv3));

    fixed_precise_measurement iv4(1e-311, precise::m);  // subnormal
    EXPECT_TRUE(is_valid(iv4));
    EXPECT_FALSE(isnormal(iv4));

    fixed_precise_measurement iv5(0.0, precise::m);
    EXPECT_TRUE(is_valid(iv5));
    EXPECT_TRUE(isnormal(iv5));
}

TEST(fixedPreciseMeasurement, cast)
{
    fixed_precise_measurement m1(2.0, precise::m);
    auto m3 = measurement_cast(m1);

    auto m4 = measurement_cast(m3);
    static_assert(
        std::is_same<decltype(m3), fixed_measurement>::value,
        "measurement cast not working for fixed_precise_measurement");
    static_assert(
        std::is_same<decltype(m4), fixed_measurement>::value,
        "measurement cast not working for fixed_measurement");
}
