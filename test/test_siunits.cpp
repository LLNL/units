/*
Copyright (c) 2019-2025,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "test.hpp"
#include "units/units.hpp"

#include <cmath>
#include <fstream>
#include <ostream>
#include <sstream>
#include <string>
#include <utility>
#include <vector>

static std::vector<std::vector<std::string>>
    loadCsvFile(const std::string& file)
{
    std::ifstream input(file);
    if (!input) {
        return {};
    }

    std::vector<std::vector<std::string>> lines;

    std::string line;
    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }
        auto ploc = line.find_first_of(',');
        std::vector<std::string> lineStrings;
        size_t sloc = 0;

        while (ploc != std::string::npos) {
            lineStrings.push_back(line.substr(sloc, ploc - sloc));
            sloc = ploc + 1;
            ploc = line.find_first_of(',', sloc);
        }

        lineStrings.push_back(line.substr(sloc));
        lines.push_back(std::move(lineStrings));
    }
    return lines;
}

TEST(SI, keyUnits)
{
    std::string fileName(TEST_FILE_FOLDER "/SI_Units.csv");
    auto data = loadCsvFile(fileName);
    for (const auto& si : data) {
        std::string type = si[0];
        std::string longForm = si[1];
        std::string shortForm = si[2];
        std::string alt = (si.size() > 3) ? si[3] : std::string{};

        auto utype = units::default_unit(type);
        auto longConv = units::unit_from_string(longForm);
        auto shortConv = units::unit_from_string(shortForm);
        units::precise_unit altConv;
        if (!alt.empty()) {
            altConv = units::unit_from_string(alt);
        }
        EXPECT_TRUE(utype.has_same_base(longConv))
            << type << " does not generate the same unit base as " << longForm;

        EXPECT_EQ(longConv, shortConv)
            << longForm << " does not generate the same unit as " << shortForm;

        if (!alt.empty()) {
            if (longConv.is_equation()) {
                // conversion between equivalent equation units is not
                // completely equivalent
                continue;
            }
            if (longConv.has_e_flag()) {
                if (unit_cast(longConv) != unit_cast(altConv)) {
                    longConv.clear_flags();
                }
            }
            EXPECT_EQ(unit_cast(longConv), unit_cast(altConv))
                << alt << " does not generate the same unit as " << longForm;
            // if (unit_cast(longConv) != unit_cast(altConv)) {
            //     longConv = units::unit_from_string(alt);
            // }
            if (shortConv.has_e_flag()) {
                if (unit_cast(shortConv) != unit_cast(altConv)) {
                    shortConv.clear_flags();
                }
            }
            EXPECT_EQ(unit_cast(shortConv), unit_cast(altConv))
                << alt << " does not generate the same unit as " << shortForm;
        }
    }
}

TEST(SI, examples)
{
    std::string fileName(TEST_FILE_FOLDER "/si_examples.csv");
    auto data = loadCsvFile(fileName);
    for (const auto& si : data) {
        std::string type = si[0];
        std::string longForm = si[1];
        std::string shortForm = si[2];

        auto utype = units::default_unit(type);
        auto longConv = units::unit_from_string(longForm);
        auto shortConv = units::unit_from_string(shortForm);

        EXPECT_TRUE(utype.has_same_base(longConv))
            << type << " does not generate the same unit base as " << longForm;

        EXPECT_EQ(longConv, shortConv)
            << longForm << " does not generate the same unit as " << shortForm;
        if (longConv != shortConv) {
            longConv = units::unit_from_string(longForm);
        }
    }
}
