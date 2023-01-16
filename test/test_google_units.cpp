/*
Copyright (c) 2019-2023,
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

TEST(googleUnits, unitTypes)
{
    std::ifstream tfile(TEST_FILE_FOLDER "/google_defined_units.txt");
    if (tfile.is_open()){   //checking whether the file is open
        std::string tp;
        while(std::getline(tfile, tp)){ //read data from file object and put it into string.
            auto cloc=tp.find_first_of(':');
            std::string utype=tp.substr(0,cloc);
            if (utype == "Misc")
            {
                continue;
            }
            auto bunit=units::default_unit(utype);
            EXPECT_TRUE(is_valid(bunit))<<utype << " does not produce a valid default unit";
        }
    }
}

TEST(googleUnits, unitNames)
{
    std::ifstream tfile(TEST_FILE_FOLDER "/google_defined_units.txt");
    if (tfile.is_open()){   //checking whether the file is open
        std::string tp;
        while(std::getline(tfile, tp)){ //read data from file object and put it into string.
            auto cloc=tp.find_first_of(':');
            std::string utype=tp.substr(0,cloc);
            if (cloc == std::string::npos)
            {
                continue;
            }
            auto bunit=units::default_unit(utype);
            auto ustring=tp.substr(cloc+1);
            while (!ustring.empty())
            {
                auto commaloc=ustring.find_first_of(',');
                auto iustring=ustring.substr(0,commaloc);
                if (iustring.back() == ')')
                {
                    auto ploc=iustring.find_last_of('(');
                    auto abbrev=iustring.substr(ploc+1);
                    abbrev.pop_back();
                    auto aunit=units::unit_from_string(abbrev);
                    EXPECT_TRUE(aunit.has_same_base(bunit))<<abbrev<<" does not convert to a unit with the same base as "<<utype;
                    iustring.erase(ploc);
                }
                auto runit=units::unit_from_string(iustring);
                if (utype == "Misc")
                {
                    EXPECT_TRUE(is_valid(runit))<<iustring<<" does not convert to a unit with the same base as "<<utype;
                }
                else
                {
                    EXPECT_TRUE(runit.has_same_base(bunit))<<iustring<<" does not convert to a unit with the same base as "<<utype;
                }
                
                if (commaloc == std::string::npos)
                {
                    ustring.clear();
                }
                else
                {
                    ustring=ustring.substr(commaloc+1);
                }
                
            }
        }
    }
}

