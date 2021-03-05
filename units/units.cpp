/*
Copyright (c) 2019-2021,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#include "units.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <cctype>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <limits>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>
#include <utility>
#include <vector>

/** @file
references http://people.csail.mit.edu/jaffer/MIXF/MIXF-08
*/

namespace UNITS_NAMESPACE {

template<typename X>
X numericalRoot(X value, int power)
{
    switch (power) {
        case 0:
            return X{1.0};
        case 1:
            return value;
        case -1:
            return X{1.0} / value;
        case 2:
            if (value < X{0.0}) {
                return constants::invalid_conversion;
            }
            return std::sqrt(value);
        case -2:
            if (value < X{0.0}) {
                return constants::invalid_conversion;
            }
            return std::sqrt(X{1.0} / value);
        case 3:
            return std::cbrt(value);
        case -3:
            return std::cbrt(X{1.0} / value);
        case 4:
            if (value < X{0.0}) {
                return constants::invalid_conversion;
            }
            return std::sqrt(std::sqrt(value));
        case -4:
            if (value < X{0.0}) {
                return constants::invalid_conversion;
            }
            return std::sqrt(std::sqrt(X{1.0} / value));
        default:
            if (value < X{0.0} && power % 2 == 0) {
                return constants::invalid_conversion;
            }
            return std::pow(value, X{1.0} / static_cast<X>(power));
    }
}

unit root(const unit& un, int power)
{
    if (power == 0) {
        return one;
    }
    if (un.multiplier() < 0.0 && power % 2 == 0) {
        return error;
    }
    return unit{un.base_units().root(power),
                numericalRoot(un.multiplier(), power)};
}

precise_unit root(const precise_unit& un, int power)
{
    if (power == 0) {
        return precise::one;
    }
    if (un.multiplier() < 0.0 && power % 2 == 0) {
        return precise::invalid;
    }
    return precise_unit{un.base_units().root(power),
                        numericalRoot(un.multiplier(), power)};
}

measurement root(const measurement& meas, int power)
{
    return {numericalRoot(meas.value(), power), root(meas.units(), power)};
}

fixed_measurement root(const fixed_measurement& fm, int power)
{
    return {numericalRoot(fm.value(), power), root(fm.units(), power)};
}

uncertain_measurement root(const uncertain_measurement& um, int power)
{
    auto new_value = numericalRoot(um.value(), power);
    auto new_tol = new_value * um.uncertainty() /
        (static_cast<double>((power >= 0) ? power : -power) * um.value());
    return uncertain_measurement(new_value, new_tol, root(um.units(), power));
}

precise_measurement root(const precise_measurement& pm, int power)
{
    return {numericalRoot(pm.value(), power), root(pm.units(), power)};
}

fixed_precise_measurement root(const fixed_precise_measurement& fpm, int power)
{
    return {numericalRoot(fpm.value(), power), root(fpm.units(), power)};
}

// sum the powers of a unit
static int order(const unit& val)
{
    auto bd = val.base_units();
    int order = std::labs(bd.meter()) + std::labs(bd.kelvin()) +
        std::labs(bd.kg()) + std::labs(bd.count()) + std::labs(bd.ampere()) +
        std::labs(bd.second()) + std::labs(bd.currency()) +
        std::labs(bd.radian()) + std::labs(bd.candela()) + std::labs(bd.mole());
    return order;
}

// NOTE no unit strings with '/' in it this can cause issues when converting to
// string with out-of-order operations
using umap = std::unordered_map<unit, const char*>;
static const umap base_unit_names{
    {m, "m"},
    {m * m, "m^2"},
    {m * m * m, "m^3"},
    {(mega * m).pow(3), "(1e9km^3)"},  // Mm^3 is a unit in gas industry for
                                       // 1000 m^3 not mega meters cubed
    {kg, "kg"},
    {mol, "mol"},
    {A, "A"},
    {A * h, "Ah"},
    {V, "V"},
    {s, "s"},
    // this is so Gs doesn't get used which can cause issues
    {giga * s, "Bs"},
    {cd, "cd"},
    {K, "K"},
    {N, "N"},
    {Pa, "Pa"},
    {J, "J"},
    {C, "C"},
    {F, "F"},
    // because GF is gram force not giga Farad which is a ridiculous unit
    // otherwise generates confusion
    {giga * F, "(1000MF)"},
    {S, "S"},
    {Wb, "Wb"},
    {T, "T"},
    {H, "H"},
    // deal with pico henry which is interpreted as acidity (pH)
    {pico * H, "(A^-2*pJ)"},
    {lm, "lm"},
    {lx, "lux"},
    // remove Bq since it is often confusing in other contexts 1/s
    // {Bq, "Bq"},
    {unit(2.58e-4, C / kg), "R"},
    {in, "in"},
    {unit_cast(precise::in.pow(2)), "in^2"},
    {unit_cast(precise::in.pow(3)), "in^3"},
    {ft, "ft"},
    {unit_cast(precise::imp::foot), "ft_br"},
    {unit_cast(precise::imp::inch), "in_br"},
    {unit_cast(precise::imp::yard), "yd_br"},
    {unit_cast(precise::imp::rod), "rd_br"},
    {unit_cast(precise::imp::mile), "mi_br"},
    {unit_cast(precise::imp::chain), "ch_br"},
    {unit_cast(precise::imp::pace), "pc_br"},
    {unit_cast(precise::imp::link), "lk_br"},
    {unit_cast(precise::imp::chain), "ch_br"},
    {unit_cast(precise::imp::nautical_mile), "nmi_br"},
    {unit_cast(precise::imp::knot), "kn_br"},
    {unit_cast(precise::cgs::curie), "Ci"},
    {(mega * m).pow(3),
     "ZL"},  // another one of those units that can be confused
    {bar, "bar"},
    {unit_cast(precise::nautical::knot), "knot"},
    {ft * ft, "ft^2"},
    {ft * ft * ft, "ft^3"},
    {unit_cast(precise::ft.pow(2)), "ft^2"},
    {unit_cast(precise::ft.pow(3)), "ft^3"},
    {yd, "yd"},
    {unit_cast(precise::us::rod), "rd"},
    {yd * yd, "yd^2"},
    {yd.pow(3), "yd^3"},
    {unit_cast(precise::yd.pow(2)), "yd^2"},
    {unit_cast(precise::yd.pow(3)), "yd^3"},
    {min, "min"},
    {ms, "ms"},
    {ns, "ns"},
    {h, "h"},
    {unit_cast(precise::time::day), "day"},
    {unit_cast(precise::time::week), "week"},
    {unit_cast(precise::time::yr), "yr"},
    {unit_cast(precise::time::syr), "syr"},
    {unit_cast(precise::time::ag), "a_g"},
    {unit_cast(precise::time::at), "a_t"},
    {unit_cast(precise::time::aj), "a_j"},
    {deg, "deg"},
    {rad, "rad"},
    {unit_cast(precise::angle::grad), "grad"},
    {degC, u8"\u00B0C"},
    {degF, u8"\u00B0F"},
    {mile, "mi"},
    {mile * mile, "mi^2"},
    {unit_cast(precise::mile.pow(2)), "mi^2"},
    {cm, "cm"},
    {km, "km"},
    {km * km, "km^2"},
    {mm, "mm"},
    {nm, "nm"},
    {unit_cast(precise::distance::ly), "ly"},
    {unit_cast(precise::distance::au), "au"},
    {percent, "%"},
    {unit_cast(precise::special::ASD), "ASD"},
    {unit_cast(precise::special::rootHertz), "rootHertz"},
    {currency, "$"},
    {count, "count"},
    {ratio, ""},
    {error, "ERROR"},
    {defunit, "defunit"},
    {iflag, "flag"},
    {eflag, "eflag"},
    {pu, "pu"},
    {Gy, "Gy"},
    {Sv, "Sv"},
    {Hz, "Hz"},
    {rpm, "rpm"},
    {kat, "kat"},
    {sr, "sr"},
    {W, "W"},
    {VAR, "VAR"},
    {MVAR, "MVAR"},
    {acre, "acre"},
    {MW, "MW"},
    {kW, "kW"},
    {mW, "mW"},
    {puMW, "puMW"},
    {puMW / mega, "puW"},
    {puV, "puV"},
    {puA, "puA"},
    {mA, "mA"},
    {kV, "kV"},
    {unit_cast(precise::energy::therm_ec), "therm"},
    {unit_cast(precise::energy::tonc), "tonc"},
    {acre, "acre"},
    {unit_cast(precise::area::are), "are"},
    {unit_cast(precise::area::hectare), "hectare"},
    {unit_cast(precise::area::barn), "barn"},
    {pu * ohm, "puOhm"},
    {puHz, "puHz"},
    {hp, "hp"},
    {mph, "mph"},
    {unit_cast(precise::energy::eV), "eV"},
    {kcal, "kcal"},
    {btu, "btu"},
    {unit_cast(precise::other::CFM), "CFM"},
    {unit_cast(precise::pressure::atm), "atm"},
    {unit_cast(precise::pressure::psi), "psi"},
    {unit_cast(precise::pressure::psig), "psig"},
    {unit_cast(precise::pressure::inHg), "inHg"},
    {unit_cast(precise::pressure::inH2O), "inH2O"},
    {unit_cast(precise::pressure::mmHg), "mmHg"},
    {unit_cast(precise::pressure::mmH2O), "mmH2O"},
    {unit_cast(precise::pressure::torr), "torr"},
    {unit_cast(precise::energy::EER), "EER"},
    {unit_cast(precise::energy::quad), "quad"},
    {unit_cast(precise::laboratory::IU), "[IU]"},
    {kWh, "kWh"},
    {MWh, "MWh"},
    {unit_cast(precise::other::MegaBuck), "M$"},
    {unit_cast(precise::other::GigaBuck), "B$"},
    {L, "L"},
    {unit_cast(precise::mL), "mL"},
    {unit_cast(precise::micro * precise::L), "uL"},
    {gal, "gal"},
    {unit_cast(precise::us::barrel), "bbl"},
    {lb, "lb"},
    {ton, "ton"},
    {tonne, "t"},  // metric ton
    {unit_cast(precise::mass::u), "u"},
    {kB, "kB"},
    {MB, "MB"},
    {GB, "GB"},
    {unit_cast(precise::data::KiB), "KiB"},
    {unit_cast(precise::data::MiB), "MiB"},
    {unit_cast(precise::us::dry::bushel), "bu"},
    {unit_cast(precise::us::floz), "floz"},
    {oz, "oz"},
    {unit_cast(precise::distance::angstrom), u8"\u00C5"},
    {g, "g"},
    {mg, "mg"},
    {unit_cast(precise::us::cup), "cup"},
    {unit_cast(precise::us::tsp), "tsp"},
    {unit_cast(precise::us::tbsp), "tbsp"},
    {unit_cast(precise::us::quart), "qt"},
    {unit_cast(precise::data::GiB), "GiB"},
    {unit_cast(precise::other::ppm), "ppm"},
    {unit_cast(precise::other::ppb), "ppb"}};

using ustr = std::pair<precise_unit, const char*>;
// units to divide into tests to explore common multiplier units
static UNITS_CPP14_CONSTEXPR_OBJECT std::array<ustr, 22> testUnits{
    {ustr{precise::m, "m"},
     ustr{precise::s, "s"},
     ustr{precise::ms, "ms"},
     ustr{precise::min, "min"},
     ustr{precise::hr, "hr"},
     ustr{precise::time::day, "day"},
     ustr{precise::lb, "lb"},
     ustr{precise::ft, "ft"},
     ustr{precise::ft.pow(2), "ft^2"},
     ustr{precise::ft.pow(3), "ft^3"},
     ustr{precise::m.pow(2), "m^2"},
     ustr{precise::L, "L"},
     ustr{precise::kg, "kg"},
     ustr{precise::km, "km"},
     ustr{precise::currency, "$"},
     ustr{precise::volt, "V"},
     ustr{precise::watt, "W"},
     ustr{precise::electrical::kW, "kW"},
     ustr{precise::electrical::mW, "mW"},
     ustr{precise::MW, "MW"},
     ustr{precise::s.pow(2), "s^2"},
     ustr{precise::count, "item"}}};

// units to divide into tests to explore common multiplier units
static UNITS_CPP14_CONSTEXPR_OBJECT std::array<ustr, 3> siTestUnits{
    {ustr{precise::h * precise::A, "Ah"},
     ustr{precise::energy::eV, "eV"},
     ustr{precise::W * precise::h, "Wh"}}};

// complex units used to reduce unit complexity
static UNITS_CPP14_CONSTEXPR_OBJECT std::array<ustr, 4> creduceUnits{
    {ustr{precise::V.inv(), "V*"},
     ustr{precise::V, "V^-1*"},
     ustr{precise::W, "W^-1*"},
     ustr{precise::W.inv(), "W*"}}};

// thought about making this constexpr array, but the problem is that runtime
// floats are not guaranteed to be the same as compile time floats so really
// this map needs to be generated at run-time once multiplier prefixes commonly
// used
static const std::unordered_map<float, char> si_prefixes{
    {0.001F, 'm'},        {1.0F / 1000.0F, 'm'},
    {1000.0F, 'k'},       {1.0F / 0.001F, 'k'},
    {1e-6F, 'u'},         {0.01F, 'c'},
    {1.0F / 100.0F, 'c'}, {1.0F / 1e6F, 'u'},
    {1000000.0F, 'M'},    {1.0F / 0.000001F, 'M'},
    {1000000000.0F, 'G'}, {1.0F / 0.000000001F, 'G'},
    {1e-9F, 'n'},         {1.0F / 1e9F, 'n'},
    {1e-12F, 'p'},        {1.0F / 1e12F, 'p'},
    {1e-15F, 'f'},        {1.0F / 1e15F, 'f'},
    {1e12F, 'T'},         {1.0F / 1e-12F, 'T'}};

// check if the character is something that could begin a number
static inline bool isNumericalStartCharacter(char X)
{
    return ((X >= '0' && X <= '9') || X == '-' || X == '+' || X == '.');
}

// check if the character is something that could be in a number
static inline bool isNumericalCharacter(char X)
{
    return (
        (X >= '0' && X <= '9') || X == '-' || X == '+' || X == '.' ||
        X == 'E' || X == 'e');
}

// forward declaration of the internal from_string function
static precise_unit unit_from_string_internal(
    std::string unit_string,
    std::uint32_t match_flags);

// forward declaration of the quick find function
static precise_unit
    unit_quick_match(std::string unit_string, std::uint32_t match_flags);
// forward declaration of the function to check for custom units
static precise_unit checkForCustomUnit(const std::string& unit_string);

// check if the character is an ascii digit
static inline bool isDigitCharacter(char X)
{
    return (X >= '0' && X <= '9');
}

/// Replace a string in place
/*static bool ReplaceStringInPlace(
    std::string& subject,
    const std::string& search,
    const std::string& replace)
{
    size_t pos = 0;
    bool changed{false};
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        changed = true;
        subject.replace(pos, search.length(), replace);
        pos += replace.length();
    }
    return changed;
}
*/
/// Replace a string in place using const char *
static bool ReplaceStringInPlace(
    std::string& subject,
    const char* search,
    int searchSize,
    const char* replace,
    int replaceSize)
{
    bool changed{false};
    size_t pos = 0;
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, searchSize, replace);
        pos += replaceSize;
        changed = true;
    }
    return changed;
}

// Generate an SI prefix or a numerical multiplier string for prepending a unit
static std::string getMultiplierString(double multiplier, bool numOnly = false)
{
    if (multiplier == 1.0) {
        return std::string{};
    }
    if (!numOnly) {
        auto si = si_prefixes.find(static_cast<float>(multiplier));
        if (si != si_prefixes.end()) {
            return std::string(1, si->second);
        }
    }
    int P = 18;  // the desired precision
    std::stringstream ss;

    ss << std::setprecision(P) << multiplier;
    auto rv = ss.str();
    if (rv.size() <= 4) {
        // modify some improper strings that cause issues later on
        // some platforms don't produce these
        if (rv == "inf") {
            return "1.00000000000000*(infinity)";  // LCOV_EXCL_LINE
        }
        if (rv == "-inf") {
            return "1.00000000000000*(-1.00000000000000*infinity)";  // LCOV_EXCL_LINE
        }
        if (rv == "nan") {
            return "1.00000000000000*(nan)";  // LCOV_EXCL_LINE
        }
    }
    return rv;
}

static std::string generateUnitSequence(double mux, std::string seq)
{
    bool noPrefix = false;
    // deal with a few common things
    if (seq.compare(0, 3, "m^3") == 0) {
        if (mux <= 0.1) {
            seq.replace(0, 3, "L");
            mux *= 1000.0;
        }
    } else if (seq.compare(0, 4, "m^-3") == 0) {
        if (mux > 100.0) {
            seq.replace(0, 4, "L^-1");
            mux /= 1000.0;
        }
    } else if (seq.compare(0, 5, "kg^-1") == 0) {
        if (mux > 100.0) {
            seq.replace(0, 5, "g^-1");
            mux /= 1000.0;
        } else {
            noPrefix = true;
        }
    } else if (seq.compare(0, 2, "kg") == 0) {
        if (mux <= 0.1) {
            if (seq.size() > 3 && seq[2] == '^') {
                noPrefix = true;
            } else {
                seq.replace(0, 2, "g");
                mux *= 1000.0;
            }
        } else {
            noPrefix = true;
        }
    }
    if (mux == 1.0) {
        if (seq.front() == '/') {
            seq.insert(seq.begin(), '1');
        }
        return seq;
    }
    if (seq.front() == '/') {
        int pw = 1;
        auto pwerloc = seq.find_first_of('^');
        if (pwerloc != std::string::npos) {
            pw = seq[pwerloc + 1] - '0';
        }
        std::string muxstr;
        switch (pw) {
            case 1:
                muxstr = getMultiplierString(1.0 / mux, noPrefix);
                if (isNumericalStartCharacter(muxstr.front())) {
                    muxstr = getMultiplierString(mux, true);
                } else {
                    seq.insert(1, muxstr);
                    muxstr = "1";
                }
                break;
            case 2:
                muxstr = getMultiplierString(std::sqrt(1.0 / mux), noPrefix);
                if (isNumericalStartCharacter(muxstr.front())) {
                    muxstr = getMultiplierString(mux, true);
                } else {
                    seq.insert(1, muxstr);
                    muxstr = "1";
                }
                break;
            case 3:
                muxstr = getMultiplierString(std::cbrt(1.0 / mux), noPrefix);
                if (isNumericalStartCharacter(muxstr.front())) {
                    muxstr = getMultiplierString(mux, true);
                } else {
                    seq.insert(1, muxstr);
                    muxstr = "1";
                }
                break;
            default:
                muxstr = getMultiplierString(mux, true);
        }
        return muxstr + seq;
    }
    auto pwerloc = seq.find_first_of('^');
    if (pwerloc == std::string::npos) {
        return getMultiplierString(mux, noPrefix) + seq;
    }
    auto mloc = seq.find_first_of("*/");
    if (mloc < pwerloc) {
        return getMultiplierString(mux, noPrefix) + seq;
    }
    int pw = stoi(seq.substr(pwerloc + 1, mloc - pwerloc));
    std::string muxstr;
    switch (pw) {
        case -1:
            muxstr = getMultiplierString(1.0 / mux, noPrefix);
            if (isNumericalStartCharacter(muxstr.front())) {
                muxstr = getMultiplierString(mux, true);
            }
            break;
        case -2:
            muxstr = getMultiplierString(std::sqrt(1.0 / mux), noPrefix);
            if (isNumericalStartCharacter(muxstr.front())) {
                muxstr = getMultiplierString(mux, true);
            }
            break;
        case -3:
            muxstr = getMultiplierString(std::cbrt(1.0 / mux), noPrefix);
            if (isNumericalStartCharacter(muxstr.front())) {
                muxstr = getMultiplierString(mux, true);
            }
            break;
        case 2:
            muxstr = getMultiplierString(std::sqrt(mux), noPrefix);
            if (isNumericalStartCharacter(muxstr.front())) {
                muxstr = getMultiplierString(mux, true);
            }
            break;
        case 3:
            muxstr = getMultiplierString(std::cbrt(mux), noPrefix);
            if (isNumericalStartCharacter(muxstr.front())) {
                muxstr = getMultiplierString(mux, true);
            }
            break;
        default:
            muxstr = getMultiplierString(mux, true);
    }
    return muxstr + seq;
}
// Add a unit power to a string
static void addUnitPower(std::string& str, const char* unit, int power)
{
    if (power != 0) {
        if (!str.empty() && str.back() != '/') {
            str.push_back('*');
        }
        str.append(unit);
        if (power != 1) {
            str.push_back('^');
            if (power < 0) {
                str.push_back('-');
                str.push_back('0' - power);
            } else {
                str.push_back('0' + power);
            }
        }
    }
}

// add the flag string to another unit string
static void addUnitFlagStrings(const precise_unit& un, std::string& unitString)
{
    if (un.base_units().has_i_flag()) {
        if (unitString.empty()) {
            unitString = "flag";
        } else {
            unitString.append("*flag");
        }
    }
    if (un.base_units().has_e_flag()) {
        if (unitString.empty()) {
            unitString = "eflag";
        } else {
            unitString.insert(0, "eflag*");
        }
    }
    if (un.base_units().is_per_unit()) {
        if (unitString.empty()) {
            unitString = "pu";
        } else {
            unitString.insert(0, "pu*");
        }
    }
}

/** add the unit power if it is positive, return true if negative and skip if 0
 */
static inline int addPosUnits(std::string& str, const char* unitName, int power)
{
    if (power > 0) {
        addUnitPower(str, unitName, power);
    }
    return (power < 0) ? 1 : 0;
}

/** add the unit power if it is negative, and skip if >= 0
 */
static inline void
    addNegUnits(std::string& str, const char* unitName, int power)
{
    if (power < 0) {
        addUnitPower(str, unitName, power);
    }
}

static std::string generateRawUnitString(const precise_unit& un)
{
    std::string val;
    auto bu = un.base_units();
    int cnt{0};
    cnt += addPosUnits(val, "m", bu.meter());
    cnt += addPosUnits(val, "kg", bu.kg());
    cnt += addPosUnits(val, "s", bu.second());
    cnt += addPosUnits(val, "A", bu.ampere());
    cnt += addPosUnits(val, "K", bu.kelvin());
    cnt += addPosUnits(val, "mol", bu.mole());
    cnt += addPosUnits(val, "cd", bu.candela());
    cnt += addPosUnits(val, "item", bu.count());
    cnt += addPosUnits(val, "$", bu.currency());
    cnt += addPosUnits(val, "rad", bu.radian());
    addUnitFlagStrings(un, val);
    if (cnt == 1) {
        val.push_back('/');
        addPosUnits(val, "m", -bu.meter());
        addPosUnits(val, "kg", -bu.kg());
        addPosUnits(val, "s", -bu.second());
        addPosUnits(val, "A", -bu.ampere());
        addPosUnits(val, "K", -bu.kelvin());
        addPosUnits(val, "mol", -bu.mole());
        addPosUnits(val, "cd", -bu.candela());
        addPosUnits(val, "item", -bu.count());
        addPosUnits(val, "$", -bu.currency());
        addPosUnits(val, "rad", -bu.radian());
    } else if (cnt > 1) {
        addNegUnits(val, "m", bu.meter());
        addNegUnits(val, "kg", bu.kg());
        addNegUnits(val, "s", bu.second());
        addNegUnits(val, "A", bu.ampere());
        addNegUnits(val, "K", bu.kelvin());
        addNegUnits(val, "mol", bu.mole());
        addNegUnits(val, "cd", bu.candela());
        addNegUnits(val, "item", bu.count());
        addNegUnits(val, "$", bu.currency());
        addNegUnits(val, "rad", bu.radian());
    }
    return val;
}

static std::atomic<bool> allowUserDefinedUnits{true};

void disableUserDefinedUnits()
{
    allowUserDefinedUnits.store(false);
}
void enableUserDefinedUnits()
{
    allowUserDefinedUnits.store(true);
}

using smap = std::unordered_map<std::string, precise_unit>;

/** convert a string into a double */
static double
    getDoubleFromString(const std::string& ustring, size_t* index) noexcept;

static std::unordered_map<unit, std::string> user_defined_unit_names;
static smap user_defined_units;

void addUserDefinedUnit(const std::string& name, const precise_unit& un)
{
    if (allowUserDefinedUnits.load(std::memory_order_acquire)) {
        user_defined_unit_names[unit_cast(un)] = name;
        user_defined_units[name] = un;
        allowUserDefinedUnits.store(
            allowUserDefinedUnits.load(std::memory_order_acquire),
            std::memory_order_release);
    }
}

void addUserDefinedInputUnit(const std::string& name, const precise_unit& un)
{
    if (allowUserDefinedUnits.load(std::memory_order_acquire)) {
        user_defined_units[name] = un;
        allowUserDefinedUnits.store(
            allowUserDefinedUnits.load(std::memory_order_acquire),
            std::memory_order_release);
    }
}

std::string definedUnitsFromFile(const std::string& filename) noexcept
{
    std::string output;
    try {
        std::ifstream infile(filename);
        if (!infile.is_open()) {
            output = "unable to read file " + filename + "\n";
            return output;
        }
        std::string line;
        while (std::getline(infile, line)) {
            auto commentloc = line.find_first_not_of(" \t\n");
            if (commentloc == std::string::npos || line[commentloc] == '#') {
                continue;
            }
            std::size_t esep{1};  // extra separation location to handle quotes
            if (line[commentloc] == '\"' || line[commentloc] == '\'') {
                bool notfound{true};
                while (notfound) {
                    esep =
                        line.find_first_of(line[commentloc], commentloc + esep);
                    if (esep == std::string::npos) {
                        esep = 1;
                        break;
                    }
                    if (line[esep - 1] != '\\') {
                        notfound = false;
                    } else {
                        // remove the escaped quote
                        line.erase(esep - 1, 1);
                    }
                    esep -= commentloc;
                }
            }
            auto sep = line.find_first_of(",;=", commentloc + esep);
            if (sep == std::string::npos) {
                output +=
                    line + " is not a valid user defined unit definition\n";
                continue;
            }
            if (sep == line.size() - 1) {
                output += line + " does not have any valid definitions\n";
            }
            int length{0};
            if (line[sep + 1] == '=' || line[sep + 1] == '>') {
                length = 1;
            }

            // get the new definition name
            std::string userdef = line.substr(commentloc, sep - commentloc);
            while (userdef.back() == ' ') {
                userdef.pop_back();
            }
            // remove quotes
            if ((userdef.front() == '\"' || userdef.front() == '\'') &&
                userdef.back() == userdef.front()) {
                userdef.pop_back();
                userdef.erase(userdef.begin());
            }
            if (userdef.empty()) {
                output += line + " does not specify a user string\n";
                continue;
            }
            // the unit string
            auto sloc = line.find_first_not_of(" \t", sep + length + 1);
            if (sloc == std::string::npos) {
                output += line + " does not specify a unit definition string\n";
                continue;
            }
            auto meas_string = line.substr(sloc);
            while (meas_string.back() == ' ') {
                meas_string.pop_back();
            }
            if ((meas_string.front() == '\"' || meas_string.front() == '\'') &&
                meas_string.back() == meas_string.front()) {
                meas_string.pop_back();
                meas_string.erase(meas_string.begin());
            }
            auto meas = measurement_from_string(meas_string);
            if (!is_valid(meas)) {
                output +=
                    line.substr(sloc) + " does not generate a valid unit\n";
                continue;
            }

            if (line[sep + length] == '>') {
                addUserDefinedInputUnit(userdef, meas.as_unit());
            } else {
                addUserDefinedUnit(userdef, meas.as_unit());
            }
        }
    }
    // LCOV_EXCL_START
    catch (const std::exception& e) {
        output += e.what();
        output.push_back('\n');
        // this is mainly just to catch any weird errors coming from somewhere
        // so this function can be noexcept
    }
    // LCOV_EXCL_STOP
    return output;
}

void clearUserDefinedUnits()
{
    user_defined_unit_names.clear();
    user_defined_units.clear();
}

// add escapes for some particular sequences
static void escapeString(std::string& str)
{
    auto fnd = str.find_first_of("{}[]()");
    while (fnd != std::string::npos) {
        if (fnd == 0 || str[fnd - 1] != '\\') {
            str.insert(fnd, 1, '\\');
            ++fnd;
        }
        fnd = str.find_first_of("{}[]()", fnd + 1);
    }
}
static void shorten_number(std::string& unit_string, size_t loc, size_t length)
{
    auto c = unit_string[loc];
    if (c == '.') {
        c = unit_string[loc + 1];
    }
    unit_string.erase(loc, length);
    if (c == '9') {
        if (unit_string[loc - 1] != '9') {
            ++unit_string[loc - 1];
        } else {
            int kk = 1;
            while (unit_string[loc - kk] == '9') {
                unit_string[loc - kk] = '0';
                if (loc - kk == 0) {
                    break;
                }
                ++kk;
            }
            if (loc - kk == 0 && unit_string[0] == '0') {
                unit_string.insert(unit_string.begin(), '1');
            } else {
                if (isDigitCharacter(unit_string[loc - kk])) {
                    ++unit_string[loc - kk];
                } else {
                    unit_string.insert(loc - kk + 1, 1, '1');
                }
            }
        }
    }
}

static void reduce_number_length(std::string& unit_string, char detect)
{
    static const std::string zstring("00000");
    static const std::string nstring("99999");
    const std::string& detseq(detect == '0' ? zstring : nstring);
    // search for a bunch of zeros in a row
    std::size_t indexingloc{0};

    auto zloc = unit_string.find(detseq);
    while (zloc != std::string::npos) {
        auto nloc = unit_string.find_first_not_of(detect, zloc + 5);
        indexingloc = zloc + 5;
        if (nloc != std::string::npos) {
            indexingloc = nloc + 1;
            if (unit_string[nloc] != '.') {
                if (!isDigitCharacter(unit_string[nloc]) ||
                    (unit_string.size() > nloc + 1 &&
                     !isDigitCharacter(unit_string[nloc + 1]))) {
                    if (isDigitCharacter(unit_string[nloc])) {
                        ++nloc;
                    }

                    auto dloc = unit_string.find_last_of('.', zloc);

                    if (dloc != std::string::npos && nloc - dloc > 12) {
                        bool valid = true;
                        if (dloc == zloc - 1) {
                            --zloc;
                            auto ploc = dloc;
                            valid = false;
                            while (true) {
                                if (ploc <= 0) {
                                    break;
                                }
                                --ploc;
                                if (!isDigitCharacter(unit_string[ploc])) {
                                    break;
                                }
                                if (unit_string[ploc] != '0') {
                                    valid = true;
                                    break;
                                }
                            }
                        } else {
                            auto ploc = dloc + 1;
                            while (ploc < zloc) {
                                if (!isDigitCharacter(unit_string[ploc])) {
                                    valid = false;
                                    break;
                                }
                                ++ploc;
                            }
                        }
                        if (valid) {
                            shorten_number(unit_string, zloc, nloc - zloc);
                            indexingloc = zloc + 1;
                        }
                    }
                }
            }
        } else if (detect != '9') {
            indexingloc = unit_string.size();
            auto dloc = unit_string.find_last_of('.', zloc);

            if (dloc != std::string::npos) {
                bool valid = true;
                if (dloc == zloc - 1) {
                    --zloc;
                    auto ploc = dloc;
                    valid = false;
                    while (true) {
                        if (ploc > 0) {
                            --ploc;
                            if (!isDigitCharacter(unit_string[ploc])) {
                                break;
                            }
                            if (unit_string[ploc] != '0') {
                                valid = true;
                                break;
                            }
                        }
                    }
                } else {
                    auto ploc = dloc + 1;
                    while (ploc < zloc) {
                        if (!isDigitCharacter(unit_string[ploc])) {
                            valid = false;
                            break;
                        }
                        ++ploc;
                    }
                }
                if (valid) {
                    shorten_number(unit_string, zloc, nloc - zloc);
                    indexingloc = zloc + 1;
                }
            }
        }
        zloc = unit_string.find(detseq, indexingloc);
    }
}
// clean up the unit string and add a commodity if necessary
static std::string
    clean_unit_string(std::string propUnitString, std::uint32_t commodity)
{
    using spair = std::tuple<const char*, const char*, int, int>;
    static UNITS_CPP14_CONSTEXPR_OBJECT std::array<spair, 10> powerseq{
        {spair{"Mm^3", "(1e9km^3)", 4, 8},  // this needs to happen before ^3^2
                                            // conversions
         spair{"^2^2", "^4", 4, 2},
         spair{"^3^2", "^6", 4, 2},
         spair{"^2^3", "^6", 4, 2},
         spair{"Gs", "Bs", 2, 2},
         spair{"*K^", "*1*K^", 3, 5},  // this one is to prevent the next from
                                       // screwing things up
         spair{"eflag*K", "degC", 7, 4},
         spair{"*1*", "*", 3, 1},
         spair{"*1/", "/", 3, 1},
         spair{"*/", "/", 2, 1}}};
    // run a few checks for unusual conditions
    for (auto& pseq : powerseq) {
        auto fnd = propUnitString.find(std::get<0>(pseq));
        while (fnd != std::string::npos) {
            propUnitString.replace(fnd, std::get<2>(pseq), std::get<1>(pseq));
            fnd =
                propUnitString.find(std::get<0>(pseq), fnd + std::get<3>(pseq));
        }
    }

    if (!propUnitString.empty()) {
        if (propUnitString.find("00000") != std::string::npos) {
            reduce_number_length(propUnitString, '0');
        }
        if (propUnitString.find("99999") != std::string::npos) {
            reduce_number_length(propUnitString, '9');
        }
    }

    // no more cleaning necessary
    if (commodity == 0 && !propUnitString.empty() &&
        !isDigitCharacter(propUnitString.front())) {
        return propUnitString;
    }

    if (commodity != 0) {
        std::string cString = getCommodityName(
            ((commodity & 0x80000000) == 0) ? commodity : (~commodity));
        if (cString.compare(0, 7, "CXCOMM[") != 0) {
            // add some escapes for problematic sequences
            escapeString(cString);
        }
        // make it look like a commodity sequence
        cString.insert(cString.begin(), '{');
        cString.push_back('}');
        if ((commodity & 0x80000000) == 0) {
            auto loc = propUnitString.find_last_of("/^");
            if (loc == std::string::npos) {
                propUnitString += cString;
            } else if (propUnitString.compare(0, 2, "1/") == 0) {
                auto rs = checkForCustomUnit(cString);
                if (!is_error(rs)) {
                    cString.insert(0, 1, '1');
                }
                propUnitString.replace(0, 1, cString.c_str());
            } else {
                auto locp = propUnitString.find_first_of("^*/");
                if (propUnitString[locp] != '^') {
                    propUnitString.insert(locp, cString);
                } else if (propUnitString[locp + 1] != '-') {
                    propUnitString.insert(locp, cString);
                } else {
                    auto rs = checkForCustomUnit(cString);
                    if (!is_error(rs)) {
                        // this condition would take a very particular and odd
                        // string to trigger I haven't figured out a test case
                        // for it yet

                        // LCOV_EXCL_START
                        cString.insert(0, 1, '1');
                        // LCOV_EXCL_STOP
                    }
                    propUnitString = cString + "*" + propUnitString;
                }
            }
        } else {  // inverse commodity
            auto loc = propUnitString.find_last_of('/');
            if (loc == std::string::npos) {
                auto rs = checkForCustomUnit(cString);
                if (!is_error(rs)) {  // this check is needed because it is
                                      // possible to define a commodity that
                                      // would look like a form
                    // of custom unit
                    // The '1' forces the interpreter to interpret it as purely
                    // a commodity, but is only needed in very particular
                    // circumstances
                    cString.insert(0, 1, '1');
                }
                if (propUnitString.empty()) {
                    propUnitString.push_back('1');
                }
                propUnitString.push_back('/');
                propUnitString.append(cString);
            } else {
                auto locp = propUnitString.find_last_of("^*");
                if (locp == std::string::npos) {
                    propUnitString.append(cString);
                } else if (locp < loc) {
                    propUnitString.append(cString);
                } else {
                    propUnitString.insert(locp, cString);
                }
            }
        }
    }
    return propUnitString;
}

static const std::pair<const unit, std::string> nullret{invalid, std::string{}};

static std::pair<unit, std::string> find_unit_pair(unit un)
{
    if (allowUserDefinedUnits.load(std::memory_order_acquire)) {
        if (!user_defined_unit_names.empty()) {
            auto fndud = user_defined_unit_names.find(un);
            if (fndud != user_defined_unit_names.end()) {
                return {fndud->first, fndud->second};
            }
        }
    }
    auto fnd = base_unit_names.find(un);
    if (fnd != base_unit_names.end()) {
        return {fnd->first, fnd->second};
    }
    return nullret;
}

static std::string find_unit(unit un)
{
    if (allowUserDefinedUnits.load(std::memory_order_acquire)) {
        if (!user_defined_unit_names.empty()) {
            auto fndud = user_defined_unit_names.find(un);
            if (fndud != user_defined_unit_names.end()) {
                return fndud->second;
            }
        }
    }
    auto fnd = base_unit_names.find(un);
    if (fnd != base_unit_names.end()) {
        return fnd->second;
    }
    return std::string{};
}
static std::string
    to_string_internal(precise_unit un, std::uint32_t match_flags)
{
    switch (std::fpclassify(un.multiplier())) {
        case FP_INFINITE: {
            std::string inf = (un.multiplier() > 0.0) ? "INF" : "-INF";
            un = precise_unit(un.base_units(), 1.0);
            if (un == precise::one) {
                return inf;
            }
            return inf + '*' + to_string_internal(un, match_flags);
        }
        case FP_NAN:
            un = precise_unit(un.base_units(), 1.0);
            if (is_error(un)) {
                return "NaN*ERROR";
            }
            if (un == precise::one) {
                return "NaN";
            }
            return "NaN*" + to_string_internal(un, match_flags);
        case FP_SUBNORMAL:
        case FP_ZERO:
            // either denormal or 0.0 in either case close enough to 0
            un = precise_unit(un.base_units(), 1.0);
            if (un == precise::one) {
                return "0";
            }
            return "0*" + to_string_internal(un, match_flags);
        case FP_NORMAL:
        default:
            break;
    }

    auto llunit = unit_cast(un);
    // deal with situations where the cast unit is not normal but the precise
    // one is
    if (std::fpclassify(llunit.multiplier_f()) != FP_NORMAL) {
        auto mstring = getMultiplierString(un.multiplier(), true);
        un = precise_unit(un.base_units(), 1.0);
        mstring.push_back('*');

        mstring.append(to_string_internal(un, match_flags));
        if (mstring.back() == '*') {
            mstring.pop_back();
        }
        return mstring;
    }
    auto fnd = find_unit(llunit);
    if (!fnd.empty()) {
        return fnd;
    }

    // lets try inverting it
    fnd = find_unit(llunit.inv());
    if (!fnd.empty()) {
        return std::string("1/") + fnd;
    }
    if (un.base_units().empty()) {
        auto mstring = getMultiplierString(un.multiplier(), true);
        un = precise_unit(un.base_units(), 1.0);
        if (un == precise::one) {
            return mstring;
        }
        if (!mstring.empty()) {
            mstring.push_back('*');
        }
        fnd = find_unit(unit_cast(un));
        if (!fnd.empty()) {
            return mstring + fnd;
        }
        addUnitFlagStrings(un, fnd);
        return mstring + fnd;
    }
    /// Check for squared units
    if (!un.base_units().root(2).has_e_flag() &&
        !un.base_units().has_i_flag() && un.multiplier() > 0.0) {
        auto squ = root(llunit, 2);
        auto fndp = find_unit_pair(squ);
        if (!fndp.second.empty()) {
            if (fndp.first.pow(2) != llunit) {
                // this is symmetric to the other sections where we have test
                // cases for no known test cases of this but could be triggered
                // by particular numeric strings

                // LCOV_EXCL_START
                return getMultiplierString(
                           (llunit / fndp.first.pow(2)).multiplier(), true) +
                    '*' + fndp.second + "^2";
                // LCOV_EXCL_STOP
            }
            return fndp.second + "^2";
        }
        auto fndpi = find_unit_pair(squ.inv());
        if (!fndpi.second.empty()) {
            if (fndpi.first.pow(2) != llunit.inv()) {
                return getMultiplierString(
                           1.0 /
                               (llunit.inv() / fndpi.first.pow(2)).multiplier(),
                           true) +
                    '/' + fndpi.second + "^2";
            }
            return std::string("1/") + fndpi.second + "^2";
        }
    }
    /// Check for cubed units
    if (!un.base_units().root(3).has_e_flag() &&
        !un.base_units().has_i_flag()) {
        auto cub = root(llunit, 3);
        fnd = find_unit(cub);
        if (!fnd.empty()) {
            return fnd + "^3";
        }
        fnd = find_unit(cub.inv());
        if (!fnd.empty()) {
            return std::string("1/") + fnd + "^3";
        }
    }

    if (un.is_equation()) {
        auto ubase = un.base_units();
        int num = precise::custom::eq_type(ubase);
        std::string cxstr = "EQXUN[" + std::to_string(num) + "]";

        auto urem = un / precise_unit(precise::custom::equation_unit(num));
        urem.clear_flags();
        urem.commodity(0);
        if (urem.multiplier() != 1.0) {
            auto ucc = unit_cast(urem);
            auto fndp = find_unit_pair(ucc);
            if (!fndp.second.empty()) {
                if (ucc.is_exactly_the_same(fndp.first)) {
                    return fndp.second + '*' + cxstr;
                }
            }

            // Equation units can amplify slight numerical differences
            // so numbers must be exact
            auto mult = getMultiplierString(urem.multiplier(), false);
            if (mult.size() > 5 && isNumericalStartCharacter(mult[0])) {
                urem = precise_unit(urem.base_units(), 1.0);
                if (!urem.base_units().empty()) {
                    return mult + '*' + to_string_internal(urem, match_flags) +
                        '*' + cxstr;
                }
                return mult + '*' + cxstr;
            }
        }
        if (!urem.base_units().empty() || urem.multiplier() != 1.0) {
            return to_string_internal(urem, match_flags) + '*' + cxstr;
        }
        return cxstr;
    }
    // check if it is a custom unit of some kind
    if (precise::custom::is_custom_unit(un.base_units())) {
        auto ubase = un.base_units();
        int num = precise::custom::custom_unit_number(ubase);
        std::string cxstr = "CXUN[" + std::to_string(num) + "]";
        auto urem = un;
        if (precise::custom::is_custom_unit_inverted(ubase)) {
            urem = un * precise::generate_custom_unit(num);
            cxstr.append("^-1");
        } else {
            urem = un / precise::generate_custom_unit(num);
        }
        urem.clear_flags();
        urem.commodity(0);
        if ((urem.multiplier() != 1.0) || (!urem.base_units().empty())) {
            return to_string_internal(urem, match_flags) + '*' + cxstr;
        }
        return cxstr;
    }
    // check for custom count units
    if (precise::custom::is_custom_count_unit(un.base_units())) {
        auto ubase = un.base_units();
        int num = precise::custom::custom_count_unit_number(ubase);
        std::string cxstr = "CXCUN[" + std::to_string(num) + "]";
        auto urem = un;
        if (precise::custom::is_custom_count_unit_inverted(ubase)) {
            urem = un * precise::generate_custom_count_unit(num);
            cxstr.append("^-1");
        } else {
            urem = un / precise::generate_custom_count_unit(num);
        }
        urem.clear_flags();
        urem.commodity(0);
        if ((urem.multiplier() != 1.0) || (!urem.base_units().empty())) {
            return to_string_internal(urem, match_flags) + '*' + cxstr;
        }
        return cxstr;
    }
    /** check for a few units with odd numbers that allow SI prefixes*/

    if (un.unit_type_count() == 1) {
        return generateUnitSequence(un.multiplier(), generateRawUnitString(un));
    }
    if (un.unit_type_count() == 2 && un.multiplier() == 1) {
        return generateUnitSequence(1.0, generateRawUnitString(un));
    }
    /** check for a few units with odd numbers that allow SI prefixes*/
    for (auto& siU : siTestUnits) {
        auto nu = un / siU.first;
        if (nu.unit_type_count() == 0) {
            auto mult = getMultiplierString(nu.multiplier());
            if (mult.empty()) {
                std::string rstring{siU.second};
                addUnitFlagStrings(nu, rstring);
                return rstring;
            }
            if (!isNumericalStartCharacter(mult.front())) {
                std::string rstring = mult + siU.second;
                addUnitFlagStrings(nu, rstring);
                return rstring;
            }
        }
        if (nu.unit_type_count() == 1) {
            auto mult = getMultiplierString(nu.multiplier());
            if (mult.empty()) {
                std::string rstring{siU.second};
                rstring.push_back('*');
                rstring.append(to_string_internal(nu, match_flags));
                return rstring;
            }

            if (!isNumericalStartCharacter(mult.front())) {
                nu = precise_unit{nu.base_units(), 1.0};
                std::string rstring = mult + siU.second;
                rstring.push_back('*');
                rstring.append(to_string_internal(nu, match_flags));
                return rstring;
            }
        }
        nu = un * siU.first;
        if (nu.unit_type_count() == 0) {
            auto mult = getMultiplierString(1.0 / nu.multiplier());
            if (mult.empty()) {
                std::string rstring;
                addUnitFlagStrings(nu, rstring);
                if (rstring.empty()) {
                    rstring.push_back('1');
                }
                rstring.push_back('/');
                rstring.append(siU.second);
                return rstring;
            }
            if (!isNumericalStartCharacter(mult.front())) {
                std::string rstring;
                addUnitFlagStrings(nu, rstring);
                if (rstring.empty()) {
                    rstring.push_back('1');
                }
                rstring.push_back('/');
                rstring.append(mult + siU.second);
                return rstring;
            }
        }
        if (nu.unit_type_count() == 1) {
            auto mult = getMultiplierString(1.0 / nu.multiplier());
            if (mult.empty()) {
                std::string rstring{to_string_internal(nu, match_flags)};
                rstring.push_back('/');
                rstring.append(siU.second);
                return rstring;
            }

            if (!isNumericalStartCharacter(mult.front())) {
                nu = precise_unit{nu.base_units(), 1.0};
                std::string rstring{to_string_internal(nu, match_flags)};
                rstring.push_back('/');
                rstring.append(mult);
                rstring.append(siU.second);
                return rstring;
            }
        }
    }
    // lets try converting to pure base unit
    auto bunit = unit(un.base_units());
    fnd = find_unit(bunit);
    if (!fnd.empty()) {
        return generateUnitSequence(un.multiplier(), fnd);
    }
    // let's try inverting the pure base unit
    fnd = find_unit(bunit.inv());
    if (!fnd.empty()) {
        auto prefix = generateUnitSequence(1.0 / un.multiplier(), fnd);
        if (isNumericalStartCharacter(prefix.front())) {
            size_t cut;
            double mx = getDoubleFromString(prefix, &cut);
            return getMultiplierString(1.0 / mx, true) + "/" +
                prefix.substr(cut);
        }
        return std::string("1/") + prefix;
    }

    // let's try common divisor units
    for (auto& tu : testUnits) {
        auto ext = un * tu.first;
        fnd = find_unit(unit_cast(ext));
        if (!fnd.empty()) {
            return fnd + '/' + tu.second;
        }
    }

    // let's try common multiplier units
    for (auto& tu : testUnits) {
        auto ext = un / tu.first;
        fnd = find_unit(unit_cast(ext));
        if (!fnd.empty()) {
            return fnd + '*' + tu.second;
        }
    }
    // let's try common divisor with inv units
    for (auto& tu : testUnits) {
        auto ext = un / tu.first;
        fnd = find_unit(unit_cast(ext.inv()));
        if (!fnd.empty()) {
            return std::string(tu.second) + '/' + fnd;
        }
    }
    // let's try inverse of common multiplier units
    for (auto& tu : testUnits) {
        auto ext = un * tu.first;
        fnd = find_unit(unit_cast(ext.inv()));
        if (!fnd.empty()) {
            return std::string("1/(") + fnd + '*' + tu.second + ')';
        }
    }

    std::string beststr;
    // let's try common divisor units on base units
    for (auto& tu : testUnits) {
        auto ext = un * tu.first;
        auto base = unit(ext.base_units());
        fnd = find_unit(base);
        if (!fnd.empty()) {
            auto prefix = generateUnitSequence(ext.multiplier(), fnd);

            auto str = prefix + '/' + tu.second;
            if (!isNumericalStartCharacter(str.front())) {
                return str;
            }
            if (beststr.empty() || str.size() < beststr.size()) {
                beststr = str;
            }
        }
    }

    // let's try common multiplier units on base units
    for (auto& tu : testUnits) {
        auto ext = un / tu.first;
        auto base = unit(ext.base_units());
        fnd = find_unit(base);
        if (!fnd.empty()) {
            auto prefix = generateUnitSequence(ext.multiplier(), fnd);
            auto str = prefix + '*' + tu.second;
            if (!isNumericalStartCharacter(str.front())) {
                return str;
            }
            if (beststr.empty() || str.size() < beststr.size()) {
                beststr = str;
            }
        }
    }
    // let's try common divisor with inv units on base units
    for (auto& tu : testUnits) {
        auto ext = un / tu.first;
        auto base = unit(ext.base_units());

        fnd = find_unit(base.inv());
        if (!fnd.empty()) {
            auto prefix = generateUnitSequence(1.0 / ext.multiplier(), fnd);
            if (isNumericalStartCharacter(prefix.front())) {
                size_t cut;
                double mx = getDoubleFromString(prefix, &cut);

                auto str = getMultiplierString(1.0 / mx, true) + tu.second +
                    "/" + prefix.substr(cut);
                if (beststr.empty() || str.size() < beststr.size()) {
                    beststr = str;
                }

            } else {
                return std::string(tu.second) + "/" + prefix;
            }
        }
    }
    // let's try inverse of common multiplier units on base units
    for (auto& tu : testUnits) {
        auto ext = un * tu.first;
        auto base = unit(ext.base_units());
        fnd = find_unit(base.inv());
        if (!fnd.empty()) {
            auto prefix = getMultiplierString(
                1.0 / ext.multiplier(), isDigitCharacter(fnd.back()));
            std::string str{"1/("};
            str += prefix;
            str += fnd;
            str.push_back('*');
            str.append(tu.second);
            str.push_back(')');
            if (!isNumericalStartCharacter(prefix.front())) {
                return str;
            }
            if (beststr.empty() || str.size() < beststr.size()) {
                beststr = std::move(str);
            }
        }
    }

    // now just to reduce the order and generate the string
    if (!beststr.empty()) {
        return beststr;
    }
    auto minorder = order(llunit);
    auto mino_unit = un;
    std::string min_mult;
    if (minorder > 3) {
        for (auto& reduce : creduceUnits) {
            auto od = 1 + order(unit_cast(un * reduce.first));
            if (od < minorder) {
                minorder = od;
                mino_unit = un * reduce.first;
                min_mult = reduce.second;
            }
        }
    }
    return generateUnitSequence(
        mino_unit.multiplier(), min_mult + generateRawUnitString(mino_unit));
}

std::string to_string(const precise_unit& un, std::uint32_t match_flags)
{
    return clean_unit_string(
        to_string_internal(un, match_flags), un.commodity());
}

std::string
    to_string(const precise_measurement& measure, std::uint32_t match_flags)
{
    std::stringstream ss;
    ss.precision(12);
    ss << measure.value();
    ss << ' ';
    auto str = to_string(measure.units(), match_flags);
    if (isNumericalStartCharacter(str.front())) {
        str.insert(str.begin(), '(');
        str.push_back(')');
    }
    ss << str;
    return ss.str();
}

std::string to_string(const measurement& measure, std::uint32_t match_flags)
{
    std::stringstream ss;
    ss.precision(6);
    ss << measure.value();
    ss << ' ';
    auto str = to_string(measure.units(), match_flags);
    if (isNumericalStartCharacter(str.front())) {
        str.insert(str.begin(), '(');
        str.push_back(')');
    }
    ss << str;
    return ss.str();
}

std::string
    to_string(const uncertain_measurement& measure, std::uint32_t match_flags)
{
    // TODO(PT) this should really follow more appropriate rules for digits of
    // precision
    std::stringstream ss;
    ss.precision(6);
    ss << measure.value_f();
    ss << "+/-";
    ss << measure.uncertainty_f() << ' ';
    ss << to_string(measure.units(), match_flags);
    return ss.str();
}

/// Generate the prefix multiplier for SI units
static double getPrefixMultiplier(char p)
{
    switch (p) {
        case 'm':
            return 0.001;
        case 'k':
        case 'K':
            return 1000.0;
        case 'M':
            return 1e6;
        case 'u':
        case 'U':
        case '\xB5':  // latin-1 encoding "micro"
            return 1e-6;
        case 'd':
        case 'D':
            return 0.1;
        case 'c':
        case 'C':
            return 0.01;
        case 'h':
        case 'H':
            return 100.0;
        case 'n':
            return 1e-9;
        case 'p':
            return 1e-12;
        case 'G':
        case 'B':  // Billion
            return 1e9;
        case 'T':
            return 1e12;
        case 'f':
        case 'F':
            return 1e-15;
        case 'E':
            return 1e18;
        case 'P':
            return 1e15;
        case 'Z':
            return 1e21;
        case 'Y':
            return 1e24;
        case 'a':
        case 'A':
            return 1e-18;
        case 'z':
            return 1e-21;
        case 'y':
            return 1e-24;
        default:
            return 0.0;
    }
}

static constexpr uint16_t charindex(char ch1, char ch2)
{
    return ch1 * 256 + ch2;
}

/// Generate the prefix multiplier for SI units and binary prefixes
static double getPrefixMultiplier2Char(char c1, char c2)
{
    using cpair = std::pair<uint16_t, double>;
    static UNITS_CPP14_CONSTEXPR_OBJECT std::array<cpair, 23> char2prefix{{
        cpair{charindex('D', 'A'), 10.0},
        cpair{charindex('E', 'X'), 1e18},
        cpair{charindex('E', 'i'),
              1024.0 * 1024.0 * 1024.0 * 1024.0 * 1024.0 * 1024.0},
        cpair{charindex('G', 'A'), 1e9},
        cpair{charindex('G', 'i'), 1024.0 * 1024.0 * 1024.0},
        cpair{charindex('K', 'i'), 1024.0},
        cpair{charindex('M', 'A'), 1e6},
        cpair{charindex('M', 'M'), 1e6},
        cpair{charindex('M', 'i'), 1024.0 * 1024.0},
        cpair{charindex('P', 'T'), 1e15},
        cpair{charindex('P', 'i'), 1024.0 * 1024.0 * 1024.0 * 1024.0 * 1024.0},
        cpair{charindex('T', 'R'), 1e15},
        cpair{charindex('T', 'i'), 1024.0 * 1024.0 * 1024.0 * 1024.0},
        cpair{charindex('Y', 'A'), 1e24},
        cpair{charindex('Y', 'O'), 1e-24},
        cpair{charindex('Y', 'i'),
              1024.0 * 1024.0 * 1024.0 * 1024.0 * 1024.0 * 1024.0 * 1024.0 *
                  1024.0},
        cpair{charindex('Z', 'A'), 1e21},
        cpair{charindex('Z', 'O'), 1e-21},
        cpair{charindex('Z', 'i'),
              1024.0 * 1024.0 * 1024.0 * 1024.0 * 1024.0 * 1024.0 * 1024.0},
        cpair{charindex('d', 'a'), 10.0},
        cpair{charindex('m', 'A'), 1e6},
        cpair{charindex('m', 'c'), 1e-6},
        cpair{charindex('p', 'T'), 1e15},
    }};
    auto code = charindex(c1, c2);
    auto fnd = std::lower_bound(
        char2prefix.begin(),
        char2prefix.end(),
        cpair{code, 0.0},
        [](const cpair& p, const cpair& test) {
            return (p.first < test.first);
        });

    if (fnd != char2prefix.end() && fnd->first == code) {
        return fnd->second;
    }
    return 0.0;
}

// do a segment check in the forward direction
static bool
    segmentcheck(const std::string& unit, char closeSegment, size_t& index);

/** generate a number representing the leading portion of a string
the index of the first non-converted character is returned in index*/
static double
    generateLeadingNumber(const std::string& ustring, size_t& index) noexcept;

/** generate a number representing the leading portion of a string if the words
are numerical in nature the index of the first non-converted character is
returned in index*/
static double readNumericalWords(const std::string& ustring, size_t& index);

// Detect if a string looks like a number
static bool looksLikeNumber(const std::string& string, size_t index = 0);

/** a function very similar to stod that uses the lower level strtod and does
 * things a little smarter for our case
 */
static double
    getDoubleFromString(const std::string& ustring, size_t* index) noexcept
{
    char* retloc = nullptr;
    auto vld = strtold(ustring.c_str(), &retloc);
    // LCOV_EXCL_START
    if (retloc == nullptr) {
        // to the best of my knowledge this should not happen but this is a
        // weird function sometimes with a lot of platform variations
        *index = 0;
        return constants::invalid_conversion;
    }
    // LCOV_EXCL_STOP
    *index = (retloc - ustring.c_str());
    // so if it converted anything then we can probably use that value if not
    // return NaN
    if (*index == 0) {
        return constants::invalid_conversion;
    }

    if (vld > static_cast<long double>(std::numeric_limits<double>::max())) {
        return constants::infinity;
    }
    if (vld < static_cast<long double>(-std::numeric_limits<double>::max())) {
        return -constants::infinity;
    }
    // floating point min gives you the smallest representable positive value
    if (std::fabs(vld) <
        static_cast<long double>(std::numeric_limits<double>::min())) {
        return 0.0;
    }
    return static_cast<double>(vld);
}

/** generate a value from a single numerical block */
static double getNumberBlock(const std::string& ustring, size_t& index) noexcept
{
    double val;
    if (ustring.front() == '(') {
        size_t ival = 1;
        if (segmentcheck(ustring, ')', ival)) {
            if (ival == 2) {
                index = ival;
                return 1.0;
            }
            bool hasOp = false;
            for (size_t ii = 1; ii < ival - 1; ++ii) {
                auto c = ustring[ii];
                if (c >= '0' && c <= '9') {
                    continue;
                }
                switch (c) {
                    case '-':
                    case '.':
                    case 'e':
                        break;
                    case '*':
                    case '/':
                    case '^':
                    case '(':
                    case ')':
                        hasOp = true;
                        break;
                    default:
                        return constants::invalid_conversion;
                }
            }
            auto substr = ustring.substr(1, ival - 2);
            size_t ind;
            if (hasOp) {
                val = generateLeadingNumber(substr, ind);
            } else {
                val = getDoubleFromString(substr, &ind);
            }
            if (ind < substr.size()) {
                return constants::invalid_conversion;
            }
            index = ival;
        } else {
            return constants::invalid_conversion;
        }
    } else {
        val = getDoubleFromString(ustring, &index);
    }
    if (!std::isnan(val) && index < ustring.size()) {
        if (ustring[index] == '^') {
            size_t nindex{0};
            double pval = getNumberBlock(ustring.substr(index + 1), nindex);
            if (!std::isnan(pval)) {
                index += nindex + 1;
                return std::pow(val, pval);
            }
            index = 0;
            return constants::invalid_conversion;
        }
    }
    return val;
}

double generateLeadingNumber(const std::string& ustring, size_t& index) noexcept
{
    index = 0;
    double val = getNumberBlock(ustring, index);
    if (std::isnan(val)) {
        return val;
    }
    while (true) {
        if (index >= ustring.size()) {
            return val;
        }
        switch (ustring[index]) {
            case '.':
            case '-':
            case '+':
                return constants::invalid_conversion;
            case '/':
            case '*':
            case 'x':
                if (looksLikeNumber(ustring, index + 1) ||
                    ustring[index + 1] == '(') {
                    size_t oindex{0};
                    double res =
                        getNumberBlock(ustring.substr(index + 1), oindex);
                    if (!std::isnan(res)) {
                        if (ustring[index] == '/') {
                            val /= res;
                        } else {
                            val *= res;
                        }

                        index = oindex + index + 1;
                    } else {
                        return val;
                    }
                } else {
                    return val;
                }
                break;
            case '(': {
                size_t oindex{0};
                double res = getNumberBlock(ustring.substr(index), oindex);
                if (!std::isnan(res)) {
                    val *= res;
                    index = oindex + index + 1;
                } else {
                    return val;
                }
                break;
            }
            default:
                return val;
        }
    }
}

// this string contains the first two letters of supported numerical words
// static const std::string first_two =
//    "on tw th fo fi si se ei ni te el hu mi bi tr ze";

static bool hasValidNumericalWordStart(const std::string& ustring)
{  // do a check if the first and second letters make sense
    static const std::string first_letters = "otfsenhmbtzaOTFSENHMBTZA";
    static const std::string second_letters = "nwhoielurNWHOIELUR";
    return (
        first_letters.find_first_of(ustring[0]) != std::string::npos &&
        second_letters.find_first_of(ustring[1]) != std::string::npos);
}
using wordpair = std::tuple<const char*, double, int>;

static UNITS_CPP14_CONSTEXPR_OBJECT std::array<wordpair, 9> lt10{
    {wordpair{"one", 1.0, 3},
     wordpair{"two", 2.0, 3},
     wordpair{"three", 3.0, 5},
     wordpair{"four", 4.0, 4},
     wordpair{"five", 5.0, 4},
     wordpair{"six", 6.0, 3},
     wordpair{"seven", 7.0, 5},
     wordpair{"eight", 8.0, 5},
     wordpair{"nine", 9.0, 4}}};

static double read1To10(const std::string& str, size_t& index)
{
    for (auto& num : lt10) {
        if (str.compare(index, std::get<2>(num), std::get<0>(num)) == 0) {
            index += std::get<2>(num);
            return std::get<1>(num);
        }
    }
    return constants::invalid_conversion;
}

static UNITS_CPP14_CONSTEXPR_OBJECT std::array<wordpair, 11> teens{
    {wordpair{"ten", 10.0, 3},
     wordpair{"eleven", 11.0, 6},
     wordpair{"twelve", 12.0, 6},
     wordpair{"thirteen", 13.0, 8},
     wordpair{"fourteen", 14.0, 8},
     wordpair{"fifteen", 15.0, 7},
     wordpair{"sixteen", 16.0, 7},
     wordpair{"seventeen", 17.0, 9},
     wordpair{"eighteen", 18.0, 8},
     wordpair{"nineteen", 19.0, 8},
     wordpair{"zero", 0.0, 4}}};

static double readTeens(const std::string& str, size_t& index)
{
    for (auto& num : teens) {
        if (str.compare(index, std::get<2>(num), std::get<0>(num)) == 0) {
            index += std::get<2>(num);
            return std::get<1>(num);
        }
    }
    return constants::invalid_conversion;
}

// NOTE: the ordering is important here
static UNITS_CPP14_CONSTEXPR_OBJECT std::array<wordpair, 5> groupNumericalWords{
    {wordpair{"trillion", 1e12, 8},
     wordpair{"billion", 1e9, 7},
     wordpair{"million", 1e6, 7},
     wordpair{"thousand", 1e3, 8},
     wordpair{"hundred", 100.0, 7}}};

static UNITS_CPP14_CONSTEXPR_OBJECT std::array<wordpair, 8> decadeWords{
    {wordpair{"twenty", 20.0, 6},
     wordpair{"thirty", 30.0, 6},
     wordpair{"forty", 40.0, 5},
     wordpair{"fifty", 50.0, 5},
     wordpair{"sixty", 60.0, 5},
     wordpair{"seventy", 70.0, 7},
     wordpair{"eighty", 80.0, 6},
     wordpair{"ninety", 90.0, 6}}};

static double readNumericalWords(const std::string& ustring, size_t& index)
{
    double val = constants::invalid_conversion;
    index = 0;
    if (ustring.size() < 3) {
        return val;
    }
    if (!hasValidNumericalWordStart(ustring)) {
        return val;
    }
    std::string lcstring = ustring;
    // make the string lower case for consistency
    std::transform(
        lcstring.begin(), lcstring.end(), lcstring.begin(), ::tolower);
    for (auto& wp : groupNumericalWords) {
        auto loc = lcstring.find(std::get<0>(wp));
        if (loc != std::string::npos) {
            if (loc == 0) {
                size_t index_sub{0};
                val = std::get<1>(wp);
                index = std::get<2>(wp);
                if (index < lcstring.size()) {
                    double val_p2 =
                        readNumericalWords(lcstring.substr(index), index_sub);
                    if (!std::isnan(val_p2)) {
                        if (val_p2 >= val) {
                            val = val * val_p2;
                        } else {
                            val += val_p2;
                        }

                        index += index_sub;
                    }
                }
                return val;
            }
            size_t index_sub{0};
            val = std::get<1>(wp);
            index = loc + std::get<2>(wp);
            // read the next component
            double val_add{0.0};
            if (index < lcstring.size()) {
                val_add = readNumericalWords(lcstring.substr(index), index_sub);
                if (!std::isnan(val_add)) {
                    if (val_add >= val) {
                        val = val * val_add;
                        val_add = 0.0;
                    }
                    index += index_sub;
                } else {
                    val_add = 0.0;
                }
            }
            // read the previous part
            double val_p2 =
                readNumericalWords(lcstring.substr(0, loc), index_sub);
            if (std::isnan(val_p2) || index_sub < loc) {
                index = index_sub;
                return val_p2;
            }
            val *= val_p2;
            val += val_add;
            return val;
        }
    }
    // clean up "and"
    if (lcstring.compare(0, 3, "and") == 0) {
        index += 3;
    }
    // what we are left with is values below a hundred
    for (auto& wp : decadeWords) {
        if (lcstring.compare(index, std::get<2>(wp), std::get<0>(wp)) == 0) {
            val = std::get<1>(wp);
            index += std::get<2>(wp);
            if (lcstring.size() > index) {
                if (lcstring[index] == '-') {
                    ++index;
                }
                double toTen = read1To10(lcstring, index);
                if (!std::isnan(toTen)) {
                    val += toTen;
                }
            }
            return val;
        }
    }
    val = readTeens(lcstring, index);
    if (!std::isnan(val)) {
        return val;
    }
    val = read1To10(lcstring, index);
    return val;
}

#ifdef ENABLE_UNIT_TESTING
namespace detail {
    namespace testing {
        // generate a number from a number sequence
        double testLeadingNumber(const std::string& test, size_t& index)
        {
            return generateLeadingNumber(test, index);
        }
        double testNumericalWords(const std::string& test, size_t& index)
        {
            return readNumericalWords(test, index);
        }

        std::string
            testUnitSequenceGeneration(double mul, const std::string& test)
        {
            return generateUnitSequence(mul, test);
        }
        std::string
            testCleanUpString(std::string testString, std::uint32_t commodity)
        {
            return clean_unit_string(std::move(testString), commodity);
        }
    }  // namespace testing
}  // namespace detail

#endif

/** Words of SI prefixes
https://physics.nist.gov/cuu/Units/prefixes.html
https://physics.nist.gov/cuu/Units/binary.html
*/
using utup = std::tuple<const char*, double, int>;
static UNITS_CPP14_CONSTEXPR_OBJECT std::array<utup, 29> prefixWords{{
    utup{"atto", 1e-18, 4},
    utup{"centi", 0.01, 5},
    utup{"deca", 10.0, 4},
    utup{"deci", 0.1, 4},
    utup{"deka", 10.0, 4},
    utup{"exa", 1e18, 3},
    utup{"exbi", 1024.0 * 1024.0 * 1024 * 1024.0 * 1024.0 * 1024.0, 4},
    utup{"femto", 1e-15, 5},
    utup{"gibi", 1024.0 * 1024.0 * 1024, 4},
    utup{"giga", 1e9, 4},
    utup{"hecto", 100.0, 5},
    utup{"kibi", 1024.0, 4},
    utup{"kilo", 1000.0, 4},
    utup{"mebi", 1024.0 * 1024.0, 4},
    utup{"mega", 1e6, 4},
    utup{"micro", 1e-6, 5},
    utup{"milli", 1e-3, 5},
    utup{"nano", 1e-9, 4},
    utup{"pebi", 1024.0 * 1024.0 * 1024 * 1024.0 * 1024.0, 4},
    utup{"peta", 1e15, 4},
    utup{"pico", 1e-12, 4},
    utup{"tebi", 1024.0 * 1024.0 * 1024 * 1024.0, 4},
    utup{"tera", 1e12, 4},
    utup{"yocto", 1e-24, 5},
    utup{"yotta", 1e24, 4},
    utup{"zepto", 1e-21, 5},
    utup{"zetta", 1e21, 5},
    utup{"zebi", 1024.0 * 1024.0 * 1024 * 1024.0 * 1024.0 * 1024.0 * 1024.0, 4},
    utup{"yobi",
         1024.0 * 1024.0 * 1024 * 1024.0 * 1024.0 * 1024.0 * 1024.0 * 1024.0,
         4},
}};

bool clearEmptySegments(std::string& unit)
{
    static const std::array<std::string, 4> Esegs{{"()", "[]", "{}", "<>"}};
    bool changed = false;
    for (auto& seg : Esegs) {
        auto fnd = unit.find(seg);
        while (fnd != std::string::npos) {
            if (fnd > 0 && unit[fnd - 1] == '\\') {
                fnd = unit.find(seg, fnd + 2);
                continue;
            }
            unit.erase(fnd, seg.size());
            changed = true;
            fnd = unit.find(seg, fnd + 1);
        }
    }
    return changed;
}
// forward declaration of this function
static precise_unit
    get_unit(const std::string& unit_string, std::uint32_t match_flags);

inline bool ends_with(std::string const& value, std::string const& ending)
{
    auto esize = ending.size();
    auto vsize = value.size();
    return (vsize > esize) ?
        (value.compare(vsize - esize, esize, ending) == 0) :
        false;
}

enum class modifier : int {
    start_tail = 0,
    start_replace = 1,
    anywhere_tail = 2,
    anywhere_replace = 3,
    tail_replace = 4,
};
using modSeq = std::tuple<const char*, const char*, size_t, modifier>;
static bool wordModifiers(std::string& unit)
{
    static UNITS_CPP14_CONSTEXPR_OBJECT std::array<modSeq, 28> modifiers{{
        modSeq{"squaremeter", "m^2", 11, modifier::anywhere_tail},
        modSeq{"cubicmeter", "m^3", 10, modifier::anywhere_tail},
        modSeq{"cubic", "^3", 5, modifier::start_tail},
        modSeq{"reciprocal", "^-1", 10, modifier::start_tail},
        modSeq{"reciprocal", "^-1", 10, modifier::tail_replace},
        modSeq{"square", "^2", 6, modifier::start_tail},
        modSeq{"squared", "^2", 7, modifier::tail_replace},
        modSeq{"cubed", "^3", 5, modifier::tail_replace},
        modSeq{"cu", "^3", 2, modifier::start_tail},
        modSeq{"sq", "^2", 2, modifier::start_tail},
        modSeq{"tenth", "0.1", 5, modifier::anywhere_replace},
        modSeq{"ten", "10", 3, modifier::anywhere_replace},
        modSeq{"one", "", 3, modifier::start_replace},
        modSeq{"quarter", "0.25", 7, modifier::anywhere_replace},
        modSeq{"half", "0.5", 4, modifier::anywhere_replace},
        modSeq{"hundred", "100", 7, modifier::anywhere_replace},
        modSeq{"million", "1e6", 7, modifier::anywhere_replace},
        modSeq{"billion", "1e9", 7, modifier::anywhere_replace},
        modSeq{"trillion", "1e12", 8, modifier::anywhere_replace},
        modSeq{"thousand", "1000", 8, modifier::anywhere_replace},
        modSeq{"tothethirdpower", "^3", 15, modifier::anywhere_replace},
        modSeq{"tothefourthpower", "^4", 16, modifier::anywhere_replace},
        modSeq{"tothefifthpower", "^5", 15, modifier::anywhere_replace},
        modSeq{"tothesixthpower", "^6", 15, modifier::anywhere_replace},
        modSeq{"square", "^2", 6, modifier::anywhere_tail},
        modSeq{"cubic", "^3", 5, modifier::anywhere_tail},
        modSeq{"sq", "^2", 2, modifier::tail_replace},
        modSeq{"cu", "^3", 2, modifier::tail_replace},
    }};
    if (unit.compare(0, 3, "cup") ==
        0) {  // this causes too many issues so skip it
        return false;
    }
    if (unit.compare(0, 13, "hundredweight") ==
        0) {  // this is a specific unit and should not be cut off
        return false;
    }
    for (auto& mod : modifiers) {
        if (unit.size() < std::get<2>(mod)) {
            continue;
        }
        switch (std::get<3>(mod)) {
            case modifier::tail_replace: {
                if (ends_with(unit, std::get<0>(mod))) {
                    unit.replace(
                        unit.size() - std::get<2>(mod),
                        std::get<2>(mod),
                        std::get<1>(mod));
                    return true;
                }
                break;
            }
            case modifier::anywhere_replace: {
                auto fnd = unit.find(std::get<0>(mod));
                if (fnd != std::string::npos) {
                    if (unit.size() == std::get<2>(mod)) {
                        return false;
                    }
                    unit.replace(fnd, std::get<2>(mod), std::get<1>(mod));
                    return true;
                }
                break;
            }
            case modifier::start_tail: {
                if (unit.compare(0, std::get<2>(mod), std::get<0>(mod)) == 0) {
                    if (unit.size() == std::get<2>(mod)) {
                        return false;
                    }
                    unit.erase(0, std::get<2>(mod));
                    unit.append(std::get<1>(mod));
                    return true;
                }
                break;
                case modifier::anywhere_tail: {
                    auto fnd = unit.find(std::get<0>(mod));
                    if (fnd != std::string::npos) {
                        // this will need to be added in again if more string
                        // are added to the search list with this type if
                        // (unit.size() == std::get<2>(mod))
                        //{
                        //    return false;
                        //}
                        if (fnd != 0) {
                            unit.replace(fnd, std::get<2>(mod), "*");
                        } else {
                            unit.erase(0, std::get<2>(mod));
                            unit.push_back('*');
                        }
                        unit.append(std::get<1>(mod));
                        return true;
                    }
                }
            } break;
            case modifier::start_replace: {
                if (unit.compare(0, std::get<2>(mod), std::get<0>(mod)) == 0) {
                    unit.replace(0, std::get<2>(mod), std::get<1>(mod));
                    return true;
                }
            } break;
        }
    }

    // deal with strings if the end is a number that didn't have a ^
    if (isDigitCharacter(unit.back())) {
        if (unit[unit.size() - 2] == '-') {
            unit.insert(unit.end() - 2, '^');
            return true;
        }
        if (!isDigitCharacter(unit[unit.size() - 2])) {
            unit.insert(unit.end() - 1, '^');
            return true;
        }
    }

    return false;
}
using ckpair = std::pair<const char*, const char*>;

static precise_unit
    localityModifiers(std::string unit, std::uint32_t match_flags)
{
    static UNITS_CPP14_CONSTEXPR_OBJECT std::array<ckpair, 42>
        internationlReplacements{{
            ckpair{"internationaltable", "_IT"},
            ckpair{"internationalsteamtable", "_IT"},
            ckpair{"international", "_i"},
            ckpair{"USandBritish", "_av"},
            ckpair{"US&British", "_av"},
            ckpair{"USAsurvey", "_us"},
            ckpair{"USsurvey", "_us"},
            ckpair{"USSurvey", "_us"},
            ckpair{"USA", "_us"},
            ckpair{"USstatute", "_us"},
            ckpair{"statutory", "_us"},
            ckpair{"statute", "_us"},
            ckpair{"gregorian", "_g"},
            ckpair{"Gregorian", "_g"},
            ckpair{"synodic", "_s"},
            ckpair{"sidereal", "_sdr"},
            ckpair{"julian", "_j"},
            ckpair{"Julian", "_j"},
            ckpair{"thermochemical", "_th"},
            ckpair{"Th", "_th"},
            ckpair{"(th)", "_th"},
            ckpair{"metric", "_m"},
            ckpair{"mean", "_m"},
            ckpair{"imperial", "_br"},
            ckpair{"Imperial", "_br"},
            ckpair{"imp", "_br"},
            ckpair{"US", "_us"},
            ckpair{"(IT)", "_IT"},
            ckpair{"troy", "_tr"},
            ckpair{"apothecary", "_ap"},
            ckpair{"apothecaries", "_ap"},
            ckpair{"avoirdupois", "_av"},
            ckpair{"Chinese", "_ch"},
            ckpair{"Canadian", "_can"},
            ckpair{"canadian", "_can"},
            ckpair{"survey", "_us"},
            ckpair{"tropical", "_t"},
            ckpair{"British", "_br"},
            ckpair{"british", "_br"},
            ckpair{"Br", "_br"},
            ckpair{"BR", "_br"},
            ckpair{"UK", "_br"},
        }};
    bool changed = false;
    for (const auto& irep : internationlReplacements) {
        auto fnd = unit.find(irep.first);
        if (fnd != std::string::npos) {
            auto len = strlen(irep.first);
            if (len == unit.size()) {  // this is a modifier if we are checking
                                       // the entire unit this is automatically
                                       // false
                return precise::invalid;
            }
            unit.erase(fnd, len);

            unit.append(irep.second);
            changed = true;
            break;
        }
    }
    changed |= clearEmptySegments(unit);
    if (changed) {
        return unit_from_string_internal(
            unit, match_flags | no_locality_modifiers | no_of_operator);
    }
    if (unit.size() < 4) {
        return precise::invalid;
    }
    static constexpr std::array<const char*, 8> rotSequences{
        {"us", "br", "av", "ch", "IT", "th", "ap", "tr"}};
    for (auto& seq : rotSequences) {
        if (unit.compare(0, 2, seq) == 0) {
            auto nunit = unit.substr(2);
            if (nunit.back() == 's') {
                nunit.pop_back();
            }
            nunit.push_back('_');
            nunit.append(seq);
            return get_unit(nunit, match_flags);
        }
        if (ends_with(unit, seq)) {
            unit.insert(unit.end() - 2, '_');
            return get_unit(unit, match_flags);
        }
    }

    return precise::invalid;
}

// just ignore some modifiers that might be assumed in particular units
static precise_unit ignoreModifiers(std::string unit, std::uint32_t match_flags)
{
    using igpair = std::pair<const char*, int>;

    static UNITS_CPP14_CONSTEXPR_OBJECT std::array<igpair, 1> ignore_word{{
        igpair{"liquid", 6},
    }};
    bool changed = false;
    for (const auto& irep : ignore_word) {
        auto fnd = unit.find(irep.first);
        if (fnd != std::string::npos) {
            if (irep.second == static_cast<int>(unit.size())) {
                // this is a modifier if we are checking the entire unit this is
                // automatically false
                return precise::invalid;
            }
            unit.erase(fnd, irep.second);
            changed = true;
            break;
        }
    }
    if (changed) {
        auto retunit = localityModifiers(unit, match_flags);
        if (!is_error(retunit)) {
            return retunit;
        }
        return unit_from_string_internal(
            unit, match_flags | no_locality_modifiers | no_of_operator);
    }
    return precise::invalid;
}

/// detect some known SI prefixes
static std::pair<double, size_t>
    getPrefixMultiplierWord(const std::string& unit)
{
    auto res = std::lower_bound(
        prefixWords.begin(),
        prefixWords.end(),
        unit.c_str(),
        [](const utup& prefix, const char* val) {
            return (strncmp(std::get<0>(prefix), val, std::get<2>(prefix)) < 0);
        });
    if (res != prefixWords.end()) {
        if (strncmp(std::get<0>(*res), unit.c_str(), std::get<2>(*res)) == 0) {
            return {std::get<1>(*res), std::get<2>(*res)};
        }
    }
    return {0.0, 0};
}

/** some specific strings for ucum compliance*/
static const smap base_ucum_vals{
    {"B", precise::log::bel},
};

/** units from several locations
http://vizier.u-strasbg.fr/vizier/doc/catstd-3.2.htx
http://unitsofmeasure.org/ucum.html#si
*/
static const smap base_unit_vals{
    {"", precise::defunit},
    {"[]", precise::defunit},
    {"def", precise::defunit},
    {"default", precise::defunit},
    {"defunit", precise::defunit},
    {"*", precise::defunit},
    {"**", precise::defunit},
    {"***", precise::defunit},
    {"****", precise::defunit},
    {"*****", precise::defunit},
    {"******", precise::defunit},
    {"^^^", precise::defunit},
    {"1", precise::one},
    {"one", precise::one},
    {"inf", precise::infinite},
    {"INF", precise::infinite},
    {"infinity", precise::infinite},
    {"INFINITY", precise::infinite},
    {"-inf", precise_unit(precise::infinite, -1.0)},
    {"-INF", precise_unit(precise::infinite, -1.0)},
    {"-infinity", precise_unit(precise::infinite, -1.0)},
    {"-INFINITY", precise_unit(precise::infinite, -1.0)},
    {"infinite", precise::infinite},
    {u8"\u221e", precise::infinite},
    {u8"-\u221e", precise_unit(precise::infinite, -1.0)},
    {"nan", precise::nan},
    {"NaN", precise::nan},
    {"NaN%", precise::nan},
    {"NAN", precise::nan},
    {"NaNQ",
     precise_unit(
         detail::unit_data(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
         std::numeric_limits<double>::quiet_NaN())},
    {"NaNS", precise::nan},
    {"qNaN",
     precise_unit(
         detail::unit_data(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
         std::numeric_limits<double>::quiet_NaN())},
    {"sNaN", precise::nan},
    {"1.#SNAN", precise::nan},
    {"#SNAN", precise::nan},
    {"1.#QNAN",
     precise_unit(
         detail::unit_data(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
         std::numeric_limits<double>::quiet_NaN())},
    {"#QNAN",
     precise_unit(
         detail::unit_data(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
         std::numeric_limits<double>::quiet_NaN())},
    {"1.#IND", precise::nan},
    {"#IND", precise::nan},
    {"0.1", precise_unit(0.1, precise::one)},
    {".1", precise_unit(0.1, precise::one)},
    {"deci", precise_unit(0.1, precise::one)},
    {"0.01", precise_unit(0.01, precise::one)},
    {".01", precise_unit(0.01, precise::one)},
    {"centi", precise_unit(0.01, precise::one)},
    {"0.001", precise::milli},
    {".001", precise::milli},
    {"milli", precise::milli},
    {"1e-3", precise::milli},
    {"1e-6", precise::micro},
    {"micro", precise::micro},
    {"1e-9", precise::nano},
    {"1e-12", precise::pico},
    {"1e-15", precise::femto},
    {"1e-18", precise::atto},
    {"nano", precise::nano},
    {"pico", precise::pico},
    {"femto", precise::femto},
    {"atto", precise::atto},
    {"10", precise_unit(10.0, precise::one)},
    {"ten", precise_unit(10.0, precise::one)},
    {"two", precise_unit(2.0, precise::one)},
    {"twenty", precise_unit(20.0, precise::one)},
    {"twelve", precise_unit(12.0, precise::one)},
    {"eight", precise_unit(8.0, precise::one)},
    {"100", precise_unit(100, precise::one)},
    {"hundred", precise_unit(100, precise::one)},
    {"fifty", precise_unit(50, precise::one)},
    {"centum", precise_unit(100, precise::one)},
    {"1000", precise::kilo},
    {"thousand", precise::kilo},
    {"1000000", precise::mega},
    {"million", precise::mega},
    {"1000000000", precise::giga},
    {"billion", precise::giga},
    {"trillion", precise::tera},
    {"quadrillion", precise::peta},
    {"1e3", precise::kilo},
    {"1e6", precise::mega},
    {"1e9", precise::giga},
    {"1e12", precise::tera},
    {"1e15", precise::peta},
    {"1e18", precise::exa},
    {"kilo", precise::kilo},
    {"mega", precise::mega},
    {"giga", precise::giga},
    {"tera", precise::tera},
    {"peta", precise::peta},
    {"exa", precise::exa},
    {"%", precise::percent},
    {"percent", precise::percent},
    {"percentage", precise::percent},
    {"permille", precise::milli},
    {u8"\u2030", precise::milli},  // per mille symbol
    {"bp", precise_unit(0.1, precise::milli)},
    {"basispoint", precise_unit(0.1, precise::milli)},
    {u8"\u2031", precise_unit(0.1, precise::milli)},  // per ten thousand symbol
    {"pct", precise::percent},
    {"pi", precise_unit(constants::pi, one)},
    {"PI", precise_unit(constants::pi, one)},
    {"[pi]", precise_unit(constants::pi, one)},
    {"thenumberpi", precise_unit(constants::pi, one)},
    {"[PI]", precise_unit(constants::pi, one)},
    {"Pi", precise_unit(constants::pi, one)},
    {u8"\u03C0", precise_unit(constants::pi, one)},
    {u8"\U0001D70B", precise_unit(constants::pi, one)},
    {u8"\U0001D745", precise_unit(constants::pi, one)},
    {u8"\U0001D6D1", precise_unit(constants::pi, one)},
    {"m", precise::m},
    {"Sm", precise::m},  // standard meter used in oil and gas usually Sm^3
    {"meter", precise::m},
    {"squaremeter",
     precise::m.pow(2)},  // to simplify some things later in the chain
    {"cubicmeter", precise::m.pow(3)},
    {"micron", precise::micro* precise::m},
    {"fermi", precise::femto* precise::m},
    {"xunit", precise::distance::xu},
    {"xu", precise::distance::xu},
    {"X", precise::distance::xu},
    {"cc", precise::cm.pow(3)},
    {"cubiccentimeter", precise::cm.pow(3)},
    {"m/s^2", precise::m / precise::s.pow(2)},
    {"kg/m^3", precise::kg / precise::m.pow(3)},
    {"kg", precise::kg},
    {"KG", precise::kg},
    {"kilogram", precise::kg},
    {"jin", precise::chinese::jin},
    {"liang", precise::chinese::liang},
    {"qian", precise::chinese::qian},
    {"mol", precise::mol},
    {"einstein", precise::mol},  // technically of photons
    {"gmol", precise::mol},
    {"lbmol", precise_unit(424.0, precise::mol)},
    {"atom", constants::Na.as_unit().inv()},
    {"avogadroconstant", constants::Na.as_unit()},
    {"molecule", constants::Na.as_unit().inv()},
    {"molec", constants::Na.as_unit().inv()},
    {"nucleon", constants::Na.as_unit().inv()},
    {"nuc", constants::Na.as_unit().inv()},
    {"MOL", precise::mol},
    {"mOL", precise::mol},
    {"mole", precise::mol},
    {"M", precise::laboratory::molarity},
    {"molar", precise::laboratory::molarity},
    {"Molar", precise::laboratory::molarity},
    {"eq", precise::mol},
    {"equivalent", precise::mol},
    {"EQ", precise::mol},
    {"osm", precise_unit(1.0, precise::mol, commodities::particles)},
    {"osmole", precise_unit(1.0, precise::mol, commodities::particles)},
    {"osmol", precise_unit(1.0, precise::mol, commodities::particles)},
    {"OSM", precise_unit(1.0, precise::mol, commodities::particles)},
    {"g%", precise_unit(10.0, precise::g / precise::L)},
    {"gram%", precise_unit(10.0, precise::g / precise::L)},
    {"grampercent", precise_unit(10.0, precise::g / precise::L)},
    {"G%", precise_unit(10.0, precise::g / precise::L)},
    {"U", precise::laboratory::enzyme_unit},
    {"units", precise::laboratory::enzyme_unit},  // this may not be best but it
                                                  // doesn't actually conflict
                                                  // with
    // anything else right now
    {"enzymeunit", precise::laboratory::enzyme_unit},
    {"A", precise::A},
    {"amp", precise::A},
    {"amps", precise::A},
    {"ampturn", precise::A* precise::count},
    {"At", precise::A* precise::count},
    {"V", precise::V},
    {"volt", precise::V},
    {"W", precise::W},
    {"W/m^2", precise::W / precise::m.pow(2)},
    {"watt", precise::W},
    {"kW", precise::electrical::kW},
    {"kilowatt", precise::electrical::kW},
    {"MW", precise::MW},
    {"megawatt", precise::MW},
    {"MVA", precise::electrical::MVAR},
    {"mvar", precise::electrical::MVAR},
    {"MVAR", precise::electrical::MVAR},
    {"VA", precise::electrical::VAR},
    {"voltampreactive", precise::electrical::VAR},
    {"VAR", precise::electrical::VAR},
    {"var", precise::electrical::VAR},
    {"s", precise::s},
    {"sec", precise::s},
    {"MAS", precise::mega* precise::s},
    {"mAS", precise::mega* precise::s},
    {"second", precise::s},
    {"second-time", precise::s},
    {"shake", precise_unit(10.0, precise::ns)},
    {"jiffy", precise_unit(0.01, precise::s)},  // use the computer science
                                                // definition for playback and
                                                // clock rate
    {"cd", precise::cd},
    {"mcd", precise::milli* precise::cd},  // prefer milli candela to micro-day
    {"CD", precise::cd},
    {"candela", precise::cd},
    {"candle", precise::other::candle},
    {"candlepower", precise::other::candle},
    {"CP", precise::other::candle},
    {"footcandle", precise::lumen / precise::ft.pow(2)},
    {"fc", precise::lumen / precise::ft.pow(2)},
    {"ftc", precise::lumen / precise::ft.pow(2)},
    {"metercandle", precise::lumen / precise::m.pow(2)},
    {"centimetercandle", precise::lumen / precise::cm.pow(2)},
    {"K", precise::K},
    {"kelvin", precise::K},
    {"kelvins", precise::K},
    {"degKelvin", precise::K},
    {"degsKelvin", precise::K},
    {"degkelvin", precise::K},
    {"degskelvin", precise::K},
    {"degK", precise::K},
    {"degsK", precise::K},
    {"N", precise::N},
    {"Ns", precise::N* precise::s},  // this would not pass through to the
                                     // separation functions
    {"Nm", precise::N* precise::m},  // this would not pass through to the
                                     // separation functions
    {"As", precise::A* precise::s},  // this would not pass through to the
                                     // separation functions
    {"Ah", precise::A* precise::hr},  // this would not pass through to the
                                      // separation functions
    {"Ahr", precise::A* precise::hr},  // this would not pass through to the
                                       // separation functions
    {"newton", precise::N},
    {"Pa", precise::Pa},
    {"pa", precise::Pa},
    {"pascal", precise::Pa},
    {"PAL", precise::Pa},
    {"pAL", precise::Pa},
    {"J", precise::J},
    {"joule", precise::J},
    {"Joule", precise::J},
    {"Joules", precise::J},
    {"C", precise::C},
    {"coulomb", precise::C},
    {"faraday", precise::other::faraday},
    {"farady", precise::other::faraday},
    {"F", precise::F},
    {"farad", precise::F},
    {"abF", precise::cgs::abFarad},
    {"abfarad", precise::cgs::abFarad},
    {"stF", precise::cgs::statFarad},
    {"statfarad", precise::cgs::statFarad},
    {"OHM", precise::ohm},
    {"ohm", precise::ohm},
    {"Ohm", precise::ohm},
    {u8"\u03A9", precise::ohm},  // Greek Omega
    {u8"\u2126", precise::ohm},  // Unicode Ohm symbol
    {"abOhm", precise::cgs::abOhm},
    {"abohm", precise::cgs::abOhm},
    {"statohm", precise::cgs::statOhm},
    {"statOhm", precise::cgs::statOhm},
    {"S", precise::S},
    {"siemen", precise::S},
    {"siemens", precise::S},
    {"SIE", precise::S},
    {"mho", precise::S},
    {"gemmho", precise_unit(1e-6, precise::S)},
    {"MHO", precise::S},
    {"mHO", precise::S},
    {u8"\u01B1", precise::S},
    {"absiemen", precise_unit(1e9, precise::S)},
    {"abmho", precise_unit(1e9, precise::S)},
    {"statmho", precise_unit(1.0 / 8.987551787e11, S)},
    {"Wb", precise::Wb},
    {"weber", precise::Wb},
    {"T", precise::T},
    {"tesla", precise::T},
    {"H", precise::H},
    {"henry", precise::H},
    {"henries", precise::H},
    {"abhenry", precise::cgs::abHenry},
    {"abH", precise::cgs::abHenry},
    {"stathenry", precise::cgs::statHenry},
    {"stH", precise::cgs::statHenry},
    {"statH", precise::cgs::statHenry},
    {"lm", precise::lm},
    {"LM", precise::lm},
    {"lumen", precise::lm},
    {"lux", precise::lx},
    {"luxes", precise::lx},
    {"nox", precise::milli* precise::lx},
    {"lx", precise::lx},
    {"LX", precise::lx},
    {"Bq", precise::Bq},
    {"BQ", precise::Bq},
    {"becquerel", precise::Bq},
    {"rutherford", precise_unit(1e6, precise::Bq)},
    {"activity", precise::Bq},
    {"curie", precise::cgs::curie},
    {"gauss", precise::cgs::gauss},
    {"G", precise::cgs::gauss},
    {"Gs", precise::cgs::gauss},
    {"Ci", precise::cgs::curie},
    {"CI", precise::cgs::curie},
    {"Gal", precise::cgs::gal},
    {"Gals", precise::cgs::gal},
    {"galileos", precise::cgs::gal},
    {"stC", precise::cgs::statC_charge},
    {"statC", precise::cgs::statC_charge},
    {"statC{charge}", precise::cgs::statC_charge},
    {"statC{flux}", precise::cgs::statC_flux},
    {"statcoulomb", precise::cgs::statC_charge},
    {"statcoulomb{charge}", precise::cgs::statC_charge},
    {"statcoulomb{flux}", precise::cgs::statC_flux},
    {"Fr", precise::cgs::statC_charge},
    {"franklin", precise::cgs::statC_charge},
    {"esu", precise::cgs::statC_charge},
    {"bar", precise::bar},
    {"BAR", precise::bar},
    {"in", precise::in},
    {"in_i", precise::in},
    {"[IN_I]", precise::in},
    {"inch_i", precise::i::inch},
    {"inches", precise::in},
    {"inches_i", precise::i::inch},
    {"in_us", precise::us::inch},
    {"inus", precise::us::inch},
    {"usin", precise::us::inch},  // this is more likely than micro square in
    {"[IN_US]", precise::us::inch},
    {"inch_us", precise::us::inch},
    {"inches_us", precise::us::inch},
    {"caliber", precise_unit(0.01, precise::i::inch)},
    {"rd", precise::us::rod},
    {"rod", precise::us::rod},
    {"rd_us", precise::us::rod},
    {"rdus", precise::us::rod},
    {"[RD_US]", precise::us::rod},
    {"rod_us", precise::us::rod},
    {"rods_us", precise::us::rod},
    {"ch", precise::us::chain},
    {"chain", precise::us::chain},
    {"chain_us", precise::us::chain},
    {"ch_us", precise::us::chain},
    {"[CH_US]", precise::us::chain},
    {"gunter'schain_us", precise::us::chain},
    {"surveyors'schain_us", precise::us::chain},
    {"surveyors'schain", precise::us::chain},
    {"lk_us", precise::us::link},
    {"li", precise::us::link},
    {"lnk", precise::us::link},
    {"link_us", precise::us::link},
    {"li_us", precise::us::link},
    {"link", precise::us::link},
    {"[LK_US]", precise::us::link},
    {"linkforGunter'schain_us", precise::us::link},
    {"rch_us", precise::us::engineers::chain},
    {"[RCH_US]", precise::us::engineers::chain},
    {"ramden'schain_us", precise::us::engineers::chain},
    {"rlk_us", precise::us::engineers::link},
    {"[RLK_US]", precise::us::engineers::link},
    {"linkforRamden'schain_us", precise::us::engineers::link},
    {"fur", precise::us::furlong},
    {"furlong", precise::us::furlong},
    {"fur_us", precise::us::furlong},
    {"[FUR_US]", precise::us::furlong},
    {"furlong_us", precise::us::furlong},
    {"fth", precise::nautical::fathom},
    {"fath", precise::nautical::fathom},
    {"fth_us", precise::nautical::fathom},
    {"fthus", precise::nautical::fathom},
    {"[FTH_US]", precise::nautical::fathom},
    {"fathom_us", precise::nautical::fathom},
    {"mi_us", precise::us::mile},
    {"[MI_US]", precise::us::mile},
    {"mile_us", precise::us::mile},
    {"lea_us", precise_unit(3.0, precise::us::mile)},
    {"league_us", precise_unit(3.0, precise::us::mile)},
    {"mil_us", precise::us::mil},
    {"[MIL_US]", precise::us::mil},
    {"mil_us", precise::us::mil},
    {"inch", precise::in},
    {"thou", precise::imp::thou},
    {"thousandth", precise::in* precise::milli},
    {"mil", precise::i::mil},
    {"mil_i", precise::i::mil},
    {"[MIL_I]", precise::i::mil},
    {"cml", precise::i::circ_mil},
    {"circularmil", precise::i::circ_mil},
    {"circularinch",
     precise_unit{constants::pi / 4.0, precise::i::inch.pow(2)}},
    {"cml_i", precise::i::circ_mil},
    {"circularmil_i", precise::i::circ_mil},
    {"[CML_I]", precise::i::circ_mil},
    {"hd", precise::i::hand},
    {"hd_i", precise::i::hand},
    {"[HD_I]", precise::i::hand},
    {"hand", precise::i::hand},
    {"jansky",
     precise_unit(1e-26, precise::W / precise::m / precise::m / precise::Hz)},
    {"Jy",
     precise_unit(1e-26, precise::W / precise::m / precise::m / precise::Hz)},
    {"ft", precise::ft},
    {"ft_i", precise::ft},
    {"[FT_I]", precise::ft},
    {"bf", precise::i::board_foot},
    {"BF", precise::i::board_foot},
    {"bf_i", precise::i::board_foot},
    {"BDFT", precise::i::board_foot},
    {"FBM", precise::i::board_foot},
    {"[BF_I]", precise::i::board_foot},
    {"boardfoot", precise::i::board_foot},
    {"boardfeet", precise::i::board_foot},
    {"boardfeet_i", precise::i::board_foot},
    {"cr", precise::i::cord},
    {"crd", precise::i::cord},
    {"cord", precise::i::cord},
    {"crd_i", precise::i::cord},
    {"cords_i", precise::i::cord},
    {"cr_i", precise::i::cord},
    {"[CR_I]", precise::i::cord},
    {"crd_us", precise::us::cord},
    {"[CRD_US]", precise::us::cord},
    {"cord_us", precise::us::cord},
    {"ftus", precise::ft},
    {"ft_us", precise::ft},
    {"[FT_US]", precise::ft},
    {"foot_us", precise::ft},
    {"feet_us", precise::ft},
    {"ft^2", precise::ft* precise::ft},
    {"sin", precise::in* precise::in},
    {"sin_i", precise::in* precise::in},
    {"[SIN_I]", precise::in* precise::in},
    {"cin", precise::in.pow(3)},  // cubic inch instead of centiinch
    {"cin_i", precise::in.pow(3)},
    {"[CIN_I]", precise::in.pow(3)},
    {"sf", precise::ft* precise::ft},
    {"sft", precise::ft* precise::ft},
    {"sft_i", precise::ft* precise::ft},
    {"[SFT_I]", precise::ft* precise::ft},
    {"SCF", precise::energy::scf},  // standard cubic foot
    {"CCF", precise::hundred* precise::energy::scf},  // centum cubic foot
    {"MCF", precise::kilo* precise::energy::scf},  // millum cubic foot
    {"MMCF", precise::mega* precise::energy::scf},  // million cubic foot
    {"BCF", precise::giga* precise::energy::scf},  // billion cubic foot
    {"TCF", precise::tera* precise::energy::scf},  // trillion cubic foot
    {"QCF", precise::exa* precise::energy::scf},  // quadrillion cubic foot
    {"Mcf", precise::kilo* precise::energy::scf},  // millum cubic foot
    {"MMcf", precise::mega* precise::energy::scf},  // million cubic foot
    {"Bcf", precise::giga* precise::energy::scf},  // billion cubic foot
    {"Tcf", precise::tera* precise::energy::scf},  // trillion cubic foot
    {"Qcf", precise::exa* precise::energy::scf},  // quadrillion cubic foot
    {"Mm^3", precise::kilo* precise::energy::scm},  // millum cubic meters
    {"Nm^3", precise::energy::ncm},  // normal cubic meter
    {"Sm^3", precise::energy::scm},  // standard cubic meter
    {"MMm^3", precise::mega* precise::energy::scm},  // million cubic meters
    {"bm^3", precise::giga* precise::energy::scm},  // billion cubic meters
    {"tm^3", precise::tera* precise::energy::scm},  // trillion cubic meters
    {"cf", precise::energy::scf},
    {"scf", precise::energy::scf},
    {"std", precise::energy::scf},
    {"ncf", precise::energy::ncf},
    {"mcf", precise::kilo* precise::energy::scf},
    {"ccf", precise::hundred* precise::energy::scf},
    {"cft", precise::ft.pow(3)},
    {"[cft_i]", precise::ft.pow(3)},
    {"[CFT_I]", precise::ft.pow(3)},
    {"foot", precise::ft},
    {"foot_i", precise::i::foot},
    {"feet", precise::ft},
    {"feet_i", precise::i::foot},
    {"YD", precise::yd},
    {"yd", precise::yd},
    {"yd_i", precise::yd},
    {"yard_i", precise::yd},
    {"yards_i", precise::yd},
    {"[YD_I]", precise::yd},
    {"ydus", precise::us::yard},
    {"yd_us", precise::us::yard},
    {"ydsus", precise::us::yard},
    {"[YD_US]", precise::us::yard},
    {"yardus", precise::us::yard},
    {"yard_us", precise::us::yard},
    {"yards_us", precise::us::yard},
    {"sy", precise::yd* precise::yd},
    {"syd", precise::yd* precise::yd},
    {"syd_i", precise::yd* precise::yd},
    {"[SYD_I]", precise::yd* precise::yd},
    {"cy", precise::yd.pow(3)},
    {"cyd", precise::yd.pow(3)},
    {"cyd_i", precise::yd.pow(3)},
    {"[CYD_I]", precise::yd.pow(3)},
    {"in_br", precise::imp::inch},
    {"inch_br", precise::imp::inch},
    {"inches_br", precise::imp::inch},
    {"barleycorn", precise::imp::barleycorn},
    {"[IN_BR]", precise::imp::inch},
    {"ft_br", precise::imp::foot},
    {"[FT_BR]", precise::imp::foot},
    {"foot_br", precise::imp::foot},
    {"rd_br", precise::imp::rod},
    {"[RD_BR]", precise::imp::rod},
    {"rod_br", precise::imp::rod},
    {"perch", precise::imp::rod},
    {"pole", precise::imp::rod},
    {"ch_br", precise::imp::chain},
    {"[CH_BR]", precise::imp::chain},
    {"gunter'schain_br", precise::imp::chain},
    {"lk_br", precise::imp::link},
    {"[LK_BR]", precise::imp::link},
    {"linkforGunter'schain_br", precise::imp::link},
    {"fth_br", precise::nautical::fathom},
    {"[FTH_BR]", precise::nautical::fathom},
    {"fathom_br", precise::nautical::fathom},
    {"fathoms_br", precise::nautical::fathom},
    {"pc_br", precise::imp::pace},
    {"[PC_BR]", precise::imp::pace},
    {"pace_br", precise::imp::pace},
    {"yd_br", precise::imp::yard},
    {"[YD_BR]", precise::imp::yard},
    {"yard_br", precise::imp::yard},
    {"nmi_br", precise::imp::nautical_mile},
    {"[NMI_BR]", precise::imp::nautical_mile},
    {"nauticalmile_br", precise::imp::nautical_mile},
    {"nauticalleague_br", precise_unit(3.0, precise::imp::nautical_mile)},
    {"kn_br", precise::imp::nautical_mile / precise::hr},
    {"[KN_BR]", precise::imp::nautical_mile / precise::hr},
    {"knot_br", precise::imp::nautical_mile / precise::hr},
    {"mi_br", precise::imp::mile},
    {"[MI_BR]", precise::imp::mile},
    {"mile_br", precise::imp::mile},
    {"gal_br", precise::imp::gallon},
    {"[GAL_BR]", precise::imp::gallon},
    {"gallon_br", precise::imp::gallon},
    {"yard", precise::yd},
    {"cubit", precise::distance::cubit},
    {"cubit_br", precise::distance::cubit},
    {"cubit(UK)", precise::distance::cubit},
    // because cubit has cu in it which indicates cubed
    {"longcubit", precise::distance::longcubit},
    {"arpent", precise::distance::arpent_us},
    {"arpent_fr", precise::distance::arpent_fr},
    {"arpentlin", precise::distance::arpent_fr},
    {"ken", precise::japan::ken},
    {"cun", precise::chinese::cun},
    {"cun(Chinese)", precise::chinese::cun},  // interaction with cu for cubic
    {"cun_ch", precise::chinese::cun},
    {"chi", precise::chinese::chi},
    {"chi_ch", precise::chinese::chi},
    {"zhang", precise::chinese::zhang},
    {"zhang_ch", precise::chinese::zhang},
    {"li_ch", precise::chinese::li},
    {"jin_ch", precise::chinese::jin},
    {"jin", precise::chinese::jin},
    {"liang_ch", precise::chinese::liang},
    {"liang", precise::chinese::liang},
    {"qian_ch", precise::chinese::qian},
    {"qian", precise::chinese::qian},
    {"min", precise::min},
    {"mins", precise::min},
    {"mIN", precise::min},
    {"minute", precise::min},
    {"ms", precise::ms},
    {"millisecond", precise::ms},
    {"hr", precise::hr},
    {"HR", precise::hr},
    {"h", precise::hr},
    {"hour", precise::hr},
    {"day", precise::time::day},
    {"dy", precise::time::day},
    {"D", precise::time::day},
    {"d", precise::time::day},
    {"week", precise::time::week},
    {"weekly", precise::one / precise::time::week},
    {"biweekly", precise_unit(0.5, precise::time::week.inv())},
    {"fortnight", precise::time::fortnight},
    {"wk", precise::time::week},
    {"WK", precise::time::week},
    {"y", precise::time::year},
    {"YR", precise::time::yr},  // this one gets 365 days exactly
    {"yr", precise::time::yr},  // this one gets 365 days exactly
    {"a", precise::time::year},  // year vs are
    {"year", precise::time::year},  // year
    {"yearly", precise::time::year.inv()},  // year
    {"annum", precise::time::year},  // year
    {"ANN", precise::time::year},  // year
    {"decade", precise::ten* precise::time::aj},  // year
    {"century", precise::hundred* precise::time::aj},  // year
    {"millennia", precise::kilo* precise::time::ag},  // year
    {"millennium", precise::kilo* precise::time::ag},  // year
    {"syr", precise::time::syr},  // sidereal year
    {"year_sdr", precise::time::syr},  // sidereal year
    {"yr_sdr", precise::time::syr},  // sidereal year
    {"month_sdr",
     precise_unit(1.0 / 12.0, precise::time::syr)},  // sidereal month
    {"mo_sdr", precise_unit(1.0 / 12.0, precise::time::syr)},  // sidereal month
    {"sday", precise::time::sday},  // sidereal day
    {"day_sdr", precise::time::sday},  // sidereal day
    {"dy_sdr", precise::time::sday},  // sidereal day
    {"d_sdr", precise::time::sday},  // sidereal day
    {"hour_sdr",
     precise_unit(1.0 / 24.0, precise::time::sday)},  // sidereal hour
    {"minute_sdr",
     precise_unit(1.0 / 24.0 / 60.0, precise::time::sday)},  // sidereal minute
    {"second_sdr",
     precise_unit(1.0 / 24.0 / 60.0 / 60.0, precise::time::sday)},  // sidereal
                                                                    // second
    {"hr_sdr", precise_unit(1.0 / 24.0, precise::time::sday)},  // sidereal hour
    {"min_sdr",
     precise_unit(1.0 / 24.0 / 60.0, precise::time::sday)},  // sidereal minute
    {"sec_sdr",
     precise_unit(1.0 / 24.0 / 60.0 / 60.0, precise::time::sday)},  // sidereal
                                                                    // second
    {"a_t", precise::time::at},  // tropical year
    {"year_t", precise::time::at},  // tropical year
    {"month_t", precise_unit(1.0 / 12.0, precise::time::at)},  // tropical month
    {"mo_t", precise_unit(1.0 / 12.0, precise::time::at)},  // tropical month
    {"solaryear", precise::time::at},  // year
    {"ANN_T", precise::time::at},  // year
    {"a_j", precise::time::aj},  // year
    {"meanyear_j", precise::time::aj},  // year
    {"meanyr_j", precise::time::aj},  // year
    {"year_j", precise::time::aj},  // year
    {"yr_j", precise::time::aj},  // year
    {"ANN_J", precise::time::aj},  // year
    {"year(leap)", precise_unit(366.0, precise::time::day)},  // year
    {"commonyear", precise_unit(365.0, precise::time::day)},  // year
    {"leapyear", precise_unit(366.0, precise::time::day)},  // year
    {"yearcommon", precise_unit(365.0, precise::time::day)},  // year
    {"yearleap", precise_unit(366.0, precise::time::day)},  // year
    {"a_g", precise::time::ag},  // year
    {"meanyear_g", precise::time::ag},  // year
    {"meanyr_g", precise::time::ag},  // year
    {"year_g", precise::time::ag},  // year
    {"yr_g", precise::time::ag},  // year
    {"ANN_G", precise::time::ag},  // year
    {"mo", precise::time::mog},  // gregorian month
    {"month", precise::time::mog},  // gregorian month
    {"monthly", precise::time::mog.inv()},  // gregorian month
    {"MO", precise::time::mog},  // gregorian month
    {"mO", precise::time::mog},  // gregorian month
    {"mos", precise::time::mos},  // synodal month
    {"mo_s", precise::time::mos},  // synodal month
    {"mO_S", precise::time::mos},  // synodal month
    {"synodalmonth", precise::time::mos},  // synodal month
    {"month_s", precise::time::mos},  // synodal month
    {"lunarmonth", precise::time::mos},  // synodal month
    {"moon", precise::time::mos},  // synodal month
    {"mo_j", precise::time::moj},  //
    {"month_j", precise::time::moj},  //
    {"mO_J", precise::time::moj},  //
    {"meanmonth_j", precise::time::moj},  //
    {"mo_g", precise::time::mog},  //
    {"mog", precise::time::mog},  //
    {"month_g", precise::time::mog},  //
    {"mO_G", precise::time::mog},  //
    {"meanmonth_g", precise::time::mog},  //
    {"eon", precise_unit(1e9, precise::time::syr)},
    {"workyear", precise_unit(2056, precise::hr)},
    {"workmonth", precise_unit(2056.0 / 12.0, precise::hr)},
    {"workweek", precise_unit(40.0, precise::hr)},
    {"workday", precise_unit(8.0, precise::hr)},
    {"arcdeg", precise::deg},
    {"deg-planeangle", precise::deg},
    {"deg(planeangle)", precise::deg},
    {"angulardeg", precise::deg},
    {"deg", precise::deg},
    {"DEG", precise::deg},
    {"degE", precise::deg* precise::direction::east},
    {"east", precise::direction::east},
    {"degW", precise::deg* precise::direction::west},
    {"west", precise::direction::west},
    {"degS", precise::deg* precise::direction::south},
    {"south", precise::direction::south},
    {"degN", precise::deg* precise::direction::north},
    {"north", precise::direction::north},
    {"degT", precise::deg* precise::direction::north},
    {"true", precise::direction::north},
    {"o", precise::deg},
    {u8"\u00B0", precise::deg},  // unicode degree symbol
    {"\xB0", precise::deg},  // latin-1 degree
    {u8"\u00B0(s)", precise::deg},  // unicode degree symbol
    {"\xB0(s)", precise::deg},  // latin-1 degree
    {"arcminute", precise::angle::arcmin},
    {"arcmin", precise::angle::arcmin},
    {"amin", precise::angle::arcmin},
    {"am", precise::angle::arcmin},  // as opposed to attometer
    {"angularminute", precise::angle::arcmin},  // as opposed to attometer
    {"'", precise::angle::arcmin},
    {"'", precise::angle::arcmin},
    {u8"\u2032", precise::angle::arcmin},  // double prime
    {"arcsecond", precise::angle::arcsec},
    {"''", precise::angle::arcsec},
    {"''", precise::angle::arcsec},
    {"arcsec", precise::angle::arcsec},
    {"asec", precise::angle::arcsec},
    {"as", precise::angle::arcsec},  // as opposed to attosecond
    {"angularsecond", precise::angle::arcsec},  // as opposed to attosecond
    {"\"", precise::angle::arcsec},
    {u8"\u2033", precise::angle::arcsec},  // double prime
    {"mas", precise_unit(0.001, precise::angle::arcsec)},  // milliarcsec
    {"rad", precise::rad},
    {"radian", precise::rad},
    {"gon", precise::angle::gon},
    {"gon(grade)", precise::angle::gon},
    {"GON", precise::angle::gon},
    {u8"\u25A1^g", precise::angle::gon},
    {"^g", precise::angle::gon},
    {"grad", precise::angle::grad},
    {"gradians", precise::angle::grad},
    {"grade", precise::angle::grad},
    {"mil(angle)", precise_unit(0.0625, precise::angle::grad)},
    {"circ", precise_unit(constants::tau, precise::rad)},
    {"CIRC", precise_unit(constants::tau, precise::rad)},
    {"quadrant", precise_unit(90.0, precise::deg)},
    {"circle", precise_unit(constants::tau, precise::rad)},
    {"cycle", precise_unit(constants::tau, precise::rad)},
    {"rotation", precise_unit(constants::tau, precise::rad)},
    {"turn", precise_unit(constants::tau, precise::rad)},
    {"brad", precise::angle::brad},
    {"circle-planeangle", precise_unit(constants::tau, precise::rad)},
    {"sph", precise_unit(4.0 * constants::pi, precise::sr)},
    {"spere-solidangle", precise_unit(4.0 * constants::pi, precise::sr)},
    {"spere", precise_unit(4.0 * constants::pi, precise::sr)},
    {"speres", precise_unit(4.0 * constants::pi, precise::sr)},
    {"Spere", precise_unit(4.0 * constants::pi, precise::sr)},
    {"SPH", precise_unit(4.0 * constants::pi, precise::sr)},
    {"east", precise::direction::east},
    {"north", precise::direction::north},
    {"south", precise::direction::south},
    {"west", precise::direction::west},
    {"\xB0"
     "C",
     precise::degC},
    {u8"\u00B0C", precise::degC},
    {u8"\u2103", precise::degC},  // direct unicode symbol
    {"\xB0"
     "K",
     precise::K},
    {u8"\u00B0K", precise::K},

    {"degC", precise::degC},
    {"oC", precise::degC},
    {"Cel", precise::degC},
    {"CEL", precise::degC},
    {"K@273.15", precise::degC},
    {"celsius", precise::degC},
    {"degF", precise::degF},
    {"degsF", precise::degF},
    {"[DEGF]", precise::degF},
    {u8"\u2109", precise::degF},  // direct unicode symbol
    {"degR", precise::temperature::degR},
    {"degsR", precise::temperature::degR},
    {"[DEGR]", precise::temperature::degR},
    {u8"\u00B0R", precise::temperature::degR},
    {u8"\u00B0r", precise::temperature::reaumur},
    {"\xB0R", precise::temperature::degR},
    {"\xB0r", precise::temperature::reaumur},
    {"[DEGRE]", precise::temperature::reaumur},
    {"degRe", precise::temperature::reaumur},
    {"degsRe", precise::temperature::reaumur},
    {u8"degR\u00e9aumur", precise::temperature::reaumur},
    {u8"\u00B0R\u00e9", precise::temperature::reaumur},
    {u8"\u00B0Re", precise::temperature::reaumur},
    {u8"\u00B0Ra", precise::temperature::degR},
    {"\xB0Re", precise::temperature::reaumur},
    {"\xB0Ra", precise::temperature::degR},
    {"degReaumur", precise::temperature::reaumur},
    {"reaumur", precise::temperature::reaumur},
    {u8"r\u00e9aumur", precise::temperature::reaumur},
    {"degCelsius", precise::degC},
    {"degC", precise::degC},
    {"degsC", precise::degC},
    {"degFahrenheit", precise::degF},
    {"degRankine", precise::temperature::degR},
    {"degrankine", precise::temperature::degR},
    {"rankine", precise::temperature::degR},
    {"degReaumur", precise::temperature::reaumur},
    {"oF", precise::degF},
    // this is two strings since F could be interpreted as hex and I don't want
    // it to be
    {"\xB0"
     "F",
     precise::degF},
    {u8"\u00b0F", precise::degF},
    {"fahrenheit", precise::degF},
    {"mi", precise::mile},
    {"mi_i", precise::mile},
    {"league", precise::i::league},
    {"lea", precise::i::league},
    {"[MI_I]", precise::mile},
    {"miI", precise::mile},
    {"smi", precise::mile* precise::mile},
    {"smi_us", precise::us::mile* precise::us::mile},
    {"[SMI_US]", precise::us::mile* precise::us::mile},
    {"mile", precise::mile},
    {"mile_i", precise::mile},
    {"miles_i", precise::mile},
    {"srd_us", precise::us::rod* precise::us::rod},
    {"[SRD_US]", precise::us::rod* precise::us::rod},
    {"sct", precise::us::section},
    {"[SCT]", precise::us::section},
    {"section", precise::us::section},
    {"homestead", precise::us::homestead},
    {"twp", precise::us::township},
    {"[TWP]", precise::us::township},
    {"township", precise::us::township},
    {"fur", precise::us::furlong},
    {"[FUR_I]", precise::us::furlong},
    {"cm", precise::cm},
    {"centimeter", precise::cm},
    {"km", precise::km},
    {"kilometer", precise::km},
    {"mm", precise::mm},
    {"millimeter", precise::mm},
    {"nm", precise::nm},
    {"nanometer", precise::nm},
    {"ly", precise::distance::ly},
    {"[LY]", precise::distance::ly},
    {"lightyear", precise::distance::ly},
    {"light-year", precise::distance::ly},
    {"pc", precise::distance::parsec},
    {"parsec", precise::distance::parsec},
    {"PRS", precise::distance::parsec},
    {"pRS", precise::distance::parsec},
    {"[c]", constants::c.as_unit()},
    {"[C]", constants::c.as_unit()},
    {"speedoflight", constants::c.as_unit()},
    {"speedoflightinvacuum", constants::c.as_unit()},
    {"light", constants::c.as_unit()},
    {"[h]", constants::h.as_unit()},
    {"[H]", constants::h.as_unit()},
    {u8"\u210E", constants::h.as_unit()},
    {u8"\u210F", precise_unit(1.0 / constants::tau, constants::h.as_unit())},
    {"[k]", constants::k.as_unit()},
    {"[K]", constants::k.as_unit()},
    {"eps_0", constants::eps0.as_unit()},
    {"vacuumpermittivity", constants::eps0.as_unit()},
    {"[EPS_0]", constants::eps0.as_unit()},
    {u8"\u03B5"
     "0",
     constants::eps0.as_unit()},
    {u8"\u03B5\u2080", constants::eps0.as_unit()},
    {"mu_0", constants::mu0.as_unit()},
    {"[MU_0]", constants::mu0.as_unit()},
    {"[e]", constants::e.as_unit()},
    {"e", constants::e.as_unit()},
    {"[E]", constants::e.as_unit()},
    {"elementarycharge", constants::e.as_unit()},
    {"[G]", constants::G.as_unit()},
    {"[GC]", constants::G.as_unit()},
    {"[g]", constants::g0.as_unit()},
    {"standardgravity", constants::g0.as_unit()},
    {"standardfreefall", constants::g0.as_unit()},
    {"freefall", constants::g0.as_unit()},
    {"standardaccelerationoffreefall", constants::g0.as_unit()},
    {"accelerationofgravity", constants::g0.as_unit()},
    {"m_e", constants::me.as_unit()},
    {"electronmass", constants::me.as_unit()},
    {"[M_E]", constants::me.as_unit()},
    {"m_p", constants::mp.as_unit()},
    {"[M_P]", constants::mp.as_unit()},
    {"protonmass", constants::mp.as_unit()},
    {"m_n", constants::mn.as_unit()},
    {"[M_N]", constants::mn.as_unit()},
    {"neutronmass", constants::mn.as_unit()},
    {"planckmass", constants::planck::mass.as_unit()},
    {"plancklength", constants::planck::length.as_unit()},
    {"plancktime", constants::planck::time.as_unit()},
    {"planckcharge", constants::planck::charge.as_unit()},
    {"plancktemperature", constants::planck::temperature.as_unit()},
    {"au", precise::distance::au},
    {"AU", precise::distance::au},
    {"ASU", precise::distance::au},
    {"astronomicalunit", precise::distance::au},
    {"astronomicunit", precise::distance::au},
    {"astronomicalunitBIPM2006", precise::distance::au_old},
    {"auold", precise::distance::au_old},
    {"ua", precise::distance::au_old},
    {"$", precise::currency},
    {"dollar", precise::currency},
    {"euro", precise::currency},
    {"yen", precise::currency},
    {"ruble", precise::currency},
    {"currency", precise::currency},
    {u8"\u00A2", precise_unit(0.01, precise::currency)},  // cent symbol
    {"\xA2", precise_unit(0.01, precise::currency)},  // cent symbol latin-1
    {u8"\u00A3", precise::currency},  // pound sign
    {"\xA3", precise::currency},  // pound sign latin-1
    {u8"\u00A4", precise::currency},  // currency sign
    {"\xA4", precise::currency},  // currency sign latin-1
    {u8"\u00A5", precise::currency},  // Yen sign
    {"\xA5", precise::currency},  // Yen sign latin-1
    {u8"\u0080", precise::currency},  // Euro sign
    {"\x80", precise::currency},  // Euro sign extended ascii
    {"count", precise::count},
    {"unit", precise::count},
    {"pair", precise_unit(2.0, precise::count)},
    {"dozen", precise_unit(12.0, precise::count)},
    {"octet", precise_unit(8.0, precise::count)},
    {"gross", precise_unit(144.0, precise::count)},
    {"half", precise_unit(0.5, precise::one)},
    {"quarter", precise_unit(0.25, precise::one)},
    {"third", precise_unit(1.0 / 3.0, precise::one)},
    {"fourth", precise_unit(0.25, precise::one)},
    {"fifth", precise_unit(0.2, precise::one)},
    {"sixth", precise_unit(1.0 / 6.0, precise::one)},
    {"eighth", precise_unit(0.125, precise::one)},
    {"tenth", precise_unit(0.1, precise::one)},
    {"cell", precise_unit(1.0, precise::count, commodities::cell)},
    {"{cells}", precise_unit(1.0, precise::count, commodities::cell)},
    // mainly to catch the commodity for using cell
    {"{#}", precise::count},
    {"[#]", precise::count},
    {"#", precise::count},
    {"number", precise::count},
    // {"ct", precise::count},
    {"pix", precise_unit(1.0, precise::count, commodities::pixel)},  // Pixel
    {"pixel", precise_unit(1.0, precise::count, commodities::pixel)},  // Pixel
    {"dot", precise_unit(1.0, precise::count, commodities::voxel)},  // Pixel
    {"voxel", precise_unit(1.0, precise::count, commodities::voxel)},  // Pixel
    {"item", precise::count},
    {"part", precise::count},
    {"ratio", precise::ratio},
    {"rat", precise::ratio},
    {"ERR", precise::error},
    {"ERROR", precise::error},
    {"error", precise::error},
    {"FEU", precise_unit(40.0 * 8.0 * 8.5, precise::ft.pow(3))},
    {"TEU", precise_unit(20.0 * 8.0 * 8.5, precise::ft.pow(3))},
    {"fortyfootequivalent", precise_unit(40.0 * 8.0 * 8.5, precise::ft.pow(3))},
    {"twentyfootequivalent",
     precise_unit(20.0 * 8.0 * 8.5, precise::ft.pow(3))},
    {"Gy", precise::Gy},
    {"gy", precise::Gy},
    {"GY", precise::Gy},
    {"gray", precise::Gy},
    {"Sv", precise::Sv},
    {"SV", precise::Sv},
    {"sievert", precise::Sv},
    {"sverdrup", precise_unit(1e6, precise::m.pow(3) / precise::s)},
    {"rem", precise::cgs::REM},
    {"[REM]", precise::cgs::REM},
    {"REM", precise::cgs::REM},
    {"radiationequivalentman", precise::cgs::REM},
    {"roentgenequivalentinman", precise::cgs::REM},
    {"Ky", precise::cgs::kayser},
    {"KY", precise::cgs::kayser},
    {"kayser", precise::cgs::kayser},
    {"Bi", precise::cgs::biot},
    {"BI", precise::cgs::biot},
    {"biot", precise::cgs::biot},
    {"abamp", precise::cgs::biot},
    {"abcoulomb", precise::cgs::biot* precise::s},
    {"abC", precise::cgs::biot* precise::s},
    {"statamp", precise::cgs::statC_charge / precise::s},
    {"stA", precise::cgs::statC_charge / precise::s},
    {"abA", precise::cgs::biot},
    {"St", precise::cgs::stokes},
    {"ST", precise::cgs::stokes},
    {"stoke", precise::cgs::stokes},
    {"Mx", precise::cgs::maxwell},
    {"MX", precise::cgs::maxwell},
    {"mX", precise::cgs::maxwell},
    {"maxwell", precise::cgs::maxwell},
    {"Oe", precise::cgs::oersted},
    {"OE", precise::cgs::oersted},
    {"oersted", precise::cgs::oersted},
    {"gilbert", precise::cgs::gilbert},
    {"Gb", precise::cgs::gilbert},
    {"Gi", precise::cgs::gilbert},
    {"p", precise::cgs::poise},
    {"cps", precise_unit(0.01, precise::cgs::poise)},
    // centipoise doesn't conflict with ps
    {"P", precise::cgs::poise},
    {"poise", precise::cgs::poise},
    {"rhe", precise_unit(10.0, (precise::Pa * precise::s).inv())},
    {"Ba", precise::cgs::barye},
    {"BA", precise::cgs::barye},
    {"barye", precise::cgs::barye},
    {"barie", precise::cgs::barye},
    {"D", precise::cgs::debye},
    {"debye", precise::cgs::debye},
    {"roentgen", precise::cgs::roentgen},
    {"r\xF6ntgen", precise::cgs::roentgen},
    {"parker", precise::cgs::roentgen},
    {u8"r\u00f6ntgen", precise::cgs::roentgen},
    {u8"ro\u0308ntgen", precise::cgs::roentgen},
    {u8"ro\u00A8ntgen", precise::cgs::roentgen},
    {"Roe", precise::cgs::roentgen},
    {"ROE", precise::cgs::roentgen},
    {"R", precise::cgs::roentgen},
    {"Lmb", precise::cgs::lambert},
    {"LMB", precise::cgs::lambert},
    {"lambert", precise::cgs::lambert},
    {"bril", precise_unit(1e-11, precise::cgs::lambert)},
    {"skot", precise_unit(1e-7, precise::cgs::lambert)},
    {"footlambert",
     precise_unit(1.0 / constants::pi, precise::cd / precise::ft.pow(2))},
    {"fl", precise_unit(1.0 / constants::pi, precise::cd / precise::ft.pow(2))},
    {"Lb", precise::cgs::lambert},
    {"langley", precise::cgs::langley},
    {"Ly", precise::cgs::langley},
    {"unitpole", precise::cgs::unitpole},
    {"sb", precise::cgs::stilb},
    {"SB", precise::cgs::stilb},
    {"stilb", precise::cgs::stilb},
    {"apostilb", precise_unit(1e-4 / constants::pi, precise::cgs::stilb)},
    {"asb", precise_unit(1e-4 / constants::pi, precise::cgs::stilb)},
    {"blondel", precise_unit(1e-4 / constants::pi, precise::cgs::stilb)},
    {"nit", precise::cd / precise::m.pow(2)},
    {"nt", precise::cd / precise::m.pow(2)},
    {"ph", precise::cgs::phot},
    {"PHT", precise::cgs::phot},
    {"pHT", precise::cgs::phot},
    {"phot", precise::cgs::phot},
    {"[RAD]", precise::cgs::RAD},
    {"RAD", precise::cgs::RAD},
    {"radiationabsorbeddose", precise::cgs::RAD},
    {"Hz", precise::Hz},
    {"HZ", precise::Hz},
    {"hertz", precise::Hz},
    {"rpm", precise::rpm},
    {"rps", precise_unit(constants::tau, precise::rad / precise::s)},
    {"r", precise_unit(constants::tau, precise::rad)},
    {"revolutions", precise_unit(constants::tau, precise::rad)},
    {"revolution", precise_unit(constants::tau, precise::rad)},
    {"rev", precise_unit(constants::tau, precise::rad)},
    {"revs", precise_unit(constants::tau, precise::rad)},
    {"rev/min", precise::rpm},
    {"rad/s", precise::rad / precise::s},
    {"kat", precise::kat},
    {"kats", precise::kat},
    {"KAT", precise::kat},
    {"katal", precise::kat},
    {"sr", precise::sr},
    {"SR", precise::sr},
    {"steradian", precise::sr},
    {"steradian-solidangle", precise::sr},
    {"$/MWh", precise::currency / precise::MWh},
    {"acre", precise::acre},
    {"acr", precise::acre},
    {"ac", precise::acre},
    {"acft", precise::acre* precise::us::foot},
    {"ac*ft", precise::acre* precise::us::foot},
    {"acre-foot", precise::acre* precise::us::foot},
    {"acrefoot", precise::acre* precise::us::foot},
    {"acre_us", precise::acre},
    {"acr_us", precise::acre},
    {"[ACR_US]", precise::acre},
    {"[ACR_BR]", precise::imp::acre},
    {"acr_br", precise::imp::acre},
    {"acre_br", precise::imp::acre},
    {"acres_br", precise::imp::acre},
    {"buildersacre", precise_unit(40000.0, precise::ft.pow(2))},
    {"Gasolineat15.5C", precise_unit(739.33, precise::kg / precise::m.pow(3))},
    {"rood", precise_unit(0.25, precise::imp::acre)},
    {"are", precise::area::are},
    {"ar", precise::area::are},
    {"AR", precise::area::are},
    {"hectare", precise::area::hectare},
    {"barn", precise::area::barn},
    {"b", precise::area::barn},
    {"BRN", precise::area::barn},
    {"ha", precise::area::hectare},
    // porous solid permeability
    {"darcy", precise_unit(9.869233e-13, precise::m.pow(2))},
    {"mW", precise::electrical::mW},
    {"milliwatt", precise::electrical::mW},
    {"puW", precise::electrical::puMW / precise::mega},
    {"puMW", precise::electrical::puMW},
    {"puMVA", precise::electrical::puMW},
    {"pumw", precise::electrical::puMW},
    {"pumegawatt", precise::electrical::puMW},
    {"puV", precise::electrical::puV},
    {"puvolt", precise::electrical::puV},
    {"puA", precise::electrical::puA},
    {"puamp", precise::electrical::puA},
    {"mA", precise::electrical::mA},
    {"milliamp", precise::electrical::mA},
    {"kV", precise::electrical::kV},
    {"kilovolt", precise::electrical::kV},
    {"abvolt", precise::cgs::abVolt},
    {"abV", precise::cgs::abVolt},
    {"statvolt", precise::cgs::statV},
    {"stV", precise::cgs::statV},
    {"erg", precise::cgs::erg},
    {"ERG", precise::cgs::erg},
    {"dyn", precise::cgs::dyn},
    {"DYN", precise::cgs::dyn},
    {"dyne", precise::cgs::dyn},
    {"pond", precise::gm::pond},
    {"$/gal", precise::currency / precise::gal},
    {"pu", precise::pu},
    {"perunit", precise::pu},
    {"flag", precise::iflag},
    {"eflag", precise::eflag},
    {"FLAG", precise::iflag},
    {"EFLAG", precise::eflag},
    {"puOhm", precise::pu* precise::ohm},
    {"puohm", precise::pu* precise::ohm},
    {"puHz", precise::electrical::puHz},
    {"puhertz", precise::electrical::puHz},
    {"hp", precise::hp},
    {"horsepower", precise::hp},
    {"horsepower_i", precise::hp},
    {"horsepower_br", precise::hp},
    {"[HP]", precise::hp},
    {"hpI", precise::hp},
    {"hp(I)", precise::hp},
    {"horsepower-mechanical", precise::hp},
    {"horsepowermechanical", precise::hp},
    {"mechanicalhorsepower", precise::hp},
    {"shafthorsepower", precise::hp},
    {"horsepower(mechanical)", precise::hp},
    {"horsepower(water)", precise::hp},
    {"waterhorsepower", precise::hp},
    {"horsepower(hydraulic)", precise::hp},
    {"horsepower(air)", precise::hp},
    {"hp(mechanical)", precise::hp},
    {"hp(water)", precise::hp},
    {"hp(hydraulic)", precise::hp},
    {"hp(air)", precise::hp},
    {"hpE", precise::power::hpE},
    {"hp(E)", precise::power::hpE},
    {"horsepower-electrical", precise::power::hpE},
    {"horsepower(electrical)", precise::power::hpE},
    {"horsepower(electric)", precise::power::hpE},
    {"electrichorsepower", precise::power::hpE},
    {"hp(electric)", precise::power::hpE},
    {"hpM", precise::power::hpM},
    {"hp_m", precise::power::hpM},
    {"hp(M)", precise::power::hpM},
    {"horsepower_m", precise::power::hpM},
    {"hpS", precise::power::hpS},
    {"hp(S)", precise::power::hpS},
    {"horsepower-steam", precise::power::hpS},
    {"horsepower(steam)", precise::power::hpS},
    {"horsepower(boiler)", precise::power::hpS},
    {"boilerhorsepower", precise::power::hpS},
    {"hp(boiler)", precise::power::hpS},
    {"mph", precise::mph},
    {"mileperhour", precise::mph},
    {"kph", precise::km / precise::hr},
    {"nauticalmile", precise::nautical::mile},
    {"nmile", precise::nautical::mile},
    {"nauticalmile_i", precise::nautical::mile},
    {"nauticalleague", precise::nautical::league},
    {"nauticalleage_i", precise::nautical::league},
    {"nauticalleague_i", precise::nautical::league},
    // this prevents some other issues with the string "br"
    {"br", precise::invalid},
    {"nmi", precise::nautical::mile},
    {"nmi_i", precise::nautical::mile},
    {"[NMI_I]", precise::nautical::mile},
    {"fth", precise::nautical::fathom},
    {"fathom", precise::nautical::fathom},
    {"fathom_i", precise::nautical::fathom},
    {"fathoms_i", precise::nautical::fathom},
    {"fth_i", precise::nautical::fathom},
    {"[FTH_I]", precise::nautical::fathom},
    {"kn", precise::nautical::knot},
    {"knot", precise::nautical::knot},
    {"knot_i", precise::nautical::knot},
    {"knots_i", precise::nautical::knot},
    {"kn_i", precise::nautical::knot},
    {"[KN_I]", precise::nautical::knot},
    {"knots", precise::nautical::knot},
    {"kts", precise::nautical::knot},
    {"fps", precise::ft / precise::s},
    {"fpm", precise::ft / precise::min},
    {"fph", precise::ft / precise::hr},
    {"mps", precise::m / precise::s},
    {"eV", precise::energy::eV},
    {"bev", precise_unit(1e9, precise::energy::eV)},
    {"EV", precise::energy::eV},
    {"Ry", precise_unit(13.60583, precise::energy::eV)},  // Rydberg
    {"electronvolt", precise::energy::eV},
    {"electronVolt", precise::energy::eV},
    {"cal", precise::cal},
    {"smallcalorie", precise::cal},
    {"Cal", precise::energy::kcal},
    {"CAL", precise::cal},
    {"[Cal]", precise::energy::kcal},
    {"[CAL]", precise::energy::kcal},
    {"nutritionlabelCalories", precise::energy::kcal},
    {"foodcalories", precise::energy::kcal},
    {"largecalories", precise::energy::kcal},
    {"kilogramcalories", precise::energy::kcal},
    {"calorie(nutritional)", precise::energy::cal_it},
    {"cal_[15]", precise::energy::cal_15},
    {u8"cal_15\u00B0C", precise::energy::cal_15},
    {u8"calorieat15\u00B0C", precise::energy::cal_15},
    {"caloriesat15C", precise::energy::cal_15},
    {"calories15C", precise::energy::cal_15},
    {"calorie15C", precise::energy::cal_15},
    {"cal_[20]", precise::energy::cal_20},
    {u8"calorieat20\u00B0C", precise::energy::cal_20},
    {"caloriesat20C", precise::energy::cal_20},
    {"calorie20C", precise::energy::cal_20},
    {"cals20C", precise::energy::cal_20},
    {"cal20C", precise::energy::cal_20},
    {"cals15C", precise::energy::cal_15},
    {"cal15C", precise::energy::cal_15},
    {u8"cal_20\u00B0C", precise::energy::cal_20},
    {"CAL_[15]", precise::energy::cal_15},
    {"CAL_[20]", precise::energy::cal_20},
    {"cal_m", precise::energy::cal_mean},
    {"meancalorie", precise::energy::cal_mean},
    {"cal_IT", precise::energy::cal_it},
    {"CAL_M", precise::energy::cal_mean},
    {"CAL_IT", precise::energy::cal_it},
    {"calorie_IT", precise::energy::cal_it},
    {"caloriesIT", precise::energy::cal_it},
    {"cal_th", precise::energy::cal_th},
    {"CAL_TH", precise::energy::cal_th},
    {"calorie_th", precise::energy::cal_th},
    {"cal", precise::cal},
    {"calorie", precise::cal},
    {"kcal", precise::energy::kcal},
    {"kCal", precise::energy::kcal},
    {"KCAL", precise::energy::kcal},
    {"Kcal", precise::energy::kcal},
    {"kilocalorie", precise::energy::kcal},
    {"btu", precise::energy::btu_it},
    {"Btu", precise::energy::btu_it},
    {"BTU", precise::energy::btu_it},
    {"Mbtu", precise_unit(1000.0, precise::energy::btu_it)},
    {"MBtu", precise_unit(1000.0, precise::energy::btu_it)},
    {"MBTU", precise_unit(1000.0, precise::energy::btu_it)},
    {"[Btu]", precise::energy::btu_th},
    {"[BTU]", precise::energy::btu_th},
    {"British thermal unit", precise::energy::btu_th},
    // this is for name matching with the UCUM standard
    {"Btu_39", precise::energy::btu_39},
    {"BTU_39", precise::energy::btu_39},
    {"BTU39F", precise::energy::btu_39},
    {u8"BTU39\u00B0F", precise::energy::btu_39},
    {u8"btu_39\u00B0F", precise::energy::btu_39},
    {"Btu_59", precise::energy::btu_59},
    {"BTU_59", precise::energy::btu_59},
    {"BTU59F", precise::energy::btu_59},
    {u8"BTU59\u00B0F", precise::energy::btu_59},
    {u8"btu_59\u00B0F", precise::energy::btu_59},
    {"Btu_60", precise::energy::btu_60},
    {"BTU_60", precise::energy::btu_60},
    {"BTU60F", precise::energy::btu_60},
    {u8"BTU60\u00B0F", precise::energy::btu_60},
    {u8"btu_60\u00B0F", precise::energy::btu_60},
    {"Btu_m", precise::energy::btu_mean},
    {"BTU_m", precise::energy::btu_mean},
    {"BTU_M", precise::energy::btu_mean},
    {"Btu_IT", precise::energy::btu_it},
    {"BTU_IT", precise::energy::btu_it},
    {"Btu_th", precise::energy::btu_th},
    {"[BTU_TH]", precise::energy::btu_th},
    {"BTU_th", precise::energy::btu_th},
    {"CHU", precise_unit(1899.0, precise::J)},
    {"TNT", precise::energy::ton_tnt},
    {"tontnt", precise::energy::ton_tnt},
    {"tonoftnt", precise::energy::ton_tnt},
    {"tonsoftnt", precise::energy::ton_tnt},
    {"tonsofTNT", precise::energy::ton_tnt},
    {"ton(explosives)", precise::energy::ton_tnt},
    {"kiloton", precise_unit(1000.0, precise::energy::ton_tnt)},
    {"megaton", precise_unit(1000000.0, precise::energy::ton_tnt)},
    {"gigaton", precise_unit(1000000000.0, precise::energy::ton_tnt)},
    {"kton", precise_unit(1000.0, precise::energy::ton_tnt)},
    {"Mton", precise_unit(1000000.0, precise::energy::ton_tnt)},
    {"Gton", precise_unit(1000000000.0, precise::energy::ton_tnt)},
    {"hartree", precise::energy::ton_tnt},
    {"boe", precise::energy::boe},
    {"boe", precise::energy::boe},
    {"foeb", precise::energy::foeb},
    {"fueloilequivalentof1barrel", precise::energy::foeb},
    {"fueloilequivalentof1kiloliter",
     precise::energy::foeb / precise::us::barrel* precise::m.pow(3)},
    {"CFM", precise::other::CFM},
    {"CFS", precise::ft.pow(3) / precise::s},
    {"cfm", precise::other::CFM},
    {"cfs", precise::ft.pow(3) / precise::s},
    {"cubicfeetperminute", precise::other::CFM},
    {"gps", precise::gal / precise::s},
    {"gpm", precise::gal / precise::min},
    {"gph", precise::gal / precise::hr},
    {"atm", precise::pressure::atm},
    {"ATM", precise::pressure::atm},
    {"atmos", precise::pressure::atm},
    {"atmosphere", precise::pressure::atm},
    {"standardatmosphere", precise::pressure::atm},
    {"stdatmosphere", precise::pressure::atm},
    {"stdatm", precise::pressure::atm},
    {"att", precise::pressure::att},
    {"at", precise::pressure::att},
    {"technicalatmosphere", precise::pressure::att},
    {"atmospheretechnical", precise::pressure::att},
    {"techatmosphere", precise::pressure::att},
    {"techatm", precise::pressure::att},
    {"ATT", precise::pressure::att},
    {"poundal", precise::av::poundal},
    {"pdl", precise::av::poundal},
    {"psi", precise::pressure::psi},
    {"psig", precise::pressure::psig},
    {"ksi", precise_unit(1000.0, precise::pressure::psi)},
    {"psia", precise::pressure::psi},
    {"poundpersquareinch",
     precise::pressure::psi},  // pressure not areal density
    {"poundspersquareinch", precise::pressure::psi},
    {"pound/squareinch", precise::pressure::psi},  // pressure not areal density
    {"pounds/squareinch", precise::pressure::psi},
    {"[PSI]", precise::pressure::psi},
    {"[psi]", precise::pressure::psi},
    {"inHg", precise::pressure::inHg},
    {"inchHg", precise::pressure::inHg},
    {"ftH2O", precise_unit(12.0, precise::pressure::inH2O)},
    {"footwater", precise_unit(12.0, precise::pressure::inH2O)},
    {"inH2O", precise::pressure::inH2O},
    {"inchH2O", precise::pressure::inH2O},
    {"inAq", precise::pressure::inH2O},
    {"in[Hg]", precise::pressure::inHg},
    {"in[H2O]", precise::pressure::inH2O},
    {"IN[HG]", precise::pressure::inHg},
    {"IN[H2O]", precise::pressure::inH2O},
    {"[in_i'Hg]", precise::pressure::inHg},
    {"[IN_I'HG]", precise::pressure::inHg},
    {"inchofmercury", precise::pressure::inHg},
    {"inchmercury", precise::pressure::inHg},
    {"inch{mercury}", precise::pressure::inHg},
    {"inchofmercury_i", precise::pressure::inHg},
    {"inchofmercurycolumn", precise::pressure::inHg},
    {"[in_i'H2O]", precise::pressure::inH2O},
    {"[IN_I'H2O]", precise::pressure::inH2O},
    {"inchwater", precise::pressure::inH2O},
    {"inch(international)ofwater", precise::pressure::inH2O},
    {"inchofwater", precise::pressure::inH2O},
    {"inchofwaterguage", precise::pressure::inH2O},
    {"inchofwater_i", precise::pressure::inH2O},
    {"inch{water}", precise::pressure::inH2O},
    {"inchofwatercolumn", precise::pressure::inH2O},
    {"iwg", precise::pressure::inH2O},
    {"mmHg", precise::pressure::mmHg},
    {"mm[Hg]", precise::pressure::mmHg},
    {"MM[HG]", precise::pressure::mmHg},
    {"cmHg", precise::ten* precise::pressure::mmHg},
    {"cm[Hg]", precise::ten* precise::pressure::mmHg},
    {"CM[HG]", precise::ten* precise::pressure::mmHg},
    {"mHg", precise::kilo* precise::pressure::mmHg},
    {"m*Hg", precise::kilo* precise::pressure::mmHg},
    {"m[Hg]", precise::kilo* precise::pressure::mmHg},
    {"M[HG]", precise::kilo* precise::pressure::mmHg},
    {"m[HG]", precise::kilo* precise::pressure::mmHg},
    {"metermercury", precise::kilo* precise::pressure::mmHg},
    {"meterofmercury", precise::kilo* precise::pressure::mmHg},
    {"meter{mercury}", precise::kilo* precise::pressure::mmHg},
    {"meter(mercury)", precise::kilo* precise::pressure::mmHg},
    {"metersofmercury", precise::kilo* precise::pressure::mmHg},
    {"meterofmercurycolumn", precise::kilo* precise::pressure::mmHg},
    {"metersofmercurycolumn", precise::kilo* precise::pressure::mmHg},
    {"mmH2O", precise::pressure::mmH2O},
    {"mm[H2O]", precise::pressure::mmH2O},
    {"MM[H2O]", precise::pressure::mmH2O},
    {"cmH2O", precise::ten* precise::pressure::mmH2O},
    {"cm[H2O]", precise::ten* precise::pressure::mmH2O},
    {"CM[H2O]", precise::ten* precise::pressure::mmH2O},
    {"mH2O", precise::kilo* precise::pressure::mmH2O},
    {"m*H2O", precise::kilo* precise::pressure::mmH2O},
    {"m[H2O]", precise::kilo* precise::pressure::mmH2O},
    {"M[H2O]", precise::kilo* precise::pressure::mmH2O},
    {"meterwater", precise::kilo* precise::pressure::mmH2O},
    {"meterofwater", precise::kilo* precise::pressure::mmH2O},
    {"metersofwater", precise::kilo* precise::pressure::mmH2O},
    {"meters{water}", precise::kilo* precise::pressure::mmH2O},
    {"meter{water}", precise::kilo* precise::pressure::mmH2O},
    {"meters(water)", precise::kilo* precise::pressure::mmH2O},
    {"meter(water)", precise::kilo* precise::pressure::mmH2O},
    {"meterofwatercolumn", precise::kilo* precise::pressure::mmH2O},
    {"metersofwatercolumn", precise::kilo* precise::pressure::mmH2O},
    {"torr", precise::pressure::torr},
    {"Torr", precise::pressure::torr},
    {"TORR", precise::pressure::torr},
    {"quad", precise::energy::quad},
    {"therm", precise::energy::therm_ec},
    {"thm", precise::energy::therm_ec},
    {"therm(EC)", precise::energy::therm_ec},
    {"thm_ec", precise::energy::therm_ec},
    {"therm_ec", precise::energy::therm_ec},
    {"therm_us", precise::energy::therm_us},
    {"thm_us", precise::energy::therm_us},
    {"therm_br", precise::energy::therm_br},
    {"thermie", precise::MTS::thermie},
    {"$/kWh", precise::currency / precise::kWh},
    {"kWh", precise::kWh},
    {"kwh", precise::kWh},
    {"Wh", precise::W* precise::h},
    {"kilowatthour", precise::kWh},
    {"MWh", precise::MWh},
    {"megawatthour", precise::MWh},
    {"M$", precise::other::MegaBuck},
    {"mil$", precise::other::MegaBuck},
    {"B$", precise::other::GigaBuck},
    {"bil$", precise::other::GigaBuck},
    {"L", precise::L},  // preferred US notation
    {"l", precise::L},
    {"LT", precise::L},
    {"liter", precise::L},
    {"litre", precise::L},
    {"mL", precise::mL},  // preferred US notation
    {"ml", precise::mL},
    {"gal", precise::gal},
    {"gal_us", precise::us::gallon},
    {"[GAL_US]", precise::us::gallon},
    {"gallon_us", precise::us::gallon},
    {"liquidgallon_us", precise::us::gallon},
    {"gal_wi", precise::us::dry::gallon},
    {"wigal", precise::us::dry::gallon},
    {"[GAL_WI]", precise::us::dry::gallon},
    {"drygallon_us", precise::us::dry::gallon},
    {"gallon-historical", precise::us::dry::gallon},
    {"firkin", precise_unit(9.0, precise::us::dry::gallon)},
    {"bbl", precise::us::barrel},
    {"barrel", precise::us::barrel},
    {"bbl(oil)", precise::us::barrel},
    {"barrel(oil)", precise::us::barrel},
    {"bbl_us", precise::us::barrel},
    {"[bbl_us]", precise::us::barrel},
    {"[BBL_US]", precise::us::barrel},
    {"barrel_us", precise::us::barrel},
    {"flbarrel_us", precise::us::flbarrel},
    {"fluidbarrel_us", precise::us::flbarrel},
    {"liquidbarrel_us", precise::us::flbarrel},
    {"flbarrel", precise::us::flbarrel},
    {"fluidbarrel", precise::us::flbarrel},
    {"liquidbarrel", precise::us::flbarrel},
    {"gal_can", precise::canada::gallon},
    {"gallon_can", precise::canada::gallon},
    {"tbsp_can", precise::canada::tbsp},
    {"tsp_can", precise::canada::tsp},
    {"tablespoon_can", precise::canada::tbsp},
    {"teaspoon_can", precise::canada::tsp},
    {"cup_can", precise::canada::cup},
    {"traditional_cup_can", precise::canada::cup_trad},
    {"drum", precise::volume::drum},
    {"gallon", precise::gal},
    {"hogshead", precise::us::hogshead},
    {"canada", precise_unit(1.4, precise::L)},
    {"tonregister", precise_unit(100.0, precise::ft.pow(3))},
    {"tonreg", precise_unit(100.0, precise::ft.pow(3))},
    {"registerton", precise_unit(100.0, precise::ft.pow(3))},
    {"waterton", precise_unit(224.0, precise::imp::gallon)},
    {"lb", precise::lb},
    {"LB", precise::lb},
    {"kip", precise::kilo* precise::lb},
    {"lb_tr", precise::troy::pound},
    {"[LB_TR]", precise::troy::pound},
    {"pound-troy", precise::troy::pound},
    {"pound", precise::lb},
    {"bag", precise_unit(96.0, precise::lb)},
    {"ton", precise::ton},
    {"ton(short)", precise::ton},
    {"t", precise::mass::tonne},  // metric tonne
    {"mt", precise::mass::tonne},  // metric tonne
    {"ton_m", precise::mass::tonne},
    {"tonne", precise::mass::tonne},
    {"TNE", precise::mass::tonne},
    {"Da", precise::mass::Da},
    {"dalton", precise::mass::Da},
    {"u", precise::mass::u},
    {"amu", precise::mass::u},
    {"AMU", precise::mass::u},
    {"unifiedatomicmassunit", precise::mass::u},
    {"atomicmassunit", precise::mass::u},
    {"longton", precise_unit(2240.0, precise::lb)},
    {"tonc", precise::energy::tonc},  // ton cooling
    {"ton(refrigeration)", precise::energy::tonc},  // ton cooling
    {"tonofrefrigeration", precise::energy::tonc},  // ton cooling
    {"tonsofrefrigeration", precise::energy::tonc},  // ton cooling
    {"refrigerationton", precise::energy::tonc},  // ton cooling
    {"ton(cooling)", precise::energy::tonc},  // ton cooling
    {"ton{refrigeration}", precise::energy::tonc},  // ton cooling
    {"ton{cooling}", precise::energy::tonc},  // ton cooling
    {"tonhour", precise::energy::tonhour},
    {"tonhour(refrigeration)", precise::energy::tonhour},
    {"tonhour{refrigeration}", precise::energy::tonhour},
    {"RT", precise::energy::tonc},  // ton cooling
    {"TR", precise::energy::tonc},  // ton cooling
    // {"tons", precise::energy::tonc* precise::s},
    {"tonh", precise::energy::tonc* precise::hr},
    {"angstrom", precise::distance::angstrom},
    {u8"\u00C5ngstr\u00F6m", precise::distance::angstrom},
    {"\xE5ngstr\xF6m", precise::distance::angstrom},
    {u8"\u00E5ngstr\u00F6m", precise::distance::angstrom},
    {"Ao", precise::distance::angstrom},
    {"AO", precise::distance::angstrom},
    {u8"\u00C5", precise::distance::angstrom},
    {u8"A\u02DA", precise::distance::angstrom},
    {"\xC5", precise::distance::angstrom},
    {u8"\u212B", precise::distance::angstrom},  // unicode
    {"bps", precise::bit / precise::s},
    {"baud", precise::bit / precise::s},
    {"Bd", precise::bit / precise::s},
    {"BD", precise::bit / precise::s},
    {"bit", precise::bit},
    {"BIT", precise::bit},
    {"bit_s", precise::data::bit_s},
    {"bit-s", precise::data::bit_s},
    {"BIT_S", precise::data::bit_s},
    {"bit-logarithmic", precise::data::bit_s},
    {"bitlogarithmic", precise::data::bit_s},
    {"logbit", precise::data::bit_s},
    // b is for unit barn
    // B is for bel
    {"Bps", precise::B / precise::s},
    {"byte", precise::B},
    {"By", precise::B},
    {"BY", precise::B},
    {"kB", precise::kB},
    {"MB", precise::MB},
    {"GB", precise::GB},
    {"floatingpointoperation", precise::computation::flop},
    {"flops", precise::computation::flops},
    {"mips", precise::computation::mips},
    {"bu", precise::us::dry::bushel},
    {"bushel", precise::us::dry::bushel},
    {"oz fl", precise::us::floz},
    {"ozfl", precise::us::floz},
    {"fl oz", precise::us::floz},
    {"floz", precise::us::floz},
    {"floz_us", precise::us::floz},
    {"foz_us", precise::us::floz},
    {"foz", precise::us::floz},
    {"[FOZ_US]", precise::us::floz},
    {"fluidounce", precise::us::floz},
    {"fluidounce_us", precise::us::floz},
    {"fluiddram", precise_unit(1.0 / 8.0, precise::us::floz)},
    {"fluiddram_us", precise_unit(1.0 / 8.0, precise::us::floz)},
    {"liquidounce", precise::us::floz},
    {"liquidounce_us", precise::us::floz},
    {"fdr_us", precise::us::dram},
    {"[FDR_US]", precise::us::dram},
    {"fluiddram_us", precise::us::dram},
    {"liquiddram_us", precise::us::dram},
    {"min_us", precise::us::minim},
    {"[MIN_US]", precise::us::minim},
    {"minim_us", precise::us::minim},
    {"ouncefl", precise::us::floz},
    {"fluidounce", precise::us::floz},
    {"fluidoz", precise::us::floz},
    {"liquidounce", precise::us::floz},
    {"liquidoz", precise::us::floz},
    {"oz", precise::oz},
    {"OZ", precise::oz},
    {u8"\u2125", precise::oz},
    {"gr", precise::i::grain},
    {"[GR]", precise::i::grain},
    {"grain", precise::i::grain},
    {"oz_av", precise::av::ounce},
    {"oz_i", precise::av::ounce},
    {"[OZ_AV]", precise::av::ounce},
    {"pound_i", precise::av::pound},
    {"pound_av", precise::av::pound},
    {"lb_av", precise::av::pound},
    {"[LB_AV]", precise::av::pound},
    {"dr", precise::us::dram},  // this most commonly implies volume vs weight
    {"dram", precise::us::dram},
    {"dr_av", precise::av::dram},
    {"dr_i", precise::av::dram},
    {"dram_av", precise::av::dram},
    {"dram_i", precise::av::dram},
    {"[DR_AV]", precise::av::dram},
    {"drammassunit", precise::av::dram},
    {"scwt", precise::av::hundredweight},
    {"scwt_av", precise::av::hundredweight},
    {"[SCWT_AV]", precise::av::hundredweight},
    {"shorthundredweight", precise::av::hundredweight},
    {"lcwt_av", precise::av::longhundredweight},
    {"[LCWT_AV]", precise::av::longhundredweight},
    {"longhundredweight", precise::av::longhundredweight},
    {"quintal", precise::mass::quintal},
    {"quintal_m", precise::mass::quintal},
    {"cwt", precise::av::hundredweight},
    {"cwt_us", precise::av::hundredweight},
    {"cwt_br", precise::av::longhundredweight},
    {"hundredweight", precise::av::hundredweight},
    {"hundredweight_us", precise::av::hundredweight},
    {"hundredweight_br", precise::av::longhundredweight},
    {"hundredweight(short)", precise::av::hundredweight},
    {"hundredweight(long)", precise::av::longhundredweight},
    {"assayton", precise::mass::ton_assay},
    {"tonassay", precise::mass::ton_assay},
    {"ton(assayUS)", precise::mass::ton_assay},
    {"ton(assay)", precise::mass::ton_assay},
    {"ton(assay)_br", precise::mass::longton_assay},
    {"longassayton", precise::mass::longton_assay},
    {"stone_av", precise::av::stone},
    {"stone_us", precise_unit(0.125, precise::av::hundredweight)},
    {"[STONE_AV]", precise::av::stone},
    {"ston_av", precise::av::ton},
    {"[STON_AV]", precise::av::ton},
    {"shortton", precise::av::ton},
    {"shortton_us", precise::av::ton},
    {"ton_us", precise::av::ton},
    {"ton_av", precise::av::ton},
    {"stone", precise::av::stone},
    {"stone_br", precise::av::stone},
    {"lton_av", precise::av::longton},
    {"[LTON_AV]", precise::av::longton},
    {"longton", precise::av::longton},
    {"longton_av", precise::av::longton},
    {"ton(long)", precise::av::longton},
    {"longton_br", precise::av::longton},
    {"ton_br", precise::av::longton},
    {"oz_tr", precise::troy::oz},
    {"[OZ_TR]", precise::troy::oz},
    {"ounce_tr", precise::troy::oz},
    {"pound_tr", precise::troy::pound},
    {"lb_tr", precise::troy::pound},
    {"pwt_tr", precise::troy::pennyweight},
    {"pwt", precise::troy::pennyweight},
    {"[PWT_TR]", precise::troy::pennyweight},
    {"pennyweight_tr", precise::troy::pennyweight},
    {"pennyweight", precise::troy::pennyweight},
    {"sc_ap", precise::apothecaries::scruple},
    {"[SC_AP]", precise::apothecaries::scruple},
    {"scruple", precise::apothecaries::scruple},
    {"scruple_ap", precise::apothecaries::scruple},
    {u8"\u2108", precise::apothecaries::scruple},
    {"dr_ap", precise::apothecaries::drachm},
    {u8"\u0292", precise::apothecaries::drachm},
    {"dram_ap", precise::apothecaries::drachm},
    {"[DR_AP]", precise::apothecaries::drachm},
    {"oz_ap", precise::apothecaries::ounce},
    {"[OZ_AP]", precise::apothecaries::ounce},
    {"ounce_ap", precise::apothecaries::ounce},
    {"lb_ap", precise::apothecaries::pound},
    {"lbsap", precise::apothecaries::pound},
    {"[LB_AP]", precise::apothecaries::pound},
    {"pound_ap", precise::apothecaries::pound},
    {"oz_m", precise::apothecaries::metric_ounce},
    {"[OZ_M]", precise::apothecaries::metric_ounce},
    {"ounce_m", precise::apothecaries::metric_ounce},
    {"[car_m]", precise::metric::carat},
    {"ct_m", precise::metric::carat},
    {"[CAR_M]", precise::metric::carat},
    {"carat_m", precise::metric::carat},
    {"photometriccarat", precise::metric::carat},
    {"car_Au", precise_unit{1.0 / 24.0, precise::one, commodities::gold}},
    {"carau", precise_unit{1.0 / 24.0, precise::one, commodities::gold}},
    {"[CAR_AU]", precise_unit{1.0 / 24.0, precise::one, commodities::gold}},
    {"caratofgoldalloys",
     precise_unit{1.0 / 24.0, precise::one, commodities::gold}},
    {"ounce", precise::oz},
    {"ounce_av", precise::av::ounce},
    {"ounce_i", precise::av::ounce},
    {"g", precise::g},
    {"gm", precise::g},
    {"gamma", precise::micro* precise::g},
    {u8"\u1D6FE", precise::micro* precise::g},
    {"gamma{mass}", precise::micro* precise::g},
    {"gamma(mass)", precise::micro* precise::g},
    {"gamma{volume}", precise::micro* precise::L},
    {"gamma(volume)", precise::micro* precise::L},
    {"lambda{volume}", precise::micro* precise::L},
    {"lambda(volume)", precise::micro* precise::L},
    {"gamma(geo)", precise::nano* precise::T},  // two different uses of gamma
    {"gamma{geo}", precise::nano* precise::T},  // two different uses of gamma
    {"gf", precise::g* constants::g0.as_unit()},
    {"gravity", constants::g0.as_unit()},  // force of gravity
    {"geopotential", constants::g0.as_unit()},  // force of gravity
    {"gp", constants::g0.as_unit()},  // force of gravity
    {"force", constants::g0.as_unit()},  // force of gravity
    {"frc", constants::g0.as_unit()},  // force of gravity
    {"kp", precise::kilo* precise::gm::pond},
    // this is probably more common than kilopoise
    {"kipf", precise::kilo* precise::lbf},
    {"kipforce", precise::kilo* precise::lbf},
    {"tonforce_m", precise::mass::tonne* constants::g0.as_unit()},
    {"tf", precise::mass::tonne* constants::g0.as_unit()},
    {"tonforce(long)", precise::av::longton* constants::g0.as_unit()},
    {"tonforce(short)", precise::av::ton* constants::g0.as_unit()},
    {"tonforce_us", precise::av::ton* constants::g0.as_unit()},
    {"tonforce_br", precise::av::longton* constants::g0.as_unit()},
    {"tonf_us", precise::av::ton* constants::g0.as_unit()},
    {"tonf_br", precise::av::longton* constants::g0.as_unit()},
    {"hyl", precise::gm::hyl},
    {"GF", precise::g* constants::g0.as_unit()},  // gram-force vs GF
    {"sn", precise::MTS::sthene},
    {"sthene", precise::MTS::sthene},
    {"pz", precise::MTS::pieze},
    {"pieze", precise::MTS::pieze},
    {"lbf_av", precise::av::lbf},
    {"lbf_us", precise::av::lbf},
    {"[LBF_AV]", precise::av::lbf},
    {"lbf", precise::lbf},
    {"ozf", precise::av::ozf},
    {"poundforce_us", precise::lbf},
    {"slug", precise::av::slug},
    {"gram", precise::g},
    {"mg", precise::mg},
    {"mcg", precise::micro* precise::g},
    {"milligram", precise::mg},
    {"carat", precise_unit(200.0, precise::mg)},
    {"karat", precise_unit(200.0, precise::mg)},
    {"ct", precise_unit(200.0, precise::mg)},
    // {"kt", precise_unit(200.0, precise::mg)},
    {"cup", precise::us::cup},
    {"cup_us", precise::us::cup},
    {"[CUP_US]", precise::us::cup},
    {"cupUScustomary", precise::us::cup},
    {"tsp", precise::us::tsp},
    {"tsp_us", precise::us::tsp},
    {"[TSP_US]", precise::us::tsp},
    {"teaspoon", precise::us::tsp},
    {"teaspoon_us", precise::us::tsp},
    {"tbsp", precise::us::tbsp},
    {"tblsp", precise::us::tbsp},
    {"tbl", precise::us::tbsp},
    {"tbs", precise::us::tbsp},
    {"tbs_us", precise::us::tbsp},
    {"[TBS_US]", precise::us::tbsp},
    {"tablespoon", precise::us::tbsp},
    {"tablespoon_us", precise::us::tbsp},
    {"smidgen", precise_unit(1.0 / 32.0, precise::us::tsp)},
    {"pinch", precise_unit(1.0 / 24.0, precise::us::tsp)},
    {"dash", precise_unit(1.0 / 16.0, precise::us::tsp)},
    {"tad", precise_unit(1.0 / 8.0, precise::us::tsp)},
    {"cup_m", precise::metric::cup},
    {"[CUP_M]", precise::metric::cup_uslegal},
    {"[cup_m]", precise::metric::cup_uslegal},  // ucum definitions I think it
                                                // is wrong there
    {"cupUSlegal", precise::metric::cup_uslegal},
    {"tsp_m", precise::metric::tsp},
    {"[TSP_M]", precise::metric::tsp},
    {"teaspoon-metric", precise::metric::tsp},
    {"teaspoon_m", precise::metric::tsp},
    {"tbs_m", precise::metric::tbsp},
    {"tbsm", precise::metric::tbsp},
    {"[TBS_M]", precise::metric::tbsp},
    {"tablespoon-metric", precise::metric::tbsp},
    {"tablespoon_m", precise::metric::tbsp},
    {"foz_m", precise::metric::floz},
    {"[FOZ_M]", precise::metric::floz},
    {"fluidounce-metric", precise::metric::floz},
    {"fluidounce_m", precise::metric::floz},
    {"liquidounce-metric", precise::metric::floz},
    {"liquidounce_m", precise::metric::floz},
    {"quart", precise::us::quart},
    {"qt", precise::us::quart},
    {"QT", precise::us::quart},
    {"qt_us", precise::us::quart},
    {"[QT_US]", precise::us::quart},
    {"quart_us", precise::us::quart},
    {"pt", precise::us::pint},
    {"PT", precise::us::pint},
    {"pint", precise::us::pint},
    {"pint_us", precise::us::pint},
    {"pt_us", precise::us::pint},
    {"[PT_US]", precise::us::pint},
    {"gill", precise::us::gill},
    {"gi", precise::us::gill},
    {"gill_us", precise::us::gill},
    {"gil_us", precise::us::gill},
    {"[GIL_US]", precise::us::gill},
    {"pk", precise::us::dry::peck},
    {"pk_us", precise::us::dry::peck},
    {"[PK_US]", precise::us::dry::peck},
    {"peck", precise::us::dry::peck},
    {"peck_us", precise::us::dry::peck},
    {"bu_us", precise::us::dry::bushel},
    {"bsh_us", precise::us::dry::bushel},
    {"[BU_US]", precise::us::dry::bushel},
    {"bushel_us", precise::us::dry::bushel},
    {"dqt", precise::us::dry::quart},
    {"dqt_us", precise::us::dry::quart},
    {"dryqt", precise::us::dry::quart},
    {"dryqt_us", precise::us::dry::quart},
    {"[DQT_US]", precise::us::dry::quart},
    {"dryquart_us", precise::us::dry::quart},
    {"dryquart", precise::us::dry::quart},
    {"dpt_us", precise::us::dry::pint},
    {"[DPT_US]", precise::us::dry::pint},
    {"drypint_us", precise::us::dry::pint},
    {"drypint", precise::us::dry::pint},
    {"drypt", precise::us::dry::pint},
    {"qt_br", precise::imp::quart},
    {"[QT_BR]", precise::imp::quart},
    {"quart_br", precise::imp::quart},
    {"pt_br", precise::imp::pint},
    {"[PT_BR]", precise::imp::pint},
    {"pint_br", precise::imp::pint},
    {"gil_br", precise::imp::gill},
    {"[GIL_BR]", precise::imp::gill},
    {"gill_br", precise::imp::gill},
    {"gi_br", precise::imp::gill},
    {"pk_br", precise::imp::peck},
    {"[PK_BR]", precise::imp::peck},
    {"peck_br", precise::imp::peck},
    {"bu_br", precise::imp::bushel},
    {"bsh_br", precise::imp::bushel},
    {"[BU_BR]", precise::imp::bushel},
    {"bushel_br", precise::imp::bushel},
    {"foz_br", precise::imp::floz},
    {"floz_br", precise::imp::floz},
    {"[FOZ_BR]", precise::imp::floz},
    {"fluidounce_br", precise::imp::floz},
    {"liquidounce_br", precise::imp::floz},
    {"fdr_br", precise::imp::dram},
    {"[FDR_BR]", precise::imp::dram},
    {"fluiddram_br", precise::imp::dram},
    {"liquiddram_br", precise::imp::dram},
    {"min_br", precise::imp::minim},
    {"[MIN_BR]", precise::imp::minim},
    {"minim_br", precise::imp::minim},
    {"barrel_br", precise::imp::barrel},
    {"bbl_br", precise::imp::barrel},
    {"tablespoon_br", precise::imp::tbsp},
    {"teaspoon_br", precise::imp::tsp},
    {"tbsp_br", precise::imp::tbsp},
    {"tsp_br", precise::imp::tsp},
    {"cup_br", precise::imp::cup},
    {"EER", precise::energy::EER},
    {"ppv", precise::one},  // parts per volume
    {"ppth", precise::other::ppm* precise::kilo},
    {"PPTH", precise::other::ppm* precise::kilo},
    {"ppm", precise::other::ppm},
    {"ppmv", precise::other::ppm},
    {"PPM", precise::other::ppm},
    {"[PPM]", precise::other::ppm},
    {"partspermillion", precise::other::ppm},
    {"ppb", precise::other::ppb},
    {"ppbv", precise::other::ppb},
    {"PPB", precise::other::ppb},
    {"[PPB]", precise::other::ppb},
    {"partsperbillion", precise::other::ppb},
    {"ppt", precise::other::ppb* precise::milli},
    {"pptv", precise::other::ppb* precise::milli},
    {"pptr", precise::other::ppb* precise::milli},
    {"PPTR", precise::other::ppb* precise::milli},
    {"[PPTR]", precise::other::ppb* precise::milli},
    {"partspertrillion", precise::other::ppb* precise::milli},
    {"ppq", precise::other::ppb* precise::micro},
    {"ppqv", precise::other::ppb* precise::micro},
    {"partsperquadrillion", precise::other::ppb* precise::micro},
    {"[lne]", precise::typographic::american::line},
    {"[LNE]", precise::typographic::american::line},
    {"line", precise::typographic::american::line},
    {"line_br", precise::typographic::american::line},
    {"pnt", precise::typographic::american::point},
    {"[PNT]", precise::typographic::american::point},
    {"point", precise::typographic::american::point},
    {"pca", precise::typographic::american::pica},
    {"[PCA]", precise::typographic::american::pica},
    {"pica", precise::typographic::american::pica},
    {"twip", precise::typographic::american::twip},
    {"printer'spoint", precise::typographic::printers::point},
    {"printerspoint", precise::typographic::printers::point},
    {"pointprinter", precise::typographic::printers::point},
    {"pnt_pr", precise::typographic::printers::point},
    {"pntpr", precise::typographic::printers::point},
    {"[PNT_PR]", precise::typographic::printers::point},
    {"printer'spica", precise::typographic::printers::pica},
    {"printerspica", precise::typographic::printers::pica},
    {"picaprinter", precise::typographic::printers::pica},
    {"pca_pr", precise::typographic::printers::pica},
    {"pcapr", precise::typographic::printers::pica},
    {"[PCA_PR]", precise::typographic::printers::pica},
    {"pied", precise::typographic::french::pied},
    {"[PIED]", precise::typographic::french::pied},
    {"pouce", precise::typographic::french::pouce},
    {"[POUCE]", precise::typographic::french::pouce},
    {"ligne", precise::typographic::french::ligne},
    {"[LIGNE]", precise::typographic::french::ligne},
    {"didot", precise::typographic::french::didot},
    {"didotpoint", precise::typographic::french::didot},
    {"[DIDOT]", precise::typographic::french::didot},
    {"cicero", precise::typographic::french::cicero},
    {"[CICERO]", precise::typographic::french::cicero},
    {"tex", precise::textile::tex},
    {"texes", precise::textile::tex},
    {"TEX", precise::textile::tex},
    {"denier", precise::textile::denier},
    {"denier(linearmassdensity)", precise::textile::denier},
    {"den", precise::textile::denier},
    {"[DEN]", precise::textile::denier},
    {"span(cloth)", precise::textile::span},
    {"finger(cloth)", precise::textile::finger},
    {"nail(cloth)", precise::textile::nail},
    {"PRU", precise::clinical::pru},
    {"peripheralvascularresistanceunit", precise::clinical::pru},
    {"peripheralresistanceunit", precise::clinical::pru},
    {"potentialvorticityunit",
     precise_unit(
         detail::unit_data(2, -1, -1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0),
         1e-6)},
    {"PVU",
     precise_unit(
         detail::unit_data(2, -1, -1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0),
         1e-6)},
    // unit of thermal resistance used in describing the insulating value of
    // clothing; the amount of thermal resistance needed to maintain in comfort
    // a resting subject in a normally ventilated room (air movement 10 cm/sec)
    // at a temperature of 20 degrees C and a humidity less than 50%
    {"clo", precise_unit(1.55e-1, precise::K* precise::m.pow(2) / precise::W)},
    {"[MET]", precise::clinical::met},
    {"MET", precise::clinical::met},
    {"metabolicEquivalentofTask", precise::clinical::met},
    {"metabolicequivalents", precise::clinical::met},
    {"[wood'U]", precise::clinical::woodu},
    {"HRU", precise::clinical::woodu},
    {"hybridreferenceunit", precise::clinical::woodu},
    {"[WOOD'U]", precise::clinical::woodu},
    {"woodunit", precise::clinical::woodu},
    {"dpt", precise::clinical::diopter},
    {"diop", precise::clinical::diopter},
    {"[DIOP]", precise::clinical::diopter},
    {"diopter", precise::clinical::diopter},
    {"dioptre", precise::clinical::diopter},
    {"diopter(lens)", precise::clinical::diopter},
    {"[p'diop]", precise::clinical::prism_diopter},
    {"pdiop", precise::clinical::prism_diopter},
    {"[P'DIOP]", precise::clinical::prism_diopter},
    {"PD", precise::clinical::prism_diopter},
    {"prismdiopter", precise::clinical::prism_diopter},
    {"prismdiopter(magnifyingpower)", precise::clinical::prism_diopter},
    {"%slope", precise::clinical::prism_diopter},
    {"%[slope]", precise::clinical::prism_diopter},
    {"%[SLOPE]", precise::clinical::prism_diopter},
    {"percentofslope", precise::clinical::prism_diopter},
    {"drp", precise::clinical::drop},
    {"[drp]", precise::clinical::drop},
    {"[DRP]", precise::clinical::drop},
    {"drop", precise::clinical::drop},
    {"drop(1/20milliliter)", precise::clinical::drop},
    {"drop-metric", precise::clinical::drop},
    {"drop_m", precise::clinical::drop},
    {"drop-metric(1/20mL)", precise::clinical::drop},
    {"Ch", precise::clinical::charriere},
    {"[CH]", precise::clinical::charriere},
    {"french(cathetergauge)", precise::clinical::charriere},
    {"mesh_i", precise::clinical::mesh},
    {"[MESH_I]", precise::clinical::mesh},
    {"mesh", precise::clinical::mesh},
    {"meshes", precise::clinical::mesh},
    {"smoot", precise::distance::smoot},
    {"[SMOOT]", precise::distance::smoot},
    {"ASD", precise::special::ASD},
    {"[m/s2/Hz^(1/2)]", precise::special::ASD},
    {"[M/s2/HZ^(1/2)]", precise::special::ASD},
    {"Hz^(1/2)", precise::special::rootHertz},
    {"HZ^(1/2)", precise::special::rootHertz},
    {"squarerootofhertz", precise::special::rootHertz},
    {"squareroothertz", precise::special::rootHertz},
    {"roothertz", precise::special::rootHertz},
    // capitalized version is needed since this is also a generated unit
    {"rootHertz", precise::special::rootHertz},
    {"B", precise::data::byte},
    {"bel", precise::log::bel},
    {"dB", precise::log::dB},
    {"decibel", precise::log::dB},
    {"decibels", precise::log::dB},
    {"DB", precise::log::dB},
    {"Np", precise::log::neper},
    {"NEP", precise::log::neper},
    {"nepers", precise::log::neper},
    {"neper", precise::log::neper},
    {"SPL", precise_unit(2 * 1e-5, precise::pascal)},
    {"soundpressurelevel", precise_unit(2 * 1e-5, precise::pascal)},
    {"soundpressure", precise_unit(2 * 1e-5, precise::pascal)},
    {"BSPL", precise::log::B_SPL},
    {"B[SPL]", precise::log::B_SPL},
    {"B(SPL)", precise::log::B_SPL},
    {"belSPL", precise::log::B_SPL},
    {"belsoundpressure", precise::log::B_SPL},
    {"dB[SPL]", precise::log::dB_SPL},
    {"decibel(SPL)", precise::log::dB_SPL},
    {"DB[SPL]", precise::log::dB_SPL},
    {"DBSPL", precise::log::dB_SPL},
    {"B[V]", precise::log::B_V},
    {"B(V)", precise::log::B_V},
    {"BV", precise::log::B_V},
    {"Bv", precise_unit(0.775, precise::log::B_V)},
    {"Bvolt", precise::log::B_V},
    {"belvolt", precise::log::B_V},
    {"belV", precise::log::B_V},
    {"dB[V]", precise::log::dB_V},
    {"decibelV", precise::log::dB_V},
    {"dB(V)", precise::log::dB_V},
    {"DB[V]", precise::log::dB_V},
    {"B[mV]", precise::log::B_mV},
    {"B(mV)", precise::log::B_mV},
    {"belmillivolt", precise::log::B_mV},
    {"belmV", precise::log::B_mV},
    {"dB[mV]", precise::log::dB_mV},
    {"dB(mV)", precise::log::dB_mV},
    {"B[MV]", precise::log::B_mV},
    {"DB[MV]", precise::log::dB_mV},
    {"B[uV]", precise::log::B_uV},
    {"dB[uV]", precise::log::dB_uV},
    {"decibelmicrovolt", precise::log::dB_uV},
    {"B[UV]", precise::log::B_uV},
    {"B(uV)", precise::log::B_uV},
    {"BuV", precise::log::B_uV},
    {"belmicrovolt", precise::log::B_uV},
    {"DB[UV]", precise::log::dB_uV},
    {"B[10.nV]", precise::log::B_10nV},
    {"B(10nV)", precise::log::B_10nV},
    {"bel10nanovolt", precise::log::B_10nV},
    {"dB[10.nV]", precise::log::dB_10nV},
    {"dB[10*nV]", precise::log::dB_10nV},
    {"decibel10nanovolt", precise::log::B_10nV},
    {"B[10*NV]", precise::log::B_10nV},
    {"B[10*nV]", precise::log::B_10nV},
    {"DB[10*NV]", precise::log::dB_10nV},
    {"B[W]", precise::log::B_W},
    {"B(W)", precise::log::B_W},
    {"belwatt", precise::log::B_W},
    {"belW", precise::log::B_W},
    {"dB[W]", precise::log::dB_W},
    {"DB[W]", precise::log::dB_W},
    {"B[kW]", precise::log::B_kW},
    {"B(kW)", precise::log::B_kW},
    {"DB[kW]", precise::log::dB_kW},
    {"B[KW]", precise::log::B_kW},
    {"belkilowatt", precise::log::B_kW},
    {"belkW", precise::log::B_kW},
    {"DB[KW]", precise::log::dB_kW},
    {"dBZ", precise::log::dBZ},
    {"BZ", precise::log::BZ},
    {"[S]", precise::laboratory::svedberg},
    {"svedbergunit", precise::laboratory::svedberg},
    {"HPF", precise::laboratory::HPF},
    {"highpowerfield", precise::laboratory::HPF},
    {"LPF", precise::laboratory::LPF},
    {"lowpowerfield", precise::laboratory::LPF},
    {"[arb'U]", precise::laboratory::arbU},
    {"[ARB'U]", precise::laboratory::arbU},
    {"[IU]", precise::laboratory::IU},
    {"[iU]", precise::laboratory::IU},
    {"dobson",
     precise_unit(446.2, precise::micro* precise::mol / precise::m.pow(2))},
    {"DU",
     precise_unit(446.2, precise::micro* precise::mol / precise::m.pow(2))},
    {"st", precise::volume::stere},
    {"stere", precise::volume::stere},
    {"STR", precise::volume::stere},
    {"[hnsf'U]", precise::clinical::hounsfield},
    {"[HNSF'U]", precise::clinical::hounsfield},
    {"[PFU]", precise::laboratory::PFU},
    {"PFU", precise::laboratory::PFU},
    {"plaqueformingunits", precise::laboratory::PFU},
    {"[Lf]", precise::laboratory::Lf},
    {"[LF]", precise::laboratory::Lf},
    {"[IR]", precise::laboratory::IR},
    {"[IR]", precise::laboratory::IR},
    {"50%tissuecultureinfectiousdose",
     precise_unit(0.69, precise::laboratory::PFU)},
    {"50%cellcultureinfectiousdose",
     precise_unit(0.69, precise::laboratory::PFU, commodities::cell)},
    {"50%embryoinfectiousdose",
     precise_unit(0.69, precise::laboratory::PFU, commodities::embryo)},
    {"TCID50", precise_unit(0.69, precise::laboratory::PFU)},
    {"CCID50", precise_unit(0.69, precise::laboratory::PFU, commodities::cell)},
    {"EID50",
     precise_unit(0.69, precise::laboratory::PFU, commodities::embryo)},
    {"[hp'_X]", precise_unit(1.0, precise::log::neglog10)},
    {"[HP'_X]", precise_unit(1.0, precise::log::neglog10)},
    {"[hp'_C]", precise_unit(1.0, precise::log::neglog100)},
    {"[HP'_C]", precise_unit(1.0, precise::log::neglog100)},
    {"[hp'_M]", precise_unit(1.0, precise::log::neglog1000)},
    {"[HP'_M]", precise_unit(1.0, precise::log::neglog1000)},
    {"[hp'_Q]", precise_unit(1.0, precise::log::neglog50000)},
    {"[HP'_Q]", precise_unit(1.0, precise::log::neglog50000)},
    {"[hp_X]",
     precise_unit(1.0, precise::log::neglog10, commodities::Hahnemann)},
    {"[HP_X]",
     precise_unit(1.0, precise::log::neglog10, commodities::Hahnemann)},
    {"[hp_C]",
     precise_unit(1.0, precise::log::neglog100, commodities::Hahnemann)},
    {"[HP_C]",
     precise_unit(1.0, precise::log::neglog100, commodities::Hahnemann)},
    {"[hp_M]",
     precise_unit(1.0, precise::log::neglog1000, commodities::Hahnemann)},
    {"[HP_M]",
     precise_unit(1.0, precise::log::neglog1000, commodities::Hahnemann)},
    {"[hp_Q]",
     precise_unit(1.0, precise::log::neglog50000, commodities::Hahnemann)},
    {"[HP_Q]",
     precise_unit(1.0, precise::log::neglog50000, commodities::Hahnemann)},
    {"[kp_X]",
     precise_unit(1.0, precise::log::neglog10, commodities::Korsakov)},
    {"[KP_X]",
     precise_unit(1.0, precise::log::neglog10, commodities::Korsakov)},
    {"[kp_C]",
     precise_unit(1.0, precise::log::neglog100, commodities::Korsakov)},
    {"[KP_C]",
     precise_unit(1.0, precise::log::neglog100, commodities::Korsakov)},
    {"[kp_M]",
     precise_unit(1.0, precise::log::neglog1000, commodities::Korsakov)},
    {"[KP_M]",
     precise_unit(1.0, precise::log::neglog1000, commodities::Korsakov)},
    {"[kp_Q]",
     precise_unit(1.0, precise::log::neglog50000, commodities::Korsakov)},
    {"[KP_Q]",
     precise_unit(1.0, precise::log::neglog50000, commodities::Korsakov)},
    {"pH", precise::laboratory::pH},
    {"pHscale", precise::laboratory::pH},
    {"[PH]", precise::laboratory::pH},
};

// LCOV_EXCL_START

// this function is pulled from elsewhere and the coverage is not important for
// error checking

// get a matching character for the sequence
static char getMatchCharacter(char mchar)
{
    switch (mchar) {
        case '{':
            return '}';
        case '[':
            return ']';
        case '(':
            return ')';
        case '<':
            return '>';
        case '}':
            return '{';
        case ']':
            return '[';
        case ')':
            return '(';
        case '>':
            return '<';
        case '\'':
        case '"':
        case '`':
        default:
            return mchar;
    }
}
// LCOV_EXCL_STOP

// This function is only used in a few locations which is after a primary
// segment check which should catch the fail checks before this function is
// called so coverage isn't expected or required.

// do a segment check in the reverse direction
static bool
    segmentcheckReverse(const std::string& unit, char closeSegment, int& index)
{
    if (index >= static_cast<int>(unit.size())) {
        // LCOV_EXCL_START
        return false;
        // LCOV_EXCL_STOP
    }
    while (index >= 0) {
        char current = unit[index];
        --index;
        if (index >= 0 && unit[index] == '\\') {
            --index;
            continue;
        }
        if (current == closeSegment) {
            return true;
        }
        switch (current) {
            case '}':
            case ')':
            case ']':
                if (!segmentcheckReverse(
                        unit, getMatchCharacter(current), index)) {
                    // LCOV_EXCL_START
                    return false;
                    // LCOV_EXCL_STOP
                }
                break;
            case '{':
            case '(':
            case '[':
                return false;
            default:
                break;
        }
    }
    // LCOV_EXCL_START
    return false;
    // LCOV_EXCL_STOP
}

// do a segment check in the forward direction
static bool
    segmentcheck(const std::string& unit, char closeSegment, size_t& index)
{
    while (index < unit.size()) {
        char current = unit[index];
        ++index;
        if (current == closeSegment) {
            return true;
        }
        switch (current) {
            case '\\':
                ++index;
                break;
            case '(':
            case '"':
                if (!segmentcheck(unit, getMatchCharacter(current), index)) {
                    return false;
                }
                break;
            case '{':
            case '[': {
                auto close = getMatchCharacter(current);
                if (close == closeSegment) {
                    return false;
                }
                if (!segmentcheck(unit, close, index)) {
                    return false;
                }
                break;
            }
            case '}':
            case ')':
            case ']':
                return false;
            default:
                break;
        }
    }
    return false;
}

static precise_unit commoditizedUnit(
    const std::string& unit_string,
    precise_unit actUnit,
    size_t& index)
{
    auto ccindex = unit_string.find_first_of('{');
    if (ccindex == std::string::npos) {
        return actUnit;
    }
    ++ccindex;
    auto start = ccindex;
    segmentcheck(unit_string, '}', ccindex);
    if (ccindex - start == 2) {
        // there are a couple units that might look like commodities
        if (unit_string[start] == '#') {
            index = ccindex;
            return actUnit * count;
        }
    }
    std::string commodStr = unit_string.substr(start, ccindex - start - 1);
    if (commodStr == "cells") {
        index = ccindex;
        return actUnit * precise_unit(1.0, precise::count, commodities::cell);
    }
    auto hcode = getCommodity(std::move(commodStr));
    index = ccindex;
    return {1.0, actUnit, hcode};
}

static precise_unit
    commoditizedUnit(const std::string& unit_string, std::uint32_t match_flags)
{
    auto finish = unit_string.find_last_of('}');
    if (finish == std::string::npos) {
        // there are checks before this would get called that would catch that
        // error but it is left in place just in case

        // LCOV_EXCL_START
        return precise::invalid;
        // LCOV_EXCL_STOP
    }
    auto ccindex = static_cast<int>(finish) - 1;
    segmentcheckReverse(unit_string, '{', ccindex);

    auto cstring = unit_string.substr(
        static_cast<size_t>(ccindex) + 2, finish - ccindex - 2);

    if (ccindex < 0) {
        return {1.0, precise::one, getCommodity(cstring)};
    }

    auto bunit = unit_from_string_internal(
        unit_string.substr(0, static_cast<size_t>(ccindex) + 1),
        match_flags + no_commodities);
    if (!is_error(bunit)) {
        return {1.0, bunit, getCommodity(cstring)};
    }
    return precise::invalid;
}
// do a check if there are additional operations outside of brackets
static bool hasAdditionalOps(const std::string& unit_string)
{
    return (unit_string.find_last_of("*^(/", unit_string.find_last_of('{')) !=
            std::string::npos) ||
        (unit_string.find_first_of("*^(/", unit_string.find_last_of('}')) !=
         std::string::npos);
}

static precise_unit
    get_unit(const std::string& unit_string, std::uint32_t match_flags)
{
    if (allowUserDefinedUnits.load(std::memory_order_acquire)) {
        if (!user_defined_units.empty()) {
            auto fnd2 = user_defined_units.find(unit_string);
            if (fnd2 != user_defined_units.end()) {
                return fnd2->second;
            }
        }
    }
    /** some specific standards*/
    switch (match_flags & 0x007CU) {
        case strict_ucum: {
            auto fnd = base_ucum_vals.find(unit_string);
            if (fnd != base_ucum_vals.end()) {
                return fnd->second;
            }
        } break;
        case strict_si:
        default:
            break;
    }
    auto fnd = base_unit_vals.find(unit_string);
    if (fnd != base_unit_vals.end()) {
        return fnd->second;
    }
    auto c = unit_string.front();
    if ((c == 'C' || c == 'E') && unit_string.size() >= 6) {
        size_t index;
        if (unit_string.compare(0, 5, "CXUN[") == 0) {
            if (!hasAdditionalOps(unit_string)) {
                char* ptr = nullptr;
                auto num = static_cast<std::uint16_t>(
                    strtol(unit_string.c_str() + 5, &ptr, 0));
                if (*ptr == ']') {
                    return commoditizedUnit(
                        unit_string, precise::generate_custom_unit(num), index);
                }
            }
        }
        if (unit_string.compare(0, 6, "CXCUN[") == 0) {
            if (!hasAdditionalOps(unit_string)) {
                char* ptr = nullptr;
                auto num = static_cast<std::uint16_t>(
                    strtol(unit_string.c_str() + 6, &ptr, 0));
                if (*ptr == ']') {
                    return commoditizedUnit(
                        unit_string,
                        precise::generate_custom_count_unit(num),
                        index);
                }
            }
        }
        if (unit_string.compare(0, 6, "EQXUN[") == 0) {
            if (!hasAdditionalOps(unit_string)) {
                char* ptr = nullptr;
                auto num = static_cast<std::uint16_t>(
                    strtol(unit_string.c_str() + 6, &ptr, 0));
                if (*ptr == ']') {
                    return commoditizedUnit(
                        unit_string,
                        precise_unit(precise::custom::equation_unit(num)),
                        index);
                }
            }
        }
    }
    return precise::invalid;
}

// Detect if a string looks like a number
static bool looksLikeNumber(const std::string& string, size_t index)
{
    if (string.size() <= index) {
        return false;
    }
    if (isDigitCharacter(string[index])) {
        return true;
    }
    if (string.size() < index + 2) {
        return false;
    }
    if (string[index] == '.' &&
        (string[index + 1] >= '0' && string[index + 1] <= '9')) {
        return true;
    }
    if (string[index] == '-' || string[index] == '+') {
        if (string[index + 1] >= '0' && string[index + 1] <= '9') {
            return true;
        }
        if (string.size() >= index + 3 && string[index + 1] == '.' &&
            (string[index + 2] >= '0' && string[index + 2] <= '9')) {
            return true;
        }
    }
    return false;
}

// Detect if a string looks like an integer
static bool looksLikeInteger(const std::string& string)
{
    if (string.empty()) {
        // LCOV_EXCL_START
        return false;
        // LCOV_EXCL_STOP
    }
    size_t index = 0;
    if (string[0] == '-' || string[0] == '+') {
        ++index;
    }
    if (!isDigitCharacter(string[index])) {
        // at some point need to figure out how to get to this line
        return false;  // LCOV_EXCL_LINE
    }
    ++index;
    while (index < string.length()) {
        if (!isDigitCharacter(string[index])) {
            if (string[index] == '.' || string[index] == 'e' ||
                string[index] == 'E') {
                if (index + 1 < string.length()) {
                    if (isDigitCharacter(string[index + 1]) ||
                        string[index + 1] == '-' || string[index + 1] == '+') {
                        return false;
                    }
                }
            }
            return true;
        }
        ++index;
    }
    return true;
}

static void removeOuterParenthesis(std::string& ustring)
{
    while (ustring.front() == '(' && ustring.back() == ')') {
        // simple case
        if (ustring.find_first_of(')', 1) == ustring.size() - 1) {
            ustring.pop_back();
            ustring.erase(ustring.begin());
            if (ustring.empty()) {
                return;
            }
            continue;
        }
        int open = 1;
        for (size_t ii = 1; ii < ustring.size() - 1; ++ii) {
            if (ustring[ii] == '(') {
                ++open;
            }
            if (ustring[ii] == ')') {
                --open;
                if (open == 0) {
                    return;
                }
            }
        }
        if (open == 1) {
            ustring.pop_back();
            ustring.erase(ustring.begin());
        } else {
            return;
        }
    }
}

// Find the last multiply or divide operation in a string
static size_t findOperatorSep(const std::string& ustring, std::string operators)
{
    operators.append(")}]");
    auto sep = ustring.find_last_of(operators);

    while (
        sep != std::string::npos && sep > 0 &&
        (ustring[sep] == ')' || ustring[sep] == '}' || ustring[sep] == ']')) {
        int index = static_cast<int>(sep) - 1;
        segmentcheckReverse(ustring, getMatchCharacter(ustring[sep]), index);
        sep = (index > 0) ? ustring.find_last_of(operators, index) :
                            std::string::npos;
    }
    if (sep == 0) {
        // this should not happen
        // LCOV_EXCL_START
        sep = std::string::npos;
        // LCOV_EXCL_STOP
    }
    return sep;
}

// find the next word operator adjusting for parenthesis and brackets and braces
static size_t
    findWordOperatorSep(const std::string& ustring, const std::string& keyword)
{
    auto sep = ustring.rfind(keyword);
    if (ustring.size() > sep + keyword.size() + 1) {
        auto keychar = ustring[sep + keyword.size()];
        while (keychar == '^' || keychar == '*' || keychar == '/') {
            if (sep == 0) {
                sep = std::string::npos;
                break;
            }
            sep = ustring.rfind(keyword, sep - 1);
            if (sep == std::string::npos) {
                break;
            }
            keychar = ustring[sep + keyword.size()];
        }
    }
    size_t findex = ustring.size();
    while (sep != std::string::npos) {
        auto lbrack = ustring.find_last_of(")}]", findex);

        if (lbrack == std::string::npos) {
            return sep;
        }
        if (lbrack < sep) {
            // this should not happen as it would mean the operator separator
            // didn't function properly
            return sep;  // LCOV_EXCL_LINE
        }
        auto cchar = getMatchCharacter(ustring[lbrack]);
        --lbrack;
        int index = static_cast<int>(lbrack) - 1;
        segmentcheckReverse(ustring, cchar, index);
        if (index < 0) {
            // this should not happen as it would mean we got this point by
            // bypassing some other checks
            return std::string::npos;  // LCOV_EXCL_LINE
        }
        findex = static_cast<size_t>(index);
        if (findex < sep) {
            sep = ustring.rfind(keyword, findex);
        }
    }
    return sep;
}

// remove spaces and insert multiplies if appropriate
static bool cleanSpaces(std::string& unit_string, bool skipMultiply)
{
    static const std::string spaceChars = std::string(" \t\n\r") + '\0';
    bool spacesRemoved = false;
    auto fnd = unit_string.find_first_of(spaceChars);
    while (fnd != std::string::npos) {
        spacesRemoved = true;
        if ((fnd > 0) && (!skipMultiply)) {
            auto nloc = unit_string.find_first_not_of(spaceChars, fnd);
            if (nloc == std::string::npos) {
                unit_string.erase(fnd, std::string::npos);
                return true;
            }
            if (fnd == 1) {  // if the second character is a space it almost
                             // always means multiply
                if (unit_string.size() < 8) {
                    if (unit_string[nloc] == '*' || unit_string[nloc] == '/') {
                        unit_string.erase(fnd, 1);
                        fnd = unit_string.find_first_of(spaceChars, fnd);
                        continue;
                    }
                    unit_string[fnd] = '*';
                    fnd = unit_string.find_first_of(spaceChars, fnd);
                    skipMultiply = true;
                    continue;
                }
            }
            if (unit_string[fnd - 1] == '/' || unit_string[fnd - 1] == '*') {
                unit_string.erase(fnd, 1);
                fnd = unit_string.find_first_of(spaceChars, fnd);
                continue;
            }
            if (unit_string.size() > nloc &&
                (unit_string[nloc] == '/' || unit_string[nloc] == '*')) {
                unit_string.erase(fnd, 1);
                fnd = unit_string.find_first_of(spaceChars, fnd);
                continue;
            }
            if (std::all_of(
                    unit_string.begin(), unit_string.begin() + fnd, [](char X) {
                        return isNumericalStartCharacter(X) || (X == '/') ||
                            (X == '*');
                    })) {
                unit_string[fnd] = '*';
                fnd = unit_string.find_first_of(spaceChars, fnd);
                skipMultiply = true;
                continue;
            }
            // if there was a single divide with no space then the next space is
            // probably a multiply
            if (std::count(
                    unit_string.begin(), unit_string.begin() + fnd, '/') == 1) {
                if (unit_string.rfind("/sq", fnd) == std::string::npos &&
                    unit_string.rfind("/cu", fnd) == std::string::npos) {
                    auto notspace =
                        unit_string.find_first_not_of(spaceChars, fnd);
                    auto f2 =
                        unit_string.find_first_of("*/^([{\xB7\xFA\xD7", fnd);
                    if (notspace != std::string::npos && f2 != notspace &&
                        !isDigitCharacter(unit_string[fnd - 1])) {
                        unit_string[fnd] = '*';

                        skipMultiply = true;
                        fnd = unit_string.find_first_of(spaceChars, fnd);
                        continue;
                    }
                }
            }
        }
        unit_string.erase(fnd, 1);
        if (fnd > 0) {
            skipMultiply = true;
        }
        fnd = unit_string.find_first_of(spaceChars, fnd);
    }
    return spacesRemoved;
}

static void
    cleanDotNotation(std::string& unit_string, std::uint32_t match_flags)
{
    // replace all dots with '*'
    size_t st = 0;
    auto dloc = unit_string.find_first_of('.');
    int skipped{0};
    while (dloc != std::string::npos) {
        if (dloc > 0) {
            if (!isDigitCharacter(unit_string[dloc - 1]) ||
                !isDigitCharacter(unit_string[dloc + 1])) {
                unit_string[dloc] = '*';
            } else {
                ++skipped;
            }
        } else if (unit_string.size() > 1) {
            if (!isDigitCharacter(unit_string[dloc + 1])) {
                unit_string[dloc] = '*';
            } else {
                ++skipped;
            }
        }
        st = dloc + 1;
        dloc = unit_string.find_first_of('.', st);
    }
    if (skipped > 1) {
        skipped = 0;
        dloc = unit_string.find_first_of('.');
        while (dloc != std::string::npos) {
            auto nloc = dloc + 1;
            while (unit_string[nloc] != '.') {
                if (!isDigitCharacter(unit_string[nloc])) {
                    dloc = unit_string.find_first_of('.', nloc + 1);
                    break;
                }
                ++nloc;
            }
            if (unit_string[nloc] == '.') {
                unit_string[nloc] = '*';
                dloc = unit_string.find_first_of('.', nloc + 1);
            } else {
                ++skipped;
            }
        }
    }
    if (skipped >
        0) {  // check for exponents which can't have dots so must be multiply
        dloc = unit_string.find_first_of('.', 2);
        while (dloc != std::string::npos) {
            auto nloc = dloc - 1;
            while (nloc > 0) {
                if (!isDigitCharacter(unit_string[nloc])) {
                    if (unit_string[nloc] == 'e' || unit_string[nloc] == 'E') {
                        unit_string[dloc] = '*';
                    }
                    break;
                }
                --nloc;
            }
            dloc = unit_string.find_first_of('.', dloc + 1);
        }
    }
    if ((match_flags & single_slash) != 0) {
        auto slashloc = unit_string.find_last_of('/');
        if (slashloc != std::string::npos) {
            unit_string.insert(slashloc + 1, 1, '(');
            unit_string.push_back(')');
        }
    }
}
// do some conversion work for CI strings to deal with a few peculiarities
static void ciConversion(std::string& unit_string)
{
    static const std::unordered_map<std::string, std::string> ciConversions{
        {"S", "s"},        {"G", "g"},     {"M", "m"},   {"MM", "mm"},
        {"NM", "nm"},      {"ML", "mL"},   {"GS", "Gs"}, {"GL", "Gal"},
        {"MG", "mg"},      {"[G]", "[g]"}, {"PG", "pg"}, {"NG", "ng"},
        {"UG", "ug"},      {"US", "us"},   {"PS", "ps"}, {"RAD", "rad"},
        {"GB", "gilbert"}, {"WB", "Wb"},   {"CP", "cP"},
    };
    // transform to upper case so we have a common starting point
    std::transform(
        unit_string.begin(), unit_string.end(), unit_string.begin(), ::toupper);
    auto fnd = ciConversions.find(unit_string);
    if (fnd != ciConversions.end()) {
        unit_string = fnd->second;
    } else {
        if (unit_string.front() == 'P') {
            unit_string[0] = 'p';
        } else if (unit_string.front() == 'M') {
            unit_string[0] = 'm';
        }
        if (unit_string.back() == 'M') {
            if (unit_string.length() == 2 &&
                getPrefixMultiplier(unit_string.front()) != 0.0) {
                unit_string.back() = 'm';
            } else if (
                unit_string.length() == 3 &&
                getPrefixMultiplier2Char(unit_string[0], unit_string[1]) !=
                    0.0) {
                unit_string.back() = 'm';
            }
        }
    }
    auto loc = unit_string.find("/S");
    if (loc != std::string::npos) {
        unit_string[loc + 1] = 's';
    }
    loc = unit_string.find("/G");
    if (loc != std::string::npos) {
        unit_string[loc + 1] = 'g';
    }
}

// run a few checks on the string to verify it looks somewhat valid
static bool checkValidUnitString(
    const std::string& unit_string,
    std::uint32_t match_flags)
{
    static constexpr std::array<const char*, 2> invalidSequences{{"-+", "+-"}};
    if (unit_string.front() == '^' || unit_string.back() == '^') {
        return false;
    }
    auto cx = unit_string.find_first_of("*/^");
    while (cx != std::string::npos) {
        auto cx2 = unit_string.find_first_of("*/^", cx + 1);
        if (cx2 == cx + 1) {
            return false;
        }
        cx = cx2;
    }
    bool skipcodereplacement = ((match_flags & skip_code_replacements) != 0);
    if (!skipcodereplacement) {
        for (auto& seq : invalidSequences) {
            if (unit_string.find(seq) != std::string::npos) {
                return false;
            }
        }

        size_t index = 0;
        while (index < unit_string.size()) {
            char current = unit_string[index];
            switch (current) {
                case '{':
                case '(':
                case '[':
                case '"':
                    ++index;
                    if (!segmentcheck(
                            unit_string, getMatchCharacter(current), index)) {
                        return false;
                    }
                    break;
                case '}':
                case ')':
                case ']':
                    return false;

                default:
                    ++index;
                    break;
            }
        }
        // check all power operations
        cx = unit_string.find_first_of('^');
        while (cx != std::string::npos) {
            char c = unit_string[cx + 1];
            if (!isDigitCharacter(c)) {
                if (c == '-') {
                    if (!isDigitCharacter(unit_string[cx + 2])) {
                        return false;
                    }
                } else if (c == '(') {
                    cx += 2;
                    if (unit_string[cx] == '-') {
                        ++cx;
                    }
                    bool dpoint_encountered = false;
                    while (unit_string[cx] != ')') {
                        if (!isDigitCharacter(unit_string[cx])) {
                            if (unit_string[cx] == '.' && !dpoint_encountered) {
                                dpoint_encountered = true;
                            } else {
                                return false;
                            }
                        }
                        ++cx;
                    }
                } else {
                    return false;
                }
            }
            cx = unit_string.find_first_of('^', cx + 1);
        }
        // check for sequences of power operations
        cx = unit_string.find_last_of('^');
        while (cx != std::string::npos) {
            auto prev = unit_string.find_last_of('^', cx - 1);
            if (prev == std::string::npos) {
                break;
            }
            switch (cx - prev) {
                case 2:  // the only way this would get here is ^D^ which is not
                         // allowed
                    return false;
                case 3:
                    if (unit_string[prev + 1] == '-') {
                        return false;
                    }
                    break;
                case 4:  // checking for ^(D)^
                    if (unit_string[prev + 1] == '(') {
                        return false;
                    }
                    break;
                case 5:  // checking for ^(-D)^
                    if (unit_string[prev + 1] == '(' &&
                        unit_string[prev + 2] == '-') {
                        return false;
                    }
                    break;
                default:
                    break;
            }
            cx = prev;
        }
    }

    return true;
}

static void multiplyRep(std::string& unit_string, size_t loc, size_t sz)
{
    if (loc == 0) {
        unit_string.erase(0, sz);
        return;
    }
    if (unit_string.size() <= loc + sz) {
        unit_string.erase(loc, sz);
        if (unit_string.back() == '^' || unit_string.back() == '*' ||
            unit_string.back() == '/') {
            unit_string.pop_back();
        }
        return;
    }
    auto tchar = unit_string[loc - 1];
    auto tchar2 = unit_string[loc + sz];
    if (tchar == '*' || tchar == '/' || tchar == '^' || tchar2 == '*' ||
        tchar2 == '/' || tchar2 == '^') {
        if ((tchar == '*' || tchar == '/' || tchar == '^') &&
            (tchar2 == '*' || tchar2 == '/' || tchar2 == '^')) {
            unit_string.erase(loc - 1, sz + 1);
        } else {
            unit_string.erase(loc, sz);
        }
    } else {
        unit_string.replace(loc, sz, "*");
    }
}

static void cleanUpPowersOfOne(std::string& unit_string)
{  // get rid of (1)^ sequences
    auto fndP = unit_string.find("(1)^");
    while (fndP != std::string::npos) {
        // string cannot end in '^' from a previous check
        size_t eraseCnt = 4;
        auto ch = unit_string[fndP + 4];
        if (ch == '+' || ch == '-') {
            ++eraseCnt;
            if (unit_string.size() <= fndP + eraseCnt) {
                multiplyRep(unit_string, fndP, eraseCnt);
                break;
            }
            ch = unit_string[fndP + eraseCnt];
        }
        while (isDigitCharacter(ch)) {
            ++eraseCnt;
            if (unit_string.size() <= fndP + eraseCnt) {
                break;
            }
            ch = unit_string[fndP + eraseCnt];
        }
        multiplyRep(unit_string, fndP, eraseCnt);
        fndP = unit_string.find("(1)^", fndP);
    }
    // get rid of ^1 sequences
    fndP = unit_string.find("^1");
    while (fndP != std::string::npos) {
        if (unit_string.size() > fndP + 2) {
            if (!isDigitCharacter(unit_string[fndP + 2])) {
                unit_string.erase(fndP, 2);
            } else {
                fndP = unit_string.find("^1", fndP + 2);
                continue;
            }
        } else {
            unit_string.erase(fndP, 2);
        }
        fndP = unit_string.find("^1", fndP);
    }
    // get rid of ^1 sequences
    fndP = unit_string.find("^(1)");
    while (fndP != std::string::npos) {
        multiplyRep(unit_string, fndP, 4);
        fndP = unit_string.find("^(1)", fndP);
    }
}

static void htmlCodeReplacement(std::string& unit_string)
{
    auto fnd = unit_string.find("<sup>");
    while (fnd != std::string::npos) {
        unit_string.replace(fnd, 5, "^");
        fnd = unit_string.find("</sup>");
        if (fnd != std::string::npos) {
            unit_string.replace(fnd, 6, "");
        } else {
            fnd = unit_string.find("<\\/sup>");
            if (fnd != std::string::npos) {
                unit_string.replace(fnd, 8, "");
            }
        }
        fnd = unit_string.find("<sup>");
    }
    fnd = unit_string.find("<sub>");
    while (fnd != std::string::npos) {
        unit_string.replace(fnd, 5, "_");
        fnd = unit_string.find("</sub>");
        if (fnd != std::string::npos) {
            unit_string.replace(fnd, 6, "");
        } else {
            fnd = unit_string.find("<\\/sub>");
            if (fnd != std::string::npos) {
                unit_string.replace(fnd, 8, "");
            }
        }
        fnd = unit_string.find("<sub>");
    }
}

/// do some unicode replacement (unicode in the loose sense any characters not
/// in the basic ascii set)
static bool unicodeReplacement(std::string& unit_string)
{
    static UNITS_CPP14_CONSTEXPR_OBJECT std::array<ckpair, 48>
        ucodeReplacements{{
            ckpair{u8"\u00d7", "*"},
            ckpair{u8"\u00f7", "/"},  // division sign
            ckpair{u8"\u00b7", "*"},
            ckpair{u8"\u2215", "*"},  // asterisk operator
            ckpair{u8"\u00B5", "u"},
            ckpair{u8"\u03BC", "u"},
            ckpair{u8"\u00E9", "e"},
            ckpair{u8"\u00E8", "e"},
            ckpair{u8"\u0301", ""},  // just get rid of the accent
            ckpair{u8"\u0300", ""},  // just get rid of the accent
            ckpair{u8"\u2212", "-"},
            ckpair{u8"\u2009", ""},  // thin space
            ckpair{u8"\u2007", ""},  // thin space
            ckpair{u8"\u202f", ""},  // narrow no break space
            ckpair{u8"\u207B\u00B9", "^(-1)"},
            ckpair{u8"\u207B\u00B2", "^(-2)"},
            ckpair{u8"\u207B\u00B3", "^(-3)"},
            ckpair{u8"-\u00B9", "^(-1)"},
            ckpair{u8"-\u00B2", "^(-2)"},
            ckpair{u8"-\u00B3", "^(-3)"},
            ckpair{u8"\u00b2", "^(2)"},
            ckpair{u8"\u00b9", "*"},  // superscript 1 which doesn't do anything
            ckpair{u8"\u00b3", "^(3)"},
            ckpair{u8"\u2215", "/"},  // Division slash
            ckpair{u8"\u00BD", "(0.5)"},  // (1/2) fraction
            ckpair{u8"\u00BC", "(0.25)"},  // (1/4) fraction
            ckpair{u8"\u00BE", "(0.75)"},  // (3/4) fraction
            ckpair{u8"\u2153", "(1/3)"},  // (1/3) fraction
            ckpair{u8"\u2154", "(2/3)"},  // (2/3) fraction
            ckpair{u8"\u215B", "0.125"},  // (1/8) fraction
            ckpair{u8"\u215F", "1/"},  // 1/ numerator operator
            ckpair{u8"\u20AC", "\x80"},  // euro sign to extended ascii
            ckpair{u8"\u20BD", "ruble"},  // Ruble sign
            ckpair{u8"\u01B7", "dr_ap"},  // drachm symbol
            ckpair{"-\xb3", "^(-3)"},
            ckpair{"-\xb9", "^(-1)"},
            ckpair{"-\xb2", "^(-2)"},
            ckpair{"\xb3", "^(3)"},
            ckpair{"\xb9", "*"},
            // superscript 1 which doesn't do anything, replace with multiply
            ckpair{"\xb2", "^(2)"},
            ckpair{"\xf7", "/"},
            ckpair{"\xB7", "*"},
            ckpair{"\xD7", "*"},
            ckpair{"\xE9", "e"},  // remove accent
            ckpair{"\xE8", "e"},  // remove accent
            ckpair{"\xBD", "(0.5)"},  // (1/2) fraction
            ckpair{"\xBC", "(0.25)"},  // (1/4) fraction
            ckpair{"\xBE", "(0.75)"},  // (3/4) fraction
        }};
    bool changed{false};
    for (auto& ucode : ucodeReplacements) {
        auto fnd = unit_string.find(ucode.first);
        while (fnd != std::string::npos) {
            changed = true;
            unit_string.replace(fnd, strlen(ucode.first), ucode.second);
            if (fnd > 0 && unit_string[fnd - 1] == '\\') {
                unit_string.erase(fnd - 1, 1);
                --fnd;
            }
            fnd = unit_string.find(ucode.first, fnd + strlen(ucode.second));
        }
    }
    return changed;
}

// 10*num usually means a power of 10 so in most cases replace it with 1e
// except it is not actually 10 or the thing after isn't a number
// but take into account the few cases where . notation is used
static void checkPowerOf10(std::string& unit_string)
{
    auto fndP = unit_string.find("10*");
    while (fndP != std::string::npos) {
        if (unit_string.size() > fndP + 3 &&
            isNumericalStartCharacter(unit_string[fndP + 3])) {
            if (fndP == 0 || !isNumericalCharacter(unit_string[fndP - 1]) ||
                (fndP >= 2 && unit_string[fndP - 1] == '.' &&
                 (unit_string[fndP - 2] < '0' ||
                  unit_string[fndP - 2] > '9'))) {
                auto powerstr = unit_string.substr(fndP + 3);
                if (looksLikeInteger(powerstr)) {
                    try {
                        int power = std::stoi(powerstr);
                        if (std::labs(power) <= 38) {
                            unit_string.replace(fndP, 3, "1e");
                        }
                    }
                    catch (const std::out_of_range&) {
                        // if it is a really big number we obviously skip it
                    }
                }
            }
        }
        fndP = unit_string.find("10*", fndP + 3);
    }
}

// do some cleaning on the unit string to standardize formatting and deal with
// some extended ascii and unicode characters
static bool cleanUnitString(std::string& unit_string, std::uint32_t match_flags)
{
    auto slen = unit_string.size();
    bool skipcodereplacement = ((match_flags & skip_code_replacements) != 0);

    static UNITS_CPP14_CONSTEXPR_OBJECT std::array<ckpair, 30>
        allCodeReplacements{{
            ckpair{"sq.", "square"},
            ckpair{"cu.", "cubic"},
            ckpair{"(US)", "US"},
            ckpair{"10^", "1e"},
            ckpair{"10-", "1e-"},
            ckpair{"^+", "^"},
            ckpair{"ampere", "amp"},
            ckpair{"Ampere", "amp"},
            ckpair{"metre", "meter"},
            ckpair{"litre", "liter"},
            ckpair{"B.Th.U.", "BTU"},
            ckpair{"B.T.U.", "BTU"},
            ckpair{"Britishthermalunits", "BTU"},
            ckpair{"Britishthermalunitat", "BTU"},
            ckpair{"Britishthermalunit", "BTU"},
            ckpair{"BThU", "BTU"},
            ckpair{"-US", "US"},
            ckpair{"--", "*"},
            // -- is either a double negative or a separator, so make it a
            // multiplier so it doesn't get erased and then converted to a power
            ckpair{"\\\\", "\\\\*"},
            // \\ is always considered a segment terminator so it won't be
            // misinterpreted as a known escape sequence
            ckpair{"perunit", "pu"},
            ckpair{"per-unit", "pu"},
            ckpair{"/square*", "/square"},
            ckpair{"/cubic*", "/cubic"},
            ckpair{"degrees", "deg"},
            ckpair{"degree", "deg"},
            ckpair{"Hz^0.5", "rootHertz"},
            ckpair{"Hz^.5", "rootHertz"},
            ckpair{"Hz^(1/2)", "rootHertz"},
            ckpair{"Hz^1/2", "rootHertz"},
            ckpair{u8"\u221AHz", "rootHertz"},
        }};

    static const std::string spchar = std::string(" \t\n\r") + '\0';
    bool changed = false;
    bool skipMultiply = false;
    std::size_t skipMultiplyInsertionAfter{std::string::npos};
    char tail = unit_string.back();
    if (tail == '^' || tail == '*' || tail == '/' || tail == '.') {
        unit_string.pop_back();
        changed = true;
    }
    auto c = unit_string.find_first_not_of(spchar);
    if (c == std::string::npos) {
        unit_string.clear();
        return true;
    }
    if (unit_string[c] == '/') {
        unit_string.insert(c, 1, '1');
        changed = true;
        skipMultiply = true;
    }
    if (!skipcodereplacement) {
        // clean up some "per" words
        if (unit_string.compare(0, 4, "per ") == 0) {
            unit_string.replace(0, 4, "1/");
            skipMultiply = true;
        }
        if (ReplaceStringInPlace(unit_string, " per ", 5, "/", 1)) {
            skipMultiply = true;
        }
        auto fndP = unit_string.find(" s");
        while (fndP != std::string::npos) {
            if (fndP + 2 == unit_string.size()) {
                unit_string[fndP] = '*';
            } else {
                switch (unit_string[fndP + 2]) {
                    case ' ':
                    case '*':
                    case '/':
                    case '^':
                    case '.':
                        unit_string[fndP] = '*';
                        break;
                    default:
                        break;
                }
            }
            fndP = unit_string.find(" s", fndP + 1);
        }
        fndP = unit_string.find(" of ");
        while (fndP != std::string::npos) {
            auto nchar = unit_string.find_first_not_of(
                std::string(" \t\n\r") + '\0', fndP + 4);
            if (nchar != std::string::npos) {
                if (unit_string[nchar] == '(' || unit_string[nchar] == '[') {
                    skipMultiplyInsertionAfter = fndP;
                    break;
                }
            }
            fndP = unit_string.find(" of ", fndP + 3);
        }
        changed |= cleanSpaces(unit_string, skipMultiply);
        if (unit_string.empty()) {
            // LCOV_EXCL_START
            return true;
            // LCOV_EXCL_STOP
        }

        checkPowerOf10(unit_string);
    } else {
        auto fndP = unit_string.find("of(");
        if (fndP != std::string::npos) {
            skipMultiplyInsertionAfter = fndP;
        }
    }
    if (unit_string.front() == '(') {
        removeOuterParenthesis(unit_string);
        if (unit_string.empty()) {
            return true;
        }
    }

    if (!skipcodereplacement) {
        // ** means power in some environments
        if (ReplaceStringInPlace(unit_string, "**", 2, "^", 1)) {
            changed = true;
        }
    }
    if ((match_flags & case_insensitive) != 0) {
        ciConversion(unit_string);
        changed = true;
    }
    if (!skipcodereplacement) {
        // deal with some html stuff
        auto bloc = unit_string.find_last_of('<');
        if (bloc != std::string::npos) {
            htmlCodeReplacement(unit_string);
        }
        // some abbreviations and other problematic code replacements
        for (auto& acode : allCodeReplacements) {
            auto fnd = unit_string.find(acode.first);
            while (fnd != std::string::npos) {
                changed = true;
                unit_string.replace(fnd, strlen(acode.first), acode.second);
                fnd = unit_string.find(acode.first, fnd + 1);
            }
        }
    }
    if (unit_string.size() >= 2) {
        auto eit = unit_string.end() - 1;
        if (*(eit) == '2' || *(eit) == '3') {
            if ((*(eit - 1) == '-' || *(eit - 1) == '+') &&
                unit_string.size() >= 3) {
                --eit;
            }
            if (!isDigitCharacter(*(eit - 1))) {
                switch (*(eit - 1)) {
                    case '^':
                    case 'e':
                    case 'E':
                    case '/':
                    case '+':
                    case '-':
                        break;
                    case '*':
                        *(eit - 1) = '^';
                        break;
                    default:
                        if (eit[0] != '+') {
                            unit_string.insert(eit, '^');
                        } else {
                            *eit = '^';
                        }

                        break;
                }
            }
        }
    }
    if (!skipcodereplacement) {
        // handle dot notation for multiplication
        auto dotloc = unit_string.find_last_of('.');
        if (dotloc < std::string::npos) {
            // strings always have a null pointer at the end
            if (!isDigitCharacter(unit_string[dotloc + 1])) {
                cleanDotNotation(unit_string, match_flags);
                changed = true;
            }
        }
        // Check for unicode or extended characters
        if (std::any_of(unit_string.begin(), unit_string.end(), [](char x) {
                return (static_cast<std::uint8_t>(x) & 0x80U) != 0;
            })) {
            if (unicodeReplacement(unit_string)) {
                changed = true;
            }
        }

        // clear empty parenthesis
        auto fndP = unit_string.find("()");
        while (fndP != std::string::npos) {
            if (unit_string.size() > fndP + 2) {
                if (unit_string[fndP + 2] == '^') {
                    unit_string.replace(fndP, 2, "*1");
                } else {
                    unit_string.erase(fndP, 2);
                }
            } else {
                unit_string.erase(fndP, 2);
            }
            fndP = unit_string.find("()", fndP);
        }
        // clear empty brackets, this would indicate commodities but if empty
        // there is no commodity
        clearEmptySegments(unit_string);
        cleanUpPowersOfOne(unit_string);
        if (unit_string.empty()) {
            unit_string.push_back('1');
            return true;
        }
    }
    // remove leading *})],  equivalent of 1* but we don't need to process that
    // further
    while (!unit_string.empty() &&
           (unit_string.front() == '*' || unit_string.front() == '}' ||
            unit_string.front() == ')' || unit_string.front() == ']')) {
        unit_string.erase(0, 1);
        changed = true;
        if (unit_string.empty()) {
            return true;
        }
        // check for parenthesis again
        if (unit_string.front() == '(') {
            removeOuterParenthesis(unit_string);
        }
    }
    // inject multiplies after bracket terminators
    auto fnd = unit_string.find_first_of(")]}");
    while (fnd < unit_string.size() - 1 && fnd < skipMultiplyInsertionAfter) {
        switch (unit_string[fnd + 1]) {
            case '^':
            case '*':
            case '/':
            case ')':
            case ']':
            case '}':
            case '>':
                fnd = unit_string.find_first_of(")]}", fnd + 1);
                break;
            case 'o':  // handle special case of commodity modifier using "of"
                if (unit_string.size() > fnd + 3) {
                    auto tc2 = unit_string[fnd + 3];
                    if (unit_string[fnd + 2] == 'f' && tc2 != ')' &&
                        tc2 != ']' && tc2 != '}') {
                        fnd = unit_string.find_first_of(")]}", fnd + 3);
                        break;
                    }
                }
                unit_string.insert(fnd + 1, 1, '*');
                fnd = unit_string.find_first_of(")]}", fnd + 3);
                break;
            case '{':
                if (unit_string[fnd] != '}') {
                    fnd = unit_string.find_first_of(")]}", fnd + 1);
                    break;
                }
                /* FALLTHRU */
            default:
                if (unit_string[fnd - 1] == '\\') {  // ignore escape sequences
                    fnd = unit_string.find_first_of(")]}", fnd + 1);
                    break;
                }
                unit_string.insert(fnd + 1, 1, '*');
                fnd = unit_string.find_first_of(")]}", fnd + 2);
                break;
        }
    }
    // insert multiplies after ^#
    fnd = unit_string.find_first_of('^');
    while (fnd < unit_string.size() - 3 && fnd < skipMultiplyInsertionAfter) {
        if (unit_string[fnd + 1] == '-') {
            ++fnd;
        }
        if (fnd < unit_string.size() - 3) {
            std::size_t seq = 1;
            auto p = unit_string[fnd + seq];
            while (p >= '0' && p <= '9' &&
                   fnd + seq <= unit_string.size() - 1U) {
                ++seq;
                p = unit_string[fnd + seq];
            }
            if (fnd + seq > unit_string.size() - 1U) {
                break;
            }
            if (seq > 1) {
                auto c2 = unit_string[fnd + seq];
                if (c2 != '\0' && c2 != '*' && c2 != '/' && c2 != '^' &&
                    c2 != 'e' && c2 != 'E') {
                    unit_string.insert(fnd + seq, 1, '*');
                }
            }
        }
        fnd = unit_string.find_first_of('^', fnd + 2);
    }

    // this still might occur from code replacements or other removal
    if (!unit_string.empty() && unit_string.front() == '/') {
        unit_string.insert(unit_string.begin(), '1');
        changed = true;
    }
    if (!skipcodereplacement) {  // make everything inside {} lower case
        auto bloc = unit_string.find_first_of('{');
        while (bloc != std::string::npos) {
            auto ind = bloc + 1;
            if (segmentcheck(unit_string, '}', ind)) {
                std::transform(
                    unit_string.begin() + bloc + 1,
                    unit_string.begin() + ind - 1,
                    unit_string.begin() + bloc + 1,
                    ::tolower);
                bloc = unit_string.find_first_of('{', ind);
            } else {
                bloc = std::string::npos;
            }
        }
    }
    return (changed || unit_string.size() != slen);
}

/// cleanup phase 2 if things still aren't working
static bool cleanUnitStringPhase2(std::string& unit_string)
{
    auto len = unit_string.length();
    unit_string.erase(
        std::remove(unit_string.begin(), unit_string.end(), '_'),
        unit_string.end());
    // cleanup extraneous dashes
    auto dpos = unit_string.find_first_of('-');
    while (dpos != std::string::npos) {
        if (dpos < unit_string.size() - 1) {
            if (unit_string[dpos + 1] >= '0' && unit_string[dpos + 1] <= '9') {
                dpos = unit_string.find_first_of('-', dpos + 1);
                continue;
            }
        }

        unit_string.erase(dpos, 1);
        dpos = unit_string.find_first_of('-', dpos);
    }
    unit_string.erase(
        std::remove(unit_string.begin(), unit_string.end(), '+'),
        unit_string.end());

    clearEmptySegments(unit_string);
    return (len != unit_string.length());
}

static precise_unit
    unit_quick_match(std::string unit_string, std::uint32_t match_flags)
{
    if ((match_flags & case_insensitive) != 0) {  // if not a case insensitive
                                                  // matching process just do a
                                                  // quick scan first
        cleanUnitString(unit_string, match_flags);
    }
    auto retunit = get_unit(unit_string, match_flags);
    if (is_valid(retunit)) {
        return retunit;
    }
    if (unit_string.size() > 2 &&
        unit_string.back() == 's') {  // if the string is of length two this is
                                      // too risky to try since there would be
                                      // many incorrect matches
        unit_string.pop_back();
        retunit = get_unit(unit_string, match_flags);
        if (is_valid(retunit)) {
            return retunit;
        }
    } else if (unit_string.front() == '[' && unit_string.back() == ']') {
        unit_string.pop_back();
        if (unit_string.back() != 'U' && unit_string.back() != 'u') {
            unit_string.erase(unit_string.begin());
            retunit = get_unit(unit_string, match_flags);
            if (is_valid(retunit)) {
                return retunit;
            }
        }
    }
    return precise::invalid;
}
/** Under the assumption units were mashed together to for some new work or
spaces were used as multiplies this function will progressively try to split
apart units and combine them.
*/
static precise_unit tryUnitPartitioning(
    const std::string& unit_string,
    std::uint32_t match_flags)
{
    std::string ustring = unit_string;
    // lets try checking for meter next which is one of the most common reasons
    // for getting here
    auto fnd = findWordOperatorSep(unit_string, "meter");
    if (fnd != std::string::npos) {
        ustring.erase(fnd, 5);
        auto bunit = unit_from_string_internal(ustring, match_flags);
        if (is_valid(bunit)) {
            return precise::m * bunit;
        }
    }
    // detect another somewhat common situation often amphour or ampsecond
    if (unit_string.compare(0, 3, "amp") == 0) {
        auto bunit =
            unit_from_string_internal(unit_string.substr(3), match_flags);
        if (is_valid(bunit)) {
            return precise::A * bunit;
        }
    }
    auto mret = getPrefixMultiplierWord(unit_string);
    if (mret.first != 0.0) {
        ustring = unit_string.substr(mret.second);

        auto retunit = unit_from_string_internal(ustring, match_flags);
        if (is_valid(retunit)) {
            return {mret.first, retunit};
        }
        return precise::invalid;
    }

    // a newton(N) in front is somewhat common
    // try a round with just a quick partition
    size_t part = (unit_string.front() == 'N') ? 1 : 3;
    ustring = unit_string.substr(0, part);
    if (ustring.back() == '(' || ustring.back() == '[' ||
        ustring.back() == '{') {
        part = 1;
        ustring.pop_back();
    }
    std::vector<std::string> valid;
    while (part < unit_string.size() - 1) {
        auto res = unit_quick_match(ustring, match_flags);
        if (!is_valid(res) && ustring.size() >= 3) {
            if (ustring.front() >= 'A' &&
                ustring.front() <= 'Z') {  // check the lower case version since
                                           // we skipped partitioning when we
                                           // did this earlier
                ustring[0] += 32;
                res = unit_quick_match(ustring, match_flags);
            }
        }
        if (is_valid(res)) {
            auto bunit = unit_from_string_internal(
                unit_string.substr(part), match_flags | skip_partition_check);
            if (is_valid(bunit)) {
                return res * bunit;
            }
            valid.push_back(ustring);
        }
        ustring.push_back(unit_string[part]);
        ++part;
        if (ustring.back() == '(' || ustring.back() == '[' ||
            ustring.back() == '{') {
            auto start = part;
            segmentcheck(unit_string, getMatchCharacter(ustring.back()), part);
            if (ustring.back() == '(') {
                if (unit_string.find_first_of("({[*/", start) < part) {
                    // this implies that the contents of the parenthesis must be
                    // a standalone segment and should not be included in a
                    // check
                    break;
                }
            }
            ustring = unit_string.substr(0, part);
        }
        if (isDigitCharacter(ustring.back())) {
            while ((part < unit_string.size() - 1) &&
                   (unit_string[part] == '.' ||
                    isDigitCharacter(unit_string[part]))) {
                ustring.push_back(unit_string[part]);
                ++part;
            }
        }
    }
    // now do a quick check with a 2 character string since we skipped that
    // earlier
    auto qm2 = unit_quick_match(unit_string.substr(0, 2), match_flags);
    if (is_valid(qm2)) {
        valid.insert(valid.begin(), unit_string.substr(0, 2));
    }
    // now pick off a couple 1 character units
    if (unit_string.front() == 'V' || unit_string.front() == 'A') {
        valid.insert(valid.begin(), unit_string.substr(0, 1));
    }
    // start with the biggest
    std::reverse(valid.begin(), valid.end());
    for (auto& vd : valid) {
        auto res = unit_quick_match(vd, match_flags);

        auto bunit = unit_from_string_internal(
            unit_string.substr(vd.size()), match_flags);
        if (is_valid(bunit)) {
            return res * bunit;
        }
    }

    return precise::invalid;
}

/** Some standards allow for custom units usually in brackets with 'U or U at
 * the end
 */
static precise_unit checkForCustomUnit(const std::string& unit_string)
{
    size_t loc = std::string::npos;
    bool index = false;
    if (unit_string.front() == '[' && unit_string.back() == ']') {
        if (ends_with(unit_string, "U]")) {
            loc = unit_string.size() - 2;
        } else if (ends_with(unit_string, "index]")) {
            loc = unit_string.size() - 6;
            index = true;
        }
    } else if (unit_string.front() == '{' && unit_string.back() == '}') {
        if (ends_with(unit_string, "'u}")) {
            loc = unit_string.size() - 3;
        } else if (ends_with(unit_string, "index}")) {
            loc = unit_string.size() - 6;
            index = true;
        }
    }
    if (loc != std::string::npos) {
        if ((unit_string[loc - 1] == '\'') || (unit_string[loc - 1] == '_')) {
            --loc;
        }
        auto csub = unit_string.substr(1, loc - 1);

        if (index) {
            auto hcode = getCommodity(csub);
            return {1.0, precise::generate_custom_count_unit(0), hcode};
        }

        std::transform(csub.begin(), csub.end(), csub.begin(), ::tolower);
        auto custcode = std::hash<std::string>{}(csub);
        return precise::generate_custom_unit(custcode & 0x3FU);
    }

    return precise::invalid;
}
/// take a string and raise it to a power after interpreting the units defined
/// in the string
static precise_unit unit_to_the_power_of(
    std::string unit_string,
    int power,
    std::uint32_t match_flags)
{
    std::uint32_t recursion_modifier = recursion_depth1;
    if ((match_flags & no_recursion) != 0) {
        recursion_modifier = 0;
    }

    precise_unit retunit = precise::invalid;
    bool partialPowerSegment = (unit_string.back() == ')');
    int index = static_cast<int>(unit_string.size() - 2);
    if (partialPowerSegment) {
        segmentcheckReverse(unit_string, '(', index);
        if (index > 0 && unit_string[index] == '^') {
            partialPowerSegment = false;
        }
    }
    if (partialPowerSegment) {
        std::string ustring = unit_string.substr(
            static_cast<size_t>(index) + 2,
            unit_string.size() - static_cast<size_t>(index) - 3);
        retunit = unit_from_string_internal(
            ustring, match_flags - recursion_modifier);
        if (!is_valid(retunit)) {
            if (index >= 0) {
                if (ustring.find_first_of("(*/^{[") == std::string::npos) {
                    retunit = unit_from_string_internal(
                        unit_string, match_flags - recursion_modifier);
                    if (!is_valid(retunit)) {
                        return precise::invalid;
                    }
                    index = -1;
                } else {
                    return precise::invalid;
                }
            } else {
                return precise::invalid;
            }
        }

        if (power == -1) {
            retunit = retunit.inv();
        } else if (power != 1) {
            retunit = retunit.pow(power);
        }

        if (index < 0) {
            return retunit;
        }
        auto a_unit = unit_from_string_internal(
            unit_string.substr(0, index), match_flags - recursion_modifier);
        if (!is_error(a_unit)) {
            return a_unit * retunit;
        }
        return precise::defunit;
    }
    // auto ustring = unit_string.substr(0, pchar + 1);

    if ((match_flags & case_insensitive) != 0) {
        cleanUnitString(unit_string, match_flags);
    }

    retunit = get_unit(unit_string, match_flags);
    if (is_valid(retunit)) {
        if (power == 1) {
            return retunit;
        }
        if (power == -1) {
            return retunit.inv();
        }
        return retunit.pow(power);
    }
    auto fnd = findWordOperatorSep(unit_string, "per");
    if (fnd == std::string::npos) {
        retunit = unit_from_string_internal(
            unit_string, match_flags - recursion_modifier);
        if (!is_valid(retunit)) {
            return precise::invalid;
        }
        if (power == 1) {
            return retunit;
        }
        if (power == -1) {
            return retunit.inv();
        }
        return retunit.pow(power);
    }
    return precise::defunit;
}

static precise_unit
    checkSIprefix(const std::string& unit_string, std::uint32_t match_flags)
{
    bool threeAgain{false};
    if (unit_string.size() >= 3) {
        if (unit_string[1] == 'A') {
            threeAgain = true;
        } else {
            auto mux = getPrefixMultiplier2Char(unit_string[0], unit_string[1]);
            if (mux != 0.0) {
                auto ustring = unit_string.substr(2);
                if (ustring == "B") {
                    return {mux, precise::data::byte};
                }
                if (ustring == "b") {
                    return {mux, precise::data::bit};
                }
                auto retunit = unit_quick_match(ustring, match_flags);
                if (is_valid(retunit)) {
                    return {mux, retunit};
                }
            }
        }
    }
    if (unit_string.size() >= 2) {
        auto c = unit_string.front();
        if (c == 'N' && ((match_flags & case_insensitive) != 0)) {
            c = 'n';
        }
        auto mux = getPrefixMultiplier(c);
        if (mux != 0.0) {
            auto ustring = unit_string.substr(1);
            if (ustring == "B") {
                return {mux, precise::data::byte};
            }
            if (ustring == "b") {
                return {mux, precise::data::bit};
            }
            auto retunit = unit_quick_match(ustring, match_flags);
            if (!is_error(retunit)) {
                return {mux, retunit};
            }
        }
    }
    if (threeAgain) {
        auto mux = getPrefixMultiplier2Char(unit_string[0], unit_string[1]);
        if (mux != 0.0) {
            auto ustring = unit_string.substr(2);
            if (ustring == "B") {
                return {mux, precise::data::byte};
            }
            if (ustring == "b") {
                return {mux, precise::data::bit};
            }
            auto retunit = unit_quick_match(ustring, match_flags);
            if (is_valid(retunit)) {
                return {mux, retunit};
            }
        }
    }
    return precise::invalid;
}

precise_unit
    unit_from_string(std::string unit_string, std::uint32_t match_flags)
{
    // always allow the code replacements on first run
    match_flags &= (~skip_code_replacements);
    return unit_from_string_internal(std::move(unit_string), match_flags);
}

// Step 1.  Check if the string matches something in the map
// Step 2.  clean the string, remove spaces, '_' and detect dot notation, check
// for some unicode stuff, check again Step 3. Find multiplication of division
// operators and split the string into two starting from the last operator
// Step 4.  If found Goto step 1 for each of the two parts, then operate on the
// results Step 5.  Check for ^ and if found goto to step 1 for interior portion
// then do a power Step 6.  Remove parenthesis and if found goto step 1 Step 7.
// Check for a SI prefix on the unit Step 8.  Check if the first character is
// upper case and if so and the string is long make it lower case Step 9.  Check
// to see if it is a number of some kind and make numerical unit Step 10.
// Return an error unit
static precise_unit unit_from_string_internal(
    std::string unit_string,
    std::uint32_t match_flags)
{
    if (unit_string.empty()) {
        return precise::one;
    }
    if (unit_string.size() > 1024) {
        // there is no reason whatsoever that a unit string would be longer than
        // 1024 characters
        return precise::invalid;
    }
    precise_unit retunit;
    if ((match_flags & case_insensitive) == 0) {
        // if not a ci matching process just do a quick scan first
        retunit = get_unit(unit_string, match_flags);
        if (is_valid(retunit)) {
            return retunit;
        }
    }
    if (cleanUnitString(unit_string, match_flags)) {
        retunit = get_unit(unit_string, match_flags);
        if (is_valid(retunit)) {
            return retunit;
        }
    }
    // verify the string is at least sort of valid
    if (!checkValidUnitString(unit_string, match_flags)) {
        return precise::invalid;
    }
    // don't do the code replacements again as it won't be needed
    match_flags |= skip_code_replacements;
    std::uint32_t recursion_modifier = recursion_depth1;
    if ((match_flags & no_recursion) != 0) {
        recursion_modifier = 0;
    }
    match_flags += recursion_modifier;
    if ((match_flags & not_first_pass) == 0) {
        match_flags |= not_first_pass;
        match_flags +=
            partition_check1;  // only allow 3 deep for unit_partitioning
    }
    if (unit_string.front() == '{' && unit_string.back() == '}') {
        if (unit_string.find_last_of('}', unit_string.size() - 2) ==
            std::string::npos) {
            retunit = checkForCustomUnit(unit_string);
            if (!is_error(retunit)) {
                return retunit;
            }
            size_t index;
            return commoditizedUnit(unit_string, precise::one, index);
        }
    }
    auto ustring = unit_string;
    // catch a preceding number on the unit
    if (looksLikeNumber(unit_string)) {
        if (unit_string.front() != '1' ||
            unit_string[1] !=
                '/') {  // this catches 1/ which should be handled differently
            size_t index;
            double front = generateLeadingNumber(unit_string, index);
            if (std::isnan(front)) {  // out of range
                return precise::invalid;
            }

            if (index >= unit_string.size()) {
                return {front, precise::one};
            }

            auto front_unit = precise_unit(front, precise::one);
            if (unit_string[index] ==
                '*') {  // for division just keep the slash
                ++index;
            }
            if ((match_flags & no_commodities) == 0 &&
                unit_string[index] == '{') {
                front_unit = commoditizedUnit(unit_string, front_unit, index);
                if (index >= unit_string.length()) {
                    return front_unit;
                }
            }
            // don't do as many partition check levels for this
            retunit = unit_from_string_internal(
                unit_string.substr(index), match_flags + partition_check1);
            if (is_error(retunit)) {
                if (unit_string[index] == '(' || unit_string[index] == '[') {
                    auto cparen = index + 1;
                    segmentcheck(
                        unit_string,
                        getMatchCharacter(unit_string[index]),
                        cparen);
                    if (cparen ==
                        std::string::npos) {  // malformed unit string;
                        return precise::invalid;
                    }
                    auto commodity = getCommodity(
                        unit_string.substr(index + 1, cparen - index - 1));
                    front_unit.commodity(commodity);
                    if (cparen < unit_string.size()) {
                        retunit = unit_from_string_internal(
                            unit_string.substr(cparen), match_flags);
                        if (!is_valid(retunit)) {
                            return precise::invalid;
                        }
                    } else {
                        retunit = precise::one;
                    }
                } else {
                    if (is_valid(retunit)) {
                        return front_unit * retunit;
                    }
                    auto commodity = getCommodity(unit_string.substr(index));
                    front_unit.commodity(commodity);
                    return front_unit;
                }
            }
            return front_unit * retunit;
        }
    }

    auto sep = findOperatorSep(unit_string, "*/");
    if (sep != std::string::npos) {
        precise_unit a_unit;
        precise_unit b_unit;
        if (sep + 1 > unit_string.size() / 2) {
            b_unit = unit_from_string_internal(
                unit_string.substr(sep + 1), match_flags - recursion_modifier);
            if (!is_valid(b_unit)) {
                return precise::invalid;
            }
            a_unit = unit_from_string_internal(
                unit_string.substr(0, sep), match_flags - recursion_modifier);

            if (!is_valid(a_unit)) {
                return precise::invalid;
            }
        } else {
            a_unit = unit_from_string_internal(
                unit_string.substr(0, sep), match_flags - recursion_modifier);

            if (!is_valid(a_unit)) {
                return precise::invalid;
            }
            b_unit = unit_from_string_internal(
                unit_string.substr(sep + 1), match_flags - recursion_modifier);
            if (!is_valid(b_unit)) {
                return precise::invalid;
            }
        }
        return (unit_string[sep] == '/') ? (a_unit / b_unit) :
                                           (a_unit * b_unit);
    }
    // flag that is used to circumvent a few checks
    bool containsPer =
        (findWordOperatorSep(unit_string, "per") != std::string::npos);

    sep = findOperatorSep(unit_string, "^");
    if (sep != std::string::npos) {
        auto pchar = sep - 1;
        if (unit_string[sep + 1] == '(') {
            ++sep;
        }
        char c1 = unit_string[sep + 1];
        int power;
        if (c1 == '-' || c1 == '+') {
            ++sep;
            if (unit_string.length() < sep + 2) {
                // this should have been caught as an invalid sequence earlier
                return precise::invalid;  // LCOV_EXCL_LINE
            }
            if (isDigitCharacter(
                    unit_string[sep + 1])) {  // the - ',' is a +/- sign
                power = -(c1 - ',') * (unit_string[sep + 1] - '0');
            } else {
                // the check function should catch this but it would be
                // problematic if not caught
                return precise::invalid;  // LCOV_EXCL_LINE
            }
        } else {
            if (isDigitCharacter(c1)) {
                power = (c1 - '0');
            } else {
                // the check functions should catch this but it would be
                // problematic if not caught
                return precise::invalid;  // LCOV_EXCL_LINE
            }
        }
        retunit = unit_to_the_power_of(
            unit_string.substr(0, (pchar > 0) ? pchar + 1 : 1),
            power,
            match_flags);
        if (retunit != precise::defunit) {
            return retunit;
        }
    }
    if ((match_flags & no_commodities) == 0 && unit_string.back() == '}' &&
        unit_string.find('{') != std::string::npos) {
        return commoditizedUnit(unit_string, match_flags);
    }
    retunit = checkSIprefix(unit_string, match_flags);
    if (is_valid(retunit)) {
        return retunit;
    }
    // don't do any further steps if recursion is not available
    if ((match_flags & no_recursion) != 0) {
        return unit_quick_match(unit_string, match_flags);
    }
    if (unit_string.size() <= 2) {
        if (isDigitCharacter(unit_string.back())) {
            unit_string.insert(1, 1, '^');
            return unit_from_string_internal(unit_string, match_flags);
        }
        return precise::invalid;
    }
    // in a few select cases make the first character lower case
    if ((unit_string.size() >= 3) && (!containsPer) &&
        (!isDigitCharacter(unit_string.back()))) {
        if (unit_string[0] >= 'A' && unit_string[0] <= 'Z') {
            if (unit_string.size() > 5 || unit_string[0] != 'N') {
                if (unit_string.find_first_of("*/^") == std::string::npos) {
                    ustring = unit_string;
                    ustring[0] += 32;
                    retunit = unit_from_string_internal(
                        ustring,
                        (match_flags & (~case_insensitive)) |
                            skip_partition_check);
                    if (!is_error(retunit)) {
                        return retunit;
                    }
                }
            }
        }
    }

    auto mret = getPrefixMultiplierWord(unit_string);
    if (mret.first != 0.0) {
        ustring = unit_string.substr(mret.second);

        retunit = unit_quick_match(ustring, match_flags);
        if (!is_error(retunit)) {
            return {mret.first, retunit};
        }
        if (ustring[0] >= 'A' && ustring[0] <= 'Z') {
            if (ustring.size() > 4 || ustring[0] != 'N') {
                if (ustring.find_first_of("*/^") == std::string::npos) {
                    ustring[0] += 32;
                    retunit = unit_quick_match(ustring, match_flags);
                    if (!is_error(retunit)) {
                        return {mret.first, retunit};
                    }
                }
            }
        }
    }
    if (unit_string.front() == '[' && unit_string.back() == ']') {
        ustring = unit_string.substr(1);
        ustring.pop_back();
        if (ustring.back() != 'U')  // this means custom unit code
        {
            retunit = get_unit(ustring, match_flags);
            if (!is_error(retunit)) {
                return retunit;
            }
        }
    }
    auto s_ = unit_string.find("s_");
    if (s_ != std::string::npos) {
        ustring = unit_string;
        ustring.replace(s_, 2, "_");
        retunit = get_unit(ustring, match_flags);
        if (!is_error(retunit)) {
            return retunit;
        }
    }

    if (!containsPer) {
        // assume - means multiply
        auto fd = unit_string.find_first_of('-');
        if (fd != std::string::npos) {
            // if there is a single one just check for a merged unit
            if (unit_string.find_first_of('-', fd + 1) == std::string::npos) {
                ustring = unit_string;
                ustring.erase(fd, 1);
                retunit = unit_quick_match(ustring, match_flags);
                if (!is_error(retunit)) {
                    return retunit;
                }
            }
            ustring = unit_string;
            while (fd != std::string::npos) {
                if (fd == ustring.size() - 1) {
                    ustring.erase(fd, 1);
                } else if (isDigitCharacter(ustring[fd + 1])) {
                    if (fd > 0 && ustring[fd - 1] != '^') {
                        ustring.insert(fd, 1, '^');
                        fd += 1;
                    }
                } else {
                    ustring[fd] = '*';
                }
                fd = ustring.find_first_of('-', fd + 1);
            }
            if (ustring != unit_string) {
                retunit = unit_from_string_internal(
                    ustring, match_flags | skip_partition_check);
                if (!is_error(retunit)) {
                    return retunit;
                }
            }
        }
    }
    // try some other cleaning steps
    ustring = unit_string;
    if (cleanUnitStringPhase2(unit_string)) {
        if (!unit_string.empty()) {
            retunit = get_unit(unit_string, match_flags);
            if (!is_error(retunit)) {
                return retunit;
            }
            if (looksLikeNumber(unit_string)) {
                size_t loc;
                auto number = getDoubleFromString(unit_string, &loc);
                if (loc >= unit_string.length()) {
                    return {number, one};
                }
                unit_string = unit_string.substr(loc);
                retunit = unit_from_string_internal(unit_string, match_flags);
                if (!is_error(retunit)) {
                    return {number, retunit};
                }
                unit_string.insert(unit_string.begin(), '{');
                unit_string.push_back('}');
                return {number, commoditizedUnit(unit_string, match_flags)};
            }
        } else {  // if we erased everything this could lead to strange units so
                  // just go back to the original
            unit_string = ustring;
        }
    }
    if (unit_string.front() == '[' && unit_string.back() == ']') {
        ustring = unit_string.substr(1);
        ustring.pop_back();
        if (ustring.back() != 'U')  // this means custom unit code
        {
            retunit = get_unit(ustring, match_flags);
            if (!is_error(retunit)) {
                return retunit;
            }
        }
    }
    // try changing out any "per" words for division sign
    if (containsPer && (match_flags & no_per_operators) == 0) {
        auto fnd = findWordOperatorSep(unit_string, "per");
        if (fnd != std::string::npos) {
            ustring = unit_string;
            if (fnd == 0) {
                ustring.replace(fnd, 3, "1/");
            } else {
                ustring.replace(fnd, 3, "/");
            }
            retunit =
                unit_from_string_internal(ustring, match_flags + per_operator1);
            if (!is_error(retunit)) {
                return retunit;
            }
        }
    }

    // remove trailing 's'
    if (unit_string.back() == 's') {
        ustring = unit_string;
        ustring.pop_back();
        retunit = get_unit(ustring, match_flags);
        if (!is_error(retunit)) {
            return retunit;
        }
    }

    if (wordModifiers(unit_string)) {
        return unit_from_string_internal(unit_string, match_flags);
    }
    if ((match_flags & no_commodities) == 0 &&
        (match_flags & no_of_operator) == 0) {
        // try changing out and words indicative of a unit commodity
        auto fnd = findWordOperatorSep(unit_string, "of");
        if (fnd < unit_string.size() - 2 && fnd != 0) {
            ustring = unit_string;
            ustring.replace(fnd, 2, "{");

            auto sloc = ustring.find_first_of("{[(", fnd + 3);
            if (sloc == std::string::npos) {
                ustring.push_back('}');
            } else {
                ustring.insert(sloc, 1, '}');
            }
            auto cunit =
                commoditizedUnit(ustring, match_flags + commodity_check1);
            if (is_valid(cunit)) {
                return cunit;
            }
        }
    }
    // make lower case
    {
        ustring = unit_string;
        std::transform(
            ustring.begin(), ustring.end(), ustring.begin(), ::tolower);
        if (ustring != unit_string) {
            retunit = unit_quick_match(ustring, match_flags);
            if (!is_error(retunit)) {
                return retunit;
            }
        }
    }
    retunit = checkForCustomUnit(unit_string);
    if (!is_error(retunit)) {
        return retunit;
    }
    // check for some international modifiers
    if ((match_flags & no_locality_modifiers) == 0) {
        retunit =
            localityModifiers(unit_string, match_flags | skip_partition_check);
        if (!is_error(retunit)) {
            return retunit;
        }
    }

    if ((match_flags & no_locality_modifiers) == 0) {
        retunit =
            ignoreModifiers(unit_string, match_flags | skip_partition_check);
        if (!is_error(retunit)) {
            return retunit;
        }
    }

    if ((match_flags & skip_partition_check) == 0) {
        // maybe some things got merged together so lets try splitting them up
        // in various ways but only allow 3 layers deep
        retunit =
            tryUnitPartitioning(unit_string, match_flags + partition_check1);
        if (!is_error(retunit)) {
            return retunit;
        }
    }
    return precise::invalid;
}  // namespace units

precise_measurement measurement_from_string(
    std::string measurement_string,
    std::uint32_t match_flags)
{
    if (measurement_string.empty()) {
        return {};
    }
    // do a cleaning first to get rid of spaces and other issues
    match_flags &= (~skip_code_replacements);
    cleanUnitString(measurement_string, match_flags);

    size_t loc;

    auto val = generateLeadingNumber(measurement_string, loc);
    if (loc == 0) {
        val = readNumericalWords(measurement_string, loc);
    }
    if (loc == 0) {
        val = 1.0;
    }
    if (loc >= measurement_string.length()) {
        return {val, precise::one};
    }
    bool checkCurrency = (loc == 0);
    auto ustring = measurement_string.substr(loc);
    auto validString = checkValidUnitString(ustring, match_flags);
    auto un = (validString) ?
        unit_from_string_internal(
            std::move(ustring), match_flags | skip_code_replacements) :
        precise::invalid;
    if (!is_error(un)) {
        if (checkCurrency) {
            if (un.base_units() == precise::currency.base_units()) {
                return {un.multiplier(),
                        precise_unit(1.0, precise::currency, un.commodity())};
            }
        }
        return {val, un};
    }
    if (checkCurrency) {
        auto c = get_unit(measurement_string.substr(0, 1), match_flags);
        if (c == precise::currency) {
            auto mstr = measurement_from_string(
                measurement_string.substr(1), match_flags);
            return mstr * c;
        }
    }

    // deal with some specifics
    /*
    if (measurement_string[loc] == '\'')
    {
        double v2 = std::stod(measurement_string.substr(loc + 1), &loc);
        if (measurement_string[loc] == '"')
        {
            return precise_measurement(val, precise::ft) +
    precise_measurement(v2, precise::in);
        }
    }
    */
    return {val, precise::invalid};
}

uncertain_measurement uncertain_measurement_from_string(
    const std::string& measurement_string,
    std::uint32_t match_flags)
{
    if (measurement_string.empty()) {
        return {};
    }
    // first task is to find the +/-
    static UNITS_CPP14_CONSTEXPR_OBJECT std::array<const char*, 9> pmsequences{
        {"+/-",
         "\xB1",
         u8"\u00B1",
         "&plusmn;",
         "+-",
         "<u>+</u>",
         "&#xB1;",
         "&pm;",
         " \\pm "}};

    for (auto pmseq : pmsequences) {
        auto loc = measurement_string.find(pmseq);
        if (loc != std::string::npos) {
            auto p1 = measurement_string.substr(0, loc);
            auto m1 = measurement_cast_from_string(p1, match_flags);
            auto p2 = measurement_string.substr(loc + strlen(pmseq));
            auto m2 = measurement_cast_from_string(p2, match_flags);
            if (m1.units() == one) {
                return uncertain_measurement(
                    m1.value(), m2.value(), unit_cast(m2.units()));
            }
            if (m2.units() == one) {
                return uncertain_measurement(m1, m2.value());
            }
            return uncertain_measurement(m1, m2);
        }
    }
    // check for consise form of uncertainty X.XXXXXX(UU) N
    auto loc = measurement_string.find_first_of('(');
    if (loc < std::string::npos && loc > 1) {
        auto eloc = measurement_string.find_first_of(')', loc + 1);
        auto diff = eloc - loc;
        if (diff >= 2 && diff <= 4) {
            int cloc = static_cast<int>(loc) - 1;
            auto lc = eloc - 1;
            char c = measurement_string[cloc];
            if (c >= '0' && c <= '9') {
                auto ustring = measurement_string;
                while (cloc >= 0) {
                    c = measurement_string[cloc];
                    if (c >= '0' && c <= '9') {
                        if (lc > loc) {
                            ustring[cloc] = measurement_string[lc];
                            --lc;
                        } else {
                            ustring[cloc] = '0';
                        }
                    }
                    --cloc;
                }
                auto p = measurement_string;
                p.erase(loc, diff + 1);
                auto m1 = measurement_cast_from_string(p, match_flags);
                ustring.erase(loc, diff + 1);
                auto u1 = measurement_cast_from_string(ustring, match_flags);
                return uncertain_measurement(m1, u1);
            }
        }
    }
    return uncertain_measurement(
        measurement_cast_from_string(measurement_string, match_flags), 0.0F);
}

// Mostly from https://en.wikipedia.org/wiki/International_System_of_Units
static const std::unordered_map<std::string, precise_unit> measurement_types{
    {"", precise::defunit},
    {"arb", precise::defunit},
    {"arbitrary", precise::defunit},
    {"length", precise::m},
    {"len", precise::m},
    {"distance", precise::m},
    {"height", precise::m},
    {"depth", precise::m},
    {"width", precise::m},
    {"diameter", precise::m},
    {"thickness", precise::m},
    {"gauge", precise::m},
    {"invlen", precise::m.inv()},
    {"volume", precise::m.pow(3)},
    {"dryvolume", precise::m.pow(3)},
    {"vol", precise::m.pow(3)},
    {"fluidvolume", precise::L},
    {"time", precise::s},
    {"duration", precise::s},
    {"mass", precise::kg},
    {"weight", precise::kg},
    {"current", precise::A},
    {"electriccurrent", precise::A},
    {"magnetomotiveforce", precise::A},
    {"temperature", precise::K},
    {u8"\u2C90", precise::K},
    {u8"\u2129", precise::K},
    {u8"\u03F4", precise::K},
    {u8"\u0398", precise::K},
    {"celsiustemperature", precise::degC},
    {"temp", precise::K},
    {"thermodynamictemperature", precise::K},
    {"thermalconductivity", precise::W / precise::m / precise::K},
    {"amount", precise::mol},
    {"amountofsubstance", precise::mol},
    {"substance", precise::mol},
    {"sub", precise::mol},
    {"luminousintensity", precise::cd},
    {"area", precise::m.pow(2)},
    {"speed", precise::m / precise::s},
    {"velocity", precise::m / precise::s},
    {"vel", precise::m / precise::s},
    {"wavenumber", precise::m.inv()},
    {"acceleration", precise::m / precise::s.pow(2)},
    {"accel", precise::m / precise::s.pow(2)},
    {"density", precise::kg / precise::m.pow(3)},
    {"massdensity", precise::kg / precise::m.pow(3)},
    {"massconcentration", precise::kg / precise::m.pow(3)},
    {"surfacedensity", precise::kg / precise::m.pow(2)},
    {"lineardensity", precise::kg / precise::m},
    {"lineicmass", precise::kg / precise::m},
    {"linearmassdensity", precise::kg / precise::m},
    {"armass", precise::kg / precise::m.pow(2)},
    {"specificvolume", precise::m.pow(3) / precise::kg},
    {"vcnt", precise::L / precise::kg},
    {"currentdensity", precise::A / precise::m.pow(2)},
    {"magneticfieldstrength", precise::A / precise::m},
    {"magneticfieldintensity", precise::A / precise::m},
    {"magnetictension", precise::Pa / precise::m},
    {"concentration", precise::mol / precise::m.pow(3)},
    {"luminance", precise::cd / precise::m.pow(2)},
    {"brightness", precise::cd / precise::m.pow(2)},
    {"lum.intensitydensity", precise::cd / precise::m.pow(2)},
    {"planeangle", precise::rad},
    {"angle", precise::rad},
    {"solidangle", precise::rad.pow(2)},
    {"angular", precise::rad},
    {"frequency", precise::Hz},
    {"freq", precise::Hz},
    {"frequencies", precise::Hz},
    {"force", precise::N},
    {"weight", precise::N},
    {"pressure", precise::Pa},
    {"pres", precise::Pa},
    {"press", precise::Pa},
    {"stress", precise::Pa},
    {"energy", precise::J},
    {"eng", precise::J},
    {"enrg", precise::J},
    {"work", precise::J},
    {"heat", precise::J},
    {"power", precise::W},
    {"powerlevel", precise::W* precise::log::neper},
    {"radiantflux", precise::W},
    {"heatfluxdensity", precise::W / precise::m.pow(2)},
    {"electriccharge", precise::C},
    {"electricfluxdensity", precise::C / precise::m.pow(2)},
    {"charge", precise::C},
    {"electricchargedensity", precise::C / precise::m.pow(3)},
    {"quantityofelectricity", precise::C},
    {"voltage", precise::V},
    {"electricalpotential", precise::V},
    {"potentialdifference", precise::V},
    {"electricpotentialdifference", precise::V},
    {"elpot", precise::V},
    {"electricpotential", precise::V},
    {"electricpotentiallevel", precise::V* precise::log::neper},
    {"logrtoelp", precise::V* precise::log::neper},
    {"emf", precise::V},
    {"electromotiveforce", precise::V},
    {"capacitance", precise::F},
    {"electriccapacitance", precise::F},
    {"resistance", precise::ohm},
    {"impedance", precise::ohm},
    {"reactance", precise::ohm},
    {"conductance", precise::siemens},
    {"electricdipolemoment", precise::C* precise::m},
    {"electricresistance", precise::ohm},
    {"electricresistivity", precise::ohm* precise::m},
    {"electricimpedance", precise::ohm},
    {"electricreactance", precise::ohm},
    {"electricalconductance", precise::siemens},
    {"electricconductance", precise::siemens},
    {"conduction", precise::siemens},
    {"magneticflux", precise::Wb},
    {"fluxofmagneticinduction", precise::Wb},
    {"magneticfluxdensity", precise::T},
    {"magneticinduction", precise::T},
    {"fluxdensity", precise::T},
    {"noisevoltagedensity", precise::V / precise::special::rootHertz},
    {"inductance", precise::H},
    {"induction", precise::H},
    {"luminousflux", precise::lm},
    {"illuminance", precise::lx},
    {"illumination", precise::lx},
    {"luminousemittance", precise::lx},
    {"intensityoflight", precise::lx},
    {"radioactivity", precise::Bq},
    {"radionuclideactivity", precise::Bq},
    {"radiation", precise::Bq},
    {"activity", precise::Bq},
    {"activities", precise::Bq},
    {"logarithmofpowerratio", precise::log::dB},
    {"absorbeddose", precise::Gy},
    {"ionizingradiation", precise::Gy},
    {"kerma", precise::Gy},
    {"energydose", precise::Gy},
    {"absorbeddoseenergy", precise::Gy},
    {"engcnt", precise::Gy},
    {"equivalentdose", precise::Sv},
    {"radiationdose", precise::Sv},
    {"ionizingradiationdose", precise::Sv},
    {"effectivedose", precise::Sv},
    {"operationaldose", precise::Sv},
    {"committeddose", precise::Sv},
    {"catalyticactivity", precise::kat},
    {"specificenergy", precise::J / precise::kg},
    {"specificheat", precise::J / precise::kg / precise::K},
    {"engcnc", precise::J / precise::m.pow(3)},
    {"momentofforce", precise::N* precise::m},
    {"moment", precise::N* precise::m},
    {"torque", precise::N* precise::m},
    {"angularvelocity", precise::rad / precise::s},
    {"rotationalspeed", precise::rad / precise::s},
    {"rotationalvelocity", precise::rad / precise::s},
    {"angularacceleration", precise::rad / precise::s.pow(2)},
    {"surfacetension", precise::N / precise::m},
    {"electricfield", precise::V / precise::m},
    {"electricfieldstrength", precise::V / precise::m},
    {"permittivity", precise::F / precise::m},
    {"permeability", precise::H / precise::m},
    {"electricpermittivity", precise::F / precise::m},
    {"magneticpermeability", precise::H / precise::m},
    {"exposure", precise::C / precise::kg},
    {"heatcapacity", precise::J / precise::K},
    {"entropy", precise::J / precise::K},
    {"specificentropy", precise::J / precise::kg / precise::K},
    {"specificenergy", precise::J / precise::kg},
    {"dynamicviscosity", precise::Pa* precise::s},
    {"viscosity", precise::Pa* precise::s},
    {"visc", precise::Pa* precise::s},
    {"fluidity", precise::one / precise::Pa / precise::s},
    {"kinematicviscosity", precise::m.pow(2) / precise::s},
    {"radiantintensity", precise::W / precise::rad.pow(2)},
    {"radiance", precise::W / precise::rad.pow(2) / precise::m.pow(2)},
    {"dimless", precise::one},
    {"ratio", precise::one},
    {"fraction", precise::one},
    {"number", precise::one},
    {"nfr", precise::one},
    {"num", precise::one},
    {"age", precise::time::ag},
    {"impulse", precise::N* precise::s},
    {"imp", precise::N* precise::s},
    {"absorbeddoserate", precise::Gy / precise::s},
    {"mcnc", precise::kg / precise::m.pow(3)},
    {"massconcentration", precise::kg / precise::m.pow(3)},
    {"scnc", precise::mol / precise::m.pow(3)},
    {"scnt", precise::mol / precise::kg},
    {"ncnc", precise::one / precise::m.pow(3)},
    {"ncncn", precise::one / precise::m.pow(3)},
    {"ncnt", precise::one / precise::kg},
    {"substanceconcentration", precise::mol / precise::m.pow(3)},
    {"level", precise::log::neper},
    {"logrto", precise::log::neper},
    {"arvol", precise::m.pow(3) / precise::m.pow(2)},
    {"arsub", precise::mol / precise::m.pow(2)},
    {"lineicvolume", precise::m.pow(3) / precise::m},
    {"lineicnumber", precise::one / precise::m},
    {"refraction", precise::one / precise::m},
    {"naric", precise::one / precise::m.pow(2)},
    {"nlen", precise::one / precise::m},
    {"acidity", precise::laboratory::pH},
    {"cact", precise::kat},
    {"doseequivalent", precise::Sv},
    {"equivalentdose", precise::Sv},
    {"magneticfield", precise::T},
    {"magnetic", precise::T},
    {"absorbeddose", precise::Gy},
    {"ionizingradiationdose", precise::Gy},
    {"iondose", precise::Gy},
    {"exposure", precise::C / precise::kg},
    {"fluence", precise::one / precise::m.pow(2)},
    {"activity", precise::Bq},
    {"mcnt", precise::pu* precise::m},
    {"ccnt", precise::kat / precise::kg},
    {"ccnc", precise::kat / precise::L},
    {"acnc", precise::one / precise::L},
    {"velcnc", precise::m / precise::s / precise::L},
    {"mrat", precise::kg / precise::s},
    {"osmol", precise_unit(1.0, precise::mol, commodities::particles)},
    {"massfraction", precise::pu* precise::kg},
    {"mfr", precise::pu* precise::kg},
    {"amplitudespectraldensity", precise::special::ASD},
    {"fluidresistance", precise::Pa* precise::s / precise::m.pow(3)},
    {"signaltransmissionrate", precise::bit / precise::s},
    {"engmass", precise::J / precise::m.pow(3)},
    {"massicenergy", precise::J / precise::m.pow(3)},
    {"energydensity", precise::J / precise::m.pow(3)},
    {"entsub", precise::mol},
    {"mnum", precise::kg},
    {"cmass", precise::kg / precise::kat},
    {"stiffness", precise::N / precise::m},
    {"signalingrate", precise::bit / precise::s},
    {"datarate", precise::bit / precise::s},
    {"elasticity", precise::N / precise::m.pow(2)},
    {"compliance", precise::m / precise::N},
    {"informationcapacity", precise::data::bit},
    {"compli", precise::m / precise::N},
    {"vascularresistance", precise::Pa* precise::s / precise::m.pow(3)},
    {"enzymaticactivity", precise::kat},
    {"catalyticconcentration", precise::kat / precise::m.pow(3)},
    {"molarenergy", precise::J / precise::mol},
    {"molarentropy", precise::J / precise::mol / precise::K},
};

precise_unit default_unit(std::string unit_type)
{
    if (unit_type.size() == 1) {
        switch (unit_type[0]) {
            case 'L':
                return precise::m;
            case 'M':
                return precise::kg;
            case 'T':
                return precise::second;
            case '\xC8':
                return precise::Kelvin;
            case 'I':
                return precise::A;
            case 'N':
                return precise::mol;
            case 'J':
                return precise::cd;
        }
    }
    std::transform(
        unit_type.begin(), unit_type.end(), unit_type.begin(), ::tolower);
    unit_type.erase(
        std::remove(unit_type.begin(), unit_type.end(), ' '), unit_type.end());
    auto fnd = measurement_types.find(unit_type);
    if (fnd != measurement_types.end()) {
        return fnd->second;
    }
    if (unit_type.compare(0, 10, "quantityof") == 0) {
        return default_unit(unit_type.substr(10));
    }
    auto fof = unit_type.rfind("of");
    if (fof != std::string::npos) {
        return default_unit(unit_type.substr(0, fof));
    }
    fof = unit_type.find_first_of("([{");
    if (fof != std::string::npos) {
        return default_unit(unit_type.substr(0, fof));
    }
    if (unit_type.compare(0, 3, "inv") == 0) {
        auto tunit = default_unit(unit_type.substr(3));
        if (!is_error(tunit)) {
            return tunit.inv();
        }
    }
    if (ends_with(unit_type, "rto")) {
        // ratio of some kind
        return precise::one;
    }
    if (ends_with(unit_type, "fr")) {
        // ratio of some kind
        return precise::one;
    }
    if (ends_with(unit_type, "quantity")) {
        return default_unit(
            unit_type.substr(0, unit_type.size() - strlen("quantity")));
    }
    if (ends_with(unit_type, "quantities")) {
        return default_unit(
            unit_type.substr(0, unit_type.size() - strlen("quantities")));
    }
    if (ends_with(unit_type, "measure")) {
        return default_unit(
            unit_type.substr(0, unit_type.size() - strlen("measure")));
    }
    if (unit_type.back() == 's' && unit_type.size() > 1) {
        unit_type.pop_back();
        return default_unit(unit_type);
    }
    return precise::invalid;
}

#ifdef ENABLE_UNIT_MAP_ACCESS
namespace detail {
    const std::unordered_map<std::string, precise_unit>& getUnitStringMap()
    {
        return base_unit_vals;
    }
    const std::unordered_map<unit, const char*>& getUnitNameMap()
    {
        return base_unit_names;
    }
}  // namespace detail
#endif

}  // namespace UNITS_NAMESPACE
