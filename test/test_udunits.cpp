/*
Copyright © 2019,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "test.hpp"
#include "units/units.hpp"
#include "xml/tinyxml2.h"
#include <cmath>
#include <fstream>
#include <ostream>
#include <sstream>

TEST(UDUNITS, accepted_name_symbols)
{
    tinyxml2::XMLDocument doc;
    auto err = doc.LoadFile(TEST_FILE_FOLDER "/UDUNITS2/udunits2-accepted.xml");
    ASSERT_FALSE(err) << err << std::endl;
    auto cs = doc.FirstChildElement("unit-system")->FirstChildElement("unit");
    int failConvert = 0;
    while (cs)
    {
        std::string def = cs->FirstChildElement("def")->FirstChild()->Value();

        auto definitionUnit = units::unit_from_string(def);
        if (is_error(definitionUnit))
        {
            std::cout << "unable to convert " << def << " into a valid unit \n";
            ++failConvert;
        }
        else
        {
            auto name = cs->FirstChildElement("name");
            if (name)
            {
                auto sname = name->FirstChildElement("singular")->FirstChild()->Value();
                auto nameUnit = units::unit_from_string(sname);
                if (is_error(nameUnit))
                {
                    std::cout << "unable to convert " << sname << " into a valid unit \n";
                    ++failConvert;
                }
                else
                {
                    if (unit_cast(nameUnit) != unit_cast(definitionUnit))
                    {
                        std::cout << "name and unit do not match " << sname << " and " << def << "\n";
                        ++failConvert;
                    }
                }
            }
            // check for symbols
            auto sym = cs->FirstChildElement("symbol");
            while (sym)
            {
                auto symString = sym->FirstChild()->Value();
                auto symUnit = units::unit_from_string(symString);
                if (is_error(symUnit))
                {
                    std::cout << "unable to convert " << symString << " into a valid unit \n";
                    ++failConvert;
                }
                else
                {
                    if (unit_cast(symUnit) != unit_cast(definitionUnit))
                    {
                        std::cout << "symbol and unit do not match " << symString << " and " << def << "\n";
                        ++failConvert;
                    }
                }
                sym = sym->NextSiblingElement("symbol");
            }
        }
        cs = cs->NextSiblingElement("unit");
    }
    EXPECT_EQ(failConvert, 0);
}

TEST(UDUNITS, accepted_alias)
{
    tinyxml2::XMLDocument doc;
    auto err = doc.LoadFile(TEST_FILE_FOLDER "/UDUNITS2/udunits2-accepted.xml");
    ASSERT_FALSE(err) << err << std::endl;
    auto cs = doc.FirstChildElement("unit-system")->FirstChildElement("unit");
    int failConvert = 0;
    while (cs)
    {
        std::string def = cs->FirstChildElement("def")->FirstChild()->Value();

        auto definitionUnit = units::unit_from_string(def);

        auto alias = cs->FirstChildElement("aliases");
        if (!alias)
        {
            cs = cs->NextSiblingElement("unit");
            continue;
        }
        auto name = alias->FirstChildElement("name");
        while (name)
        {
            auto sname = name->FirstChildElement("singular")->FirstChild()->Value();
            auto nameUnit = units::unit_from_string(sname);
            if (is_error(nameUnit))
            {
                std::cout << "unable to convert " << sname << " into a valid unit \n";
                ++failConvert;
            }
            else
            {
                if (unit_cast(nameUnit) != unit_cast(definitionUnit))
                {
                    if (std::string(sname) != "gal")
                    {
                        std::cout << "name and unit do not match " << sname << " and " << def << "\n";
                        ++failConvert;
                    }
                }
            }
            name = name->NextSiblingElement("name");
        }
        // check for symbols
        auto sym = alias->FirstChildElement("symbol");
        while (sym)
        {
            auto symString = sym->FirstChild()->Value();
            auto symUnit = units::unit_from_string(symString);
            if (is_error(symUnit))
            {
                std::cout << "unable to convert " << symString << " into a valid unit \n";
                ++failConvert;
            }
            else
            {
                if (unit_cast(symUnit) != unit_cast(definitionUnit))
                {
                    if (std::string(symString) != "a")
                    {
                        std::cout << "name and unit do not match " << symString << " and " << def << "\n";
                        ++failConvert;
                    }
                }
            }
            sym = sym->NextSiblingElement("symbol");
        }

        cs = cs->NextSiblingElement("unit");
    }
    EXPECT_EQ(failConvert, 0);
}
