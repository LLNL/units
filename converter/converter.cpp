/*
Copyright (c) 2019-2020,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include <iostream>
#include "units/units.hpp"

int main(int argc, char* argv[])
{
	auto meas = units::measurement_from_string(std::string(argv[1]) + ' ' + argv[2]);
	auto unit = units::unit_from_string(argv[3]);

	std::cout << meas.value_as(unit) << std::endl;
    return 0;
}
