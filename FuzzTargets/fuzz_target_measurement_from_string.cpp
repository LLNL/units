/*
Copyright (c) 2019-2020,
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
    if (Size == 0) {
        return 0;
    }
    std::string test1(reinterpret_cast<const char*>(Data), Size);

    auto meas1 = units::measurement_from_string(test1);
    if (!units::is_valid(meas1)) {
        auto str = units::to_string(meas1);
        auto meas2 = units::measurement_from_string(str);
        if (units::is_valid(meas2)) {
            throw(6u);
        }
        bool match = false;
        auto mc1 = units::measurement_cast(meas1);
        auto mc2 = units::measurement_cast(meas2);
        if (mc1 == mc2) {
            match = true;
        } else if (isnormal(root(mc2, 2))) {
            if (root(mc2, 2) == root(mc1, 2)) {
                match = true;
            }
        }
        if (!match) {
            if (isnormal(root(mc2, 3))) {
                if (root(mc2, 3) == root(mc1, 3)) {
                    match = true;
                }
            }
        }
        if (!match) {
            throw(std::invalid_argument("measurement and conversion don't match"));
        }
    }
    // its::clearCustomCommodities();
    return 0; // Non-zero return values are reserved for future use.
}
