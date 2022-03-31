/*
Copyright (c) 2019-2022,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "units/units_conversion_maps.hpp"
#include "test.hpp"
#include <map>

TEST(unit_string_definitions, si_vector_length) 
{
    for (int ii = 0; ii < units::defined_unit_strings_si.size(); ++ii)
    {
        EXPECT_TRUE(units::defined_unit_strings_si[ii].first != nullptr) << ii;
        if (units::defined_unit_strings_si[ii].first == nullptr) {
            break;
        }
    }
}

TEST(unit_string_definitions, customary_vector)
{
    for (int ii = 0; ii < units::defined_unit_strings_customary.size(); ++ii) {
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
    for (int ii = 0; ii < units::defined_unit_strings_si.size(); ++ii) {
        if (units::defined_unit_strings_si[ii].first==nullptr) {
            continue;
        }
        auto res = testMap.emplace(
            units::defined_unit_strings_si[ii].first,
            units::defined_unit_strings_si[ii].second);
        EXPECT_TRUE(res.second) << "duplicate si unit string "
                                << units::defined_unit_strings_si[ii].first;
    }
}

TEST(unit_string_definitions, customary_duplicates)
{
    std::map<std::string, units::precise_unit> testMap;
    for (int ii = 0; ii < units::defined_unit_strings_customary.size(); ++ii) {
        if (units::defined_unit_strings_customary[ii].first == nullptr) {
            continue;
        }
        auto res = testMap.emplace(
            units::defined_unit_strings_customary[ii].first,
            units::defined_unit_strings_customary[ii].second);
        EXPECT_TRUE(res.second) << "duplicate unit string "
            << units::defined_unit_strings_customary[ii].first;
    }
}

TEST(unit_string_definitions, combined_duplicates)
{
    std::map<std::string, units::precise_unit> testMap;
    for (int ii = 0; ii < units::defined_unit_strings_si.size(); ++ii) {
        if (units::defined_unit_strings_si[ii].first == nullptr) {
            continue;
        }
        auto res = testMap.emplace(
            units::defined_unit_strings_si[ii].first,
            units::defined_unit_strings_si[ii].second);
        EXPECT_TRUE(res.second) << "duplicate si unit string "
                                << units::defined_unit_strings_si[ii].first;
    }

     for (int ii = 0; ii < units::defined_unit_strings_customary.size(); ++ii) {
        if (units::defined_unit_strings_customary[ii].first == nullptr) {
            continue;
        }
        auto res = testMap.emplace(
            units::defined_unit_strings_customary[ii].first,
            units::defined_unit_strings_customary[ii].second);
        EXPECT_TRUE(res.second) << "duplicate unit string " << ii << " "
                                << units::defined_unit_strings_customary[ii].first;
    }
}