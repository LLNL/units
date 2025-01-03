/*
Copyright (c) 2020-2025,
Battelle Memorial Institute; Lawrence Livermore National Security, LLC; Alliance
for Sustainable Energy, LLC.  See the top-level NOTICE for additional details.
All rights reserved. SPDX-License-Identifier: BSD-3-Clause
*/

#include UNITS_LIB_HEADER

#include <iostream>

int main(int /*argc*/, char* /*argv*/[])
{
    auto u1 = units::measurement_from_string("10.7 meters per second");

    std::cout << to_string(u1) << std::endl;
    return 0;
}
