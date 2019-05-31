/*
Copyright © 2019,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#include "test.hpp"
#include "units/units.hpp"

using namespace units::detail::testing;

TEST(leadingNumbers, simple)
{
    size_t index = 0;
    auto res = testLeadingNumber("56.7", index);
    EXPECT_EQ(res, 56.7);
    EXPECT_GE(index, 4);

    res = testLeadingNumber("-2.5", index);
    EXPECT_EQ(res, -2.5);
    EXPECT_GE(index, 3);

    res = testLeadingNumber("4meters", index);
    EXPECT_EQ(res, 4);
    EXPECT_GE(index, 1);
}

TEST(leadingNumbers, multiply)
{
    size_t index = 0;
    auto res = testLeadingNumber("4*3", index);
    EXPECT_EQ(res, 12.0);
    EXPECT_GE(index, 3);

    res = testLeadingNumber("-2.5*4", index);
    EXPECT_EQ(res, -10.0);

    res = testLeadingNumber("2.45*-7.3houses", index);
    EXPECT_EQ(res, 2.45 * -7.3);
    EXPECT_GE(index, 9);
}

TEST(leadingNumbers, divide)
{
    size_t index = 0;
    auto res = testLeadingNumber("4/2", index);
    EXPECT_EQ(res, 2.0);
    EXPECT_GE(index, 3);

    res = testLeadingNumber("10.0/-4", index);
    EXPECT_EQ(res, -2.5);

    res = testLeadingNumber("1.5e-3/3e1houses", index);
    EXPECT_EQ(res, 5e-5);
    EXPECT_GE(index, 10);
}
