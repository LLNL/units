/*
Copyright (c) 2019-2021,
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
    while (cs != nullptr) {
        std::string def = cs->FirstChildElement("def")->FirstChild()->Value();

        auto definitionUnit = units::unit_from_string(def);
        if (is_error(definitionUnit)) {
            std::cout << "unable to convert " << def << " into a valid unit \n";
            ++failConvert;
        } else {
            auto name = cs->FirstChildElement("name");
            if (name != nullptr) {
                auto sname =
                    name->FirstChildElement("singular")->FirstChild()->Value();
                auto nameUnit = units::unit_from_string(sname);
                if (is_error(nameUnit)) {
                    std::cout << "unable to convert " << sname
                              << " into a valid unit \n";
                    ++failConvert;
                } else {
                    if (unit_cast(nameUnit) != unit_cast(definitionUnit)) {
                        std::cout << "name and unit do not match " << sname
                                  << " and " << def << "\n";
                        ++failConvert;
                    }
                }
            }
            // check for symbols
            auto sym = cs->FirstChildElement("symbol");
            while (sym != nullptr) {
                auto symString = sym->FirstChild()->Value();
                auto symUnit = units::unit_from_string(symString);
                if (is_error(symUnit)) {
                    std::cout << "unable to convert " << symString
                              << " into a valid unit \n";
                    ++failConvert;
                } else {
                    if (unit_cast(symUnit) != unit_cast(definitionUnit)) {
                        std::cout << "symbol and unit do not match "
                                  << symString << " and " << def << "\n";
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
    while (cs != nullptr) {
        std::string def = cs->FirstChildElement("def")->FirstChild()->Value();

        auto definitionUnit = units::unit_from_string(def);

        auto alias = cs->FirstChildElement("aliases");
        if (!alias) {
            cs = cs->NextSiblingElement("unit");
            continue;
        }
        auto name = alias->FirstChildElement("name");
        while (name != nullptr) {
            auto sname =
                name->FirstChildElement("singular")->FirstChild()->Value();
            auto nameUnit = units::unit_from_string(sname);
            if (is_error(nameUnit)) {
                std::cout << "unable to convert " << sname
                          << " into a valid unit def=" << def << '\n';
                ++failConvert;
            } else {
                if (unit_cast(nameUnit) != unit_cast(definitionUnit)) {
                    if (std::string(sname) != "gal") {
                        std::cout << "name and unit do not match " << sname
                                  << " and " << def << "\n";
                        ++failConvert;
                    }
                }
            }
            name = name->NextSiblingElement("name");
        }
        // check for symbols
        auto sym = alias->FirstChildElement("symbol");
        while (sym != nullptr) {
            auto symString = sym->FirstChild()->Value();
            auto symUnit = units::unit_from_string(symString);
            if (is_error(symUnit)) {
                std::cout << "unable to convert " << symString
                          << " into a valid unit def=" << def << '\n';
                ++failConvert;
            } else {
                if (unit_cast(symUnit) != unit_cast(definitionUnit)) {
                    if (std::string(symString) != "a") {
                        std::cout << "name and unit do not match " << symString
                                  << " and " << def << "\n";
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

TEST(UDUNITS, derived_name_symbols)
{
    tinyxml2::XMLDocument doc;
    auto err = doc.LoadFile(TEST_FILE_FOLDER "/UDUNITS2/udunits2-derived.xml");
    ASSERT_FALSE(err) << err << std::endl;
    auto cs = doc.FirstChildElement("unit-system")->FirstChildElement("unit");
    int failConvert = 0;
    while (cs != nullptr) {
        auto defEl = cs->FirstChildElement("def");
        // the only one without a definition is the radian since it is
        // dimensionless according to the xml
        std::string def =
            (defEl) ? defEl->FirstChild()->Value() : std::string("rad");

        auto definitionUnit = units::unit_from_string(def);
        if (is_error(definitionUnit)) {
            std::cout << "unable to convert " << def
                      << " into a valid definition unit \n";
            ++failConvert;
        } else {
            auto name = cs->FirstChildElement("name");
            if (name != nullptr) {
                auto sname =
                    name->FirstChildElement("singular")->FirstChild()->Value();
                auto nameUnit = units::unit_from_string(sname);
                if (is_error(nameUnit)) {
                    std::cout << "unable to convert " << sname
                              << " into a valid unit def=" << def << '\n';
                    ++failConvert;
                } else {
                    if (unit_cast(nameUnit) != unit_cast(definitionUnit)) {
                        std::cout << "name and unit do not match " << sname
                                  << " and " << def << "\n";
                        ++failConvert;
                    }
                }
            }
            // check for symbols
            auto sym = cs->FirstChildElement("symbol");
            while (sym != nullptr) {
                auto symString = sym->FirstChild()->Value();
                auto symUnit = units::unit_from_string(symString);
                if (is_error(symUnit)) {
                    std::cout << "unable to convert " << symString
                              << " into a valid unit def=" << def << '\n';
                    ++failConvert;
                } else {
                    if (unit_cast(symUnit) != unit_cast(definitionUnit)) {
                        std::cout << "symbol and unit do not match "
                                  << symString << " and " << def << "\n";
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

TEST(UDUNITS, derived_alias)
{
    tinyxml2::XMLDocument doc;
    auto err = doc.LoadFile(TEST_FILE_FOLDER "/UDUNITS2/udunits2-derived.xml");
    ASSERT_FALSE(err) << err << std::endl;
    auto cs = doc.FirstChildElement("unit-system")->FirstChildElement("unit");
    int failConvert = 0;
    while (cs != nullptr) {
        auto defEl = cs->FirstChildElement("def");
        // the only one without a definition is the radian since it is
        // dimensionless according to the xml
        std::string def =
            (defEl) ? defEl->FirstChild()->Value() : std::string("rad");

        auto definitionUnit = units::unit_from_string(def);

        auto alias = cs->FirstChildElement("aliases");
        if (alias == nullptr) {
            cs = cs->NextSiblingElement("unit");
            continue;
        }
        auto name = alias->FirstChildElement("name");
        while (name != nullptr) {
            auto sname =
                name->FirstChildElement("singular")->FirstChild()->Value();
            auto nameUnit = units::unit_from_string(sname);
            if (is_error(nameUnit)) {
                std::cout << "unable to convert " << sname
                          << " into a valid unit def=" << def << '\n';
                ++failConvert;
            } else {
                if (convert(unit_cast(nameUnit), unit_cast(definitionUnit)) !=
                    1.0) {
                    std::cout << "name and unit do not match " << sname
                              << " and " << def << "\n";
                    ++failConvert;
                }
            }
            name = name->NextSiblingElement("name");
        }
        // check for symbols
        auto sym = alias->FirstChildElement("symbol");
        while (sym != nullptr) {
            auto symString = sym->FirstChild()->Value();
            auto symUnit = units::unit_from_string(symString);
            if (is_error(symUnit)) {
                std::cout << "unable to convert " << symString
                          << " into a valid unit def=" << def << '\n';
                ++failConvert;
            } else {
                if (convert(unit_cast(symUnit), unit_cast(definitionUnit)) !=
                    1.0) {
                    std::cout << "name and unit do not match " << symString
                              << " and " << def << "\n";
                    ++failConvert;
                }
            }
            sym = sym->NextSiblingElement("symbol");
        }

        cs = cs->NextSiblingElement("unit");
    }
    EXPECT_EQ(failConvert, 0);
}

TEST(UDUNITS, common_alias)
{
    tinyxml2::XMLDocument doc;
    auto err = doc.LoadFile(TEST_FILE_FOLDER "/UDUNITS2/udunits2-common.xml");
    ASSERT_FALSE(err) << err << std::endl;
    auto cs = doc.FirstChildElement("unit-system")->FirstChildElement("unit");
    int failConvert = 0;
    while (cs != nullptr) {
        std::string def = cs->FirstChildElement("def")->FirstChild()->Value();

        auto definitionUnit = units::unit_from_string(def);

        auto alias = cs->FirstChildElement("aliases");
        if (alias == nullptr) {
            cs = cs->NextSiblingElement("unit");
            continue;
        }
        auto name = alias->FirstChildElement("name");
        while (name != nullptr) {
            auto sname =
                name->FirstChildElement("singular")->FirstChild()->Value();
            auto nameUnit = units::unit_from_string(sname);
            if (is_error(nameUnit)) {
                std::cout << "unable to convert " << sname
                          << " into a valid unit def=" << def << '\n';
                ++failConvert;
            } else {
                if (convert(unit_cast(nameUnit), unit_cast(definitionUnit)) !=
                    1.0) {
                    std::cout << "name and unit do not match " << sname
                              << " and " << def << "\n";
                    ++failConvert;
                }
            }
            if (name->FirstChildElement("plural")) {
                auto pname =
                    name->FirstChildElement("plural")->FirstChild()->Value();
                nameUnit = units::unit_from_string(pname);
                if (is_error(nameUnit)) {
                    std::cout << "unable to convert plural name " << pname
                              << " into a valid unit \n";
                    ++failConvert;
                } else {
                    if (convert(
                            unit_cast(nameUnit), unit_cast(definitionUnit)) !=
                        1.0) {
                        std::cout << "name and unit do not match " << pname
                                  << " and " << def << "\n";
                        ++failConvert;
                    }
                }
            }
            name = name->NextSiblingElement("name");
        }
        // check for symbols
        auto sym = alias->FirstChildElement("symbol");
        while (sym != nullptr) {
            auto symString = sym->FirstChild()->Value();
            auto symUnit = units::unit_from_string(symString);
            if (is_error(symUnit)) {
                std::cout << "unable to convert " << symString
                          << " into a valid unit def=" << def << '\n';
                ++failConvert;
            } else {
                if (convert(unit_cast(symUnit), unit_cast(definitionUnit)) !=
                    1.0) {
                    std::cout << "name and unit do not match " << symString
                              << " and " << def << "\n";
                    ++failConvert;
                }
            }
            sym = sym->NextSiblingElement("symbol");
        }

        cs = cs->NextSiblingElement("unit");
    }
    if (failConvert > 0) {
        std::cout << "unable to match " << failConvert
                  << " units and symbols\n";
    }
    // EXPECT_EQ(failConvert, 0);
}
