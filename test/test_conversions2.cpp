/*
Copyright (c) 2019-2025,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#include "test.hpp"
#include "units/units.hpp"

#include <algorithm>
#include <fstream>
#include <string>
#include <vector>

// structure containing information on a single conversion
struct convValue {
    std::string name;
    std::string short_name;
    double value{0};
    convValue() = default;
    convValue(const std::string& nm, const std::string& sn, double val) :
        name(nm), short_name(sn), value(val)
    {
    }
};
// fixture structure for doing the tests
struct converterApp : public ::testing::TestWithParam<std::string> {
    void loadFile(const std::string& file)
    {
        std::ifstream input(file);
        ASSERT_TRUE(input);
        std::string line;
        while (std::getline(input, line)) {
            if (line.empty()) {
                continue;
            }
            auto ploc = line.find_first_of('.');
            auto namestartloc = line.find_first_not_of(' ', ploc + 1);
            auto nameend = line.find_first_of("[:", namestartloc + 1);
            std::string name =
                line.substr(namestartloc, nameend - namestartloc);
            while (name.back() == ' ') {
                name.pop_back();
            }
            std::string shortname;
            if (line[nameend] == '[') {
                auto snameend = line.find_first_of(']', nameend + 1);
                shortname = line.substr(nameend + 1, snameend - nameend - 1);
            }
            std::getline(input, line);
            double value = stod(line);
            unit_conv.emplace_back(name, shortname, value);
        }
    }

    std::vector<convValue> unit_conv;
};

TEST_P(converterApp, fileConversions)
{
    std::string testFile = TEST_FILE_FOLDER "/test_conversions/";
    testFile.append(GetParam());
    testFile.append("_conversions.txt");
    loadFile(testFile);
    auto baseUnit = units::unit_from_string(unit_conv[0].name);
    double baseValue = unit_conv[0].value;
    for (auto& convcode : unit_conv) {
        auto unit = units::unit_from_string(convcode.name);
        EXPECT_FALSE(is_error(unit)) << "error converting " << convcode.name;
        if (!convcode.short_name.empty()) {
            auto unit_short = units::unit_from_string(convcode.short_name);
            EXPECT_FALSE(is_error(unit_short))
                << "error converting " << convcode.short_name;
            if (unit != unit_short) {
                if (unit == units::precise::log::bel) {
                    continue;  // this is a known choice to match SI
                }
                EXPECT_FLOAT_EQ(convert(1.0, unit, unit_short), 1.0F)
                    << convcode.short_name << " and " << convcode.name
                    << " do not match";
            } else {
                EXPECT_EQ(unit, unit_short)
                    << convcode.name << " and " << convcode.short_name
                    << " do not produce the same unit";
            }
        }
        double res = units::convert(baseValue, baseUnit, unit);
        EXPECT_NEAR(res / convcode.value, 1.0, 0.003)
            << unit_conv[0].name << " to " << convcode.name << " produced "
            << res << " when " << convcode.value << " expected";
        double return_value = units::convert(convcode.value, unit, baseUnit);
        EXPECT_NEAR(return_value / baseValue, 1.0, 0.003);
    }
}

static const std::vector<std::string> testFiles{
    "energy",
    "distance",
    "area",
    "volume",
    "acceleration",
    "charge",
    "concentration",
    "concentration_ratio",
    "data_transfer",
    "density",
    "flow",
    "force",
    "force_moment",
    "inductance",
    "mass",
    "mass_flow",
    "power",
    "power2",
    "pressure",
    "radiation",
    "resistivity",
    "specific_heat",
    "temperature",
    "time",
    "typography",
    "velocity",
    "viscosity",
    "volume_concentration",
    "angular_accel",
    "conductance",
    "conductivity",
    "electric_field",
    "potential",
    "resistance",
    "capacitance",
    "enzyme_activity",
    "molar_flow",
    "fraction",
    "efficiency",
    "fuel_consumption",
    "energy_per_vol",
    "heat_density",
    "heat_flux",
    "heat_transfer",
    "illumination",
    "linear_charge",
    "linear_current",
    "linear_mass",
    "luminance",
    "luminosity",
    "mag_field",
    "mag_flux_density",
    "mag_flux",
    "mmf",
    "mass_flux",
    "moment_of_inertia",
    "mol",
    "number",
    "dose",
    "activity",
    "exposure",
    "absorbed_dose",
    "sound",
    "surface_charge",
    "surface_current",
    "surface_tension",
    "thermal_conductivity",
    "angular_velocity",
    "kinematic_viscosity",
    "thermal_resistance",
    "charge_density",
};

// for generating a little nicer name
static std::string testName(const testing::TestParamInfo<std::string>& tparam)
{
    std::string res = tparam.param;
    res.erase((std::remove)(res.begin(), res.end(), '_'), res.end());
    return res;
}

INSTANTIATE_TEST_SUITE_P(
    conversionFiles,
    converterApp,
    ::testing::ValuesIn(testFiles),
    testName);
