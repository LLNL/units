/*
Copyright (c) 2019-2023,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#include "units.hpp"
#include "units_conversion_maps.hpp"

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

#if (__cplusplus >= 201703L) || (defined(_MSVC_LANG) && _MSVC_LANG >= 201703)
#ifndef UNITS_CONSTEXPR_IF_SUPPORTED
#define UNITS_CONSTEXPR_IF_SUPPORTED
#endif
#endif

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
    return unit{
        numericalRoot(un.multiplier(), power), un.base_units().root(power)};
}

precise_unit root(const precise_unit& un, int power)
{
    if (power == 0) {
        return precise::one;
    }
    if (un.multiplier() < 0.0 && power % 2 == 0) {
        return precise::invalid;
    }
    return precise_unit{
        numericalRoot(un.multiplier(), power), un.base_units().root(power)};
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

static umap getDefinedBaseUnitNames()
{
    umap definedNames{};
    for (const auto& name : defined_unit_names_si) {
        if (name.second != nullptr) {
            definedNames.emplace(name.first, name.second);
        }
    }
    for (const auto& name : defined_unit_names_customary) {
        if (name.second != nullptr) {
            definedNames.emplace(name.first, name.second);
        }
    }
    return definedNames;
}

static const umap base_unit_names = getDefinedBaseUnitNames();

using ustr = std::pair<precise_unit, const char*>;
// units to divide into tests to explore common multiplier units
static UNITS_CPP14_CONSTEXPR_OBJECT std::array<ustr, 30> testUnits{
    {ustr{precise::s, "s"},
     ustr{precise::s.pow(2), "s^2"},  // second squared need to come before
                                      // meter to deal with accelleration units
     ustr{precise::m, "m"},
     ustr{precise::kg, "kg"},
     ustr{precise::mol, "mol"},
     ustr{precise::currency, "$"},
     ustr{precise::rad, "rad"},
     ustr{precise::count, "item"},
     ustr{precise::candela, "cd"},
     ustr{precise::Ampere, "A"},
     ustr{precise::Kelvin, "K"},
     ustr{precise::ms, "ms"},
     ustr{precise::min, "min"},
     ustr{precise::hr, "hr"},
     ustr{precise::time::day, "day"},
     ustr{precise::lb, "lb"},
     ustr{precise::ft, "ft"},
     ustr{precise::mile, "mi"},
     ustr{constants::c.as_unit(), "[c]"},
     ustr{constants::h.as_unit(), "[h]"},
     ustr{precise::L, "L"},
     ustr{precise::km, "km"},
     ustr{precise::volt, "V"},
     ustr{precise::watt, "W"},
     ustr{precise::electrical::kW, "kW"},
     ustr{precise::electrical::mW, "mW"},
     ustr{precise::MW, "MW"},
     ustr{precise::giga * precise::W, "GW"},
     ustr{precise::energy::eV, "eV"},
     ustr{precise::count, "item"}}};

// units to divide into tests to explore common multiplier units which can be
// multiplied by power
static UNITS_CPP14_CONSTEXPR_OBJECT std::array<ustr, 6> testPowerUnits{
    {ustr{precise::s, "s"},
     ustr{precise::m, "m"},
     ustr{precise::radian, "rad"},
     ustr{precise::km, "km"},
     ustr{precise::ft, "ft"},
     ustr{precise::mile, "mi"}}};

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

// this is not constexpr to ensure it is done at runtime
static float invert(precise_unit val)
{
    return (1.0F / val.multiplier_f());
}
// thought about making this constexpr array, but the problem is that runtime
// floats are not guaranteed to be the same as compile time floats so really
// this map needs to be generated at run-time once multiplier prefixes commonly
// used
static const std::unordered_map<float, char> si_prefixes{
    {precise::milli.multiplier_f(), 'm'},  {invert(precise::kilo), 'm'},
    {precise::kilo.multiplier_f(), 'k'},   {invert(precise::milli), 'k'},
    {precise::micro.multiplier_f(), 'u'},  {invert(precise::mega), 'u'},
    {precise::centi.multiplier_f(), 'c'},  {invert(precise::hecto), 'c'},
    {precise::mega.multiplier_f(), 'M'},   {invert(precise::micro), 'M'},
    {precise::giga.multiplier_f(), 'G'},   {invert(precise::nano), 'G'},
    {precise::nano.multiplier_f(), 'n'},   {invert(precise::giga), 'n'},
    {precise::pico.multiplier_f(), 'p'},   {invert(precise::tera), 'p'},
    {precise::femto.multiplier_f(), 'f'},  {invert(precise::peta), 'f'},
    {precise::atto.multiplier_f(), 'a'},   {invert(precise::exa), 'a'},
    {precise::tera.multiplier_f(), 'T'},   {invert(precise::pico), 'T'},
    {precise::peta.multiplier_f(), 'P'},   {invert(precise::femto), 'P'},
    {precise::exa.multiplier_f(), 'E'},    {invert(precise::atto), 'E'},
    {precise::zetta.multiplier_f(), 'Z'},  {invert(precise::zepto), 'Z'},
    {precise::yotta.multiplier_f(), 'Y'},  {invert(precise::yocto), 'Y'},
    {precise::zepto.multiplier_f(), 'z'},  {invert(precise::zetta), 'z'},
    {precise::yocto.multiplier_f(), 'y'},  {invert(precise::yotta), 'y'},

    {precise::ronna.multiplier_f(), 'R'},  {invert(precise::ronto), 'R'},
    {precise::quetta.multiplier_f(), 'Q'}, {invert(precise::quecto), 'Q'},
    {precise::ronto.multiplier_f(), 'r'},  {invert(precise::ronna), 'r'},
    {precise::quecto.multiplier_f(), 'q'}, {invert(precise::quetta), 'q'},
};

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
    std::uint64_t match_flags);

// forward declaration of the quick find function
static precise_unit
    unit_quick_match(std::string unit_string, std::uint64_t match_flags);
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
    int replaceSize,
    std::size_t& firstReplacementIndex)
{
    bool changed{false};
    std::size_t pos{0};
    while ((pos = subject.find(search, pos)) != std::string::npos) {
        subject.replace(pos, searchSize, replace);
        if (!changed) {
            changed = true;
            firstReplacementIndex = pos;
        }
        pos += replaceSize;
    }
    return changed;
}

// Generate an SI prefix or a numerical multiplier string for prepending a unit
static std::string getMultiplierString(double multiplier, bool numOnly = false)
{
    if (multiplier == 1.0) {
        return {};
    }
    if (!numOnly) {
        auto si = si_prefixes.find(static_cast<float>(multiplier));
        if (si != si_prefixes.end()) {
            // NOLINTNEXTLINE(
            return std::string(1UL, si->second);
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
#ifdef UNITS_CONSTEXPR_IF_SUPPORTED
            if constexpr (
                detail::bitwidth::base_size == sizeof(std::uint32_t)) {
                pw = seq[pwerloc + 1] - '0';
            } else {
                if (seq.size() <= pwerloc + 2 ||
                    !isDigitCharacter(seq[pwerloc + 2])) {
                    pw = seq[pwerloc + 1] - '0';
                } else {
                    pw = 10;
                }
            }
#else
            if (detail::bitwidth::base_size == sizeof(std::uint32_t) ||
                seq.size() <= pwerloc + 2 ||
                !isDigitCharacter(seq[pwerloc + 2])) {
                pw = seq[pwerloc + 1] - '0';
            } else {
                pw = 10;
            }
#endif
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
    auto mloc = seq.find_first_of("*/)");
    if (mloc < pwerloc) {
        return getMultiplierString(mux, noPrefix) + seq;
    }
    int offset = (seq[pwerloc + 1] != '(') ? 1 : 2;
    int pw = stoi(seq.substr(pwerloc + offset, mloc - pwerloc - offset + 1));
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

// check whether large power strings should be allowed
static bool allowLargePowers(std::uint64_t flags)
{
    return (
        detail::bitwidth::base_size > 4 &&
        (flags & disable_large_power_strings) == 0U);
}

// Add a unit power to a string
static void addUnitPower(
    std::string& str,
    const char* unit,
    int power,
    std::uint64_t flags)
{
    bool div{false};
    if (power != 0) {
        if (!str.empty()) {
            if (str.back() != '/') {
                if (str.back() != '*') {
                    str.push_back('*');
                }
            } else {
                div = true;
            }
        }

        str.append(unit);
        if (power != 1) {
            str.push_back('^');
            if (std::labs(power) < 10) {
                if (power < 0) {
                    str.push_back('-');
                    str.push_back('0' - power);
                } else {
                    str.push_back('0' + power);
                }
            } else {
                if (allowLargePowers(flags)) {
                    str.push_back('(');
                    str.append(std::to_string(power));
                    str.push_back(')');
                } else {
                    if (power < 0) {
                        str.push_back('-');
                        str.push_back('9');
                        power += 9;
                    } else {
                        str.push_back('9');
                        power -= 9;
                    }
                    if (div) {
                        str.push_back('/');
                    }
                    addUnitPower(str, unit, power, flags);
                }
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

/** add the unit power to the string if it is positive and return 0, return 1 if
negative and skip the return value is if any power remains
 */
static inline int addPosUnits(
    std::string& str,
    const char* unitName,
    int power,
    std::uint64_t flags)
{
    if (power > 0) {
        addUnitPower(str, unitName, power, flags);
    }
    return (power < 0) ? 1 : 0;
}

/** add the unit power if it is negative, and skip if >= 0
 */
static inline void addNegUnits(
    std::string& str,
    const char* unitName,
    int power,
    std::uint64_t flags)
{
    if (power < 0) {
        addUnitPower(str, unitName, power, flags);
    }
}

static std::string
    generateRawUnitString(const precise_unit& un, std::uint64_t flags)
{
    std::string val;
    auto bu = un.base_units();
    int cnt{0};
    cnt += addPosUnits(val, "m", bu.meter(), flags);
    cnt += addPosUnits(val, "kg", bu.kg(), flags);
    cnt += addPosUnits(val, "s", bu.second(), flags);
    cnt += addPosUnits(val, "A", bu.ampere(), flags);
    cnt += addPosUnits(val, "K", bu.kelvin(), flags);
    cnt += addPosUnits(val, "mol", bu.mole(), flags);
    cnt += addPosUnits(val, "cd", bu.candela(), flags);
    cnt += addPosUnits(val, "item", bu.count(), flags);
    cnt += addPosUnits(val, "$", bu.currency(), flags);
    cnt += addPosUnits(val, "rad", bu.radian(), flags);
    addUnitFlagStrings(un, val);
    if (cnt == 1) {
        if (bu.second() == -1 && val.empty()) {
            // deal with 1/s  which is usually Hz
            addPosUnits(val, "Hz", 1, flags);
            return val;
        }
        val.push_back('/');
        addPosUnits(val, "m", -bu.meter(), flags);
        addPosUnits(val, "kg", -bu.kg(), flags);
        addPosUnits(val, "s", -bu.second(), flags);
        addPosUnits(val, "A", -bu.ampere(), flags);
        addPosUnits(val, "K", -bu.kelvin(), flags);
        addPosUnits(val, "mol", -bu.mole(), flags);
        addPosUnits(val, "cd", -bu.candela(), flags);
        addPosUnits(val, "item", -bu.count(), flags);
        addPosUnits(val, "$", -bu.currency(), flags);
        addPosUnits(val, "rad", -bu.radian(), flags);
    } else if (cnt > 1) {
        addNegUnits(val, "m", bu.meter(), flags);
        addNegUnits(val, "kg", bu.kg(), flags);
        addNegUnits(val, "s", bu.second(), flags);
        addNegUnits(val, "A", bu.ampere(), flags);
        addNegUnits(val, "K", bu.kelvin(), flags);
        addNegUnits(val, "mol", bu.mole(), flags);
        addNegUnits(val, "cd", bu.candela(), flags);
        addNegUnits(val, "item", bu.count(), flags);
        addNegUnits(val, "$", bu.currency(), flags);
        addNegUnits(val, "rad", bu.radian(), flags);
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

static constexpr int getDefaultDomain()
{
#ifdef UNITS_DEFAULT_DOMAIN
    return UNITS_DEFAULT_DOMAIN;
#else
    return domains::defaultDomain;
#endif
}

// how different unit strings can be specified to mean different things
static std::uint64_t unitsDomain{getDefaultDomain()};

std::uint64_t setUnitsDomain(std::uint64_t newDomain)
{
    std::swap(newDomain, unitsDomain);
    return newDomain;
}

static constexpr std::uint64_t getDefaultMatchFlags()
{
#ifdef UNITS_DEFAULT_MATCH_FLAGS
    return UNITS_DEFAULT_MATCH_FLAGS;
#else
    return 0ULL;
#endif
}

static std::uint64_t defaultMatchFlags{getDefaultMatchFlags()};

std::uint64_t setDefaultFlags(std::uint64_t defaultFlags)
{
    std::swap(defaultMatchFlags, defaultFlags);
    return defaultFlags;
}

std::uint64_t getDefaultFlags()
{
    return defaultMatchFlags;
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

void removeUserDefinedUnit(const std::string& name)
{
    // get the currently converted unit
    auto unit = unit_cast_from_string(name);
    if (is_valid(unit)) {
        user_defined_units.erase(name);
        user_defined_unit_names.erase(unit);
    } else {
        for (const auto& udun : user_defined_unit_names) {
            if (udun.second == name) {
                user_defined_unit_names.erase(udun.first);
                break;
            }
        }
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

void addUserDefinedOutputUnit(const std::string& name, const precise_unit& un)
{
    if (allowUserDefinedUnits.load(std::memory_order_acquire)) {
        user_defined_unit_names[unit_cast(un)] = name;
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
            if (length == 0 && line[sep - 1] == '<') {
                length = 1;
                --sep;
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
            } else if (line[sep] == '<') {
                addUserDefinedOutputUnit(userdef, meas.as_unit());
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
    static UNITS_CPP14_CONSTEXPR_OBJECT std::array<spair, 11> powerseq{
        {spair{"Mm^3", "(1e9km^3)", 4, 8},  // this needs to happen before ^3^2
                                            // conversions
         spair{"^2^2", "^4", 4, 2},
         spair{"^3^2", "^6", 4, 2},
         spair{"^2^3", "^6", 4, 2},
         spair{"^3^3", "^9", 4, 2},  // this can only happen with extended units
         spair{"Gs", "Bs", 2, 2},
         spair{"*K^", "*1*K^", 3, 5},  // this one is to prevent the next from
                                       // screwing things up
         spair{"eflag*K", "degC", 7, 4},
         spair{"*1*", "*", 3, 1},
         spair{"*1/", "/", 3, 1},
         spair{"*/", "/", 2, 1}}};
    // run a few checks for unusual conditions
    for (const auto& pseq : powerseq) {
        auto fnd = propUnitString.find(std::get<0>(pseq));
        while (fnd != std::string::npos) {
            propUnitString.replace(fnd, std::get<2>(pseq), std::get<1>(pseq));
            fnd =
                propUnitString.find(std::get<0>(pseq), fnd + std::get<3>(pseq));
        }
    }

    if (!propUnitString.empty()) {
        if (propUnitString.front() == '(' && propUnitString.back() == ')') {
            if (propUnitString.find_first_of('(', 1) == std::string::npos) {
                propUnitString.pop_back();
                propUnitString.erase(propUnitString.begin());
            }
        }
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
                if (propUnitString[locp] != '^' ||
                    propUnitString[locp + 1] != '-') {
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
                if (locp == std::string::npos || locp < loc) {
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
{  // cppcheck suppression active
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
{  // cppcheck suppression active
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

static std::string probeUnit(
    const precise_unit& un,
    const std::pair<precise_unit, const char*>& probe)
{
    // let's try common divisor units
    auto ext = un * probe.first;
    auto fnd = find_unit(unit_cast(ext));
    if (!fnd.empty()) {
        return fnd + '/' + probe.second;
    }
    // let's try inverse of common multiplier units
    fnd = find_unit(unit_cast(ext.inv()));
    if (!fnd.empty()) {
        return std::string("1/(") + fnd + '*' + probe.second + ')';
    }

    // let's try common multiplier units
    ext = un / probe.first;
    fnd = find_unit(unit_cast(ext));
    if (!fnd.empty()) {
        return fnd + '*' + probe.second;
    }
    // let's try common divisor with inv units
    fnd = find_unit(unit_cast(ext.inv()));
    if (!fnd.empty()) {
        return std::string(probe.second) + '/' + fnd;
    }
    return std::string{};
}

static std::string probeUnitBase(
    const precise_unit& un,
    const std::pair<precise_unit, const char*>& probe)
{
    std::string beststr;
    // let's try common divisor units on base units
    auto ext = un * probe.first;
    auto base = unit(ext.base_units());
    auto fnd = find_unit(base);
    if (!fnd.empty()) {
        auto prefix = generateUnitSequence(ext.multiplier(), fnd);

        auto str = prefix + '/' + probe.second;
        if (!isNumericalStartCharacter(str.front())) {
            return str;
        }
        if (beststr.empty() || str.size() < beststr.size()) {
            beststr = str;
        }
    }
    // let's try inverse of common multiplier units on base units
    fnd = find_unit(base.inv());
    if (!fnd.empty()) {
        auto prefix = getMultiplierString(
            1.0 / ext.multiplier(), isDigitCharacter(fnd.back()));
        std::string str{"1/("};
        str += prefix;
        str += fnd;
        str.push_back('*');
        str.append(probe.second);
        str.push_back(')');
        if (prefix.empty() || !isNumericalStartCharacter(prefix.front())) {
            return str;
        }
        if (beststr.empty() || str.size() < beststr.size()) {
            beststr = std::move(str);
        }
    }
    // let's try common multiplier units on base units
    ext = un / probe.first;
    base = unit(ext.base_units());
    fnd = find_unit(base);
    if (!fnd.empty()) {
        auto prefix = generateUnitSequence(ext.multiplier(), fnd);
        auto str = prefix + '*' + probe.second;
        if (!isNumericalStartCharacter(str.front())) {
            return str;
        }
        if (beststr.empty() || str.size() < beststr.size()) {
            beststr = str;
        }
    }
    // let's try common divisor with inv units on base units
    fnd = find_unit(base.inv());
    if (!fnd.empty()) {
        auto prefix = generateUnitSequence(1.0 / ext.multiplier(), fnd);
        if (isNumericalStartCharacter(prefix.front())) {
            size_t cut{0};
            double mx = getDoubleFromString(prefix, &cut);

            auto str = getMultiplierString(1.0 / mx, true) + probe.second +
                "/" + prefix.substr(cut);
            if (beststr.empty() || str.size() < beststr.size()) {
                beststr = str;
            }

        } else {
            return std::string(probe.second) + "/" + prefix;
        }
    }
    return beststr;
}

static std::string
    to_string_internal(precise_unit un, std::uint64_t match_flags)
{
    switch (std::fpclassify(un.multiplier())) {
        case FP_INFINITE: {
            std::string inf = (un.multiplier() > 0.0) ? "INF" : "-INF";
            un = precise_unit(un.base_units());
            if (un == precise::one) {
                return inf;
            }
            return inf + '*' + to_string_internal(un, match_flags);
        }
        case FP_NAN:
            un = precise_unit(un.base_units());
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
            un = precise_unit(un.base_units());
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
        un = precise_unit(un.base_units());
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
        un = precise_unit(un.base_units());
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
                urem = precise_unit(urem.base_units());
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
    /** check for si prefixes on common units*/

    if (un.unit_type_count() == 1) {
        return generateUnitSequence(
            un.multiplier(), generateRawUnitString(un, match_flags));
    }
    if (un.unit_type_count() == 2 && un.multiplier() == 1) {
        return generateUnitSequence(
            1.0, generateRawUnitString(un, match_flags));
    }
    /** check for a few units with odd numbers that allow SI prefixes*/
    for (const auto& siU : siTestUnits) {
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
                nu = precise_unit{nu.base_units()};
                std::string rstring = mult + siU.second;
                rstring.push_back('*');
                rstring.append(to_string_internal(nu, match_flags));
                return rstring;
            }
        }
        nu = un * siU.first;
        if (nu.unit_type_count() == 0) {
            auto mult = getMultiplierString(1.0 / nu.multiplier());
            if (mult.empty() || !isNumericalStartCharacter(mult.front())) {
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
                nu = precise_unit{nu.base_units()};
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
            size_t cut{0};
            double mx = getDoubleFromString(prefix, &cut);
            return getMultiplierString(1.0 / mx, true) + "/" +
                prefix.substr(cut);
        }
        return std::string("1/") + prefix;
    }

    // let's try common unit combinations
    std::string beststr;

    for (const auto& tu : testUnits) {
        auto str = probeUnit(un, tu);
        if (!str.empty()) {
            return str;
        }
        str = probeUnitBase(un, tu);
        if (!str.empty()) {
            if (!isNumericalStartCharacter(str.front())) {
                return str;
            }
            if (beststr.empty() || str.size() < beststr.size()) {
                beststr = str;
            }
        }
    }

    if (allowUserDefinedUnits.load(std::memory_order_acquire)) {
        for (const auto& udu : user_defined_unit_names) {
            auto res = probeUnit(
                un,
                std::make_pair(precise_unit(udu.first), udu.second.c_str()));
            if (!res.empty()) {
                return res;
            }
            std::string nstring = std::string(udu.second) + "^2";
            res = probeUnit(
                un,
                std::make_pair(
                    precise_unit(udu.first).pow(2), nstring.c_str()));
            if (!res.empty()) {
                return res;
            }
            nstring = std::string(udu.second) + "^3";
            res = probeUnit(
                un,
                std::make_pair(
                    precise_unit(udu.first).pow(3), nstring.c_str()));
            if (!res.empty()) {
                return res;
            }
        }
    }

    // let's try common units that are often multiplied by power
    for (const auto& tu : testPowerUnits) {
        std::string nstring = std::string(tu.second) + "^2";
        auto res = probeUnit(
            un, std::make_pair(precise_unit(tu.first).pow(2), nstring.c_str()));
        if (!res.empty()) {
            return res;
        }
        nstring = std::string(tu.second) + "^3";
        res = probeUnit(
            un, std::make_pair(precise_unit(tu.first).pow(3), nstring.c_str()));
        if (!res.empty()) {
            return res;
        }
    }

    if (allowUserDefinedUnits.load(std::memory_order_acquire)) {
        for (const auto& udu : user_defined_unit_names) {
            auto str = probeUnitBase(
                un,
                std::make_pair(precise_unit(udu.first), udu.second.c_str()));
            if (!str.empty()) {
                if (!isNumericalStartCharacter(str.front())) {
                    return str;
                }
                if (beststr.empty() || str.size() < beststr.size()) {
                    beststr = str;
                }
            }
            std::string nstring = std::string(udu.second) + "^2";
            str = probeUnitBase(
                un,
                std::make_pair(
                    precise_unit(udu.first).pow(2), nstring.c_str()));
            if (!str.empty()) {
                if (!isNumericalStartCharacter(str.front())) {
                    return str;
                }
                if (beststr.empty() || str.size() < beststr.size()) {
                    beststr = str;
                }
            }

            nstring = std::string(udu.second) + "^3";
            str = probeUnitBase(
                un,
                std::make_pair(
                    precise_unit(udu.first).pow(3), nstring.c_str()));
            if (!str.empty()) {
                if (!isNumericalStartCharacter(str.front())) {
                    return str;
                }
                if (beststr.empty() || str.size() < beststr.size()) {
                    beststr = str;
                }
            }
        }
    }
    for (const auto& tu : testPowerUnits) {
        std::string nstring = std::string(tu.second) + "^2";
        auto str = probeUnitBase(
            un, std::make_pair(precise_unit(tu.first).pow(2), nstring.c_str()));
        if (!str.empty()) {
            if (!isNumericalStartCharacter(str.front())) {
                return str;
            }
            if (beststr.empty() || str.size() < beststr.size()) {
                beststr = str;
            }
        }

        nstring = std::string(tu.second) + "^3";
        str = probeUnitBase(
            un, std::make_pair(precise_unit(tu.first).pow(3), nstring.c_str()));
        if (!str.empty()) {
            if (!isNumericalStartCharacter(str.front())) {
                return str;
            }
            if (beststr.empty() || str.size() < beststr.size()) {
                beststr = str;
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
        for (const auto& reduce : creduceUnits) {
            auto od = 1 + order(unit_cast(un * reduce.first));
            if (od < minorder) {
                minorder = od;
                mino_unit = un * reduce.first;
                min_mult = reduce.second;
            }
        }
    }
    return generateUnitSequence(
        mino_unit.multiplier(),
        min_mult + generateRawUnitString(mino_unit, match_flags));
}

std::string to_string(const precise_unit& un, std::uint64_t match_flags)
{
    return clean_unit_string(
        to_string_internal(un, match_flags), un.commodity());
}

std::string
    to_string(const precise_measurement& measure, std::uint64_t match_flags)
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

std::string to_string(const measurement& measure, std::uint64_t match_flags)
{
    std::stringstream ss;
    ss.precision(6);
    ss << measure.value();

    auto str = to_string(measure.units(), match_flags);
    if (!str.empty()) {
        ss << ' ';
        if (isNumericalStartCharacter(str.front())) {
            str.insert(str.begin(), '(');
            str.push_back(')');
        }
        ss << str;
    }
    return ss.str();
}

std::string
    to_string(const uncertain_measurement& measure, std::uint64_t match_flags)
{
    // compute the correct number of digits to display for uncertain precision
    auto digits = static_cast<std::streamsize>(
        ceil(-log10(measure.fractional_uncertainty())));
    digits = (digits < 2) ? 2 : digits + 1;
    std::stringstream ss;
    ss.precision(digits);
    ss << measure.value_f();
    ss << "+/-";
    ss.precision(2);
    ss << measure.uncertainty_f() << ' ';
    ss.precision(digits);
    ss << to_string(measure.units(), match_flags);
    return ss.str();
}

/// Generate the prefix multiplier for units (including SI)
static double getPrefixMultiplier(char p)
{
    switch (p) {
        case 'm':
            return precise::milli.multiplier();
        case 'k':
        case 'K':
            return precise::kilo.multiplier();
        case 'M':
            return precise::mega.multiplier();
        case 'u':
        case 'U':
        case '\xB5':  // latin-1 encoding "micro"
            return precise::micro.multiplier();
        case 'd':
        case 'D':
            return precise::deci.multiplier();
        case 'c':
        case 'C':
            return precise::centi.multiplier();
        case 'h':
        case 'H':
            return precise::hecto.multiplier();
        case 'n':
            return precise::nano.multiplier();
        case 'p':
            return precise::pico.multiplier();
        case 'G':
        case 'B':  // Billion
            return precise::giga.multiplier();
        case 'T':
            return precise::tera.multiplier();
        case 'f':
        case 'F':
            return precise::femto.multiplier();
        case 'E':
            return precise::exa.multiplier();
        case 'P':
            return precise::peta.multiplier();
        case 'Z':
            return precise::zetta.multiplier();
        case 'Y':
            return precise::yotta.multiplier();
        case 'y':
            return precise::yocto.multiplier();
        case 'a':
        case 'A':
            return precise::atto.multiplier();
        case 'z':
            return precise::zepto.multiplier();
        case 'R':
            return precise::ronna.multiplier();
        case 'r':
            return precise::ronto.multiplier();
        case 'Q':
            return precise::quetta.multiplier();
        case 'q':
            return precise::quecto.multiplier();
        default:
            return 0.0;
    }
}

/// Generate the prefix multiplier for strict SI units
static double getStrictSIPrefixMultiplier(char p)
{
    switch (p) {
        case 'm':
            return precise::milli.multiplier();
        case 'k':
            return precise::kilo.multiplier();
        case 'M':
            return precise::mega.multiplier();
        case 'u':
        case '\xB5':  // latin-1 encoding "micro"
            return precise::micro.multiplier();
        case 'd':
            return precise::deci.multiplier();
        case 'c':
            return precise::centi.multiplier();
        case 'h':
            return precise::hecto.multiplier();
        case 'n':
            return precise::nano.multiplier();
        case 'p':
            return precise::pico.multiplier();
        case 'G':
            return precise::giga.multiplier();
        case 'T':
            return precise::tera.multiplier();
        case 'f':
            return precise::femto.multiplier();
        case 'E':
            return precise::exa.multiplier();
        case 'P':
            return precise::peta.multiplier();
        case 'Z':
            return precise::zetta.multiplier();
        case 'Y':
            return precise::yotta.multiplier();
        case 'a':
            return precise::atto.multiplier();
        case 'z':
            return precise::zepto.multiplier();
        case 'y':
            return precise::yocto.multiplier();
        case 'R':
            return precise::ronna.multiplier();
        case 'r':
            return precise::ronto.multiplier();
        case 'Q':
            return precise::quetta.multiplier();
        case 'q':
            return precise::quecto.multiplier();
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
    static UNITS_CPP14_CONSTEXPR_OBJECT std::array<cpair, 25> char2prefix{{
        cpair{charindex('D', 'A'), precise::deka.multiplier()},
        cpair{charindex('E', 'X'), precise::exa.multiplier()},
        cpair{charindex('E', 'i'), precise::exbi.multiplier()},
        cpair{charindex('G', 'A'), precise::giga.multiplier()},
        cpair{charindex('G', 'i'), precise::gibi.multiplier()},
        cpair{charindex('K', 'i'), precise::kibi.multiplier()},
        cpair{charindex('M', 'A'), precise::mega.multiplier()},
        cpair{charindex('M', 'M'), precise::mega.multiplier()},
        cpair{charindex('M', 'i'), precise::mebi.multiplier()},
        cpair{charindex('P', 'T'), precise::peta.multiplier()},
        cpair{charindex('P', 'i'), precise::pebi.multiplier()},
        cpair{charindex('T', 'R'), precise::tera.multiplier()},
        cpair{charindex('T', 'i'), precise::tebi.multiplier()},
        cpair{charindex('Y', 'A'), precise::yotta.multiplier()},
        cpair{charindex('Y', 'O'), precise::yocto.multiplier()},
        cpair{charindex('Y', 'i'), precise::yobi.multiplier()},
        cpair{charindex('R', 'i'), precise::robi.multiplier()},
        cpair{charindex('Q', 'i'), precise::qubi.multiplier()},
        cpair{charindex('Z', 'A'), precise::zetta.multiplier()},
        cpair{charindex('Z', 'O'), precise::zepto.multiplier()},
        cpair{charindex('Z', 'i'), precise::zebi.multiplier()},
        cpair{charindex('d', 'a'), precise::deka.multiplier()},
        cpair{charindex('m', 'A'), precise::mega.multiplier()},
        cpair{charindex('m', 'c'), precise::micro.multiplier()},
        cpair{charindex('p', 'T'), precise::peta.multiplier()},
    }};
    auto code = charindex(c1, c2);
    // NOLINTNEXTLINE (readability-qualified-auto)
    const auto fnd = std::lower_bound(
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
    double val{constants::invalid_conversion};
    if (ustring.front() == '(') {
        size_t ival{1};
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
            size_t ind{0};
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
        index = 0;
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
                index = 0;
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
    for (const auto& num : lt10) {
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
    for (const auto& num : teens) {
        if (str.compare(index, std::get<2>(num), std::get<0>(num)) == 0) {
            index += std::get<2>(num);
            return std::get<1>(num);
        }
    }
    return constants::invalid_conversion;
}

// NOTE: the ordering is important here
static UNITS_CPP14_CONSTEXPR_OBJECT std::array<wordpair, 6> groupNumericalWords{
    {wordpair{"quadrillion", 1e15, 11},
     wordpair{"trillion", 1e12, 8},
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
    std::string lcstring{ustring};
    // make the string lower case for consistency
    std::transform(
        lcstring.begin(), lcstring.end(), lcstring.begin(), ::tolower);
    for (const auto& wp : groupNumericalWords) {
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
    for (const auto& wp : decadeWords) {
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

        void testAddUnitPower(
            std::string& str,
            const char* unit,
            int power,
            std::uint64_t flags)
        {
            return addUnitPower(str, unit, power, flags);
        }
    }  // namespace testing
}  // namespace detail

#endif

/** Words of SI prefixes
https://physics.nist.gov/cuu/Units/prefixes.html
https://physics.nist.gov/cuu/Units/binary.html
*/
using utup = std::tuple<const char*, double, int>;
static UNITS_CPP14_CONSTEXPR_OBJECT std::array<utup, 36> prefixWords{{
    utup{"atto", precise::atto.multiplier(), 4},
    utup{"centi", precise::centi.multiplier(), 5},
    utup{"deca", precise::deka.multiplier(), 4},
    utup{"deci", precise::deci.multiplier(), 4},
    utup{"deka", precise::deka.multiplier(), 4},
    utup{"exa", precise::exa.multiplier(), 3},
    utup{"exbi", precise::exbi.multiplier(), 4},
    utup{"femto", precise::femto.multiplier(), 5},
    utup{"gibi", precise::gibi.multiplier(), 4},
    utup{"giga", precise::giga.multiplier(), 4},
    utup{"hecto", precise::hecto.multiplier(), 5},
    utup{"kibi", precise::kibi.multiplier(), 4},
    utup{"kilo", precise::kilo.multiplier(), 4},
    utup{"mebi", precise::mebi.multiplier(), 4},
    utup{"mega", precise::mega.multiplier(), 4},
    utup{"micro", precise::micro.multiplier(), 5},
    utup{"milli", precise::milli.multiplier(), 5},
    utup{"nano", precise::nano.multiplier(), 4},
    utup{"pebi", precise::pebi.multiplier(), 4},
    utup{"peta", precise::peta.multiplier(), 4},
    utup{"pico", precise::pico.multiplier(), 4},
    utup{"tebi", precise::tebi.multiplier(), 4},
    utup{"tera", precise::tera.multiplier(), 4},
    utup{"yocto", precise::yocto.multiplier(), 5},
    utup{"yotta", precise::yotta.multiplier(), 4},
    utup{"zepto", precise::zepto.multiplier(), 5},
    utup{"zetta", precise::zetta.multiplier(), 5},
    utup{"zebi", precise::zebi.multiplier(), 4},
    utup{"yobi", precise::yobi.multiplier(), 4},
    utup{"robi", precise::robi.multiplier(), 4},
    utup{"qubi", precise::qubi.multiplier(), 4},
    utup{"ronto", precise::ronto.multiplier(), 5},
    utup{"quecto", precise::quecto.multiplier(), 6},
    utup{"ronna", precise::ronna.multiplier(), 5},
    utup{"quetta", precise::quetta.multiplier(), 5},
    // this one was proposed (not accepted but finds occasional use)
    utup{"hella", 1e27, 5},
}};

static const std::array<std::string, 4> Esegs{{"()", "[]", "{}", "<>"}};

bool clearEmptySegments(std::string& unit)
{
    bool changed = false;
    for (const auto& seg : Esegs) {
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
    get_unit(const std::string& unit_string, std::uint64_t match_flags);

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
    static UNITS_CPP14_CONSTEXPR_OBJECT std::array<modSeq, 39> modifiers{{
        modSeq{"reciprocalsquare", "^-2", 16, modifier::start_tail},
        modSeq{"reciprocalcubic", "^-3", 15, modifier::start_tail},
        modSeq{"squaremeter", "m^2", 11, modifier::anywhere_tail},
        modSeq{"cubicmeter", "m^3", 10, modifier::anywhere_tail},
        modSeq{"cubic", "^3", 5, modifier::start_tail},
        modSeq{"reciprocal", "^-1", 10, modifier::start_tail},
        modSeq{"reciprocal", "^-1", 10, modifier::tail_replace},
        modSeq{"square", "^2", 6, modifier::start_tail},
        modSeq{"squared", "^2", 7, modifier::start_tail},
        modSeq{"cubed", "^2", 7, modifier::start_tail},
        modSeq{"cu", "^3", 2, modifier::start_tail},
        modSeq{"sq", "^2", 2, modifier::start_tail},
        modSeq{"tenthousand", "10000", 11, modifier::anywhere_replace},
        modSeq{"tenths", "0.1", 5, modifier::anywhere_replace},
        modSeq{"tenth", "0.1", 5, modifier::anywhere_replace},
        modSeq{"ten", "10", 3, modifier::anywhere_replace},
        modSeq{"one", "", 3, modifier::start_replace},
        modSeq{"quarter", "0.25", 7, modifier::anywhere_replace},
        modSeq{"eighth", "0.125", 6, modifier::anywhere_replace},
        modSeq{"sixteenth", "0.0625", 9, modifier::anywhere_replace},
        modSeq{"thirtyseconds", "0.03125", 13, modifier::anywhere_replace},
        modSeq{"sixtyfourths", "0.015625", 12, modifier::anywhere_replace},
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
        modSeq{"squared", "^2", 7, modifier::anywhere_replace},
        modSeq{"cubed", "^3", 5, modifier::anywhere_replace},
        modSeq{"square", "^2", 6, modifier::anywhere_tail},
        modSeq{"cubic", "^3", 5, modifier::anywhere_tail},
        modSeq{"sq", "^2", 2, modifier::tail_replace},
        modSeq{"cu", "^3", 2, modifier::tail_replace},
        modSeq{"u", "unit", 1, modifier::tail_replace},
    }};
    if (unit.compare(0, 3, "cup") ==
        0) {  // this causes too many issues so skip it
        return false;
    }
    if (unit.compare(0, 13, "hundredweight") ==
        0) {  // this is a specific unit and should not be cut off
        return false;
    }
    for (const auto& mod : modifiers) {
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

static const std::unordered_map<std::string, std::string> modifiers{
    ckpair{"internationaltable", "IT"},
    ckpair{"internationalsteamtable", "IT"},
    ckpair{"international table", "IT"},
    ckpair{"international steamtable", "IT"},
    ckpair{"international", "i"},
    ckpair{"USandBritish", "av"},
    ckpair{"US and British", "av"},
    ckpair{"US&British", "av"},
    ckpair{"US & British", "av"},
    ckpair{"USAsurvey", "US"},
    ckpair{"USA survey", "US"},
    ckpair{"USsurvey", "US"},
    ckpair{"US survey", "US"},
    ckpair{"USSurvey", "US"},
    ckpair{"US Survey", "US"},
    ckpair{"USPetroleum", "US"},
    ckpair{"USshipping", "ship"},
    ckpair{"oil", "US"},
    ckpair{"USdry", "US"},
    ckpair{"US dry", "US"},
    ckpair{"USA", "US"},
    ckpair{"USstatute", "US"},
    ckpair{"USheavy", "heavy"},
    ckpair{"USlight", "light"},
    ckpair{"Heavy", "heavy"},
    ckpair{"Light", "light"},
    ckpair{"heavy", "heavy"},
    ckpair{"light", "light"},
    ckpair{"US statute", "US"},
    ckpair{"statutory", "US"},
    ckpair{"statute", "US"},
    ckpair{"shipping", "ship"},
    ckpair{"gregorian", "g"},
    ckpair{"Gregorian", "g"},
    ckpair{"angle", "ang"},
    ckpair{"synodic", "s"},
    ckpair{"sidereal", "sdr"},
    ckpair{"30-day", "[30]"},
    ckpair{"flux", "flux"},
    ckpair{"charge", "charge"},
    ckpair{"julian", "j"},
    ckpair{"Julian", "j"},
    ckpair{"thermochemical", "th"},
    ckpair{"electric", "electric"},
    ckpair{"electrical", "electric"},
    ckpair{"time", "time"},
    ckpair{"unitoftime", "time"},
    ckpair{"unit of time", "time"},
    ckpair{"Th", "th"},
    ckpair{"th", "th"},
    ckpair{"metric", "m"},
    ckpair{"mean", "m"},
    ckpair{"imperial", "br"},
    ckpair{"Imperial", "br"},
    ckpair{"English", "br"},
    ckpair{"EUR", "br"},
    ckpair{"UKPetroleum", "brl"},
    ckpair{"imp", "br"},
    ckpair{"wine", "wine"},
    ckpair{"beer", "wine"},
    ckpair{"US", "US"},
    ckpair{"30-day", "30"},
    ckpair{"IT", "IT"},
    ckpair{"troy", "tr"},
    ckpair{"apothecary", "ap"},
    ckpair{"apothecaries", "ap"},
    ckpair{"avoirdupois", "av"},
    ckpair{"Chinese", "cn"},
    ckpair{"chinese", "cn"},
    ckpair{"Canadian", "ca"},
    ckpair{"canadian", "ca"},
    ckpair{"reactive", "react"},
    ckpair{"survey", "US"},
    ckpair{"tropical", "t"},
    ckpair{"British", "br"},
    ckpair{"british", "br"},
    ckpair{"Br", "br"},
    ckpair{"BR", "br"},
    ckpair{"UK", "br"},
    ckpair{"EUR", "br"},
    ckpair{"conventional", "90"},
    ckpair{"AC", "ac"},
    ckpair{"DC", "dc"},
    ckpair{"ang", "ang"},
    ckpair{"angle", "ang"},
    ckpair{"unitofangle", "ang"},
    ckpair{"unit of angle", "ang"},
    ckpair{"planeangle", "ang"},
    ckpair{"H2O", "H2O"},
    ckpair{"water", "H2O"},
    ckpair{"Hg", "Hg"},
    ckpair{"HG", "Hg"},
    ckpair{"mercury", "Hg"},
    ckpair{"mechanical", "mech"},
    ckpair{"hydraulic", "mech"},
    ckpair{"air", "mech"},
    ckpair{"boiler", "steam"},
    ckpair{"steam", "steam"},
    ckpair{"refrigeration", "cooling"},
    ckpair{"cooling", "cooling"},
    ckpair{"cloth", "cloth"},
    ckpair{"clothing", "cloth"},
    ckpair{"SPL", "SPL"},
    ckpair{"10.nV", "tnv"},
    ckpair{"10nV", "tnv"},
    ckpair{"10*nV", "tnv"},
    ckpair{"10*NV", "tnv"},
    ckpair{"15degC", "[15]"},
    ckpair{"20degC", "[20]"},
    ckpair{"59degF", "[59]"},
    ckpair{"60degF", "[60]"},
    ckpair{"39degF", "[39]"},
    ckpair{"20degC", "[20]"},
    ckpair{"20C", "[20]"},
    ckpair{"23degC", "[23]"},
    ckpair{"23 degC", "[23]"},
    ckpair{"0degC", "[00]"},
    ckpair{"39.2degF", "[39]"},
    ckpair{"4degC", "[04]"},
    ckpair{"15 degC", "[15]"},
    ckpair{"20 degC", "[20]"},
    ckpair{"59 degF", "[59]"},
    ckpair{"60 degF", "[60]"},
    ckpair{"39 degF", "[39]"},
    ckpair{"0 degC", "[00]"},
    ckpair{"39.2 degF", "[39]"},
    ckpair{"4 degC", "[04]"},
    ckpair{"1/20milliliter", "[20]"},
    ckpair{"1/20mL", "[20]"},
};

bool bracketModifiers(std::string& unit_string)
{
    bool modified{false};
    for (const auto& seg : Esegs) {
        auto ploc = unit_string.find_first_of(seg[0], 1);
        while (ploc != std::string::npos) {
            auto cloc = unit_string.find_first_of(seg[1], ploc);
            auto tstring = unit_string.substr(ploc + 1, cloc - ploc - 1);
            auto modloc = modifiers.find(tstring);
            if (modloc != modifiers.end()) {
                auto nextloc = unit_string.find_first_not_of(' ', cloc + 1);
                if (nextloc != std::string::npos &&
                    unit_string[nextloc] != '/' &&
                    unit_string[nextloc] != '*') {
                    unit_string.insert(nextloc, 1, '*');
                }

                unit_string.replace(ploc + 1, cloc - ploc, modloc->second);
                unit_string[ploc] = '_';
                modified = true;
            }
            ploc = unit_string.find_first_of(seg[0], ploc + 1);
        }
    }
    // if (!modified) {
    auto ploc = unit_string.find_first_of('-', 1);
    if (ploc != std::string::npos) {
        auto cloc = unit_string.find_first_of("-[({_", ploc + 1);
        auto tstring = (cloc != std::string::npos) ?
            unit_string.substr(ploc + 1, cloc - ploc - 1) :
            unit_string.substr(ploc + 1);
        auto modloc = modifiers.find(tstring);
        if (modloc != modifiers.end()) {
            unit_string.replace(ploc + 1, cloc - ploc - 1, modloc->second);
            unit_string[ploc] = '_';
            modified = true;
        }
    }
    //}
    return modified;
}

static precise_unit
    localityModifiers(std::string unit, std::uint64_t match_flags)
{
    static UNITS_CPP14_CONSTEXPR_OBJECT std::array<ckpair, 62>
        internationlReplacements{{
            ckpair{"internationaltable", "IT"},
            ckpair{"internationalsteamtable", "IT"},
            ckpair{"international", "i"},
            ckpair{"USandBritish", "av"},
            ckpair{"US&British", "av"},
            ckpair{"USAsurvey", "US"},
            ckpair{"USsurvey", "US"},
            ckpair{"USSurvey", "US"},
            ckpair{"USdry", "US"},
            ckpair{"USA", "US"},
            ckpair{"USstatute", "US"},
            ckpair{"statutory", "US"},
            ckpair{"statute", "US"},
            ckpair{"US", "US"},
            ckpair{"shipping", "ship"},
            ckpair{"gregorian", "g"},
            ckpair{"Gregorian", "g"},
            ckpair{"synodic", "s"},
            ckpair{"sidereal", "sdr"},
            ckpair{"julian", "j"},
            ckpair{"Julian", "j"},
            ckpair{"thermochemical", "th"},
            ckpair{"hydraulic", "mech"},
            ckpair{"Th", "th"},
            ckpair{"(th)", "th"},
            ckpair{"metric", "m"},
            ckpair{"mean", "m"},
            ckpair{"imperial", "br"},
            ckpair{"Imperial", "br"},
            ckpair{"English", "br"},
            ckpair{"imp", "br"},
            ckpair{"wine", "wine"},
            ckpair{"beer", "wine"},
            ckpair{"(IT)", "IT"},
            ckpair{"troy", "tr"},
            ckpair{"apothecary", "ap"},
            ckpair{"apothecaries", "ap"},
            ckpair{"avoirdupois", "av"},
            ckpair{"Chinese", "cn"},
            ckpair{"chinese", "cn"},
            ckpair{"Canadian", "ca"},
            ckpair{"canadian", "ca"},
            ckpair{"survey", "US"},
            ckpair{"tropical", "t"},
            ckpair{"British", "br"},
            ckpair{"british", "br"},
            ckpair{"Br", "br"},
            ckpair{"BR", "br"},
            ckpair{"UK", "br"},
            ckpair{"conventional", "90"},
            ckpair{"AC", "ac"},
            ckpair{"DC", "dc"},
            ckpair{"fluid", "FL"},
            ckpair{"liquid", "FL"},
            ckpair{"fl", "FL"},
            ckpair{"15degC", "[15]"},
            ckpair{"20degC", "[20]"},
            ckpair{"59degF", "[59]"},
            ckpair{"60degF", "[60]"},
            ckpair{"39degF", "[39]"},
            ckpair{"0degC", "[00]"},
            // this should be last

            ckpair{"us", "US"},
        }};
    if (unit.size() < 3) {
        return precise::invalid;
    }
    if (unit.front() == 'u' && (unit[1] == 'S' || unit[1] == 'K')) {
        unit.front() = 'U';
    }
    bool changed = false;
    for (const auto& irep : internationlReplacements) {
        if (strlen(irep.first) == 2) {
            if (strncmp(irep.first, irep.second, 2) == 0) {
                if (ends_with(unit, std::string(1, '_') + irep.second)) {
                    continue;
                }
            }
            if (unit[1] > 0 && (std::isupper(unit[1]) != 0) &&
                (std::toupper(unit[0]) == irep.first[0]) &&
                (unit[1] == irep.first[1])) {
                unit[0] = std::toupper(unit[0]);
            }
        }
        auto fnd = unit.find(irep.first);
        if (fnd != std::string::npos) {
            auto len = strlen(irep.first);
            if (len == unit.size()) {  // this is a modifier if we are checking
                                       // the entire unit this is automatically
                                       // false
                return precise::invalid;
            }
            unit.erase(fnd, len);
            if (fnd > 0 && unit[fnd - 1] == '_') {
                unit.erase(fnd - 1, 1);
            }
            if (fnd < unit.size() && unit[fnd] == '_') {
                unit.erase(fnd, 1);
            }
            unit.push_back('_');
            unit.append(irep.second);
            changed = true;
            break;
        }
    }
    changed |= clearEmptySegments(unit);
    if (changed) {
        auto retunit = unit_from_string_internal(
            unit, match_flags | no_locality_modifiers | no_of_operator);
        if (is_error(retunit) && ((match_flags & no_locality_modifiers) == 0)) {
            return localityModifiers(unit, match_flags | no_locality_modifiers);
        }
        return retunit;
    }
    if (unit.size() < 4) {
        return precise::invalid;
    }
    static constexpr std::array<const char*, 7> rotSequences{
        {"br", "av", "ch", "IT", "th", "ap", "tr"}};
    for (const auto& seq : rotSequences) {
        if (unit.compare(0, 2, seq) == 0) {
            auto nunit = unit.substr((unit[3] == '_') ? 3 : 2);
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

/// detect some known SI prefixes
static std::pair<double, size_t>
    getPrefixMultiplierWord(const std::string& unit)
{
    // NOLINTNEXTLINE (readability-qualified-auto)
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

static smap loadDefinedUnits()
{
    smap knownUnits{};
    for (const auto& pr : defined_unit_strings_si) {
        if (pr.first != nullptr) {
            knownUnits.emplace(pr.first, pr.second);
        }
    }
    for (const auto& pr : defined_unit_strings_customary) {
        if (pr.first != nullptr) {
            knownUnits.emplace(pr.first, pr.second);
        }
    }
#if !defined(UNITS_DISABLE_NON_ENGLISH_UNITS) ||                               \
    UNITS_DISABLE_NON_ENGLISH_UNITS == 0
    for (const auto& pr : defined_unit_strings_non_english) {
        if (pr.first != nullptr) {
            knownUnits.emplace(pr.first, pr.second);
        }
    }
#endif
    return knownUnits;
}
/** units from several locations
http://vizier.u-strasbg.fr/vizier/doc/catstd-3.2.htx
http://unitsofmeasure.org/ucum.html#si
*/
static const smap base_unit_vals = loadDefinedUnits();

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
    commoditizedUnit(const std::string& unit_string, std::uint64_t match_flags)
{
    auto finish = unit_string.find_last_of('}');
    if (finish == std::string::npos) {
        // there are checks before this would get called that would catch that
        // error but it is left in place just in case it gets called in an
        // isolated context

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
        if (bunit.has_same_base(m)) {
            static const std::unordered_map<std::string, precise_unit>
                commUnits{
                    {"mercury", precise::pressure::bases::Hg},
                    {"mercurycolumn", precise::pressure::bases::Hg},
                    {"mercuryguage", precise::pressure::bases::Hg},
                    {"mercury_i", precise::pressure::bases::Hg},
                    {"Hg", precise::pressure::bases::Hg},
                    {"water", precise::pressure::bases::water},
                    {"watercolumn", precise::pressure::bases::water},
                    {"water_i", precise::pressure::bases::water},
                    {"waterguage", precise::pressure::bases::water},
                    {"H2O", precise::pressure::bases::water},
                    {"mercury_[00]", precise::pressure::bases::Hg_0},
                    {"water_[04]", precise::pressure::bases::water_4},
                    {"water_[39]", precise::pressure::bases::water_39},
                    {"mercury_[32]", precise::pressure::bases::Hg_32},
                    {"mercury_[60]", precise::pressure::bases::Hg_60},
                    {"water_[60]", precise::pressure::bases::water_60},
                };
            auto tunit = commUnits.find(cstring);
            if (tunit != commUnits.end()) {
                return bunit * tunit->second;
            }
        }
        return {1.0, bunit, getCommodity(cstring)};
    }
    return precise::invalid;
}
static precise_unit checkMultiplierCharacter(
    const std::string& unit_string,
    std::uint64_t match_flags,
    char mchar)
{
    // assume mchar means multiply
    std::string ustring;
    precise_unit retunit;
    auto fd = unit_string.find_first_of(mchar);
    if (fd != std::string::npos) {
        // if there is a single one just check for a merged unit
        if (unit_string.find_first_of(mchar, fd + 1) == std::string::npos) {
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
            } else if (ustring[fd + 1] == mchar) {
                // repeated characters,  cannot mean separator
                return precise::invalid;
            } else if (ustring[fd + 1] != '[' && ustring[fd + 1] != '(') {
                ustring[fd] = '*';
            }
            // ignore adjacent ones
            fd = ustring.find_first_of(mchar, fd + 2);
        }
        if (ustring != unit_string) {
            retunit = unit_from_string_internal(
                ustring, match_flags | skip_partition_check);
            if (!is_error(retunit)) {
                return retunit;
            }
        }
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

static std::uint64_t hashGen(std::uint64_t index, const std::string& str)
{
    return std::hash<std::string>{}(str) ^ std::hash<std::uint64_t>{}(index);
}

static const std::unordered_map<std::uint64_t, precise_unit> domainSpecificUnit{
    {hashGen(domains::ucum, "B"), precise::log::bel},
    {hashGen(domains::ucum, "a"), precise::time::aj},
    {hashGen(domains::ucum, "year"), precise::time::aj},
    {hashGen(domains::ucum, "equivalent"), precise::mol},
    {hashGen(domains::astronomy, "am"), precise::angle::arcmin},
    {hashGen(domains::astronomy, "as"), precise::angle::arcsec},
    {hashGen(domains::astronomy, "year"), precise::time::at},
    {hashGen(domains::cooking, "C"), precise::us::cup},
    {hashGen(domains::cooking, "T"), precise::us::tbsp},
    {hashGen(domains::cooking, "c"), precise::us::cup},
    {hashGen(domains::cooking, "smi"),
     precise_unit(1.0 / 32.0, precise::us::tsp)},
    {hashGen(domains::cooking, "scruple"),
     precise_unit(1.0 / 4.0, precise::us::tsp)},
    {hashGen(domains::cooking, "t"), precise::us::tsp},
    {hashGen(domains::cooking, "ds"),
     precise_unit(1.0 / 16.0, precise::us::tsp)},
    {hashGen(domains::cooking, "TB"), precise::us::tbsp},
    {hashGen(domains::surveying, "'"), precise::us::foot},
    {hashGen(domains::surveying, "`"), precise::us::foot},
    {hashGen(domains::surveying, u8"\u2032"), precise::us::foot},
    {hashGen(domains::surveying, "''"), precise::us::inch},
    {hashGen(domains::surveying, "``"), precise::us::inch},
    {hashGen(domains::surveying, "\""), precise::us::inch},
    {hashGen(domains::surveying, u8"\u2033"), precise::us::inch},
    {hashGen(domains::nuclear, "rad"), precise::cgs::RAD},
    {hashGen(domains::nuclear, "rd"), precise::cgs::RAD},
    {hashGen(domains::climate, "kt"), precise::kilo* precise::t},
    {hashGen(domains::us_customary, "C"), precise::us::cup},
    {hashGen(domains::us_customary, "T"), precise::us::tbsp},
    {hashGen(domains::us_customary, "c"), precise::us::cup},
    {hashGen(domains::us_customary, "t"), precise::us::tsp},
    {hashGen(domains::us_customary, "TB"), precise::us::tbsp},
    {hashGen(domains::us_customary, "'"), precise::us::foot},
    {hashGen(domains::us_customary, "`"), precise::us::foot},
    {hashGen(domains::us_customary, u8"\u2032"), precise::us::foot},
    {hashGen(domains::us_customary, "''"), precise::us::inch},
    {hashGen(domains::us_customary, "``"), precise::us::inch},
    {hashGen(domains::us_customary, "\""), precise::us::inch},
    {hashGen(domains::us_customary, u8"\u2033"), precise::us::inch},
    {hashGen(domains::us_customary, "smi"),
     precise_unit(1.0 / 32.0, precise::us::tsp)},
    {hashGen(domains::us_customary, "scruple"),
     precise_unit(1.0 / 4.0, precise::us::tsp)},
    {hashGen(domains::us_customary, "ds"),
     precise_unit(1.0 / 16.0, precise::us::tsp)},
    {hashGen(domains::allDomains, "B"), precise::log::bel},
    {hashGen(domains::allDomains, "a"), precise::time::aj},
    {hashGen(domains::allDomains, "year"), precise::time::aj},
    {hashGen(domains::allDomains, "am"), precise::angle::arcmin},
    {hashGen(domains::allDomains, "as"), precise::angle::arcsec},
    {hashGen(domains::allDomains, "C"), precise::us::cup},
    {hashGen(domains::allDomains, "T"), precise::us::tbsp},
    {hashGen(domains::allDomains, "c"), precise::us::cup},
    {hashGen(domains::allDomains, "t"), precise::us::tsp},
    {hashGen(domains::allDomains, "TB"), precise::us::tbsp},
    {hashGen(domains::allDomains, "rad"), precise::cgs::RAD},
    {hashGen(domains::allDomains, "kt"), precise::kilo* precise::t},
    {hashGen(domains::allDomains, "rd"), precise::cgs::RAD},
    {hashGen(domains::allDomains, "smi"),
     precise_unit(1.0 / 32.0, precise::us::tsp)},
    {hashGen(domains::allDomains, "scruple"),
     precise_unit(1.0 / 4.0, precise::us::tsp)},
    {hashGen(domains::allDomains, "ds"),
     precise_unit(1.0 / 16.0, precise::us::tsp)},
};

static precise_unit
    getDomainUnit(std::uint64_t domain, const std::string& unit_string)
{
    auto h1 = hashGen(domain, unit_string);
    auto fnd = domainSpecificUnit.find(h1);
    return (fnd != domainSpecificUnit.end()) ? fnd->second : precise::invalid;
}
static std::uint64_t getCurrentDomain(std::uint64_t match_flags)
{
    static constexpr std::uint64_t flagMask{0xFFULL};
    std::uint64_t dmn = match_flags & flagMask;

    return (dmn == 0ULL) ? unitsDomain : dmn;
}

static precise_unit
    get_unit(const std::string& unit_string, std::uint64_t match_flags)
{
    if (allowUserDefinedUnits.load(std::memory_order_acquire)) {
        if (!user_defined_units.empty()) {
            auto fnd2 = user_defined_units.find(unit_string);
            if (fnd2 != user_defined_units.end()) {
                return fnd2->second;
            }
        }
    }

    auto cdomain = getCurrentDomain(match_flags);
    if (cdomain != domains::defaultDomain) {
        auto dmunit = getDomainUnit(cdomain, unit_string);
        if (is_valid(dmunit)) {
            return dmunit;
        }
    }

    auto fnd = base_unit_vals.find(unit_string);
    if (fnd != base_unit_vals.end()) {
        return fnd->second;
    }
    // empty string would have been found already
    auto c = unit_string.front();
    if ((c == 'C' || c == 'E') && unit_string.size() >= 6) {
        size_t index{0};
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

static inline bool isOperator(char X)
{
    return (X == '*') || (X == '/');
}

static bool isolatePriorModifier(
    std::string& unit_string,
    const std::string& modifier,
    char check1,
    char check2)
{
    bool modified{false};
    auto modfind = unit_string.find(modifier);
    if (modfind != std::string::npos) {
        auto offset = modfind + modifier.size();
        // LCOV_EXCL_START
        // this condition is not used in current use cases but it is dangerous
        // to make assumptions that will always be the case
        if (modifier.back() != ' ') {
            ++offset;
        }
        // LCOV_EXCL_STOP
        auto kloc = unit_string.find_first_not_of(' ', offset);
        if (kloc != std::string::npos &&
            (unit_string[kloc] == check1 || unit_string[kloc] == check2)) {
            // this handles a misinterpretation of square+d to squared when in
            // middle of a unit
            unit_string[kloc - 1] = '_';
            modified = true;
        }
        auto nspace = unit_string.find_first_of(' ', kloc);
        auto skip = (nspace >= unit_string.size());
        if (!skip) {
            skip = isOperator(unit_string[nspace + 1]) ||
                isOperator(unit_string[nspace - 1]);
        }
        if (!skip) {
            skip = unit_string[nspace + 1] == '(' ||
                unit_string[nspace - 1] == '(';
            skip |=
                (skip || unit_string[nspace + 1] == '-' ||
                 unit_string[nspace - 1] == '-');
            skip |= skip || (unit_string.compare(nspace + 1, 2, "of") == 0);
            skip |= skip ||
                unit_string[nspace + 1] ==
                    'U';  // handle distance units with UK or US modifier
        }
        if (!skip) {
            auto divloc = unit_string.find_last_of('/', modfind);
            auto divloc2 = unit_string.find_first_of('/', modfind + 1);
            if (divloc < modfind) {
                unit_string.insert(divloc + 1, 1, '(');
                ++nspace;
                if (divloc2 != std::string::npos) {
                    unit_string.insert(divloc2 + 1, 1, ')');
                    // don't worry about nspace increment here as it will be
                    // skipped or the insertion after the space
                } else {
                    unit_string.push_back(')');
                }

                skip |= (divloc2 < nspace);
                modified = true;
            } else if (divloc2 < nspace) {
                skip = true;
            }
        }
        if (!skip) {
            unit_string[nspace] = '*';
            modified = true;
        }
    }
    return modified;
}

static bool
    isolatePostModifier(std::string& unit_string, const std::string& modifier)
{
    bool modified{false};
    auto modfind = unit_string.find(modifier);
    if (modfind != std::string::npos) {
        auto kloc = unit_string.find_last_not_of(' ', modfind - 1);

        auto nspace = unit_string.find_last_of(" */", kloc);
        auto skip =
            (nspace == 0 || (nspace >= unit_string.size()) ||
             unit_string[nspace] != ' ');
        if (!skip) {
            skip |= skip || isOperator(unit_string[nspace + 1]);
            skip |= skip || isOperator(unit_string[nspace - 1]);
            skip |= skip || unit_string[nspace + 1] == ')';
            skip |= skip || unit_string[nspace + 1] == '-';
            skip |= skip || unit_string[nspace - 1] == ')';
            skip |= skip || unit_string[nspace - 1] == '-';
        }
        if (!skip) {
            auto divloc = unit_string.find_last_of('/', modfind);
            if (divloc < modfind) {
                unit_string.insert(divloc + 1, 1, '(');
                auto divloc2 = unit_string.find_first_of('/', modfind + 1);
                if (divloc2 != std::string::npos) {
                    unit_string.insert(divloc2, 1, ')');
                } else {
                    unit_string.push_back(')');
                }
                modified = true;
                ++nspace;
                skip |= (divloc2 < nspace);
            }
        }
        if (!skip) {
            unit_string[nspace] = '*';
            modified = true;
        }
    }
    return modified;
}

// remove spaces and insert multiplies if appropriate
static bool cleanSpaces(std::string& unit_string, bool skipMultiply)
{
    static const std::string spaceChars = std::string(" \t\n\r") + '\0';

    bool spacesRemoved = isolatePriorModifier(unit_string, "square ", 'd', 'D');
    spacesRemoved |= isolatePriorModifier(unit_string, "cubic ", '_', '-');
    spacesRemoved |= isolatePostModifier(unit_string, " squared");
    auto fnd = unit_string.find_first_of(spaceChars);
    while (fnd != std::string::npos) {
        spacesRemoved = true;
        if ((fnd > 0) && (!skipMultiply)) {
            auto nloc = unit_string.find_first_not_of(spaceChars, fnd);
            if (nloc == std::string::npos) {
                unit_string.erase(fnd);
                return true;
            }
            if (fnd == 1) {  // if the second character is a space it almost
                             // always means multiply
                if (isOperator(unit_string[nloc]) || unit_string[nloc] == '^' ||
                    unit_string[nloc] == '@') {
                    unit_string.erase(fnd, 1);
                    fnd = unit_string.find_first_of(spaceChars, fnd);
                    continue;
                }

                if (unit_string.size() < 8) {
                    unit_string[fnd] = '*';
                    fnd = unit_string.find_first_of(spaceChars, fnd);
                    skipMultiply = true;
                    continue;
                }
                if (unit_string.find_first_of('*') == std::string::npos) {
                    auto oloc = unit_string.find_first_of("/^", nloc);
                    if (oloc < 9) {
                        unit_string[fnd] = '*';
                        fnd = unit_string.find_first_of(spaceChars, fnd);
                        skipMultiply = true;
                        continue;
                    }
                }
            }
            if (isOperator(unit_string[fnd - 1])) {
                unit_string.erase(fnd, 1);
                fnd = unit_string.find_first_of(spaceChars, fnd);
                continue;
            }
            if (unit_string[fnd - 1] == '.') {
                // this is an abbreviation so deal with it later
                fnd = unit_string.find_first_of(spaceChars, fnd + 1);
                continue;
            }
            if (unit_string.size() > nloc && isOperator(unit_string[nloc])) {
                unit_string.erase(fnd, 1);
                fnd = unit_string.find_first_of(spaceChars, fnd);
                continue;
            }
            if (std::all_of(
                    unit_string.begin(), unit_string.begin() + fnd, [](char X) {
                        return isNumericalStartCharacter(X) || isOperator(X);
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
        if (fnd > 0 && unit_string[fnd - 1] == '.') {
            // this now gets dealt with in the dotMultiply code
            ++fnd;
        } else {
            unit_string.erase(fnd, 1);
            if (fnd > 0) {
                skipMultiply = true;
            }
        }

        fnd = unit_string.find_first_of(spaceChars, fnd);
    }
    return spacesRemoved;
}

enum class DotInterpretation : char { none = 0, multiply = 1, abbrev = 2 };

static DotInterpretation findDotInterpretation(const std::string& unit_string)
{
    auto dloc = unit_string.find_first_of('.');
    if (dloc == std::string::npos) {
        // LCOV_EXCL_START
        // in all internal contexts this function wouldn't be called if there
        // was no dots
        return DotInterpretation::none;
        // LCOV_EXCL_STOP
    }
    DotInterpretation dInt{DotInterpretation::none};
    while (dloc != std::string::npos) {
        if (dloc > 0) {
            if (!isDigitCharacter(unit_string[dloc - 1]) ||
                !isDigitCharacter(unit_string[dloc + 1])) {
                if (unit_string[dloc - 1] == '*' ||
                    unit_string[dloc + 1] == ' ') {
                    return DotInterpretation::abbrev;
                }
                if (dloc == unit_string.size() - 1) {
                    return DotInterpretation::abbrev;
                }
                dInt = DotInterpretation::multiply;
            }
        } else if (unit_string.size() > 1) {
            if (!isDigitCharacter(unit_string[dloc + 1])) {
                dInt = DotInterpretation::multiply;
            }
        }
        dloc = unit_string.find_first_of('.', dloc + 1);
    }
    return dInt;
}

static void
    cleanDotNotation(std::string& unit_string, std::uint64_t match_flags)
{
    const auto dInt = findDotInterpretation(unit_string);

    // replace all dots with '*'
    size_t st = 0;
    auto dloc = unit_string.find_first_of('.');
    int skipped{0};
    while (dloc != std::string::npos) {
        if (dloc > 0) {
            if (!isDigitCharacter(unit_string[dloc - 1]) ||
                !isDigitCharacter(unit_string[dloc + 1])) {
                if (unit_string[dloc - 1] == '*') {
                    ++skipped;
                } else {
                    if (dInt == DotInterpretation::multiply) {
                        unit_string[dloc] = '*';
                    } else {
                        while (unit_string[dloc] == '.' ||
                               unit_string[dloc] == ' ') {
                            unit_string.erase(dloc, 1);
                        }
                        --dloc;
                    }
                }
            } else {
                ++skipped;
            }
        } else if (unit_string.size() > 1) {
            if (!isDigitCharacter(unit_string[dloc + 1])) {
                if (dInt == DotInterpretation::multiply) {
                    unit_string[dloc] = '*';
                } else {
                    while (unit_string[dloc] == '.' ||
                           unit_string[dloc] == ' ') {
                        unit_string.erase(dloc, 1);
                    }
                    --dloc;
                }
            } else {
                ++skipped;
            }
        }
        st = dloc + 1;
        dloc = unit_string.find_first_of('.', st);
    }

    if (skipped > 1) {
        /* this is needed for some sequential conditions with unusual dot
         * notation that forces some to be a multiply
         */
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
        {"GB", "gilbert"}, {"WB", "Wb"},   {"CP", "cP"}, {"EV", "eV"},
        {"PT", "pT"},
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
        // deal with situation with meter as captital M
        if (unit_string.back() == 'M') {
            if ((unit_string.length() == 2 &&
                 getPrefixMultiplier(unit_string.front()) != 0.0) ||
                (unit_string.length() == 3 &&
                 getPrefixMultiplier2Char(unit_string[0], unit_string[1]) !=
                     0.0)) {
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
static bool checkExponentOperations(const std::string& unit_string)
{
    // check all power operations
    auto cx = unit_string.find_first_of('^');
    while (cx != std::string::npos) {
        bool ndigit = isDigitCharacter(unit_string[cx - 1]);
        ++cx;
        char c = unit_string[cx];
        if (!isDigitCharacter(c)) {
            if (c == '-') {
                if (!isDigitCharacter(unit_string[cx + 1])) {
                    return false;
                }
                ++cx;
            } else if (c == '(') {
                ++cx;
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
#ifdef UNITS_CONSTEXPR_IF_SUPPORTED
        if constexpr (detail::bitwidth::base_size == sizeof(std::uint32_t)) {
#else
        if (detail::bitwidth::base_size == sizeof(std::uint32_t)) {
#endif
            if (unit_string.size() > cx + 1 &&
                isDigitCharacter(unit_string[cx + 1]) && !ndigit) {
                // non representable unit power
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
            case 2:  // the only way this would get here is ^D^ which is
                     // not allowed
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
    return true;
}

// run a few checks on the string to verify it looks somewhat valid
static bool checkValidUnitString(
    const std::string& unit_string,
    std::uint64_t match_flags)
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
        for (const auto& seq : invalidSequences) {
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
        if (!checkExponentOperations(unit_string)) {
            return false;
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

/// do some unicode replacement (unicode in the loose sense any characters
/// not in the basic ascii set)
static bool unicodeReplacement(std::string& unit_string)
{
    static UNITS_CPP14_CONSTEXPR_OBJECT std::array<ckpair, 66>
        ucodeReplacements{{
            ckpair{u8"\u00d7", "*"},
            ckpair{u8"\u00f7", "/"},  // division sign
            ckpair{u8"\u00b7", "*"},
            ckpair{u8"\u2217", "*"},  // asterisk operator
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
            ckpair{u8"\u207B\u2074", "^(-4)"},
            ckpair{u8"\u207B\u2075", "^(-5)"},
            ckpair{u8"\u207B\u2076", "^(-6)"},
            ckpair{u8"\u207B\u2077", "^(-7)"},
            ckpair{u8"\u207B\u2078", "^(-8)"},
            ckpair{u8"\u207B\u2079", "^(-9)"},
            ckpair{u8"-\u00B9", "^(-1)"},
            ckpair{u8"-\u00B2", "^(-2)"},
            ckpair{u8"-\u00B3", "^(-3)"},
            ckpair{u8"-\u2074", "^(-4)"},
            ckpair{u8"-\u2075", "^(-5)"},
            ckpair{u8"-\u2076", "^(-6)"},
            ckpair{u8"-\u2077", "^(-7)"},
            ckpair{u8"-\u2078", "^(-8)"},
            ckpair{u8"-\u2079", "^(-9)"},
            ckpair{u8"\u00b9", "*"},  // superscript 1 which doesn't do
                                      // anything
            ckpair{u8"\u00b2", "^(2)"},
            ckpair{u8"\u00b3", "^(3)"},
            ckpair{u8"\u2074", "^(4)"},
            ckpair{u8"\u2075", "^(5)"},
            ckpair{u8"\u2076", "^(6)"},
            ckpair{u8"\u2077", "^(7)"},
            ckpair{u8"\u2078", "^(8)"},
            ckpair{u8"\u2079", "^(9)"},
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
            // superscript 1 which doesn't do anything, replace with
            // multiply
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
    for (const auto& ucode : ucodeReplacements) {
        auto fnd = unit_string.find(ucode.first);
        while (fnd != std::string::npos) {
            std::size_t codelength = strlen(ucode.first);
            if (codelength == 1 && fnd > 0 &&
                static_cast<unsigned char>(unit_string[fnd - 1]) > 0xC0) {
                // skip the conversion in this case as it is likely a unicode
                // sequence
                fnd = unit_string.find(ucode.first, fnd + 1);
                continue;
            }
            changed = true;
            unit_string.replace(fnd, codelength, ucode.second);
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

static std::string shortStringReplacement(char U)
{
    // LCOV_EXCL_START
    // not sure why this isn't showing up as covered
    static const std::unordered_map<char, std::string> singleCharUnitStrings{
        {'m', "meter"},       {'s', "second"}, {'S', "siemens"},
        {'l', "liter"},       {'g', "gram"},   {'b', "barn"},
        {'r', "revolutions"}, {'V', "volt"},   {'F', "farad"},
        {'y', "year"},        {'p', "poise"},  {'K', "kelvin"},
        {'a', "are"},         {'N', "newton"}, {'d', "day"},
        {'B', "byte"},        {'X', "xu"},     {'T', "tesla"},
        {'U', "units"},       {'M', "molar"},  {'P', "poise"},
        {'W', "watt"},        {'A', "ampere"}, {'C', "coulomb"},
        {'J', "joule"},       {'H', "henry"},  {'G', "gauss"},
        {'h', "hour"},        {'D', "day"},    {'o', "arcdeg"},
        {'L', "liter "},      {'W', "watt"},   {'e', "elementarycharge"},
        {'t', "tonne"}};

    // LCOV_EXCL_STOP

    auto res = singleCharUnitStrings.find(U);
    return (res == singleCharUnitStrings.end()) ? std::string(1, U) :
                                                  res->second;
}

static bool checkShortUnits(std::string& unit_string, std::uint64_t match_flags)
{
    bool mod = false;
    auto fndNS = unit_string.find_first_not_of(" \t");
    auto fndP = unit_string.find_first_of(" \t", fndNS + 1);
    auto fndM = unit_string.find_first_of("*/");
    if (fndP == 2) {
        if (unit_string.size() > 4) {
            // the single character section next will catch 4 character string
            // issues
            auto fndPn = unit_string.find_first_not_of(" \t", fndP);

            if (fndPn != std::string::npos && unit_string[fndPn] != '(' &&
                fndM == std::string::npos) {
                auto str = unit_string.substr(0, 2);
                if (str != "fl") {
                    auto retunit = get_unit(str, match_flags);
                    if (is_valid(retunit)) {
                        unit_string[2] = '_';
                        retunit = get_unit(unit_string, match_flags);
                        if (!is_valid(retunit)) {
                            unit_string[2] = '*';
                        }
                        fndP = unit_string.find_first_of(" \t", 3);
                        mod = true;
                    }
                }
            }
        }
    }
    while (fndP != std::string::npos) {
        if (fndP + 2 == unit_string.size()) {
            // we are at the end of the string
            if (fndM == std::string::npos) {
                auto str = unit_string.substr(0, fndP);
                auto retunit = get_unit(str, match_flags);
                if (is_valid(retunit)) {
                    unit_string[fndP] = '_';
                    retunit = get_unit(unit_string, match_flags);
                    if (!is_valid(retunit)) {
                        unit_string[fndP] = '*';
                    }
                    return mod;
                }
            }
            unit_string.replace(
                fndP + 1, 1, shortStringReplacement(unit_string[fndP + 1]));
            mod = true;
        } else {
            switch (unit_string[fndP + 1]) {
                case ' ':
                case '*':
                case '/':
                case '^':
                case '.':
                    unit_string.replace(
                        fndP + 1,
                        1,
                        shortStringReplacement(unit_string[fndP + 1]));
                    mod = true;
                    break;
                default:
                    break;
            }
        }
        fndP = unit_string.find_first_of(" \t", fndP + 1);
    }
    return mod;
}
// do some cleaning on the unit string to standardize formatting and deal
// with some extended ascii and unicode characters
static bool cleanUnitString(std::string& unit_string, std::uint64_t match_flags)
{
    auto slen = unit_string.size();
    bool skipcodereplacement = ((match_flags & skip_code_replacements) != 0);
    static UNITS_CPP14_CONSTEXPR_OBJECT std::array<ckpair, 4>
        earlyCodeReplacements{{
            ckpair{"degree", "deg"},
            ckpair{"Degree", "deg"},
            ckpair{"degs ", "deg"},
            ckpair{"deg ", "deg"},
        }};

    static UNITS_CPP14_CONSTEXPR_OBJECT std::array<ckpair, 33>
        allCodeReplacements{{
            ckpair{"sq.", "square"},
            ckpair{"cu.", "cubic"},
            ckpair{"U.S.", "US"},
            ckpair{"10^", "1e"},
            ckpair{"10-", "1e-"},
            ckpair{"^+", "^"},
            ckpair{"'s", "s"},
            ckpair{"ampere", "amp"},
            ckpair{"Ampere", "amp"},
            ckpair{"metre", "meter"},
            ckpair{"Metre", "meter"},
            ckpair{"litre", "liter"},
            ckpair{"Litre", "liter"},
            ckpair{"B.Th.U.", "BTU"},
            ckpair{"B.T.U.", "BTU"},
            ckpair{"Britishthermalunits", "BTU"},
            ckpair{"Britishthermalunitat", "BTU"},
            ckpair{"Britishthermalunit", "BTU"},
            ckpair{"BThU", "BTU"},
            ckpair{"-US", "US"},
            ckpair{"--", "*"},
            // -- is either a double negative or a separator, so make it a
            // multiplier so it doesn't get erased and then converted to a
            // power
            ckpair{"\\\\", "\\\\*"},
            // \\ is always considered a segment terminator so it won't be
            // misinterpreted as a known escape sequence
            ckpair{"perunit", "pu"},
            ckpair{"percent", "%"},
            ckpair{"per-unit", "pu"},
            ckpair{"per unit ", "pu"},
            ckpair{"/square*", "/square"},
            ckpair{"/cubic*", "/cubic"},
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
    if (tail == '^' || tail == '*' || tail == '/') {
        unit_string.pop_back();
        changed = true;
    }
    auto c = unit_string.find_first_not_of(spchar);
    if (c == std::string::npos) {
        unit_string.clear();
        return true;
    }
    if (c != 0) {
        unit_string.erase(0, c);
        c = unit_string.find_first_not_of(spchar);
        changed = true;
    }
    if (unit_string[c] == '/') {
        unit_string.insert(c, 1, '1');
        changed = true;
        skipMultiply = true;
    }
    if (!skipcodereplacement) {
        // Check for unicode or extended characters
        if (std::any_of(unit_string.begin(), unit_string.end(), [](char x) {
                return (static_cast<std::uint8_t>(x) & 0x80U) != 0;
            })) {
            if (unicodeReplacement(unit_string)) {
                changed = true;
            }
        }

        // some code replacement that needs to be done before single character
        // and space replacements
        for (const auto& acode : earlyCodeReplacements) {
            auto fnd = unit_string.find(acode.first);
            while (fnd != std::string::npos) {
                changed = true;
                unit_string.replace(fnd, strlen(acode.first), acode.second);
                fnd = unit_string.find(acode.first, fnd + 1);
            }
        }

        if (unit_string.find_first_of(spchar) != std::string::npos) {
            // deal with some particular string with a space in them
            std::size_t reploc{0};
            // clean up some "per" words
            if (unit_string.compare(0, 4, "per ") == 0) {
                reploc = 2;
                unit_string.replace(0, 4, "1/");
                skipMultiply = true;
            }
            if (ReplaceStringInPlace(unit_string, " per ", 5, "/", 1, reploc)) {
                skipMultiply = true;
            }
            if (reploc > 0) {
                auto ploc = unit_string.find_first_of('(', reploc);
                if (ploc != std::string::npos) {
                    auto fdiv = unit_string.find_first_of('/', reploc);
                    std::size_t ndiv{0};
                    do {
                        ndiv = unit_string.find_first_of('/', fdiv + 1);
                        if (ploc < ndiv) {
                            if (ndiv != std::string::npos) {
                                unit_string.insert(ndiv, 1, ')');
                            } else {
                                unit_string.push_back(')');
                            }
                            unit_string.insert(fdiv + 1, 1, '(');
                            fdiv = ndiv + 2;
                        } else {
                            fdiv = ndiv;
                        }
                    } while (ndiv != std::string::npos);
                }
            }
            checkShortUnits(unit_string, match_flags);
            auto fndP = unit_string.find(" of ");
            while (fndP != std::string::npos) {
                auto nchar = unit_string.find_first_not_of(spchar, fndP + 4);
                if (nchar != std::string::npos) {
                    if (unit_string[nchar] == '(' ||
                        unit_string[nchar] == '[') {
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
        std::size_t loc{0};
        if (ReplaceStringInPlace(unit_string, "**", 2, "^", 1, loc)) {
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
        for (const auto& acode : allCodeReplacements) {
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
                    case '.':
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
        // clear empty brackets, this would indicate commodities but if
        // empty there is no commodity
        clearEmptySegments(unit_string);
        cleanUpPowersOfOne(unit_string);
        if (unit_string.empty()) {
            unit_string.push_back('1');
            return true;
        }
    }
    // remove leading *})],  equivalent of 1* but we don't need to process
    // that further
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
            case 'o':  // handle special case of commodity modifier using
                       // "of"
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

static bool modifyTailCodes(std::string& unit_string)
{
    if (!unit_string.empty() &&
        (unit_string.back() == 'F' || unit_string.back() == 'C')) {
        static UNITS_CPP14_CONSTEXPR_OBJECT std::array<ckpair, 12>
            trailTempCodeReplacements{{
                ckpair{"at39F", "[39]"},
                ckpair{"39F", "[39]"},
                ckpair{"at60F", "[60]"},
                ckpair{"60F", "[60]"},
                ckpair{"at20C", "[20]"},
                ckpair{"20C", "[20]"},
                ckpair{"at23C", "[23]"},
                ckpair{"23C", "[23]"},
                ckpair{"at4C", "[04]"},
                ckpair{"4C", "[04]"},
                ckpair{"at0C", "[00]"},
                ckpair{"0C", "[00]"},

            }};

        for (const auto& endTemp : trailTempCodeReplacements) {
            if (ends_with(unit_string, endTemp.first)) {
                auto sz = strlen(endTemp.first);
                unit_string.replace(
                    unit_string.end() - sz, unit_string.end(), endTemp.second);
                if (unit_string[unit_string.size() - 5] != '_') {
                    unit_string.insert(unit_string.size() - 4, 1, '_');
                }
                return true;
            }
        }
    }
    return false;
}

/// cleanup phase 2 if things still aren't working
static bool cleanUnitStringPhase2(std::string& unit_string)
{
    bool changed{false};
    auto len = unit_string.length();

    if (bracketModifiers(unit_string)) {
        changed = true;
    }

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

    return changed || (len != unit_string.length());
}

static precise_unit
    unit_quick_match(std::string unit_string, std::uint64_t match_flags)
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
        unit_string.back() == 's') {  // if the string is of length two this
                                      // is too risky to try since there
                                      // would be many incorrect matches
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

static inline std::uint64_t getMinPartitionSize(std::uint64_t match_flags)
{
    return (match_flags & minimum_partition_size7) >>
        detail::minPartionSizeShift;
}
static precise_unit
    checkPerModifications(std::string unit_string, std::uint64_t match_flags)
{
    // try changing out any "per" words for division sign
    if ((match_flags & no_per_operators) == 0) {
        auto fnd = findWordOperatorSep(unit_string, "per");
        if (fnd != std::string::npos) {
            if (fnd == 0) {
                unit_string.replace(fnd, 3, "1/");
            } else {
                unit_string.replace(fnd, 3, "/");
            }
            auto retunit = unit_from_string_internal(
                unit_string, match_flags + per_operator1);
            if (!is_error(retunit)) {
                return retunit;
            }
        }
    }
    return precise::invalid;
}

static precise_unit
    checkSpecialUnits(const std::string& unit_string, std::uint64_t match_flags)
{
    // lets try checking for meter next which is one of the most common
    // reasons for getting here
    auto fnd = findWordOperatorSep(unit_string, "meter");
    if (fnd != std::string::npos) {
        std::string ustring = unit_string;
        ustring.erase(fnd, 5);
        auto bunit = unit_from_string_internal(ustring, match_flags);
        if (is_valid(bunit)) {
            return precise::m * bunit;
        }
    }
    // detect another somewhat common situation often amphour or ampsecond
    if (unit_string.compare(0, 3, "amp") == 0) {
        auto bunit = unit_from_string_internal(
            unit_string.substr(3), match_flags | minimum_partition_size3);
        if (is_valid(bunit)) {
            return precise::A * bunit;
        }
    }
    if (unit_string.front() == '%') {
        auto bunit = default_unit(unit_string.substr(1));
        if (is_valid(bunit)) {
            return precise::percent * precise::pu * bunit;
        }
        bunit = unit_from_string_internal(
            unit_string.substr(1), match_flags | minimum_partition_size3);
        if (is_valid(bunit)) {
            return precise::percent * precise::pu * bunit;
        }
    }
    if (unit_string.compare(0, 2, "pu") == 0) {
        auto bunit = default_unit(unit_string.substr(2));
        if (is_valid(bunit)) {
            return precise::pu * bunit;
        }
        bunit = unit_from_string_internal(
            unit_string.substr(2), match_flags | minimum_partition_size3);
        if (is_valid(bunit)) {
            return precise::pu * bunit;
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
    std::uint64_t match_flags)
{
    std::string ustring = unit_string;

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
    auto minPartitionSize = getMinPartitionSize(match_flags);
    std::vector<std::string> valid;
    precise_unit possible = precise::invalid;
    bool hasSep{false};
    while (part < unit_string.size() - 1) {
        if (unit_string.size() - part < minPartitionSize) {
            break;
        }
        if (ustring.size() >= minPartitionSize) {
            auto res = unit_quick_match(ustring, match_flags);
            if (!is_valid(res) && ustring.size() >= 3) {
                if (ustring.front() >= 'A' &&
                    ustring.front() <= 'Z') {  // check the lower case version
                                               // since we skipped partitioning
                                               // when we did this earlier
                    ustring[0] += 32;
                    res = unit_quick_match(ustring, match_flags);
                }
            }
            if (is_valid(res)) {
                auto bunit = unit_from_string_internal(
                    unit_string.substr(part),
                    match_flags | skip_partition_check);
                if (is_valid(bunit)) {
                    if (!is_valid(possible)) {
                        possible = res * bunit;
                    } else {
                        auto temp = res * bunit;
                        if (std::abs(log10(temp.multiplier())) <
                            std::abs(log10(possible.multiplier()))) {
                            possible = temp;
                        }
                    }
                }
                valid.push_back(ustring);
            }
        }
        ustring.push_back(unit_string[part]);
        ++part;
        if (ustring.back() == '(' || ustring.back() == '[' ||
            ustring.back() == '{') {
            auto start = part;
            segmentcheck(unit_string, getMatchCharacter(ustring.back()), part);
            if (ustring.back() == '(') {
                if (unit_string.find_first_of("({[*/", start) < part) {
                    // this implies that the contents of the parenthesis
                    // must be a standalone segment and should not be
                    // included in a check
                    break;
                }
            }
            ustring = unit_string.substr(0, part);
        }
        while ((ustring.back() == '_' || ustring.back() == '-') &&
               (part < unit_string.size() - 1)) {
            hasSep = true;
            ustring.push_back(unit_string[part]);
            ++part;
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
    if (is_valid(possible)) {
        return possible;
    }
    if (minPartitionSize <= 1) {
        // meter is somewhat common ending so just check that one too
        if (unit_string.back() == 'm') {
            auto res = unit_quick_match(ustring, match_flags);
            if (is_valid(res)) {
                return res * m;
            }
        }
    }
    if (minPartitionSize <= 2 && !hasSep) {
        // now do a quick check with a 2 character string since we skipped that
        // earlier
        auto qm2 = unit_quick_match(unit_string.substr(0, 2), match_flags);
        if (is_valid(qm2)) {
            valid.insert(valid.begin(), unit_string.substr(0, 2));
        } else if (unit_string.size() == 4) {  // length of 4 is a bit odd so
                                               // check the back two characters
                                               // for a quick match
            qm2 = unit_quick_match(unit_string.substr(2, 2), match_flags);
            auto bunit = unit_from_string_internal(
                unit_string.substr(0, 2), match_flags);
            if (is_valid(bunit)) {
                return qm2 * bunit;
            }
        }
    }
    if (minPartitionSize <= 1) {
        // now pick off a couple 1 character units
        if (unit_string.front() == 'V' || unit_string.front() == 'A') {
            valid.insert(valid.begin(), unit_string.substr(0, 1));
        }
    }
    // start with the biggest
    std::reverse(valid.begin(), valid.end());
    for (auto& vd : valid) {
        auto res = unit_quick_match(vd, match_flags);
        auto nmatch_flags = (vd.size() > 3) ?
            match_flags :
            match_flags | minimum_partition_size3;
        auto bunit = unit_from_string_internal(
            unit_string.substr(vd.size()), nmatch_flags);
        if (is_valid(bunit)) {
            return res * bunit;
        }
    }

    return precise::invalid;
}

/** Some standards allow for custom units usually in brackets with 'U or U
 * at the end
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
/// take a string and raise it to a power after interpreting the units
/// defined in the string
static precise_unit unit_to_the_power_of(
    std::string unit_string,
    int power,
    std::uint64_t match_flags)
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
    checkSIprefix(const std::string& unit_string, std::uint64_t match_flags)
{
    bool threeAgain{false};
    if (unit_string.size() >= 3) {
        if (unit_string[1] == 'A') {
            threeAgain = true;
        } else {
            auto mux = getPrefixMultiplier2Char(unit_string[0], unit_string[1]);
            if (mux != 0.0) {
                auto ustring = unit_string.substr(2);
                if (ustring.size() == 1) {
                    switch (ustring.front()) {
                        case 'B':
                            return {mux, precise::data::byte};
                        case 'b':
                            return {mux, precise::data::bit};
                        case 'k':
                            return precise::invalid;
                        default:
                            break;
                    }
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
        auto mux = ((match_flags & strict_si) == 0) ?
            getPrefixMultiplier(c) :
            getStrictSIPrefixMultiplier(c);
        if (mux != 0.0) {
            auto ustring = unit_string.substr(1);
            if (ustring.size() == 1) {
                switch (ustring.front()) {
                    case 'B':
                        return {mux, precise::data::byte};
                    case 'b':
                        return {mux, precise::data::bit};
                    case 'k':
                        return precise::invalid;
                    default:
                        break;
                }
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

/** handle addition of similar units as a regular unit*/
static precise_unit
    checkUnitAddition(const std::string& unit_string, std::uint64_t match_flags)
{
    auto sep = findOperatorSep(unit_string, "+");
    if (sep != std::string::npos && sep > 0) {
        if ((unit_string[sep - 1] == '+') || sep == unit_string.size() - 1 ||
            unit_string[sep + 1] == '+') {
            return precise::invalid;
        }
        if (unit_string[sep - 1] == 'e' || unit_string[sep - 1] == 'E') {
            // this is scientific notation not addition
            if (isDigitCharacter(unit_string[sep + 1]) &&
                (sep > 1 && isDigitCharacter(unit_string[sep - 2]))) {
                return precise::invalid;
            }
        }
        precise_unit a_unit;
        precise_unit b_unit;
        if (sep + 1 > unit_string.size() / 2) {
            b_unit = unit_from_string_internal(
                unit_string.substr(sep + 1), match_flags);
            if (!is_valid(b_unit)) {
                return precise::invalid;
            }
            a_unit = unit_from_string_internal(
                unit_string.substr(0, sep), match_flags);

            if (!is_valid(a_unit)) {
                return precise::invalid;
            }
        } else {
            a_unit = unit_from_string_internal(
                unit_string.substr(0, sep), match_flags);

            if (!is_valid(a_unit)) {
                return precise::invalid;
            }
            b_unit = unit_from_string_internal(
                unit_string.substr(sep + 1), match_flags);
            if (!is_valid(b_unit)) {
                return precise::invalid;
            }
        }
        auto res = convert(b_unit, a_unit);
        if (!std::isnan(res)) {
            return {
                a_unit.multiplier() + a_unit.multiplier() * res,
                a_unit.base_units()};
        }
    }
    return precise::invalid;
}

precise_unit
    unit_from_string(std::string unit_string, std::uint64_t match_flags)
{
    // always allow the code replacements on first run
    match_flags &= (~skip_code_replacements);
    return unit_from_string_internal(std::move(unit_string), match_flags);
}

// Step 1.  Check if the string matches something in the map
// Step 2.  clean the string, remove spaces, '_' and detect dot notation,
// check for some unicode stuff, check again Step 3.  Find multiplication or
// division operators and split the string into two starting from the last
// operator Step 4.  If found Goto step 1 for each of the two parts, then
// operate on the results Step 5.  Check for ^ and if found goto to step 1
// for interior portion then do a power Step 6.  Remove parenthesis and if
// found goto step 1. Step 7.  Check for a SI prefix on the unit. Step 8.
// Check if the first character is upper case and if so and the string is
// long make it lower case. Step 9.  Check to see if it is a number of some
// kind and make numerical unit. Step 10. Return an error unit.
static precise_unit unit_from_string_internal(
    std::string unit_string,
    std::uint64_t match_flags)
{
    if (unit_string.empty()) {
        return precise::one;
    }
    if (unit_string.size() > 1024) {
        // there is no reason whatsoever that a unit string would be longer
        // than 1024 characters
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
            size_t index{0};
            return commoditizedUnit(unit_string, precise::one, index);
        }
    }
    auto ustring = unit_string;
    // catch a preceding number on the unit
    if (looksLikeNumber(unit_string)) {
        if (unit_string.front() != '1' ||
            unit_string[1] != '/') {  // this catches 1/ which should be
                                      // handled differently
            size_t index{0};
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

    if (((match_flags & no_addition) == 0) &&
        unit_string.find_first_of('+') != std::string::npos) {
        retunit = checkUnitAddition(unit_string, match_flags);
        if (is_valid(retunit)) {
            return retunit;
        }
        // don't allow recursive addition after this point
        match_flags |= no_addition;
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
                if ((unit_string[sep] == '*') &&
                    (a_unit == precise::pu || a_unit == precise::percent)) {
                    b_unit = default_unit(unit_string.substr(sep + 1));
                    if (is_valid(b_unit)) {
                        return a_unit * b_unit;
                    }
                }
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
        int power{+1};
        if (c1 == '-' || c1 == '+') {
            ++sep;
            if (unit_string.length() < sep + 2) {
                // this should have been caught as an invalid sequence
                // earlier
                return precise::invalid;  // LCOV_EXCL_LINE
            }
            // the - ',' is a +/- sign
            power = -(c1 - ',');
        }
        if (isDigitCharacter(unit_string[sep + 1])) {
#ifdef UNITS_CONSTEXPR_IF_SUPPORTED
            if constexpr (sizeof(UNITS_BASE_TYPE) == 8) {
#else
            if (sizeof(UNITS_BASE_TYPE) == 8) {
#endif
                size_t end = sep + 2;
                for (; end < unit_string.size() &&
                     isDigitCharacter(unit_string[end]);
                     ++end) {
                }
                auto powerStringLength = end - sep - 1;
                if (powerStringLength > 1) {
                    auto pstring =
                        unit_string.substr(sep + 1, powerStringLength);
                    char* eptr{nullptr};
                    auto mpower = strtoul(pstring.c_str(), &eptr, 10);
                    if (eptr - pstring.c_str() ==
                        static_cast<std::ptrdiff_t>(powerStringLength)) {
                        power *= mpower;
                    } else {
                        return precise::invalid;  // LCOV_EXCL_LINE
                    }
                } else {
                    power *= (unit_string[sep + 1] - '0');
                }

            } else {
                power *= (unit_string[sep + 1] - '0');
            }
        } else {
            // the check functions should catch this but it would be
            // problematic if not caught
            return precise::invalid;  // LCOV_EXCL_LINE
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
            std::transform(
                ustring.begin(), ustring.end(), ustring.begin(), ::toupper);

            retunit = get_unit(ustring, match_flags);
            if (is_valid(retunit)) {
                return retunit;
            }
            ustring.insert(ustring.begin(), '[');
            ustring.push_back(']');
            retunit = get_unit(ustring, match_flags);
            if (is_valid(retunit)) {
                return retunit;
            }
        }
    }
    auto s_location = unit_string.find("s_");
    if (s_location != std::string::npos) {
        ustring = unit_string;
        ustring.replace(s_location, 2, "_");
        retunit = get_unit(ustring, match_flags);
        if (!is_error(retunit)) {
            return retunit;
        }
    }

    if (modifyTailCodes(unit_string)) {
        retunit = get_unit(unit_string, match_flags);
        if (!is_error(retunit)) {
            return retunit;
        }
    }

    if (!containsPer) {
        retunit = checkMultiplierCharacter(unit_string, match_flags, '-');
        if (!is_error(retunit)) {
            return retunit;
        }

        retunit = checkMultiplierCharacter(unit_string, match_flags, '_');
        if (!is_error(retunit)) {
            return retunit;
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
                size_t loc{0};
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
            retunit = checkSIprefix(unit_string, match_flags);
            if (!is_error(retunit)) {
                return retunit;
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
    {
        // try removing the _ and checking for a match with no partitioning
        ustring = unit_string;
        ustring.erase(
            std::remove(ustring.begin(), ustring.end(), '_'), ustring.end());
        if (ustring != unit_string && !ustring.empty()) {
            retunit = get_unit(ustring, match_flags | skip_partition_check);
            if (!is_error(retunit)) {
                return retunit;
            }
            if (looksLikeNumber(ustring)) {
                size_t loc{0};
                auto number = getDoubleFromString(ustring, &loc);
                if (loc >= ustring.length()) {
                    return {number, one};
                }
                ustring = ustring.substr(loc);
                retunit = unit_from_string_internal(
                    ustring, match_flags | skip_partition_check);
                if (!is_error(retunit)) {
                    return {number, retunit};
                }
                ustring.insert(ustring.begin(), '{');
                ustring.push_back('}');
                return {
                    number,
                    commoditizedUnit(
                        ustring, match_flags | skip_partition_check)};
            }
        }
    }
    if (containsPer) {
        retunit = checkPerModifications(unit_string, match_flags);
        if (is_valid(retunit)) {
            return retunit;
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
        // try changing and words indicative of a unit commodity
        auto fnd = findWordOperatorSep(unit_string, "of");
        if (fnd < unit_string.size() - 2 && fnd != 0) {
            if (unit_string[fnd + 2] == 'w' || unit_string[fnd + 2] == 'm') {
                // this could indicate pressure
            }
            ustring = unit_string;
            ustring.replace(fnd, 2, "{");

            auto sloc = ustring.find_first_of("{[(", fnd + 3);
            if (sloc == std::string::npos) {
                ustring.push_back('}');
            } else if (ustring[sloc - 1] != '_') {
                ustring.insert(sloc, 1, '}');
            } else {
                sloc = ustring.find_first_of(
                    getMatchCharacter(ustring[sloc]), sloc);
                if (sloc != std::string::npos) {
                    ustring.insert(sloc + 1, 1, '}');
                }
            }

            auto cunit =
                commoditizedUnit(ustring, match_flags + commodity_check1);
            if (is_valid(cunit)) {
                return cunit;
            }
        }
    }
    // deal with string like sixty-fourths of an inch, so this part catch
    // of a(n) and removes it getting the following unit
    if (unit_string.size() > 3 && unit_string.compare(0, 3, "ofa") == 0) {
        if (unit_string.size() > 4 && unit_string[3] == 'n') {
            retunit = get_unit(unit_string.substr(4), match_flags);
            if (!is_error(retunit)) {
                return retunit;
            }
        }
        retunit = get_unit(unit_string.substr(3), match_flags);
        if (!is_error(retunit)) {
            return retunit;
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

    if ((match_flags & skip_partition_check) == 0) {
        // check for some special partitioned units
        retunit = checkSpecialUnits(unit_string, match_flags);
        if (!is_error(retunit)) {
            return retunit;
        }
        // maybe some things got merged together so lets try splitting them up
        // in various ways but only allow 3 layers deep
        retunit =
            tryUnitPartitioning(unit_string, match_flags + partition_check1);
        if (!is_error(retunit)) {
            return retunit;
        }
    }
    return precise::invalid;
}  // namespace UNITS_NAMESPACE

precise_measurement measurement_from_string(
    std::string measurement_string,
    std::uint64_t match_flags)
{
    if (measurement_string.empty()) {
        return {};
    }
    // do a cleaning first to get rid of spaces and other issues
    match_flags &= (~skip_code_replacements);
    cleanUnitString(measurement_string, match_flags);

    size_t loc{0};
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
                return {
                    un.multiplier(),
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
    auto unit = unit_from_string_internal(
        std::move(measurement_string), match_flags | skip_code_replacements);
    if (is_valid(unit) && !unit.has_same_base(one.base_units())) {
        return {1.0, unit};
    }
    return {val, precise::invalid};
}

uncertain_measurement uncertain_measurement_from_string(
    const std::string& measurement_string,
    std::uint64_t match_flags)
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

    for (const auto* pmseq : pmsequences) {
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

static smap loadDefinedMeasurementTypes()
{
    smap knownMeasurementTypes{};
    for (const auto& pr : defined_measurement_types) {
        if (pr.first != nullptr) {
            knownMeasurementTypes.emplace(pr.first, pr.second);
        }
    }
    return knownMeasurementTypes;
}

precise_unit default_unit(std::string unit_type)
{
    static const smap measurement_types = loadDefinedMeasurementTypes();

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
    if (unit_type.compare(0, 6, "rateof") == 0) {
        return default_unit(unit_type.substr(6)) / precise::s;
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
    if (ends_with(unit_type, "size")) {
        return default_unit(
            unit_type.substr(0, unit_type.size() - strlen("size")));
    }
    if (unit_type.back() == 's' && unit_type.size() > 1) {
        unit_type.pop_back();
        return default_unit(unit_type);
    }
    if (ends_with(unit_type, "rate")) {
        return default_unit(
                   unit_type.substr(0, unit_type.size() - strlen("rate"))) /
            precise::s;
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
