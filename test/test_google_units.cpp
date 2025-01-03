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

TEST(googleUnits, unitTypes)
{
    std::ifstream tfile(TEST_FILE_FOLDER "/google_defined_units.txt");
    if (tfile.is_open()) {  // checking whether the file is open
        std::string tp;
        while (std::getline(tfile, tp)) {  // read data from file object and put
                                           // it into string.
            auto cloc = tp.find_first_of(':');
            std::string utype = tp.substr(0, cloc);
            if (utype == "Misc") {
                continue;
            }
            auto bunit = units::default_unit(utype);
            EXPECT_TRUE(is_valid(bunit))
                << utype << " does not produce a valid default unit";
        }
    }
}

TEST(googleUnits, unitNames)
{
    std::ifstream tfile(TEST_FILE_FOLDER "/google_defined_units.txt");
    int invalidMatches{0};
    if (tfile.is_open()) {  // checking whether the file is open
        std::string tp;
        while (std::getline(tfile, tp)) {  // read data from file object and put
                                           // it into string.
            auto cloc = tp.find_first_of(':');
            std::string utype = tp.substr(0, cloc);
            if (cloc == std::string::npos) {
                continue;
            }
            auto bunit = units::default_unit(utype);
            if (utype == "Unitless") {
                bunit = units::precise::count;
            }
            if (utype != "Misc") {
                EXPECT_TRUE(is_valid(bunit)) << "Base unit not found:" << utype;
            } else {
                bunit = units::precise::one;
            }

            auto ustring = tp.substr(cloc + 1);
            while (!ustring.empty()) {
                auto commaloc = ustring.find_first_of(',');
                auto iustring = ustring.substr(0, commaloc);
                if (iustring.back() == ')') {
                    auto ploc = iustring.find_last_of('(');
                    auto abbrev = iustring.substr(ploc + 1);
                    abbrev.pop_back();
                    auto aunit = units::unit_from_string(abbrev);
                    if (bunit != units::precise::one) {
                        EXPECT_TRUE(aunit.has_same_base(bunit))
                            << abbrev
                            << " is valid but had different base than "
                            << utype;
                    }
                    iustring.erase(ploc);
                }
                auto runit = units::unit_from_string(iustring);
                EXPECT_TRUE(is_valid(runit))
                    << iustring << "does not convert to a valid unit of "
                    << utype;
                if (commaloc == std::string::npos) {
                    ustring.clear();
                } else {
                    ustring = ustring.substr(commaloc + 1);
                }

                if (!is_valid(runit)) {
                    ++invalidMatches;
                } else if (utype != "Misc") {
                    bool convertible = runit.has_same_base(bunit) ||
                        runit.inv().has_same_base(bunit);
                    if (!convertible) {
                        auto conv = units::convert(runit, bunit);
                        if (!std::isnan(conv)) {
                            convertible = true;
                        }
                    }
                    EXPECT_TRUE(convertible)
                        << (++invalidMatches, iustring)
                        << " is valid but had different base than " << utype;
                }
            }
        }
        if (invalidMatches > 0) {
            std::cout << "Unable to match " << invalidMatches << " units "
                      << std::endl;
        }
    }
}
