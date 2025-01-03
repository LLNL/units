/*
Copyright (c) 2019-2025,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#include "units/units.hpp"

#include "test.hpp"
#include <algorithm>
#include <limits>
#include <string>
#include <vector>

#if (__cplusplus >= 201703L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703)
#ifndef UNITS_CONSTEXPR_IF_SUPPORTED
#define UNITS_CONSTEXPR_IF_SUPPORTED
#endif
#endif

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
    EXPECT_EQ(to_string(precise::other::ppb), "ppb");
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
    EXPECT_EQ(to_string(V / km), "mV/m");
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

TEST(unitStrings, laboratoryArbUnits)
{
    EXPECT_EQ(to_string(precise::laboratory::arbU), "arb. unit");
}

TEST(unitStrings, numerical)
{
    auto odd = unit(0.125, m);
    EXPECT_EQ(to_string(odd), "0.125m");

    EXPECT_EQ(to_string(odd.inv()), "8/m");
}

TEST(unitStrings, prefixes)
{
    EXPECT_EQ(to_string(precise::mm), "mm");
    EXPECT_EQ(to_string(precise::micro * precise::m), "um");
    EXPECT_EQ(to_string((precise::micro * precise::m).pow(2)), "um^2");
    EXPECT_EQ(to_string((precise::micro * precise::m).pow(-2)), "1/um^2");
    EXPECT_EQ(to_string(precise::mm.pow(2)), "mm^2");
    EXPECT_EQ(to_string(precise::nm.pow(2)), "nm^2");
    EXPECT_EQ(to_string(precise::mm.pow(3)), "uL");
    EXPECT_EQ(to_string(precise::mm.pow(3).inv()), "1/uL");
    EXPECT_EQ(to_string(precise::mL.inv()), "1/mL");
    EXPECT_EQ(to_string(precise::g * precise::mL.inv()), "kg/L");
    EXPECT_EQ(to_string(precise::micro * precise::L), "uL");
}

TEST(unitStrings, siPrefixes)
{
    EXPECT_EQ(to_string(unit_from_string("mm")), "mm");
    EXPECT_EQ(to_string(unit_from_string("cm")), "cm");
    EXPECT_EQ(to_string(unit_from_string("um")), "um");
    EXPECT_EQ(to_string(unit_from_string("nm")), "nm");
    EXPECT_EQ(to_string(unit_from_string("pm")), "pm");
    EXPECT_EQ(to_string(unit_from_string("fm")), "fm");
    EXPECT_EQ(to_string(unit_from_string("am")), "am");
    EXPECT_EQ(to_string(unit_from_string("zm")), "zm");
    EXPECT_EQ(to_string(unit_from_string("ym")), "ym");
    EXPECT_EQ(to_string(unit_from_string("dm")), "dm");
    EXPECT_EQ(to_string(unit_from_string("rm")), "rm");
    EXPECT_EQ(to_string(unit_from_string("qm")), "qm");

    EXPECT_EQ(to_string(unit_from_string("km")), "km");
    EXPECT_EQ(to_string(unit_from_string("Km")), "km");
    EXPECT_EQ(to_string(unit_from_string("Mm")), "Mm");
    EXPECT_EQ(to_string(unit_from_string("Gm")), "Gm");
    EXPECT_EQ(to_string(unit_from_string("Tm")), "Tm");
    EXPECT_EQ(to_string(unit_from_string("Em")), "Em");
    EXPECT_EQ(to_string(unit_from_string("Zm")), "Zm");
    EXPECT_EQ(to_string(unit_from_string("Ym")), "Ym");
    EXPECT_EQ(to_string(unit_from_string("Rm")), "Rm");
    EXPECT_EQ(to_string(unit_from_string("Qm")), "Qm");
}

TEST(unitStrings, siPrefixesStrict)
{
    EXPECT_EQ(to_string(unit_from_string("mm", strict_si)), "mm");
    EXPECT_EQ(to_string(unit_from_string("mF", strict_si)), "mF");
    EXPECT_EQ(to_string(unit_from_string("cm", strict_si)), "cm");
    EXPECT_EQ(to_string(unit_from_string("um", strict_si)), "um");
    EXPECT_EQ(to_string(unit_from_string("nm", strict_si)), "nm");
    EXPECT_EQ(to_string(unit_from_string("ng", strict_si)), "ng");
    EXPECT_EQ(to_string(unit_from_string("cL", strict_si)), "cL");
    EXPECT_EQ(to_string(unit_from_string("h$", strict_si)), "100$");
    EXPECT_EQ(to_string(unit_from_string("pm", strict_si)), "pm");
    EXPECT_EQ(to_string(unit_from_string("fm", strict_si)), "fm");
    EXPECT_EQ(to_string(unit_from_string("am", strict_si)), "am");
    EXPECT_EQ(to_string(unit_from_string("zm", strict_si)), "zm");
    EXPECT_EQ(to_string(unit_from_string("ym", strict_si)), "ym");
    EXPECT_EQ(to_string(unit_from_string("dm", strict_si)), "dm");
    EXPECT_EQ(to_string(unit_from_string("rm", strict_si)), "rm");
    EXPECT_EQ(to_string(unit_from_string("qm", strict_si)), "qm");

    EXPECT_EQ(to_string(unit_from_string("km", strict_si)), "km");
    EXPECT_EQ(to_string(unit_from_string("kA", strict_si)), "kA");
    EXPECT_EQ(to_string(unit_from_string("Mm", strict_si)), "Mm");
    EXPECT_EQ(to_string(unit_from_string("Gm", strict_si)), "Gm");
    EXPECT_EQ(to_string(unit_from_string("Tm", strict_si)), "Tm");
    EXPECT_EQ(to_string(unit_from_string("Pm", strict_si)), "Pm");
    EXPECT_EQ(to_string(unit_from_string("Em", strict_si)), "Em");
    EXPECT_EQ(to_string(unit_from_string("Zm", strict_si)), "Zm");
    EXPECT_EQ(to_string(unit_from_string("Ym", strict_si)), "Ym");
    EXPECT_EQ(to_string(unit_from_string("Rm", strict_si)), "Rm");
    EXPECT_EQ(to_string(unit_from_string("Qm", strict_si)), "Qm");
}

TEST(unitStrings, strictSi)
{
    auto unit = unit_from_string("Um", strict_si);
    EXPECT_NE(unit, unit_from_string("um"));

    unit = unit_from_string("Dm", strict_si);
    EXPECT_NE(unit, unit_from_string("dm"));

    unit = unit_from_string("Cm", strict_si);
    EXPECT_NE(unit, unit_from_string("cm"));

    unit = unit_from_string("Am", strict_si);
    EXPECT_NE(unit, unit_from_string("am"));

    unit = unit_from_string("Fm", strict_si);
    EXPECT_NE(unit, unit_from_string("fm"));
}

TEST(unitStrings, astronomyUnits)
{
    auto unit = unit_from_string("am", astronomy_units);
    EXPECT_EQ(unit, unit_from_string("arcmin"));

    unit = unit_from_string("am");
    EXPECT_NE(unit, unit_from_string("am", astronomy_units));
}

TEST(unitStrings, readability)
{
    EXPECT_EQ(to_string(precise::m / precise::s.pow(2)), "m/s^2");
}

TEST(unitStrings, dotInterpretation)
{
    // interpret . as multiply
    EXPECT_EQ(precise::m * precise::s, unit_from_string("m.s"));
    // interpret as abbreviation so millisecond
    EXPECT_EQ(precise::milli * precise::s, unit_from_string("m. s"));
    // interpret as abbreviation so millisecond
    EXPECT_EQ(precise::milli * precise::s, unit_from_string("m. s."));
    // connector so space = multiply
    EXPECT_EQ(precise::m * precise::s, unit_from_string("m- s"));
    // connector so space = multiply
    EXPECT_EQ(precise::milli * precise::s, unit_from_string("m-s"));
    // s is abbreviation so first . is also abbreviation so millisecond
    EXPECT_EQ(precise::milli * precise::s, unit_from_string("m.s."));
    // s is abbreviation so first . is also abbreviation so millisecond
    EXPECT_EQ(precise::milli * precise::s, unit_from_string(".m.s."));

    // s is abbreviation so first . is also abbreviation so millisecond
    EXPECT_EQ(
        precise_unit(0.9, precise::milli * precise::s),
        unit_from_string("0.9.m.s."));
}

TEST(unitStrings, endwithU)
{
    EXPECT_EQ(
        unit_from_string("astronomical unit"),
        unit_from_string("astronomicalu"));
    EXPECT_EQ(unit_from_string("arb. u."), unit_from_string("arbitraryunit"));
    EXPECT_EQ(unit_from_string("arb. u."), unit_from_string("arbitrary u."));
    EXPECT_EQ(unit_from_string("arb.u."), unit_from_string("arbitrary u."));
    EXPECT_EQ(unit_from_string("arb. unit"), unit_from_string("arbitrary u."));
    EXPECT_EQ(unit_from_string("p.d.u."), unit_from_string("arbitraryunit"));
    EXPECT_EQ(
        unit_from_string("arbitrary unit"), unit_from_string("arbitrary u."));
}

TEST(unitStrings, infinite)
{
    EXPECT_EQ(
        to_string(precise_unit(
            std::numeric_limits<double>::infinity(), precise::m / precise::s)),
        "INF*m/s");

    EXPECT_EQ(
        to_string(unit(-std::numeric_limits<double>::infinity(), m / s)),
        "-INF*m/s");
}

TEST(unitString, almostInfinite)
{
    precise_unit almost_inf(
        4.414e307, precise::s.pow(3) * precise::kg * precise::mol);

    auto res = to_string(almost_inf);
    auto ai2 = unit_from_string(res);
    EXPECT_EQ(unit_cast(almost_inf), unit_cast(ai2));
}

TEST(unitStrings, nan)
{
    EXPECT_EQ(to_string(precise::error), "ERROR");

    EXPECT_EQ(to_string(precise::invalid), "NaN*ERROR");
    auto nanunit =
        precise_unit(std::numeric_limits<double>::quiet_NaN(), precise::one);
    auto res = to_string(nanunit);
    EXPECT_EQ(res, "NaN");
    auto nanresult = unit_from_string(res);
    EXPECT_TRUE(isnan(nanresult));
    EXPECT_EQ(to_string(nanunit * precise::m / precise::s), "NaN*m/s");

    EXPECT_EQ(
        to_string(unit(std::numeric_limits<double>::signaling_NaN(), m / s)),
        "NaN*m/s");
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
    auto funit =
        precise_unit(0.000001, precise::one / precise::cd / precise::time::day);
    auto res = to_string(funit);
    EXPECT_EQ(res, "1/(Mcd*day)");
}

TEST(unitStrings, downConvert)
{
    EXPECT_EQ(
        to_string(precise_unit(1000.0, precise::one / precise::kg)), "1/g");
}

TEST(unitStrings, powerUnits)
{
    EXPECT_EQ(to_string((precise::giga * precise::m).pow(2)), "Gm^2");
    EXPECT_EQ(
        to_string(precise_unit(1000000000, s.inv() * m.pow(-3))), "1/(uL*s)");
}

TEST(unitString, bigpowers)
{
#ifdef UNITS_CONSTEXPR_IF_SUPPORTED
    if constexpr (detail::bitwidth::base_size == sizeof(std::uint64_t)) {
#else
    if (detail::bitwidth::base_size == sizeof(std::uint64_t)) {
#endif
        auto bp = precise::m.pow(12);
        EXPECT_EQ(bp, unit_from_string(to_string(bp)));
        auto kp = precise::kg.pow(-11);
        EXPECT_EQ(kp, unit_from_string(to_string(kp)));
        auto sp = precise::s.pow(23);
        auto spstr = to_string(sp);
        EXPECT_EQ(sp, unit_from_string(spstr));
    }
}

TEST(unitString, wrapping)
{
    std::string const mol_m3_str("mol/m^3666");
    unit const mol_m3_unit(units::mol / units::m.pow(3));
    auto fstr = unit_from_string(mol_m3_str);
    EXPECT_NE(fstr, mol_m3_unit);
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

TEST(unitStrings, charge)
{
    // A * s = C
    EXPECT_EQ(to_string(precise::A * precise::s), "C");
    // A * h = 3600 C, better use A * h
    EXPECT_EQ(to_string(precise::A * precise::hr), "Ah");
    EXPECT_EQ(to_string(precise::femto * precise::A * precise::hr), "fAh");
    EXPECT_EQ(to_string(precise::quecto * precise::A * precise::hr), "qAh");
    EXPECT_EQ(to_string(precise::pico * precise::A * precise::hr), "pAh");
    EXPECT_EQ(to_string(precise::nano * precise::A * precise::hr), "nAh");
    EXPECT_EQ(to_string(precise::micro * precise::A * precise::hr), "uAh");
    EXPECT_EQ(to_string(precise::milli * precise::A * precise::hr), "mAh");
    EXPECT_EQ(to_string(precise::kilo * precise::A * precise::hr), "kAh");
    EXPECT_EQ(to_string(precise::mega * precise::A * precise::hr), "MAh");
    EXPECT_EQ(to_string(precise::giga * precise::A * precise::hr), "GAh");
    EXPECT_EQ(to_string(precise::tera * precise::A * precise::hr), "TAh");
}

TEST(unitStrings, electronVolt)
{
    EXPECT_EQ(to_string(precise::energy::eV), "eV");
    EXPECT_EQ(to_string(precise::nano * precise::energy::eV), "neV");
    EXPECT_EQ(to_string(precise::micro * precise::energy::eV), "ueV");
    EXPECT_EQ(to_string(precise::milli * precise::energy::eV), "meV");
    EXPECT_EQ(to_string(precise::kilo * precise::energy::eV), "keV");
    EXPECT_EQ(to_string(precise::mega * precise::energy::eV), "MeV");
    EXPECT_EQ(to_string(precise::giga * precise::energy::eV), "GeV");
    EXPECT_EQ(to_string(precise::tera * precise::energy::eV), "TeV");

    auto str =
        to_string(precise::count / (precise::milli * precise::energy::eV));
    EXPECT_EQ(str, "count/meV");

    str = to_string(precise::one / (precise::micro * precise::energy::eV));
    EXPECT_EQ(str, "1/ueV");

    str = to_string(precise::m / precise::energy::eV);
    EXPECT_EQ(str, "m/eV");
}

TEST(unitStrings, Hertz)
{
    EXPECT_EQ(to_string(precise::Hz), "Hz");
    EXPECT_EQ(to_string(precise::milli * precise::Hz), "mHz");
    EXPECT_EQ(to_string(precise::kilo * precise::Hz), "kHz");
    EXPECT_EQ(to_string(precise::mega * precise::Hz), "MHz");
    EXPECT_EQ(to_string(precise::giga * precise::Hz), "GHz");
    EXPECT_EQ(to_string(precise::tera * precise::Hz), "THz");
}

TEST(unitStrings, singleBase)
{
    EXPECT_EQ(to_string(precise::kilo * precise::J / precise::mol), "kJ/mol");
    EXPECT_EQ(
        to_string(precise::kilo * precise::J / precise::rad.pow(2)),
        "kJ/rad^2");
}

TEST(unitStrings, watthours)
{
    EXPECT_EQ(to_string(precise::A * precise::s), "C");
    EXPECT_EQ(to_string(precise::W * precise::hr), "Wh");
    EXPECT_EQ(to_string(precise::W * precise::hr * precise::iflag), "Wh*flag");
    EXPECT_EQ(to_string(precise::W * precise::hr * precise::m), "Wh*m");
    EXPECT_EQ(to_string(precise::kilo * precise::W * precise::h), "kWh");
    EXPECT_EQ(to_string(precise::mega * precise::W * precise::h), "MWh");
    EXPECT_EQ(to_string(precise::giga * precise::W * precise::h), "GWh");

    auto str = to_string(
        precise::currency / (precise::giga * precise::W * precise::h));
    EXPECT_EQ(str, "$/GWh");

    str = to_string(precise::m * (precise::giga * precise::W * precise::h));
    EXPECT_EQ(str, "GWh*m");
}

TEST(unitStrings, mm)
{
    auto speedUnit = unit_from_string("mm/s");
    EXPECT_EQ(to_string(speedUnit), "mm/s");

    auto accUnit = unit_from_string("mm/s^2");
    EXPECT_EQ(to_string(accUnit), "mm/s^2");

    speedUnit = unit_from_string("km/s");
    EXPECT_EQ(to_string(speedUnit), "km/s");

    accUnit = unit_from_string("km/s^2");
    EXPECT_EQ(to_string(accUnit), "km/s^2");
}

TEST(unitStrings, customUnits)
{
    EXPECT_EQ(to_string(precise::generate_custom_unit(762)), "CXUN[762]");
    EXPECT_EQ(to_string(precise::generate_custom_unit(0)), "CXUN[0]");
    EXPECT_EQ(to_string(precise::generate_custom_count_unit(12)), "CXCUN[12]");
    EXPECT_EQ(to_string(precise::generate_custom_count_unit(0)), "CXCUN[0]");
}

TEST(unitStrings, eV)
{
    EXPECT_EQ(
        to_string(precise::energy::eV / constants::c.as_unit()), "eV/[c]");
    EXPECT_EQ(
        to_string(precise::energy::eV / (constants::c.as_unit().pow(2))),
        "eV*[c]^-2");
}

TEST(stringToUnits, Simple)
{
    EXPECT_EQ(precise::m, unit_from_string("m"));
    EXPECT_EQ(precise::m, unit_from_string("meter"));
}

TEST(stringToUnits, pressure)
{
    EXPECT_EQ(
        unit_from_string("M[HG]"), unit_from_string("meter of mercury column"));
    EXPECT_EQ(
        unit_from_string("M[HG]", units::case_insensitive),
        unit_from_string("meter of mercury column"));
    EXPECT_EQ(unit_from_string("millimeter_Hg_0C"), precise::pressure::mmHg);
    EXPECT_EQ(
        unit_from_string("MM[HG]", units::case_insensitive),
        unit_from_string("millimeter of mercury column"));
}

TEST(stringToUnit, fluid)
{
    EXPECT_EQ(unit_from_string("US_fluid_ounce"), precise::us::floz);
}

TEST(stringToUnits, withSpace)
{
    EXPECT_EQ(precise::m.inv(), unit_from_string("1 /m"));
    EXPECT_EQ(precise::m.inv(), unit_from_string("1  /m"));
    EXPECT_EQ(precise::m.inv(), unit_from_string("1   /m"));
    EXPECT_EQ(precise::m.inv(), unit_from_string("1   / m"));
    EXPECT_EQ(precise::m.inv(), unit_from_string("1   /  m"));
    EXPECT_EQ(precise::m.inv(), unit_from_string("1   /   m"));
    EXPECT_EQ(precise::m.inv(), unit_from_string("1/   m"));
    EXPECT_EQ(precise::m.inv(), unit_from_string("1\t/          m"));
    EXPECT_EQ(precise::m.inv(), unit_from_string("1\t/\tm"));
    EXPECT_EQ(precise::m.inv(), unit_from_string("1/\tm"));
    EXPECT_EQ(precise::m.inv(), unit_from_string("  1/\tm  "));
}

TEST(stringToUnits, toDefaultUnit)
{
    EXPECT_EQ(precise::defunit, unit_from_string("*"));
    EXPECT_EQ(precise::defunit, unit_from_string("**"));
    EXPECT_EQ(precise::defunit, unit_from_string("}"));
    EXPECT_EQ(precise::defunit, unit_from_string("}()"));
    EXPECT_EQ(precise::defunit, unit_from_string("***"));
    EXPECT_EQ(precise::defunit, unit_from_string("*******"));
}

TEST(stringToUnits, measurementTypes)
{
    EXPECT_EQ(pu * m.pow(2), unit_from_string("pu*area"));
    EXPECT_EQ(percent * m.pow(2), unit_from_string("percent*Area"));
}

TEST(stringToUnits, Power)
{
    EXPECT_EQ(precise::m.pow(2), unit_from_string("m^2"));
    EXPECT_EQ(precise::kg.pow(-3), unit_from_string("kg^-3"));
    EXPECT_EQ(precise::V.pow(-2), unit_from_string("1/V^2"));
    EXPECT_EQ(
        precise_unit(27.0, precise::one).pow(3), unit_from_string("27^3"));
#ifdef UNITS_CONSTEXPR_IF_SUPPORTED
    if constexpr (detail::bitwidth::base_size == sizeof(std::uint64_t)) {
#else
    if (detail::bitwidth::base_size == sizeof(std::uint64_t)) {
#endif
        EXPECT_EQ(precise::m.pow(12), unit_from_string("m^12"));
        EXPECT_EQ(precise::kg.pow(-11), unit_from_string("kg^-11"));
        EXPECT_EQ(precise::s.pow(23), unit_from_string("s^+23"));

        EXPECT_EQ(precise::m.pow(8975), unit_from_string("m^8975"));
    }
}

TEST(stringToUnits, morePower)
{
    EXPECT_EQ(precise::us::mile.pow(2), unit_from_string("mi(USA)^(2)"));
}

TEST(stringToUnits, specialUnits)
{
    EXPECT_EQ(
        precise::percent * precise::pu * precise::kg,
        unit_from_string("percentkg"));
    EXPECT_EQ(
        precise::percent * precise::pu * precise::kg,
        unit_from_string("percentkilogram"));
    EXPECT_EQ(
        precise::percent * precise::pu * precise::kg,
        unit_from_string("percentmass"));
    EXPECT_EQ(
        precise::percent * precise::pu * precise::kg, unit_from_string("%kg"));
    EXPECT_EQ(
        precise::percent * precise::pu * precise::kg,
        unit_from_string("%kilogram"));
    EXPECT_EQ(
        precise::percent * precise::pu * precise::kg,
        unit_from_string("%mass"));

    EXPECT_EQ(precise::pu * precise::kg, unit_from_string("perunitkilogram"));
    EXPECT_EQ(precise::pu * precise::kg, unit_from_string("perunitkg"));
    EXPECT_EQ(precise::pu * precise::kg, unit_from_string("pukilogram"));
    EXPECT_EQ(precise::pu * precise::kg, unit_from_string("pukg"));
    EXPECT_EQ(precise::pu * precise::kg, unit_from_string("pumass"));
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

TEST(stringToUnits, hartree)
{
    EXPECT_EQ(precise::energy::hartree, unit_from_string("Eh"));
    EXPECT_EQ(precise::energy::hartree, unit_from_string("E_h"));
}

TEST(stringToUnits, degC)
{
    EXPECT_EQ(precise::degC, root(unit_from_string("degC^2"), 2));
    EXPECT_EQ(to_string(precise::degC.pow(2)), "degC^2");
}

TEST(stringToUnits, conventional)
{
    EXPECT_EQ(precise::conventional::coulomb90, unit_from_string("C_90"));
    EXPECT_EQ(precise::conventional::watt90, unit_from_string("V_90*A90"));
}

TEST(stringToUnits, specificCombinations)
{
    EXPECT_EQ(precise::kg * precise::m, unit_from_string("kg m"));
    EXPECT_EQ(precise::s.pow(2), unit_from_string("sq s"));
    EXPECT_EQ(precise::kg * precise::L, unit_from_string("kg l"));
}

TEST(stringToUnits, barrels)
{
    EXPECT_EQ(
        precise::kilo * precise::us::barrel / precise::day,
        unit_from_string("kbbl (US)/d"));
}
TEST(stringToUnits, gasConstant)
{
    auto rval = unit_from_string("J mol^-1 K^-1");

    EXPECT_TRUE(rval.has_same_base(units::constants::R.as_unit().base_units()));
}

TEST(stringToUnits, Parenthesis)
{
    EXPECT_EQ(
        precise::W / (precise::hr * precise::lb),
        unit_from_string("W/(hr*lb)"));
    EXPECT_EQ(
        precise::W * precise::mol / (precise::hr * precise::lb),
        unit_from_string("(W*mol)/(hr*lb)"));
    EXPECT_EQ(
        (precise::N * precise::time::yr).pow(2), unit_from_string("(N*yr)^2"));
    EXPECT_EQ(
        (precise::N * precise::time::yr).pow(-2),
        unit_from_string("(N*yr)^-2"));
    EXPECT_EQ(
        (precise::N * precise::time::yr).pow(-2) / precise::currency,
        unit_from_string("(N*yr)^-2/$"));
}

TEST(stringToUnits, multipower)
{
    auto res = unit_from_string("(4.56^3)^3");
    EXPECT_DOUBLE_EQ(res.multiplier(), std::pow(4.56, 9.0));
}

TEST(stringToUnits, bitsBytes)
{
    auto res = unit_from_string("KiB");
    EXPECT_EQ(res, precise::kibi * precise::data::byte);

    res = unit_from_string("Mib");
    EXPECT_EQ(res, precise::mebi * precise::data::bit);

    res = unit_from_string("MAB");
    EXPECT_EQ(res, precise::mega * precise::data::byte);

    res = unit_from_string("MAb");
    EXPECT_EQ(res, precise::mega * precise::data::bit);
}

TEST(stringToUnits, dotNotation)
{
    EXPECT_EQ(precise::m, unit_from_string("m.m.m/m.m", single_slash));
    EXPECT_EQ(precise::m.pow(3), unit_from_string("m.m.m/m.m"));
    EXPECT_EQ(
        precise::W / (precise::hr * precise::lb),
        unit_from_string("W/hr.lb", single_slash));
    EXPECT_EQ(
        precise::W * precise::mol / (precise::hr * precise::lb),
        unit_from_string("W.mol/hr.lb", single_slash));
}

TEST(stringToUnits, customUnits)
{
    EXPECT_EQ(
        precise::generate_custom_unit(762), unit_from_string("CXUN[762]"));
    EXPECT_EQ(precise::generate_custom_unit(0), unit_from_string("CXUN[]"));
    EXPECT_EQ(precise::generate_custom_unit(0), unit_from_string("CXUN[0]"));
    EXPECT_EQ(
        precise::generate_custom_count_unit(11), unit_from_string("CXCUN[11]"));
    EXPECT_EQ(
        precise::generate_custom_count_unit(0), unit_from_string("CXCUN[0]"));
}

TEST(stringToUnits, NumericalMultipliers)
{
    EXPECT_EQ(precise::km, unit_from_string("1000*m"));
    EXPECT_EQ(precise::mm, unit_from_string("km*1e-6"));
    EXPECT_EQ(precise::km, unit_from_string("1e3*0.001*1e6/1e3*m"));
    EXPECT_EQ(precise_unit(0.7564, precise::kg), unit_from_string("0.7564*kg"));
    EXPECT_EQ(precise::km, unit_from_string("1000m"));
    EXPECT_EQ(
        precise::kilo * precise::kg / precise::m,
        unit_from_string("1e3*0.001*1e6kg/1e3m"));
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
    EXPECT_EQ(precise::ft * precise::lbf, unit_from_string("ftlb"));
    EXPECT_EQ(
        precise::ft * precise::lbf * precise::s,
        unit_from_string("ftlbsecond"));
    EXPECT_EQ(
        precise::gal * precise::second * precise::hp,
        unit_from_string("gallon second horsepower"));
    EXPECT_EQ(
        precise::Hz * precise::milli * precise::micro * precise::m,
        unit_from_string("millihertz micrometer"));

    EXPECT_TRUE(is_error(unit_from_string("bob and harry")));
    EXPECT_EQ(precise::pico * precise::T, unit_from_string("picotesla"));
    EXPECT_EQ(precise::pico * precise::A, unit_from_string("picoampere"));
    EXPECT_EQ(
        precise::pressure::psi, unit_from_string("pound per square inch"));
    EXPECT_EQ(
        precise::lm * precise::m.pow(2),
        unit_from_string("lumen square meter"));

    EXPECT_EQ(
        unit_from_string("ampere per square metre kelvin squared"),
        precise::A / (m.pow(2) * K.pow(2)));

    EXPECT_EQ(unit_from_string("reciprocal cubic meter"), precise::m.pow(-3));
    EXPECT_EQ(
        unit_from_string("sixty fourths of an inch"),
        precise_unit(1.0 / 64.0, precise::in));

    EXPECT_EQ(
        unit_from_string("sixty fourths of a   mile"),
        precise_unit(1.0 / 64.0, precise::mile));

    EXPECT_EQ(
        unit_from_string("thirty-seconds of a yard"),
        precise_unit(1.0 / 32.0, precise::yd));
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
    EXPECT_EQ(
        (precise::micro * precise::meter).pow(2), unit_from_string("um2"));
    EXPECT_EQ(precise::cm.pow(5), unit_from_string("cm5"));
    EXPECT_EQ(
        unit_from_string("CM2", case_insensitive), unit_from_string("cm2"));
}

TEST(stringToUnits, invalidStrings)
{
    EXPECT_FALSE(is_valid(unit_from_string("Mik")));
}

TEST(stringToUnits, complex)
{
    EXPECT_EQ(
        precise::m / precise::s / precise::s,
        unit_from_string("meters per second per second"));
    EXPECT_EQ(
        precise::mile / precise::hr / precise::s,
        unit_from_string("miles per hour per second"));
    EXPECT_EQ(
        precise::micro * precise::mol / (precise_unit(0.1, precise::L)),
        unit_from_string("micromole per deciliter"));
    EXPECT_EQ(
        unit_from_string("$ per 24 hrs"),
        precise::currency / precise_unit(24.0, precise::hr));

    EXPECT_NE(
        unit_from_string("kilogram / minute(Gasoline at 15.5 C)"),
        precise::error);
    EXPECT_NE(unit_from_string("g/(8.h){shift}"), precise::error);

    EXPECT_TRUE((precise::ohm * precise::meter)
                    .is_convertible(unit_from_string("statohm micrometer")));
    EXPECT_TRUE(
        unit_from_string("coulomb/square meter")
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

    unit = unit_from_string("tenth minute");
    EXPECT_EQ(unit, precise_unit(6.0, precise::s));

    unit = unit_from_string("fluid ounce(UK)");
    EXPECT_EQ(unit, precise::imp::floz);
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
    EXPECT_EQ(
        unit_from_string("DM2/S2", case_insensitive),
        unit_from_string("dm2/s2"));
    EXPECT_EQ(unit_from_string("/cu. m"), m.pow(3).inv());
    EXPECT_EQ(unit_from_string("a gregorian"), precise::time::ag);
    EXPECT_EQ(unit_from_string("cubic inches"), unit_from_string("[cin_i]"));
    EXPECT_EQ(unit_from_string("/mcl"), unit_from_string("/uL"));
    EXPECT_EQ(unit_from_string("/sec"), unit_from_string("/s"));
    EXPECT_EQ(unit_from_string("g.m"), unit_from_string("gram meter"));
    EXPECT_EQ(unit_from_string("gf.m"), unit_from_string("gram-force meter"));
}

TEST(stringToUnits, equivalents2)
{
    EXPECT_EQ(unit_from_string("in us"), unit_from_string("in_us"));
    EXPECT_EQ(unit_from_string("us in"), unit_from_string("in_us"));
    EXPECT_EQ(unit_from_string("[in_us]"), unit_from_string("inch - US"));
    EXPECT_EQ(unit_from_string("CXCUN[1]^-1"), unit_from_string("/[arb'U]"));
    EXPECT_EQ(
        unit_from_string("[CCID_50]"), unit_from_string("CCID<sub>50</sub> "));
    EXPECT_EQ(
        unit_from_string("[CCID_50]"),
        unit_from_string("CCID<sub>50<\\/sub> "));
    EXPECT_EQ(
        unit_from_string("/100 neutrophils"),
        unit_from_string("/100{neutrophils}"));
    EXPECT_EQ(unit_from_string("carat/cubic inch"), unit_from_string("ct/in3"));
    EXPECT_EQ(
        unit_from_string("10.(UN.S)/(CM5.M2)", case_insensitive),
        unit_from_string("10.uN.s/(cm5.m2)"));
    EXPECT_EQ(unit_from_string("imperial miles"), unit_from_string("mi_br"));
    EXPECT_EQ(unit_from_string("cup us"), precise::us::cup);
    EXPECT_EQ(unit_from_string(" per sec"), unit_from_string("/s"));
    EXPECT_EQ(unit_from_string("ampere-hour"), unit_from_string("A.h"));
    EXPECT_EQ(
        unit_from_string("mL/cm[H2O]"),
        unit_from_string("milliliter per centimeter of water"));
    EXPECT_EQ(unit_from_string("sq m"), unit_from_string("m2"));
    EXPECT_EQ(unit_from_string("U/ 12hrs"), unit_from_string("U/(12.h)"));
    EXPECT_EQ(unit_from_string("Ncm"), unit_from_string("N.cm"));
    EXPECT_EQ(unit_from_string("ohm reciprocal"), unit_from_string("mho"));
    EXPECT_EQ(unit_from_string("per mins"), unit_from_string("/min"));
    EXPECT_EQ(unit_from_string("/100 WBCs"), unit_from_string("/100{WBCs}"));

    EXPECT_EQ(
        unit_from_string("lumen meters squared"), unit_from_string("lm.m2"));
}

TEST(stringToUnits, equivalents3)
{
    EXPECT_EQ(
        unit_from_string("grams per hour per metre sq"),
        unit_from_string("g/h/m2"));
    EXPECT_EQ(unit_from_string("dyn sec per cm"), unit_from_string("dyn.s/cm"));
    EXPECT_EQ(unit_from_string("Ns"), precise::N * precise::s);
    EXPECT_EQ(unit_from_string("N.s"), precise::N * precise::s);
    EXPECT_EQ(unit_from_string("Newton second"), precise::N * precise::s);
    EXPECT_EQ(unit_from_string("As"), precise::A * precise::s);
    EXPECT_EQ(unit_from_string("Ah"), precise::A * precise::h);
    EXPECT_EQ(unit_from_string("Ahr"), precise::A * precise::h);
    auto u2 = unit_from_string("molcubicfoot");
    EXPECT_FALSE(is_error(u2));
    EXPECT_EQ(u2, precise::mol * precise::ft.pow(3));
    EXPECT_EQ(unit_from_string("(1)^345"), precise::one);
    EXPECT_EQ(unit_from_string("\t\t\t\t \r\n\n"), precise::defunit);
    auto u3 = unit_from_string("2^345");
    EXPECT_EQ(u3.multiplier(), std::pow(2.0, 345.0));
}

TEST(stringToUnits, equivalents4)
{
    EXPECT_EQ(unit_from_string("[CAR_AU]"), unit_from_string("[car_Au]"));
    EXPECT_EQ(unit_from_string("[bu_us]"), unit_from_string("bushel - US"));
    EXPECT_EQ(
        unit_from_string("[drp]"), unit_from_string("drop - metric (1/20 mL)"));
    EXPECT_EQ(
        unit_from_string("[in_i'Hg]"),
        unit_from_string("inch of mercury column"));

    EXPECT_EQ(
        unit_cast(unit_from_string("nmol/mg{prot}")),
        unit_cast(unit_from_string(
            "nanomole of (1/2) cystine per milligram of protein")));
}

TEST(stringToUnits, ofCommodities)
{
    EXPECT_EQ(m, unit_cast(unit_from_string("meter of gold")));
    EXPECT_EQ(m, unit_cast(unit_from_string("meter of gold_(95%)")));
}

TEST(stringToUnits, commodityConsiderations)
{
    auto ut = unit_from_string("mile(USA){\\{}");
    EXPECT_NE(ut.commodity(), 0U);
}
TEST(stringToUnits, electronVolt)
{
    EXPECT_EQ(unit_from_string("eV"), precise::energy::eV);
    EXPECT_EQ(unit_from_string("neV"), precise::nano * precise::energy::eV);
    EXPECT_EQ(unit_from_string("ueV"), precise::micro * precise::energy::eV);
    EXPECT_EQ(unit_from_string("meV"), precise::milli * precise::energy::eV);
    EXPECT_EQ(unit_from_string("keV"), precise::kilo * precise::energy::eV);
    EXPECT_EQ(unit_from_string("MeV"), precise::mega * precise::energy::eV);
    EXPECT_EQ(unit_from_string("GeV"), precise::giga * precise::energy::eV);
    EXPECT_EQ(unit_from_string("TeV"), precise::tera * precise::energy::eV);
}

class roundTripString : public ::testing::TestWithParam<std::string> {};

TEST_P(roundTripString, testRoundTrip)
{
    auto u1 = unit_from_string(GetParam());
    auto str = to_string(u1);
    EXPECT_EQ(unit_from_string(str), u1)
        << "Round trip string didn't match " << GetParam();
}

static const std::vector<std::string> testStrings{
    "million per milliliter",
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

INSTANTIATE_TEST_SUITE_P(
    roundTrip,
    roundTripString,
    ::testing::ValuesIn(testStrings));

TEST(roundTrip, DebugTest)
{
    auto u1 = unit_from_string("1*ampere90");
    auto str = to_string(u1);
    auto u2 = unit_from_string(str);
    EXPECT_EQ(unit_cast(u2), unit_cast(u1));
}

TEST(CommodityStrings, simple)
{
    auto u1 = unit_from_string("{absorbance}");
    EXPECT_FALSE(is_error(u1));
    EXPECT_NE(u1.commodity(), 0U);
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

    EXPECT_FALSE(units::is_valid(unit_from_string("liquid")));
    EXPECT_FALSE(is_valid(unit_from_string("_liquid_()")));
}

TEST(stringToUnits, ParseIssues)
{
    auto u1 = unit_from_string("Metres");
    EXPECT_EQ(u1, precise::m);

    u1 = unit_from_string("degrees C");
    EXPECT_EQ(u1, precise::degC);

#if !defined(UNITS_DISABLE_NON_ENGLISH_UNITS) ||                               \
    UNITS_DISABLE_NON_ENGLISH_UNITS == 0
    u1 = unit_from_string("gramm");
    EXPECT_EQ(u1, precise::g);
    u1 = unit_from_string("kilogramm");
    EXPECT_EQ(u1, precise::kilo * precise::g);
#else
    u1 = unit_from_string("gramm");
    EXPECT_EQ(u1, precise::g * precise::m);
    u1 = unit_from_string("kilogramm");
    EXPECT_EQ(u1, precise::kilo * precise::g * precise::m);
#endif

    u1 = unit_from_string("inchy");
    EXPECT_FALSE(is_valid(u1));

    u1 = unit_from_string("fln_oz");
    EXPECT_FALSE(is_valid(u1));

    u1 = unit_from_string("wattm");
    EXPECT_EQ(u1, precise::W * precise::m);

    u1 = unit_from_string("British Thermal Unit");
    EXPECT_EQ(u1, precise::btu);
}

TEST(stringToUnits, rotSequences)
{
    auto u1 = unit_from_string("BTU_IT");
    EXPECT_EQ(u1, unit_from_string("BtuIT"));
    EXPECT_EQ(u1, unit_from_string("BTU-IT"));
}

TEST(stringToUnits, parentheticalModifier)
{
    auto u1 = unit_from_string("mile(statute)");
    EXPECT_EQ(u1, precise::us::mile);
    auto u2 = unit_from_string("mile (statute)");
    EXPECT_EQ(u2, precise::us::mile);
    auto u3 = unit_from_string("British thermal unit (thermochemical)");
    EXPECT_EQ(u3, precise::energy::btu_th);
    auto u4 = unit_from_string("short(US) ton per day");
    EXPECT_EQ(u4, precise::av::ton / precise::day);
}

TEST(stringToUnit, handlingOfSquared)
{
    auto u1 = unit_from_string("square foot second");
    EXPECT_EQ(u1, precise::ft.pow(2) * precise::s);

    auto u2 = unit_from_string("pascal squared second");
    EXPECT_EQ(u2, precise::Pa.pow(2) * precise::s);

    auto u2b = unit_from_string("pascal squared per second squared");
    EXPECT_EQ(u2b, precise::Pa.pow(2) / precise::s.pow(2));

    auto u2c = unit_from_string("pascal squared per second squared per kg");
    EXPECT_EQ(u2c, precise::Pa.pow(2) / precise::s.pow(2) / precise::kg);

    auto u2d = unit_from_string("pascal cubed second");
    EXPECT_EQ(u2d, precise::Pa.pow(3) * precise::s);

    auto u2e = unit_from_string("pascal cubed per second cubed");
    EXPECT_EQ(u2e, precise::Pa.pow(3) / precise::s.pow(3));

    auto u2f = unit_from_string("pascal squared per second squared per kg");
    EXPECT_EQ(u2f, precise::Pa.pow(2) / precise::s.pow(2) / precise::kg);

    auto u2g = unit_from_string("pascal squared / second squared / kg");
    EXPECT_EQ(u2g, precise::Pa.pow(2) / precise::s.pow(2) / precise::kg);

    auto u3 = unit_from_string("coulomb metre squared per volt");
    EXPECT_EQ(u3, precise::C * precise::m.pow(2) / precise::V);

    auto u3v = unit_from_string(
        "kg per coulomb metre squared per second squared per volt");
    EXPECT_EQ(
        u3v,
        precise::kg / (precise::C * precise::m.pow(2)) / precise::V /
            precise::s.pow(2));

    auto u4 = unit_from_string("ampere square metre per joule second");
    EXPECT_EQ(u4, precise::A * precise::m.pow(2) / (precise::J * precise::s));

    auto u5 =
        unit_from_string("meter per square seconds per square root of hertz");
    EXPECT_EQ(u5, precise::special::ASD);

    auto u6 = unit_from_string("degree Fahrenheit hour square foot per British "
                               "thermal unit (international table) inch");

    EXPECT_EQ(
        u6,
        precise::degF * precise::hr * precise::ft.pow(2) /
            precise::energy::btu_it / precise::in);

    auto u7 = unit_from_string("gram square decimeter");
    EXPECT_EQ(u7, g * (deci * m).pow(2));
}

TEST(stringToUnits, bracketExpansion)
{
    EXPECT_EQ(unit_from_string("B(SPL)"), unit_from_string("B[SPL]"));
}

TEST(stringToUnits, modifiedStrings)
{
    auto u1 = unit_from_string("linear yard");
    u1.commodity(0);
    EXPECT_EQ(u1, precise::yd);

    auto u2 = unit_from_string("kilogram per millimetre");
    EXPECT_EQ(u2, precise::kg / precise::mm);

    auto u3 = unit_from_string("British thermal unit (59 degF)");
    EXPECT_EQ(u3, precise::energy::btu_59);

    auto u4 = unit_from_string("calorie (20 degC)");
    EXPECT_EQ(u4, precise::energy::cal_20);

    auto u5 = unit_from_string("nanometre");
    EXPECT_EQ(u5, precise::nano * precise::m);

    auto u6 = unit_from_string("ton (US) per hour");
    EXPECT_EQ(u6, precise::ton / precise::hr);

    auto u7 = unit_from_string("foot of water(39.2 degF)");
    EXPECT_EQ(u7, precise_unit(2988.98400, Pa));

    auto u8 = unit_from_string("[qt_us]");
    EXPECT_EQ(u8, unit_from_string("[QT_US]"));
}

TEST(stringToUnits, tempString)
{
    auto u4 = unit_from_string("calorie (20 degC)");
    EXPECT_EQ(u4, precise::energy::cal_20);

    u4 = unit_from_string("calorie 20C");
    EXPECT_EQ(u4, precise::energy::cal_20);

    u4 = unit_from_string("calorie at 20C");
    EXPECT_EQ(u4, precise::energy::cal_20);

    u4 = unit_from_string("calorie_20C");
    EXPECT_EQ(u4, precise::energy::cal_20);
}

TEST(stringToUnits, addition)
{
    auto u1 = unit_from_string("cm+cm");
    EXPECT_EQ(u1, precise_unit(2, precise::cm));

    u1 = unit_from_string("km + m+ cm+mm");
    EXPECT_EQ(u1, precise_unit(1001.011, precise::m));
    u1 = unit_from_string("kilometer+ 3in");
    EXPECT_EQ(u1, (1.0 * precise::km + 3.0 * precise::in).as_unit());

    u1 = unit_from_string("km+  7.0 feet");
    EXPECT_EQ(u1, (1.0 * precise::km + 7.0 * precise::ft).as_unit());
    u1 = unit_from_string("m*km+  ft*in");
    EXPECT_EQ(
        u1,
        (1.0 * precise::km * precise::m + 1.0 * precise::ft * precise::in)
            .as_unit());

    u1 = unit_from_string("meter + hippyhoppy");
    EXPECT_FALSE(is_valid(u1));

    u1 = unit_from_string("hippyhoppy + meter");
    EXPECT_FALSE(is_valid(u1));

    u1 = unit_from_string("arggh + kilometer");
    EXPECT_FALSE(is_valid(u1));

    u1 = unit_from_string("kilometer + arggh");
    EXPECT_FALSE(is_valid(u1));

    u1 = unit_from_string("meter + kg");
    EXPECT_EQ(u1, precise::m * precise::kg);
}

TEST(stringToUnits, cleanPhase2)
{
    auto u1 = unit_from_string("tech-nical-at-mosphere");
    EXPECT_EQ(u1, precise::pressure::att);

    u1 = unit_from_string("+++m++++");
    EXPECT_EQ(u1, precise::m);
    u1 = unit_from_string("+++6.7+++m+++");
    EXPECT_EQ(u1, units::precise_unit(6.7, precise::m));

    auto uhippos = unit_from_string("+++6.7+++hippos+++");
    EXPECT_EQ(uhippos.commodity(), getCommodity("hippos"));

    u1 = unit_from_string("[+++m+++]");
    EXPECT_EQ(u1, precise::m);
    u1 = unit_from_string("(++++++++)");
    EXPECT_FALSE(is_valid(u1));
}

TEST(stringToUnits, partitionMinimum)
{
    auto u1 = unit_from_string("milefoot");
    EXPECT_EQ(u1, precise::mile * precise::ft);

    u1 = unit_from_string("milefoot", minimum_partition_size2);
    EXPECT_EQ(u1, precise::mile * precise::ft);

    u1 = unit_from_string("milefoot", minimum_partition_size3);
    EXPECT_EQ(u1, precise::mile * precise::ft);

    u1 = unit_from_string("milefoot", minimum_partition_size4);
    EXPECT_EQ(u1, precise::mile * precise::ft);

    u1 = unit_from_string("milefoot", minimum_partition_size5);
    EXPECT_TRUE(is_error(u1));

    u1 = unit_from_string("milefoot", minimum_partition_size6);
    EXPECT_TRUE(is_error(u1));

    u1 = unit_from_string("milefoot", minimum_partition_size7);
    EXPECT_TRUE(is_error(u1));
}

TEST(stringToUnits, partitionMinimumDefault)
{
    auto u1 = unit_from_string("mifoot");
    EXPECT_EQ(u1, precise::mile * precise::ft);

    auto prev = setDefaultFlags(minimum_partition_size3);
    EXPECT_EQ(prev, 0ULL);
    EXPECT_EQ(getDefaultFlags(), minimum_partition_size3);

    u1 = unit_from_string("mifoot");
    EXPECT_TRUE(is_error(u1));

    prev = setDefaultFlags(0ULL);
    EXPECT_EQ(prev, minimum_partition_size3);
}

TEST(userDefinedUnits, definitions)
{
    precise_unit clucks(19.3, precise::m * precise::A);
    precise_unit sclucks(23, precise::m * precise::mol * precise::currency);
    addUserDefinedUnit("clucks", clucks);
    addUserDefinedUnit("sclucks", sclucks);

    EXPECT_EQ(unit_from_string("clucks/A"), precise_unit(19.3, precise::m));

    EXPECT_EQ(
        unit_from_string("sclucks/$"),
        precise_unit(23, precise::m * precise::mol));

    EXPECT_EQ(to_string(clucks), "clucks");

    EXPECT_EQ(to_string(clucks.inv()), "1/clucks");

    EXPECT_EQ(to_string(clucks.pow(2)), "clucks^2");

    EXPECT_EQ(to_string(clucks * kg), "clucks*kg");

    EXPECT_EQ(to_string(clucks * V), "clucks*V");

    EXPECT_EQ(to_string(clucks * mol), "clucks*mol");

    EXPECT_EQ(to_string(clucks * hp), "hp*clucks");

    EXPECT_EQ(to_string(clucks.pow(2) * kg), "kg*clucks^2");

    EXPECT_EQ(to_string(clucks.pow(3) * kg), "kg*clucks^3");

    EXPECT_EQ(to_string(precise::kg / clucks), "kg/clucks");

    EXPECT_EQ(to_string(precise::kg / clucks.pow(2)), "kg/clucks^2");

    EXPECT_EQ(to_string(precise::kg / clucks.pow(3)), "kg/clucks^3");

    EXPECT_EQ(
        to_string(precise::micro * precise::g / sclucks.pow(3)),
        "ug/sclucks^3");

    EXPECT_EQ(
        to_string(precise_unit(17, precise::kg) / sclucks), "17kg/sclucks");

    EXPECT_EQ(
        to_string(precise_unit(17, precise::kg) / sclucks.pow(2)),
        "17kg/sclucks^2");

    EXPECT_EQ(
        to_string(precise_unit(17, precise::kg) / sclucks.pow(3)),
        "17kg/sclucks^3");

    clearUserDefinedUnits();
}

TEST(userDefinedUnits, removeUserDefinition)
{
    precise_unit nidgit2(3.256, cd / m.pow(2));
    addUserDefinedUnit("nidgitS", nidgit2);
    precise_unit nidgit3(3.256, cd / m.pow(3));
    addUserDefinedUnit("nidgitC", nidgit3);

    auto ipm = unit_from_string("$/nidgitS");
    EXPECT_EQ(ipm, precise::currency / nidgit2);

    auto ipm2 = unit_from_string("$/nidgitC");
    EXPECT_EQ(ipm2, precise::currency / nidgit3);

    EXPECT_EQ(to_string(precise::currency / nidgit2), "$/nidgitS");
    EXPECT_EQ(to_string(precise::mol / nidgit3), "mol/nidgitC");

    removeUserDefinedUnit("nidgitS");

    auto ipm3 = unit_from_string("$/nidgitS");
    EXPECT_FALSE(is_valid(ipm3));

    auto ipm4 = unit_from_string("mol/nidgitC");
    EXPECT_EQ(ipm4, precise::mol / nidgit3);

    EXPECT_NE(to_string(precise::currency / nidgit2), "$/nidgitS");
    EXPECT_EQ(to_string(precise::s / nidgit3), "s/nidgitC");
}

TEST(userDefinedUnits, removeUserInputDefinition)
{
    precise_unit nidgit2(3.256, cd / m.pow(2));
    addUserDefinedInputUnit("nidgitS", nidgit2);
    precise_unit nidgit3(3.256, cd / m.pow(3));
    addUserDefinedInputUnit("nidgitC", nidgit3);

    auto ipm = unit_from_string("$/nidgitS");
    EXPECT_EQ(ipm, precise::currency / nidgit2);

    auto ipm2 = unit_from_string("$/nidgitC");
    EXPECT_EQ(ipm2, precise::currency / nidgit3);

    removeUserDefinedUnit("nidgitS");
    EXPECT_NO_THROW(removeUserDefinedUnit("chaos"));

    auto ipm3 = unit_from_string("$/nidgitS");
    EXPECT_FALSE(is_valid(ipm3));

    auto ipm4 = unit_from_string("mol/nidgitC");
    EXPECT_EQ(ipm4, precise::mol / nidgit3);
}

TEST(userDefinedUnits, removeUserOutputDefinition)
{
    precise_unit nidgit2(3.256, cd / m.pow(2));
    addUserDefinedOutputUnit("nidgitS", nidgit2);
    precise_unit nidgit3(3.256, cd / m.pow(3));
    addUserDefinedInputUnit("nidgitC", nidgit3);

    EXPECT_EQ(to_string(precise::currency / nidgit2), "$/nidgitS");
    EXPECT_EQ(to_string(precise::mol / nidgit3), "mol/nidgitC");

    removeUserDefinedUnit("nidgitS");

    EXPECT_NE(to_string(precise::currency / nidgit2), "$/nidgitS");
    EXPECT_EQ(to_string(precise::s / nidgit3), "s/nidgitC");
}

TEST(userDefinedUnits, definitionsAngstrom)
{
    addUserDefinedUnit("angstrom", precise::distance::angstrom);

    auto str = to_string(units::unit_from_string("us / angstrom^2"));
    EXPECT_EQ(str, "us/angstrom^2");
    str = to_string(units::unit_from_string("us / angstrom"));
    EXPECT_EQ(str, "us/angstrom");
    clearUserDefinedUnits();
}

TEST(userDefinedUnits, definitionStrings)
{
    precise_unit idgit(4.754, mol / m.pow(2));
    addUserDefinedUnit("idgit", idgit);

    auto ipm = unit_from_string("idgit/min");
    EXPECT_EQ(ipm, idgit / minute);

    auto str = to_string(ipm);
    EXPECT_EQ(str, "idgit/min");

    str = to_string(ipm.inv());
    EXPECT_EQ(str, "min/idgit");
    clearUserDefinedUnits();
}

TEST(userDefinedUnits, definitionStringsInputOnly)
{
    precise_unit idgit(4.754, mol / m.pow(2));
    addUserDefinedInputUnit("idgit", idgit);

    auto ipm = unit_from_string("idgit/min");
    EXPECT_EQ(ipm, idgit / minute);

    auto str = to_string(ipm);
    /** input only should not result in any string result with the user defined
     * input*/
    EXPECT_EQ(str.find("idgit"), std::string::npos);
    clearUserDefinedUnits();
}

TEST(userDefinedUnits, definitionStringsOutputOnly)
{
    precise_unit idgit(4.754, mol / m.pow(2));
    addUserDefinedOutputUnit("idgit", idgit);

    auto ipm = unit_from_string("idgit/min");
    EXPECT_NE(ipm, idgit / minute);

    auto str = to_string(idgit / minute);
    /** output only should make this work*/
    EXPECT_EQ(str, "idgit/min");
    clearUserDefinedUnits();
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

TEST(userDefinedUnits, fileOp1)
{
    auto outputstr = definedUnitsFromFile(TEST_FILE_FOLDER
                                          "/test_unit_files/other_units.txt");
    EXPECT_TRUE(outputstr.empty());
    auto res = unit_from_string("meeter");
    EXPECT_EQ(res, precise::m);

    res = unit_from_string("meh");
    EXPECT_EQ(res, precise::m / precise::hr);
    res = unit_from_string("mehmeh");
    EXPECT_EQ(res, precise::m / precise::hr / precise::s);
    EXPECT_EQ(unit_from_string("mehmeh"), unit_from_string("meh/s"));
    clearUserDefinedUnits();
}

TEST(userDefinedUnits, fileOp2)
{
    auto outputstr = definedUnitsFromFile(TEST_FILE_FOLDER
                                          "/test_unit_files/other_units2.txt");
    EXPECT_TRUE(outputstr.empty());
    auto y1 = unit_from_string("yodles");
    EXPECT_EQ(y1, precise_unit(73.0, count));

    auto y2 = unit_from_string("yeedles");
    EXPECT_EQ(y2, precise_unit(19.0, y1));
    auto y3 = unit_from_string("yimdles");
    EXPECT_EQ(y3, precise_unit(12.0, y2));
    EXPECT_EQ(unit_from_string("yimdles"), unit_from_string("19*yodles*12"));
    clearUserDefinedUnits();
}

TEST(userDefinedUnits, fileOp3)
{
    auto outputstr = definedUnitsFromFile(TEST_FILE_FOLDER
                                          "/test_unit_files/other_units3.txt");
    EXPECT_TRUE(outputstr.empty());
    auto y1 = unit_from_string("bl==p");
    EXPECT_EQ(y1, precise_unit(18.7, precise::us::cup));
    EXPECT_EQ(to_string(y1), "bl==p");

    auto y2 = unit_from_string("y,,p");
    EXPECT_EQ(y2, precise_unit(9.0, precise::ton));
    EXPECT_EQ(to_string(y2), "y,,p");

    auto y3 = unit_from_string("'np");
    EXPECT_EQ(y3, precise_unit(14.0, precise::kg));
    EXPECT_EQ(to_string(y3), "'np");

    auto y4 = unit_from_string("j\"\"");
    EXPECT_EQ(y4, precise_unit(13.5, precise::W));
    EXPECT_EQ(to_string(y4), "j\"\"");

    auto y5 = unit_from_string("q\"\"");
    EXPECT_EQ(y5, precise_unit(15.5, precise::W));
    EXPECT_EQ(to_string(y5), "q\"\"");
    clearUserDefinedUnits();
}

TEST(userDefinedUnits, fileOp4)
{
    auto outputstr = definedUnitsFromFile(TEST_FILE_FOLDER
                                          "/test_unit_files/other_units4.txt");
    EXPECT_TRUE(outputstr.empty());

    constexpr precise_unit agV(12.2, precise::V);
    constexpr precise_unit auV(14.2, precise::V);
    constexpr precise_unit HgV(17.7, precise::V);
    constexpr precise_unit FeV(17.7, precise::V);

    auto y1 = unit_from_string("agV");

    EXPECT_EQ(y1, agV);
    EXPECT_EQ(to_string(y1), "agV");

    auto y2 = unit_from_string("auV");
    EXPECT_EQ(y2, auV);
    EXPECT_EQ(to_string(y2), "auV");

    auto y3 = unit_from_string("HgV");
    EXPECT_EQ(y3, HgV);
    EXPECT_EQ(to_string(y3), "FeV");

    auto y4 = unit_from_string("FeV");
    EXPECT_NE(y4, FeV);

    clearUserDefinedUnits();
}

TEST(userDefinedUnits, invalidFile)
{
    auto outputstr = definedUnitsFromFile("not_a_file.txt");
    EXPECT_FALSE(outputstr.empty());
}

TEST(userDefinedUnits, badDefinitions)
{
    auto outputstr = definedUnitsFromFile(
        TEST_FILE_FOLDER "/test_unit_files/bad_unit_definitions.txt");
    EXPECT_FALSE(outputstr.empty());
    auto cnt = std::count(outputstr.begin(), outputstr.end(), '\n');
    EXPECT_EQ(cnt, 5);
}

TEST(defaultUnits, unitTypes)
{
    EXPECT_EQ(default_unit("impedance quantity"), precise::ohm);
    EXPECT_EQ(default_unit("distance"), precise::m);
    EXPECT_EQ(default_unit("Quantity of distance"), precise::m);
    EXPECT_EQ(default_unit("frequency measure"), precise::Hz);
}

TEST(defaultUnits, rate)
{
    EXPECT_EQ(
        default_unit("entropy rate"), precise::J / precise::K / precise::s);
    EXPECT_EQ(default_unit("rate of mass"), precise::kg / s);
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
    auto bigNumber =
        unit_from_string("_45_625_252_22524_252452_25242522562_E522_");
    EXPECT_FALSE(isfinite(bigNumber));
    EXPECT_TRUE(isinf(bigNumber));

    EXPECT_EQ(
        precise_unit(45625252.0, precise::m),
        unit_from_string("_45_625_252_m_"));

    auto ukittens = unit_from_string("_45_625_252_kittens_");
    EXPECT_EQ(ukittens.commodity(), getCommodity("kittens"));

    EXPECT_EQ(precise_unit(45.0, precise::one), unit_from_string("45"));

    EXPECT_FALSE(is_valid(unit_from_string("_____-_____")));
}

TEST(funnyStrings, outofrange)
{  // these are mainly testing that it doesn't throw
    EXPECT_FALSE(
        isfinite(unit_from_string("1532^34e505")));  // out of range error
    EXPECT_TRUE(isinf(unit_from_string("34e505")));  // out of range
    EXPECT_TRUE(isinf(unit_from_string("-34e505")));  // out of range

    EXPECT_TRUE(isinf(
        unit_from_string("34.785e12458")));  // out of range of quad precision
}

TEST(funnyStrings, powersof1)
{  // check the power of 1 removals
    EXPECT_EQ(precise::m, unit_from_string("m*(1)^5"));
    EXPECT_EQ(precise::m, unit_from_string("m*(1)^"));
    EXPECT_EQ(precise::m.pow(2), unit_from_string("m*(1)^2*m"));  // invalid

    EXPECT_EQ(precise::m, unit_from_string("m*(1)^-1"));
    EXPECT_EQ(precise::m, unit_from_string("m*(1)^-1*(1)^7"));
    EXPECT_EQ(precise::m, unit_from_string("m*(1)^+*(1)^+"));
    EXPECT_EQ(precise::m, unit_from_string("m^1^1"));
    EXPECT_EQ(precise::m, unit_from_string("m^(1)^1"));
    EXPECT_EQ(precise::m, unit_from_string("m^(1)^-"));
}

TEST(funnyString, google)
{
    EXPECT_EQ(precise::milli * precise::W, unit_from_string("mw"));
    EXPECT_EQ(
        precise::milli * precise::W * precise::hr, unit_from_string("mwhr"));
    EXPECT_TRUE(is_valid(unit_from_string("imperial beer barrel")));
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

#ifdef ENABLE_UNIT_TESTING
TEST(stringGeneration, test1)
{
    auto res = detail::testing::testCleanUpString(
        detail::testing::testUnitSequenceGeneration(2100.0, "m^-3"), 0);
    EXPECT_EQ(res, "2.1L^-1");

    res = detail::testing::testCleanUpString(
        detail::testing::testUnitSequenceGeneration(2100.0, "kg^-1"), 0);
    EXPECT_EQ(res, "2.1g^-1");

    res = detail::testing::testCleanUpString(
        detail::testing::testUnitSequenceGeneration(1.0, "/kg"), 0);
    EXPECT_EQ(res, "1/kg");

    res = detail::testing::testCleanUpString(
        detail::testing::testUnitSequenceGeneration(1000.0 * 1000.0, "m^-2"),
        0);
    EXPECT_EQ(res, "mm^-2");

    res = detail::testing::testCleanUpString(
        detail::testing::testUnitSequenceGeneration(
            1000.0 * 1000.0 * 1000.0, "s^-3"),
        0);
    EXPECT_EQ(res, "ms^-3");

    res = detail::testing::testCleanUpString(
        detail::testing::testUnitSequenceGeneration(217.5632, "m^-2"), 0);
    EXPECT_EQ(res, "217.5632m^-2");

    res = detail::testing::testCleanUpString(
        detail::testing::testUnitSequenceGeneration(157.1, "s^-3"), 0);
    EXPECT_EQ(res, "157.1s^-3");
}

TEST(stringCleanup, testZstrings)
{
    auto res = detail::testing::testCleanUpString("0.000000045lb", 0);
    EXPECT_EQ(res, "0.000000045lb");

    res = detail::testing::testCleanUpString("0.0000000000000045lb", 0);
    EXPECT_EQ(res, "0.0000000000000045lb");

    res = detail::testing::testCleanUpString("s*00.000000000000004lb", 0);
    EXPECT_EQ(res, "s*00.000000000000004lb");

    res = detail::testing::testCleanUpString("1.00000000000009lb", 0);
    EXPECT_EQ(res, "1lb");

    res = detail::testing::testCleanUpString("1.00000000000019lb", 0);
    EXPECT_EQ(res, "1.00000000000019lb");

    res = detail::testing::testCleanUpString("1.00000009000009lb", 0);
    EXPECT_EQ(res, "1.00000009lb");

    res = detail::testing::testCleanUpString("100000009000009lb", 0);
    EXPECT_EQ(res, "100000009000009lb");

    res = detail::testing::testCleanUpString("1.0000000000000lb", 0);
    EXPECT_EQ(res, "1lb");

    res = detail::testing::testCleanUpString("1.0000000000000", 0);
    EXPECT_EQ(res, "1");

    /** make sure it doesn't skip a multiplier*/
    res = detail::testing::testCleanUpString("1.0005*10000008*lb", 0);
    EXPECT_EQ(res, "1.0005*10000008*lb");

    res = detail::testing::testCleanUpString("n.000000000000000", 0);
    EXPECT_EQ(res, "n.000000000000000");

    res = detail::testing::testCleanUpString("1.0005*10000008", 0);
    EXPECT_EQ(res, "1.0005*10000008");

    res = detail::testing::testCleanUpString("1.0005*10000000", 0);
    EXPECT_EQ(res, "1.0005*10000000");

    res = detail::testing::testCleanUpString(".0000000000000000000000004lb", 0);
    EXPECT_EQ(res, ".0000000000000000000000004lb");
}

TEST(stringCleanup, test9strings)
{
    auto res = detail::testing::testCleanUpString("4.5999999999999999994lb", 0);
    EXPECT_EQ(res, "4.6lb");

    res = detail::testing::testCleanUpString("99.999999999999999998lb", 0);
    EXPECT_EQ(res, "100lb");

    res = detail::testing::testCleanUpString("49.999999999999999998lb", 0);
    EXPECT_EQ(res, "50lb");

    res = detail::testing::testCleanUpString(
        "10.7*999999999999999999999999lb", 0);
    EXPECT_EQ(res, "10.7*999999999999999999999999lb");
}

TEST(stringCleanup, withCommodities)
{
    auto res = detail::testing::testCleanUpString("m^2", commodities::aluminum);
    EXPECT_EQ(res, "m{aluminum}^2");

    res = detail::testing::testCleanUpString("m^-2", commodities::aluminum);
    EXPECT_EQ(res, "{aluminum}*m^-2");

    res = detail::testing::testCleanUpString("1/m^2", ~commodities::aluminum);
    EXPECT_EQ(res, "1/m{aluminum}^2");
}

TEST(stringGeneration, addPowerString)
{
    std::string t1{"bbb"};
    detail::testing::testAddUnitPower(t1, "cc", 1, 0);
    EXPECT_EQ(t1, "bbb*cc");
    t1 = "bbb";
    detail::testing::testAddUnitPower(t1, "cc", -1, 0);
    EXPECT_EQ(t1, "bbb*cc^-1");
    t1 = "bbb/";
    detail::testing::testAddUnitPower(t1, "cc", 1, 0);
    EXPECT_EQ(t1, "bbb/cc");
    t1 = "bbb";
    detail::testing::testAddUnitPower(t1, "cc", -2, 0);
    EXPECT_EQ(t1, "bbb*cc^-2");
    t1 = "bbb*";
    detail::testing::testAddUnitPower(t1, "cc", -2, 0);
    EXPECT_EQ(t1, "bbb*cc^-2");
    t1 = "bbb/";
    detail::testing::testAddUnitPower(t1, "cc", -2, 0);
    EXPECT_EQ(t1, "bbb/cc^-2");
    t1 = "bbb";
    detail::testing::testAddUnitPower(t1, "cc", 2, 0);
    EXPECT_EQ(t1, "bbb*cc^2");
    t1 = "bbb";
    detail::testing::testAddUnitPower(t1, "cc", 12, 0);
#ifdef UNITS_CONSTEXPR_IF_SUPPORTED
    if constexpr (detail::bitwidth::base_size > sizeof(std::uint32_t)) {
#else
    if (detail::bitwidth::base_size > sizeof(std::uint32_t)) {
#endif
        EXPECT_EQ(t1, "bbb*cc^(12)");
    } else {
        EXPECT_EQ(t1, "bbb*cc^9*cc^3");
    }
    t1 = "bbb";
    detail::testing::testAddUnitPower(t1, "cc", -14, 0);
#ifdef UNITS_CONSTEXPR_IF_SUPPORTED
    if constexpr (detail::bitwidth::base_size > sizeof(std::uint32_t)) {
#else
    if (detail::bitwidth::base_size > sizeof(std::uint32_t)) {
#endif
        EXPECT_EQ(t1, "bbb*cc^(-14)");
    } else {
        EXPECT_EQ(t1, "bbb*cc^-9*cc^-5");
    }
    t1 = "bbb/";
    detail::testing::testAddUnitPower(t1, "cc", -14, 0);
#ifdef UNITS_CONSTEXPR_IF_SUPPORTED
    if constexpr (detail::bitwidth::base_size > sizeof(std::uint32_t)) {
#else
    if (detail::bitwidth::base_size > sizeof(std::uint32_t)) {
#endif
        EXPECT_EQ(t1, "bbb/cc^(-14)");
    } else {
        EXPECT_EQ(t1, "bbb/cc^-9/cc^-5");
    }
}

#endif

#ifdef ENABLE_UNIT_MAP_ACCESS
TEST(mapTests, testRoundTrip)
{
    const auto& map = detail::getUnitStringMap();
    int invalidCount = 0;
    for (const auto& val : map) {
        auto runit = val.second;
        if (!val.first.empty() && val.first.front() != '*') {
            if (is_default(val.second)) {
                // any multiplier by default units is just the multiplier
                // which doesn't make sense for this test
                continue;
            }
            if (!is_valid(val.second)) {
                // this would cause issues as well so not a useful test
                continue;
            }
            if (val.first.find_last_of(' ') != std::string::npos) {
                // these are special cases and not useful for testing here
                continue;
            }
            // some specicialized units
            if (val.second == precise::special::rootHertz) {
                continue;
            }
            if (val.second == precise::special::ASD) {
                continue;
            }
            if (val.first.compare(0, 2, "50") == 0) {
                // some specialized tissue culture units
                continue;
            }
            if (val.first.find(")_") != std::string::npos) {
                continue;
            }
            std::string str = "1*" + val.first;

            auto strUnit = unit_from_string(str);
            if (isnan(runit)) {
                EXPECT_TRUE(isnan(strUnit));
                if (!isnan(strUnit)) {
                    strUnit = unit_from_string(str);
                    ++invalidCount;
                }
            } else {
                if (strUnit != runit) {
                    if (val.second.has_same_base(precise::rad)) {
                        continue;
                    }
                    strUnit = unit_from_string(str);
                    EXPECT_EQ(strUnit, runit)
                        << str << " failed to convert properly";
                    ++invalidCount;
                }
            }
        }
    }
    EXPECT_EQ(invalidCount, 0);
}

TEST(mapTests, testRoundTripFromUnit)
{
    const auto& map = detail::getUnitNameMap();
    for (const auto& val : map) {
        auto runit = val.first;
        std::string uname(val.second);
        if (!uname.empty() && uname.front() != '*') {
            std::string str = "1*" + uname;

            auto strUnit = unit_from_string(str);
            if (isnan(strUnit)) {
                EXPECT_TRUE(isnan(runit));
            } else {
                EXPECT_EQ(strUnit, runit)
                    << str << " failed to convert properly";
                if (strUnit != runit) {
                    strUnit = unit_from_string(str);
                }
            }
        }
    }
}

// test combinations of 2 character units string and one character unit strings
// for misinterpretation
TEST(mapTests, twoByOne)
{
    const auto& map = detail::getUnitStringMap();
    std::vector<std::pair<std::string, precise_unit>> twocharunits;
    std::vector<std::pair<std::string, precise_unit>> onecharunits;
    for (const auto& val : map) {
        if (val.first.size() == 2) {
            if (val.first.front() > 0 && std::isalpha(val.first.front()) != 0) {
                if (is_valid(val.second)) {
                    twocharunits.emplace_back(val);
                }
            }
        }
        if (val.first.size() == 1) {
            if (val.first.front() > 0 && std::isalpha(val.first.front()) != 0) {
                if (is_valid(val.second)) {
                    onecharunits.emplace_back(val);
                }
            }
        }
    }

    for (const auto& twochar : twocharunits) {
        if (twochar.first == "mo" || twochar.first == "mO") {
            // month has some expected patterns which will cause problems in
            // this test
            continue;
        }
        for (const auto& onechar : onecharunits) {
            std::string ustring = twochar.first + ' ' + onechar.first;
            auto unit = twochar.second * onechar.second;
            EXPECT_EQ(unit_from_string(ustring), unit)
                << ustring << " does not produce equivalent unit";
        }
    }

    for (const auto& onechar : onecharunits) {
        for (const auto& twochar : twocharunits) {
            std::string ustring = onechar.first + ' ' + twochar.first;
            auto unit = twochar.second * onechar.second;
            EXPECT_EQ(unit_from_string(ustring), unit)
                << ustring << " does not produce equivalent unit";
        }
    }
}

// test combinations of 2 character units string and another 2 character unit
// strings for misinterpretation
TEST(mapTests, twoByTwo)
{
    const auto& map = detail::getUnitStringMap();
    std::vector<std::pair<std::string, precise_unit>> twocharunits;
    for (const auto& val : map) {
        // fluid is a modifier that applies to several other units so can't be
        // disambiguated
        if (val.first.size() == 2 && val.first != "fl") {
            if (val.first.front() > 0 && std::isalpha(val.first.front()) != 0) {
                if (is_valid(val.second)) {
                    twocharunits.emplace_back(val);
                }
            }
        }
    }

    for (const auto& twochar : twocharunits) {
        for (const auto& twocharb : twocharunits) {
            std::string ustring = twochar.first + ' ' + twocharb.first;
            auto unit = twochar.second * twocharb.second;
            EXPECT_EQ(unit_from_string(ustring), unit)
                << ustring << " does not produce equivalent unit";

            std::string ustring2 = twocharb.first + ' ' + twochar.first;
            EXPECT_EQ(unit_from_string(ustring2), unit)
                << ustring2 << " does not produce equivalent unit";
        }
    }
}
#endif

namespace units {

static std::ostream& operator<<(std::ostream& os, const units::precise_unit& u)
{
    os << to_string(u);
    return os;
}

}  // namespace units

TEST(stream, testOutstream)
{
    std::stringstream sss;

    sss << units::precise::m;

    auto res = sss.str();
    EXPECT_EQ(res, "m");
}

#ifdef EXTRA_UNIT_STANDARDS

TEST(extra, r20)
{
    auto unit = r20_unit("NOT A VALID STRING");
    EXPECT_TRUE(is_error(unit));
    unit = r20_unit("E43");
    EXPECT_FALSE(is_error(unit));
}

TEST(extra, dod)
{
    auto unit = dod_unit("NOT A VALID STRING");
    EXPECT_TRUE(is_error(unit));

    unit = dod_unit("YD");
    EXPECT_FALSE(is_error(unit));
}

TEST(extra, x12)
{
    auto unit = x12_unit("NOT A VALID STRING");
    EXPECT_TRUE(is_error(unit));

    unit = x12_unit("RB");
    EXPECT_FALSE(is_error(unit));
}
#endif
