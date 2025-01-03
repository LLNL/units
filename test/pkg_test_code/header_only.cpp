/*
Copyright (c) 2019-2025,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include UNITS_LIB_HEADER

#include <iostream>

int main()
{
    using namespace units;
    units::measurement b(50.0, m);
    units::measurement c(25.0, ft);
    auto k = b * c;

    if (k.units().has_same_base(m.pow(2))) {
        std::cout << "PASS\n";
    } else {
        std::cout << "FAILE\n";
    }
    return 0;
}
