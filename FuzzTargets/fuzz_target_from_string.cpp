/*
Copyright (c) 2019-2025,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "units/units.hpp"
#include <exception>
#include <string>

static const bool cflag = []() {
    units::disableCustomCommodities();
    return true;
}();

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size)
{
    if (Size == 0) {
        return 0;
    }
    std::string test1(reinterpret_cast<const char*>(Data), Size);
    auto unit1 = units::unit_from_string(test1);
    if (!is_error(unit1)) {
        auto str = units::to_string(unit1);
        auto u2 = units::unit_from_string(str);
        if (units::is_error(u2)) {
            throw(6u);
        }
        bool match = false;
        if (units::unit_cast(u2) == units::unit_cast(unit1)) {
            match = true;
        } else if (units::isnormal(root(u2, 2))) {
            if (root(units::unit_cast(u2), 2) ==
                root(units::unit_cast(unit1), 2)) {
                match = true;
            }
        }
        if (!match) {
            if (units::isnormal(root(u2, 3))) {
                if (root(units::unit_cast(u2), 3) ==
                    root(units::unit_cast(unit1), 3)) {
                    match = true;
                }
            }
        }
        if (!match) {
            if (std::isnormal(u2.multiplier()) &&
                std::isnormal(unit1.multiplier())) {
                throw(5.0);
            } else if (u2.base_units() != unit1.base_units()) {
                throw(7);
            } else {
                throw("nan");
            }
        }
    }
    // its::clearCustomCommodities();
    return 0;  // Non-zero return values are reserved for future use.
}
