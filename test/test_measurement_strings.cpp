/*
Copyright (c) 2019-2025,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#include "test.hpp"
#include "units/units.hpp"

#include <iostream>
#include <string>
#include <type_traits>
#include <vector>

using namespace units;
TEST(MeasurementStrings, basic)
{
    auto pm = measurement_from_string("45 m");
    EXPECT_EQ(pm, 45.0 * precise::m);

    pm = measurement_from_string("9.0 * 5.0 m");
    EXPECT_EQ(pm, 45.0 * precise::m);

    pm = measurement_from_string("23.7 m/s");
    EXPECT_EQ(pm, 23.7 * precise::m / precise::s);
    pm = measurement_from_string("99.9 N * m");
    EXPECT_EQ(pm, 99.9 * precise::N * precise::m);

    EXPECT_NO_THROW(pm = measurement_from_string(""));
    EXPECT_EQ(pm.value(), 0.0);
}

TEST(MeasurementStrings, basicCast)
{
    auto pm = measurement_cast_from_string("45 m");
    EXPECT_EQ(pm, 45.0 * m);

    pm = measurement_cast_from_string("9.0 * 5.0 m");
    EXPECT_EQ(pm, 45.0 * m);

    pm = measurement_cast_from_string("23.7 m/s");
    EXPECT_EQ(pm, 23.7 * m / s);
    EXPECT_EQ(pm, 23.7 * (m / s));
    pm = measurement_cast_from_string("99.9 N * m");
    EXPECT_EQ(pm, 99.9 * N * m);
    EXPECT_EQ(pm, 99.9 * (N * m));
}

TEST(MeasurementStrings, invalid)
{
    auto pm = measurement_from_string("345 blarg");
    EXPECT_FALSE(is_valid(pm.units()));
}

TEST(MeasurementStrings, currency)
{
    auto pm = measurement_from_string("$9.99");
    EXPECT_EQ(pm, 9.99 * precise::currency);

    pm = measurement_from_string("$ 9.99");
    EXPECT_EQ(pm, 9.99 * precise::currency);
}

TEST(MeasurementStrings, interesting)
{
    auto pm = measurement_from_string("nanometre");
    EXPECT_EQ(pm.as_unit(), precise::nano * precise::m);
}

TEST(MeasurementToString, simple)
{
    auto pm = precise_measurement(45.0, precise::m);
    EXPECT_EQ(to_string(pm), "45 m");
    auto meas = 45.0 * m;
    EXPECT_EQ(to_string(meas), "45 m");
}

TEST(MeasurementToString, test)
{
    measurement density = 10.0 * kg / m.pow(3);
    measurement meas2 = 2.7 * puMW;

    auto str1 = to_string(density);
    auto str2 = to_string(meas2);

    // from google tests
    EXPECT_EQ(str1, "10 kg/m^3");
    EXPECT_EQ(str2, "2.7 puMW");
}

TEST(MeasurementToString, unitWithNumbers)
{
    measurement ounit = 10.0 * unit(0.712412, kg.pow(2));
    precise_measurement ounitp =
        10.0 * precise_unit(0.712412, precise::kg.pow(2));
    auto str1 = to_string(ounit);
    auto str2 = to_string(ounitp);
    EXPECT_EQ(str1.compare(0, 11, "10 (0.71241"), 0);
    EXPECT_EQ(str2.compare(0, 11, "10 (0.71241"), 0);
}

TEST(MeasurementToString, numbers)
{
    auto m1 = measurement_from_string("0.2");
    EXPECT_DOUBLE_EQ(m1.value(), 0.2);
}

TEST(MeasurementToString, empty)
{
    measurement m1(45.7, one);
    EXPECT_EQ(to_string(m1), "45.7");
}

TEST(MeasurementToString, caseSensitive)
{
    static const std::vector<std::pair<unit, std::string>> twoc_units{
        {lb, "lb"},
        {oz, "oz"},
        {yd, "yd"},
        {unit_cast(precise::us::quart), "qt"}};

    for (const auto& up : twoc_units) {
        std::string singular = std::string("17 ") + up.second;
        std::string plural = singular;
        plural.push_back('s');

        std::string singular_caps = singular;
        std::transform(
            singular.begin(), singular.end(), singular_caps.begin(), ::toupper);

        std::string plural_caps = plural;
        std::transform(
            plural.begin(), plural.end(), plural_caps.begin(), ::toupper);

        precise_measurement case_sensitive_plural =
            measurement_from_string(plural);
        // true
        EXPECT_TRUE(case_sensitive_plural.units().has_same_base(up.first))
            << "error with " << up.second;
        precise_measurement case_sensitive_singular =
            measurement_from_string(singular);
        EXPECT_TRUE(case_sensitive_singular.units().has_same_base(up.first));

        // Case insensitive string conversion
        precise_measurement case_insensitive_plural =
            measurement_from_string(plural, case_insensitive);
        // true
        EXPECT_TRUE(case_insensitive_plural.units().has_same_base(up.first))
            << up.second;

        units::precise_measurement case_insensitive_singular =
            measurement_from_string(singular, case_insensitive);
        // false
        EXPECT_TRUE(case_insensitive_singular.units().has_same_base(up.first));

        // Case insensitive string conversion caps
        precise_measurement case_insensitive_plural_caps =
            measurement_from_string(plural_caps, case_insensitive);
        // true
        EXPECT_TRUE(
            case_insensitive_plural_caps.units().has_same_base(up.first));

        units::precise_measurement case_insensitive_singular_caps =
            measurement_from_string(singular_caps, case_insensitive);
        // false
        EXPECT_TRUE(
            case_insensitive_singular_caps.units().has_same_base(up.first));

        // Round trip
        const std::string case_insensitive_plural_str =
            units::to_string(case_insensitive_plural);
        units::precise_measurement round_trip = units::measurement_from_string(
            case_insensitive_plural_str, units::case_insensitive);
        EXPECT_TRUE(round_trip.units().has_same_base(up.first));
    }
}
