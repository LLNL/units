/*
Copyright (c) 2019-2020,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#include "test.hpp"
#include "units/units.hpp"

#include <iostream>
#include <type_traits>

/*
unsigned int getCommodity(std::string comm);


std::string getCommodityName(unsigned int commodity);


void addCustomCommodity(std::string comm, unsigned int code);

void clearCustomCommodities();

bool disableCustomCommodities();

bool enableCustomCommodities();
*/

using namespace units;
TEST(commodities, get)
{
    auto c = getCommodity("oil");
    EXPECT_EQ(c, commodities::oil);

    c = getCommodity("corn");
    EXPECT_EQ(c, commodities::corn);

    c = getCommodity("soybeans");
    EXPECT_EQ(c, commodities::soybeans);

    c = getCommodity("cells");
    EXPECT_EQ(c, commodities::cell);
}

TEST(commodities, asString)
{
    auto c = getCommodity("oil");
    auto str = getCommodityName(c);
    EXPECT_EQ(str, "oil");
}

TEST(commodities, custom)
{
    addCustomCommodity("unit_tests", 26262352u);
    auto c = getCommodity("unit_tests");
    EXPECT_EQ(c, 26262352u);
    clearCustomCommodities();

    c = getCommodity("unit_tests");
    EXPECT_NE(c, 26262352u);
    clearCustomCommodities();
}

TEST(commodities, custom_disabled)
{
    disableCustomCommodities();
    addCustomCommodity("unit_tests", 26262352u);
    auto c = getCommodity("unit_tests");
    EXPECT_NE(c, 26262352u);
    enableCustomCommodities();
    addCustomCommodity("unit_tests", 26262352u);
    c = getCommodity("unit_tests");
    EXPECT_EQ(c, 26262352u);
    clearCustomCommodities();
}

TEST(commodities, custom_short)
{
    disableCustomCommodities();
    auto c = getCommodity("QQQQ");
    auto ss = getCommodityName(c);
    EXPECT_EQ(ss, "qqqq");

    c = getCommodity("a");
    ss = getCommodityName(c);
    EXPECT_EQ(ss, "a");

    c = getCommodity("bb");
    ss = getCommodityName(c);
    EXPECT_EQ(ss, "bb");

    c = getCommodity("ccc");
    ss = getCommodityName(c);
    EXPECT_EQ(ss, "ccc");

    c = getCommodity("ddddd");
    ss = getCommodityName(c);
    EXPECT_EQ(ss, "ddddd");

    // this one should fail
    c = getCommodity("eeeeee");
    ss = getCommodityName(c);
    EXPECT_NE(ss, "eeeeee");
    enableCustomCommodities();
}

TEST(commodities, custom_cx_number)
{
    disableCustomCommodities();
    unsigned int comm = 56474732;
    auto ss = getCommodityName(comm);
    EXPECT_EQ(ss, "CXCOMM[56474732]");
    auto c = getCommodity(ss);
    EXPECT_EQ(c, comm);

    enableCustomCommodities();
}

TEST(commodities, escape_strings)
{
    auto hcode = getCommodity("c\\{a");
    auto cstring = getCommodityName(hcode);
    EXPECT_EQ(cstring, "c{a");

    hcode = getCommodity("c\\}ab");
    cstring = getCommodityName(hcode);
    EXPECT_EQ(cstring, "c}ab");

    hcode = getCommodity("c\\(a");
    cstring = getCommodityName(hcode);
    EXPECT_EQ(cstring, "c(a");

    hcode = getCommodity("c\\)abc");
    cstring = getCommodityName(hcode);
    EXPECT_EQ(cstring, "c)abc");

    hcode = getCommodity("c\\[a");
    cstring = getCommodityName(hcode);
    EXPECT_EQ(cstring, "c[a");

    hcode = getCommodity("c\\]abc");
    cstring = getCommodityName(hcode);
    EXPECT_EQ(cstring, "c]abc");

    hcode = getCommodity("c\\\\abc");
    cstring = getCommodityName(hcode);
    EXPECT_EQ(cstring, "c\\abc");

    hcode = getCommodity("c\\%ab");
    cstring = getCommodityName(hcode);
    EXPECT_EQ(cstring, "c\\%ab");

    hcode = getCommodity(" ty ");
    cstring = getCommodityName(hcode);
    EXPECT_EQ(cstring, "_ty");
    clearCustomCommodities();
}

TEST(commodities, unusual_to_string)
{
    precise_unit com(1.0, precise::kg.inv(), getCommodity("happy'u"));
    auto str = to_string(com);
    EXPECT_EQ(unit_from_string(str), com);

    precise_unit com2(12.0, precise::kg.pow(-2), getCommodity("happy'u"));
    str = to_string(com2);
    EXPECT_EQ(unit_from_string(str), com2);

    precise_unit cominv = com.inv();
    str = to_string(cominv);
    EXPECT_EQ(unit_from_string(str), cominv);

    precise_unit com2inv(12.0, precise::m.pow(-2) * precise::kg, getCommodity("happy'u"));
    com2inv = com2inv.inv();
    str = to_string(com2inv);
    EXPECT_EQ(unit_from_string(str), com2inv);
}

TEST(commodities, unusual_from_string)
{
    auto punit = unit_from_string("{happy'u}");
    EXPECT_EQ(punit.commodity(), 0u);
    EXPECT_TRUE(precise::custom::is_custom_unit(punit.base_units()));
}
