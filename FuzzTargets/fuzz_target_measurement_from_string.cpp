/*
Copyright (c) 2019-2025,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "units/units.hpp"
#include <cstring>
#include <exception>
#include <stdexcept>
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

    auto meas1 = units::measurement_from_string(test1);
    if (isnormal(meas1)) {
        auto str = to_string(meas1);
        auto meas2 = units::measurement_from_string(str);
        if (!meas2.units().has_same_base(meas1.units()) && !isnormal(meas2)) {
            throw(6u);
        }
        bool match = (meas1 == meas2);
        if (!match) {
            auto mc1 = units::measurement_cast(meas1);
            auto mc2 = units::measurement_cast(meas2);
            match = (mc1 == mc2);

            if (!match && isnormal(root(meas2, 2))) {
                match = (root(mc2, 2) == root(mc1, 2));
            }
            if (!match && isnormal(root(meas2, 3))) {
                match = (root(mc2, 3) == root(mc1, 3));
            }
            if (!match && !(isnormal(mc1) && isnormal(mc2))) {
                auto uc1 = unit_cast(meas1.as_unit());
                auto uc2 = unit_cast(meas2.as_unit());
                match = (uc1 == uc2);
            }
        }
        if (!match) {
            if (meas1.units() == meas2.units()) {
                throw(std::invalid_argument(
                    "measurement and conversion don't match but units do"));
            }
            throw(std::invalid_argument(
                "measurement and conversion don't match, units do not match"));
        }
    }
    // its::clearCustomCommodities();
    return 0;  // Non-zero return values are reserved for future use.
}
