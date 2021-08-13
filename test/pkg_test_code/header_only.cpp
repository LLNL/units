/*
Copyright (c) 2019,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include "units/units.hpp"

#include <iostream>

int main()
{
    units::measurement b(50.0, units::m);
    units::measurement c(25.0, units::f);
    auto k = b * c;

    if (k.units() == units::units(m.pow(2))) {
        std::cout << "PASS\n";
    } else {
        std::cout << "FAILE\n";
    }
    return 0;
}
