/*
Copyright (c) 2019-2025,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "test.hpp"
#include "units/units.hpp"
#include "xml/tinyxml2.h"

#include <cmath>
#include <fstream>
#include <iostream>
#include <json.hpp>
#include <ostream>
#include <set>
#include <sstream>
#include <string>

TEST(UCUM, domain_exclusion)
{
    using namespace units::domains;
    // EXPECT_EQ(ucum | astronomy, allDomains);
    // EXPECT_EQ(ucum | nuclear, allDomains);
    // EXPECT_EQ(nuclear | astronomy, allDomains);
    EXPECT_EQ(surveying | cooking, us_customary);
    EXPECT_NE(ucum | cooking, us_customary);
    EXPECT_NE(nuclear | cooking, us_customary);
    EXPECT_NE(astronomy | cooking, us_customary);
    EXPECT_NE(ucum | surveying, us_customary);
    EXPECT_NE(nuclear | surveying, us_customary);
    EXPECT_NE(astronomy | surveying, us_customary);
}

TEST(UCUM, TestAllUnits)
{
    units::setUnitsDomain(units::domains::ucum);
    std::ifstream i(TEST_FILE_FOLDER "/ucumDefs.json");
    nlohmann::json defs;
    i >> defs;
    i.close();
    int ciFail = 0;
    int csFail = 0;
    int ceqFail = 0;
    for (const auto& junit : defs["units"]) {
        auto csCode = junit["csCode_"].get<std::string>();
        auto ciCode = junit["ciCode_"].get<std::string>();

        auto csact = units::unit_from_string(csCode);

        if (is_error(csact)) {
            std::cout << csCode << " does not result in a valid cs unit for "
                      << junit["name_"] << '\n';
            units::unit_from_string(csCode, 0);
            ++csFail;
        }
        auto ciact = units::unit_from_string(ciCode, units::case_insensitive);
        if (is_error(ciact)) {
            std::cout << ciCode << " does not result in a valid ci unit for "
                      << junit["name_"] << '\n';

            ++ciFail;
        }
        if (!(ciCode.empty() || csCode.empty())) {
            if (ciact != csact) {
                if (csCode.find(".h") != std::string::npos) {
                    continue;
                }
                if (csact / units::precise::count == ciact) {
                    continue;
                }
                std::cout << ciCode << " and " << csCode
                          << " do not produce the same unit " << junit["name_"]
                          << '\n';
                ++ceqFail;
            }
        }
    }
    EXPECT_EQ(csFail, 0);
    EXPECT_EQ(ciFail, 0);
    if (csFail > 0 || ciFail > 0) {
        std::cout << csFail << " units failed Case sensitive conversion\n";
        std::cout << ciFail << " units failed Case insensitive conversion\n";
    }
    if (ceqFail > 0) {
        std::cout << ceqFail
                  << " units failed to produce matching conversions\n";
    }
}

TEST(UCUM, TestAllVerify)
{
    std::ifstream i(TEST_FILE_FOLDER "/ucumDefs.json");
    nlohmann::json defs;
    i >> defs;
    i.close();
    int magError = 0;
    for (const auto& junit : defs["units"]) {
        auto csCode = junit["csCode_"].get<std::string>();

        auto csact = units::unit_from_string(csCode, units::strict_ucum);
        if (units::precise::custom::is_custom_unit(csact.base_units())) {
            continue;
        }
        if (units::precise::custom::is_custom_count_unit(csact.base_units())) {
            continue;
        }
        if (csact.base_units().has_e_flag()) {
            continue;
        }
        if (csact.is_equation()) {
            continue;
        }
        if (csCode.find("_50") != std::string::npos) {
            continue;
        }
        auto mag = junit["magnitude_"].get<double>();
        int mets = junit["dim_"]["dimVec_"][0].get<int>();
        int secs = junit["dim_"]["dimVec_"][1].get<int>();
        int kgs = junit["dim_"]["dimVec_"][2].get<int>();
        int rads = junit["dim_"]["dimVec_"][3].get<int>();
        int Ks = junit["dim_"]["dimVec_"][4].get<int>();
        int Cs = junit["dim_"]["dimVec_"][5].get<int>();
        int Cds = junit["dim_"]["dimVec_"][6].get<int>();
        int mols = junit["moleExp_"].get<int>();
        // different basis C vs A;  1 A = 1C/s;
        int As = Cs;
        secs += Cs;

        auto ubase = csact.base_units();
        EXPECT_EQ(ubase.meter(), mets) << csCode << ":error in meters";
        EXPECT_EQ(ubase.second(), secs) << csCode << ":error in seconds";
        EXPECT_EQ(ubase.kg(), kgs) << csCode << ":error in kilogram";
        EXPECT_EQ(ubase.radian(), rads) << csCode << ":error in radian";
        EXPECT_EQ(ubase.kelvin(), Ks) << csCode << ":error in kelvin";
        EXPECT_EQ(ubase.ampere(), As) << csCode << ":error in ampere";
        EXPECT_EQ(ubase.candela(), Cds) << csCode << ":error in candela";
        EXPECT_EQ(ubase.mole(), mols) << csCode << ":error in moles";
        if (mols == 1) {
            mag /= 6.0221366999999997e+23;
        }
        if (mols == -1) {
            mag *= 6.0221366999999997e+23;
        }
        if (kgs != 0) {
            mag /= pow(1000, kgs);
        }
        EXPECT_NEAR(mag / csact.multiplier(), 1.0, 0.01)
            << csCode << ":error in magnitude " << mag << " to "
            << csact.multiplier();
        if (std::fabs(mag / csact.multiplier() - 1.0) >= 0.01) {
            ++magError;
        }
    }
    if (magError > 0) {
        std::cout << magError << "Errors in definition magnitudes\n";
    }
}

TEST(UCUM, TestMatchingName)
{
    std::ifstream i(TEST_FILE_FOLDER "/ucumDefs.json");
    nlohmann::json defs;
    i >> defs;
    i.close();
    int nameMismatch = 0;
    for (const auto& junit : defs["units"]) {
        auto csCode = junit["csCode_"].get<std::string>();

        auto csact = units::unit_from_string(csCode, units::strict_ucum);
        csact.commodity(0);
        if (units::precise::custom::is_custom_unit(csact.base_units())) {
            continue;
        }
        if (units::precise::custom::is_custom_count_unit(csact.base_units())) {
            continue;
        }
        if (junit.contains("name_")) {
            auto name = junit["name_"].get<std::string>();
            if (name.empty()) {
                continue;
            }
            if (name.find("constant") != std::string::npos) {
                continue;
            }
            if (name.find("series") != std::string::npos) {
                continue;
            }
            if (name.find("the number") != std::string::npos) {
                continue;
            }
            if (name.find("vacuum") != std::string::npos) {
                continue;
            }
            if (name.find("(typography)") != std::string::npos) {
                name.erase(name.find("(typography)"), std::string::npos);
            }
            auto nameact = units::unit_from_string(name, units::strict_ucum);
            nameact.commodity(0);
            if (nameact.base_units().count() != csact.base_units().count()) {
                if (nameact.base_units().count() == -1) {
                    nameact = nameact * units::precise::count;
                }
            }
            EXPECT_EQ(nameact, csact) << csCode << " and " << name;
            if (nameact != csact) {
                ++nameMismatch;
                nameact = units::unit_from_string(name);
            }
        } else {
            std::cout << "no name for " << csCode << '\n';
        }
    }
    if (nameMismatch > 0) {
        std::cout << nameMismatch
                  << "name mismatches between written name and csName\n";
    }
}

TEST(UCUM, TestClass)
{
    static const std::set<std::string> ignore{
        "[h]", "[MET]", "[S]", "b", "[CCID_50]", "[TCID_50]", "[EID_50]"};

    std::ifstream i(TEST_FILE_FOLDER "/ucumDefs.json");
    nlohmann::json defs;
    i >> defs;
    i.close();
    int loincErr = 0;
    int propErr = 0;
    for (const auto& junit : defs["units"]) {
        auto csCode = junit["csCode_"].get<std::string>();

        auto csact = units::unit_from_string(csCode, units::strict_ucum);
        if (units::precise::custom::is_custom_unit(csact.base_units())) {
            continue;
        }
        if (units::precise::custom::is_custom_count_unit(csact.base_units())) {
            continue;
        }
        if (csact.is_equation()) {
            continue;
        }
        if (ignore.find(csCode) != ignore.end()) {
            continue;
        }
        if (!junit["loincProperty_"].is_null()) {
            auto propertyCode = junit["loincProperty_"].get<std::string>();
            if (!propertyCode.empty() &&
                propertyCode.find_first_of(';') == std::string::npos) {
                if (propertyCode.find("Rat") == std::string::npos) {
                    auto defMUnit = units::default_unit(propertyCode);
                    if (is_error(defMUnit)) {
                        std::cout << propertyCode
                                  << " does not produce a valid unit LPROP for "
                                  << junit["csCode_"].get<std::string>()
                                  << '\n';
                        ++loincErr;
                    }
                }
            }
        }
        if (!junit["property_"].is_null()) {
            auto propertyCode = junit["property_"].get<std::string>();
            auto defMUnit = units::default_unit(propertyCode);
            if (is_error(defMUnit)) {
                std::cout << propertyCode
                          << " does not produce a valid unit for "
                          << junit["csCode_"].get<std::string>() << "\n";
                ++propErr;
            }
        }
    }
    EXPECT_EQ(loincErr, 0);
    EXPECT_EQ(propErr, 0);
    if (loincErr > 0) {
        std::cout << loincErr << " error from LOINC property codes\n";
    }
    if (propErr > 0) {
        std::cout << propErr << " error from UCUM property codes\n";
    }
}

TEST(UCUM, TestMatchingPrint)
{
    std::ifstream i(TEST_FILE_FOLDER "/ucumDefs.json");
    nlohmann::json defs;
    i >> defs;
    i.close();
    int nameMismatch = 0;
    for (const auto& junit : defs["units"]) {
        auto csCode = junit["csCode_"].get<std::string>();

        auto csact = units::unit_from_string(csCode, units::strict_ucum);
        csact.commodity(0);
        if (units::precise::custom::is_custom_unit(csact.base_units())) {
            continue;
        }
        if (units::precise::custom::is_custom_count_unit(csact.base_units())) {
            continue;
        }
        if (junit.contains("printSymbol_")) {
            if (!junit["printSymbol_"].is_string()) {
                continue;
            }
            auto name = junit["printSymbol_"].get<std::string>();
            if (name.empty()) {
                continue;
            }
            if (name.find("<i>") != std::string::npos) {
                continue;
            }
            if (name.find("<r>") != std::string::npos) {
                continue;
            }
            if (name == "a" || name == "K" || name == "oz fl" ||
                name == "btu" || name == "S") {  // known conflicts are and
                                                 // Kayser with annum and Kelvin
                // oz fl has multiple meanings
                // btu default is different for power dominant vs thermal
                // dominant
                continue;
            }
            if (name == "X" || name == "C" || name == "M" || name == "Q" ||
                name == "B" || name == "%") {  // known conflicts are and Kayser
                                               // with annum and Kelvin
                // oz fl has multiple meanings
                // btu default is different for power dominant vs thermal
                // dominant
                continue;
            }
            if (csCode.front() == '[' && csCode.back() == ']' &&
                name.find_first_of('.') != std::string::npos) {
                continue;
            }

            auto nameact = units::unit_from_string(name, units::strict_ucum);
            nameact.commodity(0);

            if (nameact != csact) {
                if (csCode.find(".h") != std::string::npos) {
                    continue;
                }
                if (csact / units::precise::count == nameact) {
                    continue;
                }
                EXPECT_EQ(nameact, csact) << csCode << " and " << name;
                ++nameMismatch;
                nameact = units::unit_from_string(name);
            }
        } else {
            std::cout << "no name for " << csCode << '\n';
        }
    }
    if (nameMismatch > 0) {
        std::cout << nameMismatch
                  << "Print symbol mismatches between print name and csName\n";
    }
}

TEST(UCUM, TestMatchingSynonym)
{
    std::ifstream i(TEST_FILE_FOLDER "/ucumDefs.json");
    nlohmann::json defs;
    i >> defs;
    i.close();
    int nameMismatch = 0;
    for (const auto& junit : defs["units"]) {
        auto csCode = junit["csCode_"].get<std::string>();

        auto csact = units::unit_from_string(csCode, units::strict_ucum);
        csact.commodity(0);
        if (units::precise::custom::is_custom_unit(csact.base_units())) {
            continue;
        }
        if (units::precise::custom::is_custom_count_unit(csact.base_units())) {
            continue;
        }
        if (junit.contains("synonyms_")) {
            if (!junit["synonyms_"].is_string()) {
                continue;
            }
            auto name = junit["synonyms_"].get<std::string>();
            while (!name.empty()) {
                auto fs = name.find_first_of(';');
                auto synonym = name;
                if (fs != std::string::npos) {
                    synonym = name.substr(0, fs);
                    name = name.substr(fs + 1);
                } else {
                    name.clear();
                }
                if (name.find("constant") != std::string::npos) {
                    continue;
                }
                auto nameact =
                    units::unit_from_string(synonym, units::strict_ucum);
                nameact.commodity(0);

                if (nameact != csact) {
                    if (synonym.find("per volume") != std::string::npos) {
                        continue;
                    }
                    if (synonym.find("per time") != std::string::npos) {
                        continue;
                    }
                    if (csact / units::precise::count == nameact) {
                        continue;
                    }
                    bool hasDiv = (csCode.find('/') != std::string::npos);
                    bool hasPer = (synonym.find("per") != std::string::npos);
                    bool hasNDiv = (synonym.find('/') != std::string::npos);
                    if (hasDiv && !(hasPer || hasNDiv)) {
                        continue;
                    }
                    auto defMUnit = units::default_unit(synonym);
                    if (!is_error(defMUnit)) {
                        if (csact.has_same_base(defMUnit)) {
                            continue;
                        }
                    }
                    if (nameact.has_same_base(csact)) {
                        std::cout << synonym << " is not an exact match for "
                                  << csCode << " but has the same base units\n";
                    } else {
                        std::cout << synonym << " is not a synonym for "
                                  << csCode << '\n';
                    }
                    ++nameMismatch;
                    nameact = units::unit_from_string(synonym);
                }
            }
        }
    }
    if (nameMismatch > 0) {
        std::cout << nameMismatch
                  << "synonym name mismatches between synonym and csName\n";
    }
}

TEST(UCUM, TestExampleCodes)
{
    std::ifstream i(TEST_FILE_FOLDER "/example_ucum_codes.csv");
    std::string line;
    int nameMismatch = 0;
    while (std::getline(i, line)) {
        auto fc = line.find_first_of(',');
        auto fc2 = line.find_first_of(',', fc + 1);
        auto ustring = line.substr(fc + 1, fc2 - fc - 1);
        auto name = line.substr(fc2 + 1);
        auto csact = units::unit_from_string(ustring, units::strict_ucum);
        if (is_error(csact)) {
            std::cout
                << ustring
                << " did not produce a valid defined unit!!!!!!!!!!!!!!!\n";
        }
        if (units::precise::custom::is_custom_unit(csact.base_units())) {
            continue;
        }
        if (units::precise::custom::is_custom_count_unit(csact.base_units())) {
            continue;
        }
        if (csact.commodity() != 0) {
            continue;
        }
        auto nameact = units::unit_from_string(name, units::strict_ucum);
        bool hasCommodity = (csact.commodity() != 0);
        if (is_error(nameact)) {
            if (!hasCommodity) {
                std::cout << name << " did not produce a valid unit for "
                          << ustring << "\n";
                nameact = units::unit_from_string(name, units::strict_ucum);
            }
            continue;
        }

        csact.commodity(0);
        nameact.commodity(0);
        if (nameact != csact) {
            // if (csCode.find(".h") != std::string::npos)
            //{
            //   continue;
            //}
            if (csact / units::precise::count == nameact) {
                continue;
            }
            if (!hasCommodity) {
                std::cout << name << " is not a matching unit for " << ustring
                          << '\n';
                ++nameMismatch;
                nameact = units::unit_from_string(name, units::strict_ucum);
            }

            // nameact = units::unit_from_string(synonym);
        }
    }
    if (nameMismatch > 0) {
        std::cout << nameMismatch
                  << "test unit name mismatches between unit and description\n";
    }
}

TEST(UCUM, TestRoundTrip)
{
    std::ifstream i(TEST_FILE_FOLDER "/ucumDefs.json");
    nlohmann::json defs;
    i >> defs;
    i.close();
    int ciFail = 0;
    int csFail = 0;
    int ceqFail = 0;
    for (const auto& junit : defs["units"]) {
        auto csCode = junit["csCode_"].get<std::string>();
        auto csact = units::unit_from_string(csCode, units::strict_ucum);

        std::string ustring = to_string(csact);
        auto uact = units::unit_from_string(ustring, units::strict_ucum);

        if (is_error(uact)) {
            std::cout << csCode << "->" << ustring
                      << " does not result in a valid ustring unit for "
                      << junit["name_"] << '\n';

            ++ciFail;
        }

        if (uact != csact) {
            std::cout << ustring << " and " << csCode
                      << " do not produce the same unit " << junit["name_"]
                      << '\n';
            ++ceqFail;
        }
    }
    EXPECT_EQ(csFail, 0);
    EXPECT_EQ(ciFail, 0);
    EXPECT_EQ(ceqFail, 0);
    if (ceqFail > 0) {
        std::cout << ceqFail
                  << " units failed to produce matching conversions\n";
    }
}

TEST(UCUM, TestRoundTrip2)
{
    std::ifstream i(TEST_FILE_FOLDER "/example_ucum_codes.csv");
    std::string line;
    int ciFail = 0;
    int ceqFail = 0;
    while (std::getline(i, line)) {
        auto fc = line.find_first_of(',');
        auto fc2 = line.find_first_of(',', fc + 1);
        auto ustring = line.substr(fc + 1, fc2 - fc - 1);
        auto name = line.substr(fc2 + 1);
        auto csact = units::unit_from_string(ustring, units::strict_ucum);
        std::string genstring = to_string(csact);
        auto uact = units::unit_from_string(genstring, units::strict_ucum);

        if (is_error(uact)) {
            std::cout << ustring << "->" << genstring
                      << " does not result in a valid ustring unit for " << name
                      << '\n';

            ++ciFail;
            // units::unit_from_string(ustring);
            // to_string(csact);
        }

        if (uact != csact) {
            std::cout << genstring << " and " << ustring
                      << " do not produce the same unit " << name << '\n';
            ++ceqFail;
        }
    }
    if (ceqFail > 0) {
        std::cout << ceqFail
                  << "test unit name mismatches between unit and description\n";
    }
    EXPECT_EQ(ceqFail, 0);
    EXPECT_EQ(ciFail, 0);
}

TEST(UCUMConversions, Interpret1)
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(TEST_FILE_FOLDER "/UcumFunctionalTests.xml");

    auto cs = doc.FirstChildElement("ucumTests")
                  ->FirstChildElement("validation")
                  ->FirstChildElement("case");
    int failConvert = 0;
    while (cs != nullptr) {
        std::string unit = cs->FindAttribute("unit")->Value();
        bool valid = cs->FindAttribute("valid")->BoolValue();
        std::string id = cs->FindAttribute("id")->Value();
        auto prodUnit = units::unit_from_string(unit, units::strict_ucum);
        if (is_error(prodUnit)) {
            if (valid) {
                std::cout << "unable to convert " << unit
                          << " into a valid unit : unit test id " << id << '\n';
                ++failConvert;
            }
        } else if (!valid) {
            if (unit.back() != 'h') {
                std::cout << "converted " << unit
                          << " even though specified invalid " << id << '\n';
            }
        }

        cs = cs->NextSiblingElement("case");
    }
    EXPECT_EQ(failConvert, 0);
}

TEST(UCUMConversions, convert1)
{
    tinyxml2::XMLDocument doc;
    doc.LoadFile(TEST_FILE_FOLDER "/UcumFunctionalTests.xml");

    auto cs = doc.FirstChildElement("ucumTests")
                  ->FirstChildElement("conversion")
                  ->FirstChildElement("case");
    int failConvert = 0;
    while (cs != nullptr) {
        std::string unitFromString = cs->FindAttribute("srcUnit")->Value();
        std::string unitToString = cs->FindAttribute("dstUnit")->Value();
        double value = cs->FindAttribute("value")->DoubleValue();
        double outcome = cs->FindAttribute("outcome")->DoubleValue();
        // std::string id = cs->FindAttribute("id")->Value();
        auto fromUnit =
            units::unit_from_string(unitFromString, units::strict_ucum);
        auto toUnit = units::unit_from_string(unitToString, units::strict_ucum);

        double act = units::convert(value, fromUnit, toUnit);
        EXPECT_FALSE(std::isnan(act));
        if (!std::isnan(act)) {
            EXPECT_NEAR(outcome, act, 0.001 * outcome);
            if (std::fabs(outcome - act) / outcome > 0.001) {
                std::cout << unitFromString << " and " << unitToString
                          << " do not convert " << act
                          << " and expected=" << outcome << '\n';
            }
        }

        cs = cs->NextSiblingElement("case");
    }
    EXPECT_EQ(failConvert, 0);
}
