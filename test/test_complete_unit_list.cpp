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
#include <iostream>
#include <ostream>
#include <sstream>
#include <string>

static std::string hexConvert(const std::string& str)
{
    std::string outstring;

    outstring.reserve(str.size());
    std::size_t loc{0};
    while (loc < str.size()) {
        if (loc < str.size() - 2 && str[loc] == '=') {
            auto c1 = str[loc + 1];
            auto c2 = str[loc + 2];
            int res{0};
            bool invalid{false};
            if (c1 >= '0' && c1 <= '9') {
                res = (c1 - '0') * 16;
            } else if (c1 >= 'A' && c1 <= 'F') {
                res = (c1 - 'A' + 10) * 16;
            } else if (c1 >= 'a' && c1 <= 'f') {
                res = (c1 - 'a' + 10) * 16;
            } else {
                invalid = true;
            }

            if (c2 >= '0' && c2 <= '9') {
                res += (c2 - '0');
            } else if (c2 >= 'A' && c2 <= 'F') {
                res += (c2 - 'A' + 10);
            } else if (c2 >= 'a' && c2 <= 'f') {
                res += (c2 - 'a' + 10);
            } else {
                invalid = true;
            }
            if (!invalid) {
                loc += 3;
                outstring.push_back(static_cast<char>(res));
                continue;
            }
        }
        outstring.push_back(str[loc]);
        ++loc;
    }
    return outstring;
}

TEST(completeUnits, unitTypes)
{
    std::ifstream tfile(TEST_FILE_FOLDER "/complete_unit_list.txt");
    ASSERT_TRUE(tfile.is_open());
    std::string tp;
    int validCount{0};
    int invalidCount{0};

    while (std::getline(tfile, tp)) {
        if (tp.size() < 3 || tp.front() == '#') {
            continue;
        }
        auto cloc = tp.find_first_of(',');
        std::string ostring = tp.substr(0, cloc);
        std::string testUnitString = hexConvert(ostring);

        auto bunit = units::unit_from_string(testUnitString);
        if (is_valid(bunit)) {
            ++validCount;
        } else {
            ++invalidCount;
            std::cout << "could not convert " << testUnitString << "("
                      << ostring << ")\n";
        }
    }
    // this number should go down as more of them are supported
    EXPECT_LE(invalidCount, 1174);
    std::cout << "converted " << validCount << " units successfully\n";
    std::cout << " failed to convert " << invalidCount << " unit strings\n";
}
