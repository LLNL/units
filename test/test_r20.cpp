/*
Copyright (c) 2019-2020,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/

#include "test.hpp"
#include "units/units.hpp"

#include <iostream>
#include <string>
#include <tuple>

using unitD = std::tuple<const char*, const char*, units::precise_unit>;
#ifdef ENABLE_UNIT_MAP_ACCESS
// Check the order of the array make sure it is order for searching
TEST(r20, order)
{
    std::size_t unit_count{0};
    const void* r20 = units::detail::r20rawData(unit_count);
    const auto* r20data = reinterpret_cast<const unitD*>(r20);
    for (size_t ii = 1; ii < unit_count; ++ii) {
        EXPECT_LT(
            std::string(std::get<0>(r20data[ii - 1])),
            std::string(std::get<0>(r20data[ii])));
    }
}

// check to make sure the name conversions match
TEST(r20, conversions)
{
    std::size_t unit_count{0};
    const void* r20 = units::detail::r20rawData(unit_count);
    const auto* r20data = reinterpret_cast<const unitD*>(r20);
    int missed{0};
    int correct{0};
    int matchWithCount{0};
    int matchWithPU{0};
    int matchUnitBase{0};
    int mismatchCommodity{0};
    int commQuantity{0};
    int commQuantity2{0};
    int convertible{0};
    int skipped{0};
    int failedConversion{0};
    int defaulted{0};
    for (size_t ii = 1; ii < unit_count; ++ii) {
        std::string ustr = std::string(std::get<1>(r20data[ii]));
        auto unit = units::measurement_from_string(ustr).as_unit();
        auto udir = units::unit_from_string(ustr);
        auto r20unit = std::get<2>(r20data[ii]);
        if (is_valid(unit)) {
            if (unit != r20unit && udir != r20unit) {
                if (units::unit_cast(unit) == units::unit_cast(r20unit)) {
                    ++mismatchCommodity;
                } else {
                    if (r20unit.commodity() != 0) {
                        ++commQuantity;
                        std::cout << ' ' << ii << " \""
                                  << std::get<0>(r20data[ii]) << "\" " << ustr
                                  << " conversion with commodity doesn't match "
                                  << to_string(unit) << " vs. "
                                  << to_string(r20unit) << '\n';
                    } else if (unit.commodity() != 0) {
                        ++commQuantity2;
                        std::cout << ' ' << ii << " \""
                                  << std::get<0>(r20data[ii]) << "\" " << ustr
                                  << " conversion with commodity doesn't match "
                                  << to_string(unit) << " vs. "
                                  << to_string(r20unit) << '\n';
                    } else if (r20unit.is_per_unit()) {
                        ++matchWithPU;
                        continue;
                    } else if (
                        unit / units::precise::count == r20unit ||
                        unit * units::precise::count == r20unit) {
                        ++matchWithCount;
                        continue;
                    } else if (unit.has_same_base(r20unit)) {
                        ++matchUnitBase;
                    } else if (!std::isnan(units::convert(unit, r20unit))) {
                        ++convertible;
                    } else {
                        EXPECT_EQ(unit, r20unit)
                            << ' ' << ii << " \"" << std::get<0>(r20data[ii])
                            << "\" " << ustr << " conversion does not match "
                            << to_string(unit) << " vs. " << to_string(r20unit);

                        ++missed;
                    }
                }
            } else {
                ++correct;
            }
        } else {
            if (r20unit == units::precise::one / units::precise::count) {
                ++defaulted;
            } else if (r20unit.commodity() == 0) {
                std::cout << "FAILED CONVERSION " << ii << " \""
                          << std::get<0>(r20data[ii]) << "\" " << ustr
                          << " not convertible to " << to_string(r20unit)
                          << '\n';
                ++failedConversion;
            } else {
                std::cout << "SKIPPED " << ii << " \""
                          << std::get<0>(r20data[ii]) << "\" " << ustr
                          << " not convertible to " << to_string(r20unit)
                          << '\n';
                ++skipped;
            }
        }
    }
    std::cout << missed << " r20 units not translated properly\n";
    std::cout << mismatchCommodity << " r20 units with mismatched commodity\n";
    std::cout << commQuantity << " r20 units with commodities not matching\n";
    std::cout << commQuantity2
              << " unit translations with commodities not matching\n";
    std::cout << skipped << " r20 units skipped\n";
    std::cout << failedConversion << " r20 units with failed conversion\n";
    std::cout << defaulted << " r20 units are still on default value\n";
    std::cout << matchWithPU << " r20 units matched with pu modifier\n";
    std::cout << matchUnitBase
              << " r20 units matched with same base but different number\n";
    std::cout << matchWithCount << " r20 units match with a count modifier\n";
    std::cout << convertible << " r20 unit are convertible to eachother\n";
    std::cout << correct << " r20 units correctly translated\n";
}

#endif

TEST(r20, units)
{
    using namespace units::precise;
    EXPECT_EQ(units::r20_unit("A65"), cgs::erg / (cm.pow(2) * s));

    EXPECT_EQ(
        units::r20_unit("54"),
        units::precise_unit(
            1.0, mass::tonne, units::commodities::packaging::theoretical));

    EXPECT_FALSE(is_valid(units::r20_unit("chaos")));
}
