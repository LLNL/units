/*
Copyright (c) 2019,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "units/units.hpp"
#include <cstring>
#include <exception>
#include <string>

static bool cflag = units::disableCustomCommodities();

extern "C" int LLVMFuzzerTestOneInput(const uint8_t* Data, size_t Size)
{
    if (Size <= 4) {
        return 0;
    }
    std::string test1(reinterpret_cast<const char*>(Data + 4), Size - 4);
    std::uint32_t flags;
    std::memcpy(&flags, Data, 4);
    auto unit1 = units::unit_from_string(test1, flags);
    if (!units::is_error(unit1)) {
        auto str = units::to_string(unit1);
        auto u2 = units::unit_from_string(str);
        if (units::is_error(u2)) {
            throw(6u);
        }
        bool match = false;
        if (units::unit_cast(u2) == units::unit_cast(unit1)) {
            match = true;
        } else if (isnormal(root(u2, 2))) {
            if (root(units::unit_cast(u2), 2) == root(units::unit_cast(unit1), 2)) {
                match = true;
            }
        }
        if (!match) {
            if (isnormal(root(u2, 3))) {
                if (root(units::unit_cast(u2), 3) == root(units::unit_cast(unit1), 3)) {
                    match = true;
                }
            }
        }
        if (!match) {
            if (std::isnormal(u2.multiplier()) && std::isnormal(unit1.multiplier())) {
                throw(5.0);
            } else if (u2.base_units() != unit1.base_units()) {
                throw(7);
            } else {
                throw("nan");
            }
        }
    }
    // its::clearCustomCommodities();
    return 0; // Non-zero return values are reserved for future use.
}
