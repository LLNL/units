/*
Copyright (c) 2019,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#include "test.hpp"
#include "units/units.hpp"

#include <cmath>

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

TEST(leadingNumbers, power)
{
    size_t index = 0;
    auto res = testLeadingNumber("3^2", index);
    EXPECT_EQ(res, 9.0);
    EXPECT_GE(index, 3);

    res = testLeadingNumber("10.0^-2", index);
    EXPECT_DOUBLE_EQ(res, 0.01);

    res = testLeadingNumber("3.1^4.3houses", index);
    EXPECT_DOUBLE_EQ(res, pow(3.1, 4.3));
    EXPECT_GE(index, 7);
}

TEST(leadingNumbers, parenthesis_single)
{
    size_t index = 0;
    auto res = testLeadingNumber("(4.3)", index);
    EXPECT_EQ(res, 4.3);
    EXPECT_GE(index, 5);

    res = testLeadingNumber("1.2*(-2.4)", index);
    EXPECT_EQ(res, 1.2 * -2.4);

    res = testLeadingNumber("3.1^(4.3)houses", index);
    EXPECT_EQ(res, pow(3.1, 4.3));
    EXPECT_GE(index, 9);

    res = testLeadingNumber("1.2*-.9", index);
    EXPECT_EQ(res, 1.2 * -0.9);

    res = testLeadingNumber("1.2*", index);
    EXPECT_EQ(res, 1.2);
}

TEST(leadingNumbers, power_cascade)
{
    size_t index = 0;
    auto res = testLeadingNumber("2^2^2", index);
    EXPECT_EQ(res, 16.0);

    res = testLeadingNumber("2^(2^2)", index);
    EXPECT_EQ(res, 16.0);

    res = testLeadingNumber("3^(2^2)^2", index);
    EXPECT_EQ(res, pow(3, 16));

    res = testLeadingNumber("2^(2)^-2", index);
    EXPECT_EQ(res, pow(2.0, 0.25));
}

TEST(leadingNumbers, parenthesis_dual)
{
    size_t index = 0;
    auto res = testLeadingNumber("((2))", index);
    EXPECT_EQ(res, 2.0);

    res = testLeadingNumber("(((-2)))", index);
    EXPECT_EQ(res, -2.0);

    res = testLeadingNumber("(2)(2)", index);
    EXPECT_EQ(res, 4.0);

    res = testLeadingNumber("(2)(m)", index);
    EXPECT_EQ(res, 2.0);
    EXPECT_EQ(index, 3u);
}

TEST(leadingNumbers, complex)
{
    size_t index = 0;
    auto res = testLeadingNumber("4.1*0.7/9.5*8.6", index);
    EXPECT_EQ(res, 4.1 * 0.7 / 9.5 * 8.6);

    res = testLeadingNumber("4.1*0.7/(9.5*8.6)", index);
    EXPECT_EQ(res, 4.1 * 0.7 / (9.5 * 8.6));

    res = testLeadingNumber(
        "(1/2)^2*(1/2)^2*(1/2)^2*(1/2)^2*(1/2)^2*(1/2)^2*(1/2)^2*(1/2)^2*(1/2)^2", index);
    EXPECT_NEAR(res, pow(0.5, 18), 1e-20);
}

TEST(leadingNumbers, invalid)
{
    size_t index = 0;
    auto res = testLeadingNumber("(45*7*m)", index);
    EXPECT_TRUE((std::isnan)(res));
    EXPECT_EQ(index, 0);

    res = testLeadingNumber("meter", index);
    EXPECT_TRUE((std::isnan)(res));
    EXPECT_EQ(index, 0);

    res = testLeadingNumber("92*(452.252.34)", index);
    EXPECT_FALSE((std::isnan)(res));
    EXPECT_EQ(res, 92);
    EXPECT_EQ(index, 2);

    res = testLeadingNumber("2.3^(12m)", index);
    EXPECT_TRUE((std::isnan)(res));
    EXPECT_EQ(index, 0);
}

TEST(leadingNumbers, strange)
{
    size_t index = 0;
    auto res = testLeadingNumber("()", index);
    EXPECT_EQ(res, 1.0);
    EXPECT_GE(index, 2);

    res = testLeadingNumber("5*(45+)", index);
    EXPECT_EQ(res, 5.0);
    EXPECT_EQ(index, 1);

    res = testLeadingNumber("56*(45.6*34.2", index);
    EXPECT_EQ(res, 56.0);
}


TEST(numericalwords, simple)
{
	size_t index = 0;
	auto res = testNumericalWords("hundredm", index);
	EXPECT_EQ(res, 100.0);
	EXPECT_EQ(index, 8);

	index = 0;
	res = testNumericalWords("millionmiles", index);
	EXPECT_EQ(res, 1e6);
	EXPECT_EQ(index, 8);

	index = 0;
	res = testNumericalWords("billionliters", index);
	EXPECT_EQ(res, 1e9);
	EXPECT_EQ(index, 8);
	index = 0;
	res = testNumericalWords("trillionpounds", index); 
	EXPECT_EQ(res, 1e12);
	EXPECT_EQ(index, 9);
}

TEST(numericalwords, compound)
{
	size_t index = 0;
	std::string tword = "hundredthousandm";
	auto res = testNumericalWords(tword, index);
	EXPECT_EQ(res, 1e5);
	EXPECT_EQ(tword[index], 'm');
	res = testNumericalWords("hundredmillion", index);
	EXPECT_EQ(res, 1e8);
	EXPECT_EQ(index, 14);
	res = testNumericalWords("fourhundredmillion", index);
	EXPECT_EQ(res, 4e8);
	EXPECT_EQ(index, 18);
}