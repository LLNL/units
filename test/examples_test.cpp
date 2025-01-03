/*
Copyright (c) 2019-2025,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "units/units_decl.hpp"

#include "units/units.hpp"

/** @file Test just to load up some unit definitions and do some operations with
them it tests linking and some usefulness in code coverage */

int main(int /*argc*/, char* /*argv*/[])
{
    units::fixed_precise_measurement fpm1(1250.0, units::precise::cm.pow(3));
    units::fixed_precise_measurement fpm2(1.25, units::precise::L);
    if (fpm1 != fpm2) {
        return -1;
    }

    units::uncertain_measurement um1(1250.0, 25.0, units::cm.pow(3));
    units::uncertain_measurement um2(1.25, 0.1, units::L);
    if (um1 != um2) {
        return -1;
    }

    units::measurement m1(1250.0, units::cm.pow(3));
    if (m1 != 1.25 * units::L) {
        return -1;
    }

    units::fixed_measurement fm1(1250.0, units::cm.pow(3));
    if (fm1 != 1.25 * units::L) {
        return -1;
    }

    units::precise_measurement pm1 =
        units::measurement_from_string("1250 cm^3");
    if (pm1 != 1.25 * units::precise::L) {
        return -1;
    }

    units::precise_unit prec1(1.25, units::precise::L);

    if (prec1 != units::precise_unit(0.00125, units::precise::m.pow(3))) {
        return -1;
    }

    units::unit u1(1.25, units::L);

    if (u1 != units::unit(0.00125, units::m.pow(3))) {
        return -1;
    }
    return 0;
}
