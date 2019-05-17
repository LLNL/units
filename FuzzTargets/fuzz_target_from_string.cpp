/*
Copyright © 2019,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "units/units.hpp"
#include <exception>
#include <string>

static bool cflag = units::disableCustomCommodities();

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    std::string test1(reinterpret_cast<const char *>(Data), Size);
    auto unit = units::unit_from_string(test1);
    if (!unit.is_error())
    {
        auto str = units::to_string(unit);
        auto u2 = units::unit_from_string(str);
        if (u2.is_error())
        {
            throw(6u);
        }
        if (units::unit_cast(u2) != units::unit_cast(unit))
        {
            if (std::isnormal(u2.multiplier()) && std::isnormal(unit.multiplier()))
            {
                throw(5.0);
            }
            else if (u2.base_units() != unit.base_units())
            {
                throw(7);
            }
        }
    }
    // its::clearCustomCommodities();
    return 0;  // Non-zero return values are reserved for future use.
}
