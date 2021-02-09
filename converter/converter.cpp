/*
Copyright (c) 2019-2021,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "CLI11.hpp"
#include "units/units.hpp"
#include <cstdio>

int main(int argc, char* argv[])
{
    CLI::App app(
        "application to perform a conversion of a value from one unit to another",
        "unit_convert");
    bool full_string{false};
    bool simplified{false};
    app.add_flag(
        "--full,-f",
        full_string,
        "specify that the output should include the measurement and units");
    app.add_flag(
        "--simplified,-s",
        simplified,
        "simplify the units using the units library to_string functions"
        "and print the conversion string like full. "
        "This option will take precedence over --full");

    std::string measurement;
    app.add_option(
           "--measurement,measure",
           measurement,
           "measurement to convert .e.g '57.4 m', 'two thousand GB' '45.7*22.2 feet^3/s^2' ")
        ->expected(CLI::detail::expected_max_vector_size)
        ->type_name("[TEXT ...]")
        ->required()
        ->join(' ');
    std::string newUnits;
    app.add_option(
           "--convert,convert",
           newUnits,
           "the units to convert the measurement to, '*' to convert to base units")
        ->required();
    app.add_flag_callback("--version,-v", []() {
        std::cout << "Units conversion " UNITS_VERSION_STRING << '\n';
        throw CLI::Success();
    });
    app.positionals_at_end();

    CLI11_PARSE(app, argc, argv);

    auto meas = units::measurement_from_string(measurement);
    units::precise_unit u2;
    if (newUnits == "*" || newUnits == "<base>") {
        u2 = meas.convert_to_base().units();
        newUnits = units::to_string(u2);
    } else {
        u2 = units::unit_from_string(newUnits);
    }
    if (simplified) {
        std::printf(
            "%s = %g %s\n",
            to_string(meas).c_str(),
            meas.value_as(u2),
            to_string(u2).c_str());
    } else if (full_string) {
        std::printf(
            "%s = %g %s\n",
            measurement.c_str(),
            meas.value_as(u2),
            newUnits.c_str());
    } else {
        std::printf("%g\n", meas.value_as(u2));
    }

    return 0;
}
