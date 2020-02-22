/*
Copyright (c) 2019-2020,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#include "units/units.hpp"

#include "test.hpp"

using namespace units;
TEST(unitStrings, Simple)
{
    EXPECT_EQ(to_string(m), "m");
    EXPECT_EQ(to_string(kg), "kg");
    EXPECT_EQ(to_string(mol), "mol");
    EXPECT_EQ(to_string(A), "A");
    EXPECT_EQ(to_string(V), "V");
    EXPECT_EQ(to_string(rad), "rad");
    EXPECT_EQ(to_string(cd), "cd");
    EXPECT_EQ(to_string(pu), "pu");
}

TEST(unitStrings, Derived)
{
    EXPECT_EQ(to_string(precise::pressure::atm), "atm");
    EXPECT_EQ(to_string(N), "N");
    EXPECT_EQ(to_string(Pa), "Pa");
    EXPECT_EQ(to_string(in), "in");
    EXPECT_EQ(to_string(oz), "oz");
    EXPECT_EQ(to_string(ppb), "ppb");
    EXPECT_EQ(to_string(kat), "kat");
    EXPECT_EQ(to_string(precise::data::MiB), "MiB");
}

TEST(unitStrings, Inverted)
{
    EXPECT_EQ(to_string(N.inv()), "1/N");
    EXPECT_EQ(to_string(Pa.inv()), "1/Pa");
    EXPECT_EQ(to_string(in.inv()), "1/in");
    EXPECT_EQ(to_string(m.inv()), "1/m");
    EXPECT_EQ(to_string(s.inv()), "Hz");
}

TEST(unitStrings, squared)
{
    EXPECT_EQ(to_string(ft.pow(2)), "ft^2");
    EXPECT_EQ(to_string(N * N), "N^2");
    EXPECT_EQ(to_string(mm * mm), "mm^2");
    EXPECT_EQ(to_string((mm * mm).inv()), "1/mm^2");
    EXPECT_EQ(to_string(Hz * Hz), "Hz^2");
}

TEST(unitStrings, Combos)
{
    EXPECT_EQ(to_string(V / km), "V/km");
    EXPECT_EQ(to_string(currency / MWh), "$/MWh");
    EXPECT_EQ(to_string(currency / mile), "$/mi");
    EXPECT_EQ(to_string(kW / gal), "kW/gal");
}

TEST(unitStrings, si)
{
    auto nF = unit(1e-9, F);
    EXPECT_EQ(to_string(nF), "nF");
    EXPECT_EQ(to_string(one / nF), "1/nF");

    EXPECT_EQ(to_string(currency / nF), "$/nF");

    auto GPa = unit(1e9, Pa);
    EXPECT_EQ(to_string(GPa), "GPa");
    EXPECT_EQ(to_string(one / GPa), "1/GPa");
    // automatic reduction
    EXPECT_EQ(to_string(GPa / km), "MPa/m");

    // two non-si units
    EXPECT_EQ(to_string(ft * lb), "ft*lb");
}

TEST(unitStrings, numerical)
{
    auto odd = unit(0.125, m);
    EXPECT_EQ(to_string(odd), "0.125m");

    EXPECT_EQ(to_string(odd.inv()), "8m^-1");
}

TEST(unitStrings, prefixes)
{
    EXPECT_EQ(to_string(precise::mm), "mm");
    EXPECT_EQ(to_string(precise::micro * precise::m), "um");
    EXPECT_EQ(to_string((precise::micro * precise::m).pow(2)), "um^2");
    EXPECT_EQ(to_string(precise::mm.pow(2)), "mm^2");
    EXPECT_EQ(to_string(precise::nm.pow(2)), "nm^2");
    EXPECT_EQ(to_string(precise::mm.pow(3)), "uL");
    EXPECT_EQ(to_string(precise::mm.pow(3).inv()), "1/uL");
    EXPECT_EQ(to_string(precise::mL.inv()), "1/mL");
    EXPECT_EQ(to_string(precise::g * precise::mL.inv()), "kg/L");
    EXPECT_EQ(to_string(precise::micro * precise::L), "uL");
}

TEST(unitStrings, infinite)
{
    EXPECT_EQ(
        to_string(precise_unit(std::numeric_limits<double>::infinity(), precise::m / precise::s)),
        "INF*m/s");

    EXPECT_EQ(to_string(unit(-std::numeric_limits<double>::infinity(), m / s)), "-INF*m/s");
}

TEST(unitString, almostInfinite)
{
    precise_unit almost_inf(precise::s.pow(3) * precise::kg * precise::mol, 4.414e307);

    auto res = to_string(almost_inf);
    auto ai2 = unit_from_string(res);
    EXPECT_EQ(unit_cast(almost_inf), unit_cast(ai2));
}

TEST(unitStrings, nan)
{
    EXPECT_EQ(to_string(precise::error), "ERROR");

    EXPECT_EQ(to_string(precise::invalid), "NaN*ERROR");
    auto nanunit = precise_unit(std::numeric_limits<double>::quiet_NaN(), precise::one);
    auto res = to_string(nanunit);
    EXPECT_EQ(res, "NaN");
    auto nanresult = unit_from_string(res);
    EXPECT_TRUE(isnan(nanresult));
    EXPECT_EQ(to_string(nanunit * precise::m / precise::s), "NaN*m/s");

    EXPECT_EQ(to_string(unit(std::numeric_limits<double>::signaling_NaN(), m / s)), "NaN*m/s");
    auto retunit = unit_from_string("NaN*m/s");
    EXPECT_EQ(retunit.base_units(), (precise::m / precise::s).base_units());
    EXPECT_TRUE(isnan(retunit));
}

TEST(unitStrings, zero)
{
    auto zunit = precise_unit(0.0, precise::one);
    auto res = to_string(zunit);
    EXPECT_EQ(res, "0");
    auto zresult = unit_from_string(res);
    EXPECT_EQ(zresult, zunit);

    EXPECT_EQ(to_string(zunit * precise::m / precise::s), "0*m/s");
}

TEST(unitStrings, one)
{
    auto funit = precise_unit(4.5, precise::iflag * precise::pu);
    auto res = to_string(funit);
    EXPECT_EQ(res, "4.5*defunit");
}

TEST(unitStrings, invcube)
{
    auto funit = precise::us::rod.pow(-3);
    auto res = to_string(funit);
    EXPECT_EQ(res, "1/rd^3");
}

TEST(unitStrings, invtestUnits)
{
    auto funit = precise_unit(0.000001, precise::one / precise::cd / precise::time::day);
    auto res = to_string(funit);
    EXPECT_EQ(res, "1/(Mcd*day)");
}

TEST(unitStrings, downconvert)
{
    EXPECT_EQ(to_string(precise_unit(1000.0, precise::one / precise::kg)), "1/g");
}

TEST(unitStrings, powerunits)
{
    EXPECT_EQ(to_string((precise::giga * precise::m).pow(2)), "Gm^2");
    EXPECT_EQ(to_string(precise_unit(1000000000, s.inv() * m.pow(-3))), "1/(uL*s)");
}

TEST(unitStrings, crazyunits)
{
    unit cz{detail::unit_data(1, 2, 3, 1, -1, -1, 0, 0, 0, 0, 0, 0, 0, 0)};
    auto str = to_string(cz);
    EXPECT_EQ(str, "m*kg^2*s^3*A*K^-1*mol^-1");
    // thermal conductivity W/(m*K)
    unit tc{detail::unit_data(1, 1, -3, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0)};
    str = to_string(tc);
    EXPECT_EQ(str, "W*m^-1*K^-1");
    EXPECT_EQ(to_string(precise_unit(10, precise::pu)), "10*pu");
}

TEST(unitStrings, customUnits)
{
    EXPECT_EQ(to_string(precise::generate_custom_unit(762)), "CXUN[762]");
    EXPECT_EQ(to_string(precise::generate_custom_unit(0)), "CXUN[0]");
    EXPECT_EQ(to_string(precise::generate_custom_count_unit(12)), "CXCUN[12]");
    EXPECT_EQ(to_string(precise::generate_custom_count_unit(0)), "CXCUN[0]");
}

TEST(stringToUnits, Simple)
{
    EXPECT_EQ(precise::m, unit_from_string("m"));
    EXPECT_EQ(precise::m, unit_from_string("meter"));
}

TEST(stringToUnits, Power)
{
    EXPECT_EQ(precise::m.pow(2), unit_from_string("m^2"));
    EXPECT_EQ(precise::kg.pow(-3), unit_from_string("kg^-3"));
    EXPECT_EQ(precise::V.pow(-2), unit_from_string("1/V^2"));
    EXPECT_EQ(precise_unit(27.0, precise::one).pow(3), unit_from_string("27^3"));
}

TEST(stringToUnits, mult)
{
    EXPECT_EQ(precise::m.pow(2), unit_from_string("m*m"));
    EXPECT_EQ(precise::kg * precise::m, unit_from_string("kg*m"));
    EXPECT_EQ(precise::m.pow(5), unit_from_string("m*m*m*m*m"));
}

TEST(stringToUnits, Div)
{
    EXPECT_EQ(precise::one, unit_from_string("m/m"));
    EXPECT_EQ(precise::kg / precise::m, unit_from_string("kg/m"));
    EXPECT_EQ(precise::m, unit_from_string("m*m/m*m/m"));
}

TEST(stringToUnits, SIprefix)
{
    EXPECT_EQ(precise_unit(1e18, precise::W), unit_from_string("EW"));
    EXPECT_EQ(precise_unit(1e-9, precise::H), unit_from_string("nH"));
    EXPECT_EQ(precise_unit(1e-15, precise::s), unit_from_string("fs"));

    EXPECT_EQ(unit(1e18, W), unit_cast_from_string("EW"));
    EXPECT_EQ(unit(1e-9, H), unit_cast_from_string("nH"));
    EXPECT_EQ(unit(1e-15, s), unit_cast_from_string("fs"));
}

TEST(stringToUnits, Parenthesis)
{
    EXPECT_EQ(precise::W / (precise::hr * precise::lb), unit_from_string("W/(hr*lb)"));
    EXPECT_EQ(
        precise::W * precise::mol / (precise::hr * precise::lb),
        unit_from_string("(W*mol)/(hr*lb)"));
    EXPECT_EQ((precise::N * precise::time::yr).pow(2), unit_from_string("(N*yr)^2"));
    EXPECT_EQ((precise::N * precise::time::yr).pow(-2), unit_from_string("(N*yr)^-2"));
    EXPECT_EQ(
        (precise::N * precise::time::yr).pow(-2) / precise::currency,
        unit_from_string("(N*yr)^-2/$"));
}

TEST(stringToUnits, dotNotation)
{
    EXPECT_EQ(precise::m, unit_from_string("m.m.m/m.m", single_slash));
    EXPECT_EQ(precise::m.pow(3), unit_from_string("m.m.m/m.m"));
    EXPECT_EQ(precise::W / (precise::hr * precise::lb), unit_from_string("W/hr.lb", single_slash));
    EXPECT_EQ(
        precise::W * precise::mol / (precise::hr * precise::lb),
        unit_from_string("W.mol/hr.lb", single_slash));
}

TEST(stringToUnits, customUnits)
{
    EXPECT_EQ(precise::generate_custom_unit(762), unit_from_string("CXUN[762]"));
    EXPECT_EQ(precise::generate_custom_unit(0), unit_from_string("CXUN[]"));
    EXPECT_EQ(precise::generate_custom_unit(0), unit_from_string("CXUN[0]"));
    EXPECT_EQ(precise::generate_custom_count_unit(11), unit_from_string("CXCUN[11]"));
    EXPECT_EQ(precise::generate_custom_count_unit(0), unit_from_string("CXCUN[0]"));
}

TEST(stringToUnits, NumericalMultipliers)
{
    EXPECT_EQ(precise::km, unit_from_string("1000*m"));
    EXPECT_EQ(precise::mm, unit_from_string("km*1e-6"));
    EXPECT_EQ(precise::km, unit_from_string("1e3*0.001*1e6/1e3*m"));
    EXPECT_EQ(precise_unit(0.7564, precise::kg), unit_from_string("0.7564*kg"));
    EXPECT_EQ(precise::km, unit_from_string("1000m"));
    EXPECT_EQ(precise::kilo * precise::kg / precise::m, unit_from_string("1e3*0.001*1e6kg/1e3m"));
    EXPECT_EQ(precise_unit(0.7564, precise::kg), unit_from_string("0.7564kg"));
}

TEST(stringToUnits, outOfRangeNumbers)
{
    auto u1 = unit_from_string("2.76e309m");
    EXPECT_TRUE(isinf(u1));
    auto ucs = unit_cast_from_string("2.76e309m");
    EXPECT_TRUE(isinf(ucs));
}

TEST(stringToUnits, words)
{
    EXPECT_EQ(precise::mph, unit_from_string("miles per hour"));
    EXPECT_EQ(precise::mm.pow(3), unit_from_string("cubic millimeters"));
    EXPECT_EQ(
        precise::kg.pow(3) / precise::m.pow(2),
        unit_from_string("cubic kilograms per square meter"));
    EXPECT_EQ(precise::K * precise::milli, unit_from_string("milliKelvins"));
    EXPECT_EQ(precise::Hz, unit_from_string("reciprocal seconds"));
    EXPECT_EQ(precise::m.pow(2), unit_from_string("metres squared"));
    EXPECT_EQ(precise::mm.pow(2), unit_from_string("square millimeter"));
    EXPECT_EQ(precise::N * precise::m, unit_from_string("newton meter"));
    EXPECT_EQ(precise::ft * precise::lb, unit_from_string("ftlb"));
    EXPECT_EQ(precise::ft * precise::lb * precise::s, unit_from_string("ftlbsecond"));
    EXPECT_EQ(
        precise::gal * precise::second * precise::hp, unit_from_string("gallon second horsepower"));
    EXPECT_EQ(
        precise::Hz * precise::milli * precise::micro * precise::m,
        unit_from_string("millihertz micrometer"));

    EXPECT_TRUE(is_error(unit_from_string("bob and harry")));
    EXPECT_EQ(precise::pico * precise::T, unit_from_string("picotesla"));
    EXPECT_EQ(precise::pico * precise::A, unit_from_string("picoampere"));
    EXPECT_EQ(precise::pressure::psi, unit_from_string("pound per square inch"));
    EXPECT_EQ(precise::lm * precise::m.pow(2), unit_from_string("lumen square meter"));
}

TEST(stringToUnits, exponentForms)
{
    EXPECT_EQ(precise::m.pow(2), unit_from_string("m2"));
    EXPECT_EQ(precise::kg.pow(3), unit_from_string("kg3"));
    EXPECT_EQ(precise::kg / precise::m.pow(3), unit_from_string("kg/m3"));
    EXPECT_EQ(precise::kg / precise::m.pow(3), unit_from_string("kg.m-3"));
    EXPECT_EQ(precise::kg / precise::m.pow(3), unit_from_string("kg.m^(-3)"));
    EXPECT_EQ(precise::kg * precise::m.pow(3), unit_from_string("kg.m+3"));
    EXPECT_EQ(precise::kg / precise::m.pow(3), unit_from_string("kg/m**3"));
    EXPECT_EQ(precise::kg / precise::m.pow(3), unit_from_string("kg.m**-3"));
    EXPECT_EQ(precise::kg / precise::m.pow(3), unit_from_string("kg/m**+3"));

    EXPECT_EQ(precise::cm.pow(2), unit_from_string("cm2"));
    EXPECT_EQ(precise::mm.pow(2), unit_from_string("mm2"));
    EXPECT_EQ(precise::mm.pow(2), unit_from_string("mm<sup>2</sup>"));
    EXPECT_EQ(precise::mm.pow(2), unit_from_string("mm<sup>2<\\/sup>"));
    EXPECT_EQ((precise::micro * precise::meter).pow(2), unit_from_string("um2"));
    EXPECT_EQ(precise::cm.pow(5), unit_from_string("cm5"));
    EXPECT_EQ(unit_from_string("CM2", true), unit_from_string("cm2"));
}

TEST(stringToUnits, complex)
{
    EXPECT_EQ(
        precise::m / precise::s / precise::s, unit_from_string("meters per second per second"));
    EXPECT_EQ(
        precise::mile / precise::hr / precise::s, unit_from_string("miles per hour per second"));
    EXPECT_EQ(
        precise::micro * precise::mol / (precise_unit(0.1, precise::L)),
        unit_from_string("micromole per deciliter"));
    EXPECT_EQ(
        unit_from_string("$ per 24 hrs"), precise::currency / precise_unit(24.0, precise::hr));

    EXPECT_NE(unit_from_string("kilogram / minute(Gasoline at 15.5 C)"), precise::error);
    EXPECT_NE(unit_from_string("g/(8.h){shift}"), precise::error);

    EXPECT_TRUE(
        (precise::ohm * precise::meter).is_convertible(unit_from_string("statohm micrometer")));
    EXPECT_TRUE(unit_from_string("coulomb/square meter")
                    .is_convertible(unit_from_string("ampere hour/square meter")));
}

TEST(stringToUnits, interestingUnits)
{
    auto unit = unit_from_string("cubit (UK)");
    EXPECT_TRUE(precise::m.has_same_base(unit));

    unit = unit_from_string("ZAM", case_insensitive);
    EXPECT_EQ(unit, precise_unit(1e21, precise::m));

    unit = unit_from_string("m per s2 per Hz^1/2");
    EXPECT_EQ(unit, precise::special::ASD);
}

TEST(stringToUnits, customUnitforms)
{
    auto unit = unit_from_string("{APS'U}");
    EXPECT_TRUE(precise::custom::is_custom_unit(unit.base_units()));
}
TEST(stringToUnits, equivalents)
{
    EXPECT_EQ(unit_from_string("dB"), unit_from_string("decibels"));
    EXPECT_EQ(unit_from_string("daL"), unit_from_string("dekalitres"));
    EXPECT_EQ(unit_from_string("ug per m^3"), unit_from_string("ug/m3"));
    EXPECT_EQ(unit_from_string("10^-6 grams"), unit_from_string("ug"));
    EXPECT_EQ(unit_from_string("10^-12 grams"), unit_from_string("pg"));
    EXPECT_EQ(unit_from_string("U per 2 hrs"), unit_from_string("U/(2.h)"));
    EXPECT_EQ(unit_from_string("100 m^2"), precise::area::are);
    EXPECT_EQ(unit_from_string("/gram"), unit_from_string("/g"));
    EXPECT_EQ(unit_from_string(" per g"), precise::g.inv());
    EXPECT_EQ(unit_from_string("/deciliter"), unit_from_string("/dL"));
    EXPECT_EQ(unit_from_string("DM2/S2", true), unit_from_string("dm2/s2"));
    EXPECT_EQ(unit_from_string("/cu. m"), m.pow(3).inv());
    EXPECT_EQ(unit_from_string("a gregorian"), precise::time::ag);
    EXPECT_EQ(unit_from_string("cubic inches"), unit_from_string("[cin_i]"));
    EXPECT_EQ(unit_from_string("/mcl"), unit_from_string("/uL"));
    EXPECT_EQ(unit_from_string("/sec"), unit_from_string("/s"));
    EXPECT_EQ(unit_from_string("g.m"), unit_from_string("gram meter"));
}

TEST(stringToUnits, equivalents2)
{
    EXPECT_EQ(unit_from_string("in us"), unit_from_string("in_us"));
    EXPECT_EQ(unit_from_string("us in"), unit_from_string("in_us"));
    EXPECT_EQ(unit_from_string("CXCUN[1]^-1"), unit_from_string("/[arb'U]"));
    EXPECT_EQ(unit_from_string("[CCID_50]"), unit_from_string("CCID<sub>50</sub> "));
    EXPECT_EQ(unit_from_string("[CCID_50]"), unit_from_string("CCID<sub>50<\\/sub> "));
    EXPECT_EQ(unit_from_string("/100 neutrophils"), unit_from_string("/100{neutrophils}"));
    EXPECT_EQ(unit_from_string("carat/cubic inch"), unit_from_string("ct/in3"));
    EXPECT_EQ(
        unit_from_string("10.(UN.S)/(CM5.M2)", case_insensitive),
        unit_from_string("10.uN.s/(cm5.m2)"));
    EXPECT_EQ(unit_from_string("imperial miles"), unit_from_string("mi_br"));
    EXPECT_EQ(unit_from_string("cup us"), precise::us::cup);
    EXPECT_EQ(unit_from_string(" per sec"), unit_from_string("/s"));
    EXPECT_EQ(unit_from_string("ampere-hour"), unit_from_string("A.h"));
    EXPECT_EQ(
        unit_from_string("mL/cm[H2O]"), unit_from_string("milliliter per centimeter of water"));
    EXPECT_EQ(unit_from_string("sq m"), unit_from_string("m2"));
    EXPECT_EQ(unit_from_string("U/ 12hrs"), unit_from_string("U/(12.h)"));
    EXPECT_EQ(unit_from_string("Ncm"), unit_from_string("N.cm"));
    EXPECT_EQ(unit_from_string("ohm reciprocal"), unit_from_string("mho"));
    EXPECT_EQ(unit_from_string("per mins"), unit_from_string("/min"));
    EXPECT_EQ(unit_from_string("/100 WBCs"), unit_from_string("/100{WBCs}"));

    //   EXPECT_EQ(unit_from_string("lumen meters squared"), unit_from_string("lm.m2"));
}

TEST(stringToUnits, equivalents3)
{
    EXPECT_EQ(unit_from_string("grams per hour per metre sq"), unit_from_string("g/h/m2"));
    EXPECT_EQ(unit_from_string("dyn sec per cm"), unit_from_string("dyn.s/cm"));
    EXPECT_EQ(unit_from_string("Ns"), precise::N * precise::s);
    EXPECT_EQ(unit_from_string("N.s"), precise::N * precise::s);
    EXPECT_EQ(unit_from_string("Newton second"), precise::N * precise::s);
    auto u2 = unit_from_string("molcubicfoot");
    EXPECT_FALSE(is_error(u2));
    EXPECT_EQ(u2, precise::mol * precise::ft.pow(3));
    EXPECT_EQ(unit_from_string("(1)^345"), precise::one);
    EXPECT_EQ(unit_from_string("\t\t\t\t \r\n\n"), precise::defunit);
    auto u3 = unit_from_string("2^345");
    EXPECT_EQ(u3.multiplier(), std::pow(2.0, 345.0));
}

class roundTripString : public ::testing::TestWithParam<std::string> {
};

TEST_P(roundTripString, testRoundTrip)
{
    auto u1 = unit_from_string(GetParam());
    auto str = to_string(u1);
    EXPECT_EQ(unit_from_string(str), u1) << "Round trip string didn't match " << GetParam();
}

static const std::vector<std::string> testStrings{"million per milliliter",
                                                  "ng/(8.h)",
                                                  "mL/kg/d",
                                                  "ng/mL/h",
                                                  "10.L/min",
                                                  "[car_Au]",
                                                  "/[arb'U]",
                                                  "U/10*10{cells}",
                                                  "ag/{cell}",
                                                  "ug/mmol{creat}",
                                                  "[ppm]{v/v}",
                                                  "[IU]/g{Hb}",
                                                  "U/mL{RBCs}",
                                                  "{#}/{platelet}",
                                                  "[IU]/mL"};

INSTANTIATE_TEST_SUITE_P(roundTrip, roundTripString, ::testing::ValuesIn(testStrings));

TEST(roundTrip, DebugTest)
{
    auto u1 = unit_from_string("U/mL{RBCs}");
    auto str = to_string(u1);
    auto u2 = unit_from_string(str);
    EXPECT_EQ(u2, u1);
}

TEST(CommodityStrings, simple)
{
    auto u1 = unit_from_string("{absorbance}");
    EXPECT_FALSE(is_error(u1));
    EXPECT_NE(u1.commodity(), 0u);
}

#include <fstream>
#include <sstream>
TEST(fileops, ExtendedCharFile)
{
    std::ifstream input(TEST_FILE_FOLDER "/test_units_unicode.txt");
    ASSERT_TRUE(input);
    std::string line;
    std::getline(input, line);
    auto uni = unit_from_string(line);
    EXPECT_EQ(uni, precise::m / precise::s / precise::s);
    std::getline(input, line);
    uni = unit_from_string(line);
    EXPECT_EQ(uni, precise::N / precise::s);
    std::getline(input, line);
    uni = unit_from_string(line);
    EXPECT_EQ(uni, precise::N * precise::m * precise::s);
    std::getline(input, line);
    uni = unit_from_string(line);
    EXPECT_EQ(uni, precise::degC);
    std::getline(input, line);
    uni = unit_from_string(line);
    EXPECT_EQ(uni, precise::degF);
    std::getline(input, line);
    uni = unit_from_string(line);
    EXPECT_EQ(uni, precise::deg);
    std::getline(input, line);
    uni = unit_from_string(line);
    EXPECT_EQ(uni, precise::mph);
    std::getline(input, line);
    uni = unit_from_string(line);
    EXPECT_EQ(uni, precise::m * precise::micro);
    std::getline(input, line);
    uni = unit_from_string(line);
    EXPECT_EQ(uni, precise::N * precise::micro);
    std::getline(input, line);
    uni = unit_from_string(line);
    EXPECT_EQ(uni, precise::kg * precise::micro);
}

TEST(fileops, UnicodeFile)
{
    std::ifstream input(TEST_FILE_FOLDER "/test_units_unicode_u8.txt");
    ASSERT_TRUE(input);
    std::string line;
    std::getline(input, line);
    auto uni = unit_from_string(line);
    EXPECT_EQ(uni, precise::m / precise::s / precise::s);
    std::getline(input, line);
    uni = unit_from_string(line);
    EXPECT_EQ(uni, precise::N / precise::s);
    std::getline(input, line);
    uni = unit_from_string(line);
    EXPECT_EQ(uni, precise::N * precise::m * precise::s);
    std::getline(input, line);
    uni = unit_from_string(line);
    EXPECT_EQ(uni, precise::degC);
    std::getline(input, line);
    uni = unit_from_string(line);
    EXPECT_EQ(uni, precise::degF);
    std::getline(input, line);
    uni = unit_from_string(line);
    EXPECT_EQ(uni, precise::deg);
    std::getline(input, line);
    uni = unit_from_string(line);
    EXPECT_EQ(uni, precise::mph);
    std::getline(input, line);
    uni = unit_from_string(line);
    EXPECT_EQ(uni, precise::m * precise::micro);
    std::getline(input, line);
    uni = unit_from_string(line);
    EXPECT_EQ(uni, precise::N * precise::micro);
    std::getline(input, line);
    uni = unit_from_string(line);
    EXPECT_EQ(uni, precise::kg * precise::micro);
}

TEST(stringToUnits, invalid)
{
    auto u1 = unit_from_string("{(test}");
    EXPECT_TRUE(is_error(u1));
    EXPECT_FALSE(is_error(unit_from_string("{\\(test}")));

    auto u2 = unit_from_string("cubed");
    EXPECT_TRUE(is_error(u2));

    u2 = unit_from_string("tothethirdpower");
    EXPECT_TRUE(is_error(u2));

    u2 = unit_from_string("cubic");
    EXPECT_TRUE(is_error(u2));

    u2 = unit_from_string("m^-t");
    EXPECT_TRUE(is_error(u2));
    u2 = unit_from_string("m^4^-4");
    EXPECT_TRUE(is_error(u2));
    u2 = unit_from_string("m^(4)^-4");
    EXPECT_TRUE(is_error(u2));
    u2 = unit_from_string("m^-4^4");
    EXPECT_TRUE(is_error(u2));

    u2 = unit_from_string("m^(-4)^4");
    EXPECT_TRUE(is_error(u2));
}

TEST(userDefinedUnits, definitions)
{
    precise_unit clucks(19.3, precise::m * precise::A);
    addUserDefinedUnit("clucks", clucks);

    EXPECT_EQ(unit_from_string("clucks/A"), precise_unit(19.3, precise::m));

    EXPECT_EQ(to_string(clucks), "clucks");
}

TEST(userDefinedUnits, disableUserDefinitions)
{
    clearUserDefinedUnits();
    precise_unit clucks(19.3, precise::m * precise::A);
    disableUserDefinedUnits();
    addUserDefinedUnit("clucks", clucks);

    EXPECT_FALSE(is_valid(unit_from_string("clucks/A")));

    enableUserDefinedUnits();
    addUserDefinedUnit("clucks", clucks);

    EXPECT_EQ(unit_from_string("clucks/A"), precise_unit(19.3, precise::m));

    EXPECT_EQ(to_string(clucks), "clucks");
    clearUserDefinedUnits();
}

TEST(userDefinedUnits, clearDefs)
{
    precise_unit clucks(19.3, precise::m * precise::A);
    addUserDefinedUnit("clucks", clucks);

    EXPECT_EQ(unit_from_string("clucks/A"), precise_unit(19.3, precise::m));

    EXPECT_EQ(to_string(clucks), "clucks");

    clearUserDefinedUnits();
    EXPECT_FALSE(is_valid(unit_from_string("clucks/A")));

    EXPECT_NE(to_string(clucks), "clucks");
}

TEST(defaultUnits, unitTypes)
{
    EXPECT_EQ(default_unit("impedance quantity"), precise::ohm);
    EXPECT_EQ(default_unit("distance"), precise::m);
    EXPECT_EQ(default_unit("Quantity of distance"), precise::m);
    EXPECT_EQ(default_unit("frequency measure"), precise::Hz);
}

TEST(commoditizedUnits, basic)
{
    auto commu = unit_from_string("meter{cloth}");
    EXPECT_EQ(getCommodityName(commu.commodity()), "cloth");

    auto commu2 = unit_from_string("meter{Cloth}");
    EXPECT_EQ(getCommodityName(commu2.commodity()), "cloth");

    auto commu3 = unit_from_string("meter{CLOTH}");
    EXPECT_EQ(getCommodityName(commu3.commodity()), "cloth");

    auto commu4 = unit_from_string("meter of cloth");
    EXPECT_EQ(getCommodityName(commu4.commodity()), "cloth");

    EXPECT_EQ(commu4, commu);

    auto commu5 = unit_from_string("metre{CLOTH}");
    EXPECT_EQ(getCommodityName(commu5.commodity()), "cloth");

    auto commu6 = unit_from_string("metre of cloth");
    EXPECT_EQ(getCommodityName(commu6.commodity()), "cloth");

    EXPECT_EQ(commu6, commu);
}

TEST(commoditizedUnits, prefixed)
{
    auto commu = unit_from_string("millimeter{cloth}");
    EXPECT_EQ(getCommodityName(commu.commodity()), "cloth");

    auto commu2 = unit_from_string("KB{info}");
    EXPECT_EQ(getCommodityName(commu2.commodity()), "info");
    EXPECT_TRUE(commu2.has_same_base(precise::data::byte));
}

TEST(commoditizedUnits, numericalWords)
{
    auto commu = unit_from_string("20{unicorns}");
    EXPECT_EQ(getCommodityName(commu.commodity()), "unicorns");

    auto commu2 = unit_from_string("twenty{unicorns}");
    EXPECT_EQ(getCommodityName(commu2.commodity()), "unicorns");
    EXPECT_EQ(commu2, commu);

    auto commu3 = unit_from_string("two-million{unicorns}");
    EXPECT_EQ(commu3.multiplier(), 2000000);

    auto commu4 = unit_from_string("two-million{unicorns}{_}");
    EXPECT_EQ(commu3, commu4);
}

TEST(funnyStrings, underscore)
{
    auto bigNumber = unit_from_string("_45_625_252_22524_252452_25242522562_E522_");
    EXPECT_FALSE(isfinite(bigNumber));
    EXPECT_TRUE(isinf(bigNumber));

    EXPECT_EQ(precise_unit(45625252.0, precise::m), unit_from_string("_45_625_252_m_"));

    auto ukittens = unit_from_string("_45_625_252_kittens_");
    EXPECT_EQ(ukittens.commodity(), getCommodity("kittens"));

    EXPECT_EQ(precise_unit(45.0, precise::one), unit_from_string("45"));

    EXPECT_FALSE(is_valid(unit_from_string("_____-_____")));
}

TEST(funnyStrings, outofrange)
{ // these are mainly testing that it doesn't throw
    EXPECT_FALSE(isfinite(unit_from_string("1532^34e505"))); // out of range error
    EXPECT_TRUE(isinf(unit_from_string("34e505"))); // out of range
    EXPECT_TRUE(isinf(unit_from_string("-34e505"))); // out of range

    EXPECT_TRUE(isinf(unit_from_string("34.785e12458"))); // out of range of quad precision
}

TEST(funnyStrings, powersof1)
{ // check the power of 1 removals
    EXPECT_EQ(precise::m, unit_from_string("m*(1)^5"));
    EXPECT_EQ(precise::m, unit_from_string("m*(1)^"));
    EXPECT_EQ(precise::m.pow(2), unit_from_string("m*(1)^2*m")); // invalid

    EXPECT_EQ(precise::m, unit_from_string("m*(1)^-1"));
    EXPECT_EQ(precise::m, unit_from_string("m*(1)^-1*(1)^7"));
    EXPECT_EQ(precise::m, unit_from_string("m*(1)^+*(1)^+"));
    EXPECT_EQ(precise::m, unit_from_string("m^1^1"));
    EXPECT_EQ(precise::m, unit_from_string("m^(1)^1"));
    EXPECT_EQ(precise::m, unit_from_string("m^(1)^-"));
}

TEST(defaultUnits, singleCharacter)
{
    EXPECT_EQ(precise::m, default_unit("L"));
    EXPECT_EQ(precise::kg, default_unit("M"));
    EXPECT_EQ(precise::s, default_unit("T"));
    EXPECT_EQ(precise::A, default_unit("I"));
    EXPECT_EQ(precise::mol, default_unit("N"));
    EXPECT_EQ(precise::cd, default_unit("J"));
    EXPECT_EQ(precise::K, default_unit("\xC8"));
}
