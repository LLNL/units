/*
Copyright (c) 2019-2025,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "test.hpp"
#include "units/units_conversion_maps.hpp"
#include <map>
#include <string>
#include <unordered_map>

TEST(unitStringDefinitions, siVectorLength)
{
    for (std::size_t ii = 0; ii < units::defined_unit_strings_si.size(); ++ii) {
        EXPECT_TRUE(units::defined_unit_strings_si[ii].first != nullptr) << ii;
        if (units::defined_unit_strings_si[ii].first == nullptr) {
            break;
        }
    }
}

TEST(unitStringDefinitions, customaryVector)
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

#if !defined(UNITS_DISABLE_NON_ENGLISH_UNITS) ||                               \
    UNITS_DISABLE_NON_ENGLISH_UNITS == 0
TEST(unitStringDefinitions, nonenglishVector)
{
    for (std::size_t ii = 0;
         ii < units::defined_unit_strings_non_english.size();
         ++ii) {
        EXPECT_TRUE(
            units::defined_unit_strings_non_english[ii].first != nullptr)
            << ii;
        if (units::defined_unit_strings_non_english[ii].first == nullptr) {
            break;
        }
    }
}
#endif

TEST(unitStringDefinitions, siDuplicates)
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

TEST(unitStringDefinitions, customaryDuplicates)
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

#if !defined(UNITS_DISABLE_NON_ENGLISH_UNITS) ||                               \
    UNITS_DISABLE_NON_ENGLISH_UNITS == 0
TEST(unitStringDefinitions, nonenglishDuplicates)
{
    std::map<std::string, units::precise_unit> testMap;
    for (const auto& ustring : units::defined_unit_strings_non_english) {
        if (ustring.first == nullptr) {
            continue;
        }
        auto res = testMap.emplace(ustring.first, ustring.second);
        EXPECT_TRUE(res.second)
            << "duplicate non english unit string " << ustring.first;
    }
}
#endif
TEST(unitStringDefinitions, combinedDuplicates)
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
#if !defined(UNITS_DISABLE_NON_ENGLISH_UNITS) ||                               \
    UNITS_DISABLE_NON_ENGLISH_UNITS == 0
    for (std::size_t ii = 0;
         ii < units::defined_unit_strings_non_english.size();
         ++ii) {
        if (units::defined_unit_strings_non_english[ii].first == nullptr) {
            continue;
        }
        auto res = testMap.emplace(
            units::defined_unit_strings_non_english[ii].first,
            units::defined_unit_strings_non_english[ii].second);
        EXPECT_TRUE(res.second)
            << "duplicate non english unit string " << ii << " "
            << units::defined_unit_strings_non_english[ii].first;
    }
#endif
}

TEST(unitStringDefinitions, measurementVectorLength)
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

TEST(unitStringDefinitions, measurementDuplicates)
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

// test the output names

TEST(unitNameDefinitions, siVectorLength)
{
    for (std::size_t ii = 0; ii < units::defined_unit_names_si.size(); ++ii) {
        EXPECT_TRUE(units::defined_unit_names_si[ii].second != nullptr) << ii;
        if (units::defined_unit_names_si[ii].second == nullptr) {
            break;
        }
    }
}

TEST(unitNameDefinitions, customaryVector)
{
    for (std::size_t ii = 0; ii < units::defined_unit_names_customary.size();
         ++ii) {
        EXPECT_TRUE(units::defined_unit_names_customary[ii].second != nullptr)
            << ii;
        if (units::defined_unit_names_customary[ii].second == nullptr) {
            break;
        }
    }
}

TEST(unitNameDefinitions, siDuplicates)
{
    std::unordered_map<units::unit, std::string> testMap;
    for (const auto& ustring : units::defined_unit_names_si) {
        if (ustring.second == nullptr) {
            continue;
        }
        auto res = testMap.emplace(ustring.first, ustring.second);
        EXPECT_TRUE(res.second) << "duplicate si unit string " << ustring.second
                                << " matching with " << res.first->second;
    }
}

TEST(unitNameDefinitions, customaryDuplicates)
{
    std::unordered_map<units::unit, std::string> testMap;
    for (const auto& ustring : units::defined_unit_names_customary) {
        if (ustring.second == nullptr) {
            continue;
        }
        auto res = testMap.emplace(ustring.first, ustring.second);
        EXPECT_TRUE(res.second) << "duplicate unit string " << ustring.second
                                << " matching with " << res.first->second;
    }
}

TEST(unitNameDefinitions, combinedDuplicates)
{
    std::unordered_map<units::unit, std::string> testMap;
    for (const auto& ustring : units::defined_unit_names_si) {
        if (ustring.second == nullptr) {
            continue;
        }
        auto res = testMap.emplace(ustring.first, ustring.second);
        EXPECT_TRUE(res.second)
            << "duplicate si unit string " << ustring.second;
    }

    for (std::size_t ii = 0; ii < units::defined_unit_names_customary.size();
         ++ii) {
        if (units::defined_unit_names_customary[ii].second == nullptr) {
            continue;
        }
        auto res = testMap.emplace(
            units::defined_unit_names_customary[ii].first,
            units::defined_unit_names_customary[ii].second);
        EXPECT_TRUE(res.second)
            << "duplicate unit string " << ii << " "
            << units::defined_unit_names_customary[ii].second;
    }
}
