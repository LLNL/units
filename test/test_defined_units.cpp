/*
Copyright (c) 2019-2022,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "test.hpp"
#include "units/units_conversion_maps.hpp"
#include <map>

TEST(unit_string_definitions, si_vector_length)
{
    for (std::size_t ii = 0; ii < units::defined_unit_strings_si.size(); ++ii) {
        EXPECT_TRUE(units::defined_unit_strings_si[ii].first != nullptr) << ii;
        if (units::defined_unit_strings_si[ii].first == nullptr) {
            break;
        }
    }
}

TEST(unit_string_definitions, customary_vector)
{
    for (std::size_t ii = 0; ii < units::defined_unit_strings_customary.size();
         ++ii) {
        EXPECT_TRUE(units::defined_unit_strings_customary[ii].first != nullptr)
            << ii;
        if (units::defined_unit_strings_customary[ii].first == nullptr) {
            break;
        }
    }
}

TEST(unit_string_definitions, si_duplicates)
{
    std::map<std::string, units::precise_unit> testMap;
    for (const auto& ustring : units::defined_unit_strings_si) {
        if (ustring.first == nullptr) {
            continue;
        }
        auto res = testMap.emplace(ustring.first, ustring.second);
        EXPECT_TRUE(res.second) << "duplicate si unit string " << ustring.first;
    }
}

TEST(unit_string_definitions, customary_duplicates)
{
    std::map<std::string, units::precise_unit> testMap;
    for (const auto& ustring : units::defined_unit_strings_customary) {
        if (ustring.first == nullptr) {
            continue;
        }
        auto res = testMap.emplace(ustring.first, ustring.second);
        EXPECT_TRUE(res.second) << "duplicate unit string " << ustring.first;
    }
}

TEST(unit_string_definitions, combined_duplicates)
{
    std::map<std::string, units::precise_unit> testMap;
    for (const auto& ustring : units::defined_unit_strings_si) {
        if (ustring.first == nullptr) {
            continue;
        }
        auto res = testMap.emplace(ustring.first, ustring.second);
        EXPECT_TRUE(res.second) << "duplicate si unit string " << ustring.first;
    }

    for (std::size_t ii = 0; ii < units::defined_unit_strings_customary.size();
         ++ii) {
        if (units::defined_unit_strings_customary[ii].first == nullptr) {
            continue;
        }
        auto res = testMap.emplace(
            units::defined_unit_strings_customary[ii].first,
            units::defined_unit_strings_customary[ii].second);
        EXPECT_TRUE(res.second)
            << "duplicate unit string " << ii << " "
            << units::defined_unit_strings_customary[ii].first;
    }
}

TEST(unit_string_definitions, measurement_vector_length)
{
    for (std::size_t ii = 0; ii < units::defined_measurement_types.size();
         ++ii) {
        EXPECT_TRUE(units::defined_measurement_types[ii].first != nullptr)
            << ii;
        if (units::defined_measurement_types[ii].first == nullptr) {
            break;
        }
    }
}

TEST(unit_string_definitions, measurement_duplicates)
{
    std::map<std::string, units::precise_unit> testMap;
    for (const auto& ustring : units::defined_measurement_types) {
        if (ustring.first == nullptr) {
            continue;
        }
        auto res = testMap.emplace(ustring.first, ustring.second);
        EXPECT_TRUE(res.second)
            << "duplicate measurement type string " << ustring.first;
    }
}
