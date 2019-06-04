/*
Copyright © 2019,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#pragma once
#include "unit_definitions.hpp"
#include <string>
#include <type_traits>

namespace units
{
/// Generate a conversion factor between two units
template <typename UX, typename UX2>
double convert(UX start, UX2 result)
{
    return convert(1.0, start, result);
}

/// Convert a value from one unit base to another
template <typename UX, typename UX2>
double convert(double val, UX start, UX2 result)
{
    static_assert(std::is_same<UX, unit>::value || std::is_same<UX, precise_unit>::value,
                  "convert argument types must be unit or precise_unit");
    static_assert(std::is_same<UX2, unit>::value || std::is_same<UX2, precise_unit>::value,
                  "convert argument types must be unit or precise_unit");
    if (start == result || start.is_default() || result.is_default())
    {
        return val;
    }
    if ((start.is_temperature() || result.is_temperature()) && start.has_same_base(result.base_units()))
    {
        if (start.is_temperature())
        {
            if (units::degF == unit_cast(start))
            {
                val = (val - 32.0) * 5.0 / 9.0;
            }
            else if (start.multiplier() != 1.0)
            {
                val = val * start.multiplier();
            }
            val += 273.15;
            // convert to K
        }
        else
        {
            val = val * start.multiplier();
        }
        if (result.is_temperature())
        {
            val -= 273.15;
            if (units::degF == unit_cast(result))
            {
                val *= 9.0 / 5.0;
                val += 32.0;
            }
            else if (result.multiplier() != 1.0)
            {
                val = val / result.multiplier();
            }
            return val;
        }
        return val / result.multiplier();
    }
    if (start.is_equation() || result.is_equation())
    {
        if (!start.base_units().equivalent_non_counting(result.base_units()))
        {
            return constants::invalid_conversion;
        }
        double keyval = precise::equations::convert_equnit_to_value(val, start.base_units());
        keyval = keyval * start.multiplier() / result.multiplier();
        return precise::equations::convert_value_to_equnit(keyval, result.base_units());
    }
    if (start.base_units() == result.base_units())
    {
        return val * start.multiplier() / result.multiplier();
    }
    // check if both are pu since this doesn't require knowing a base unit
    if (start.is_per_unit() && result.is_per_unit())
    {  // we know they have different unit basis
        if (unit_cast(start) == pu || unit_cast(result) == pu)
        {  // generic pu just means the units are equivalent since the the other is puXX already
            return val;
        }
        if (start.has_same_base(puOhm.base_units()))
        {
            if (result.has_same_base(puMW.base_units()) || result.has_same_base(puA.base_units()))
            {  // V^2/R assuming voltage=1.0 pu; or //I=V/R
                return 1.0 / val;
            }
        }
        else if (start.has_same_base(puA.base_units()))
        {
            if (result.has_same_base(puMW.base_units()))
            {  // P=IV assuming voltage=1.0 pu or //R=V/I
                return val;
            }
            if (result.has_same_base(puOhm.base_units()))
            {  // P=IV assuming voltage=1.0 pu or //R=V/I
                return 1.0 / val;
            }
        }
        else if (start.has_same_base(puMW.base_units()))
        {  // P=IV, or P=V^2/R
            if (result.has_same_base(puA.base_units()))
            {  // IV assuming voltage=1.0 pu or
                return val;
            }
            if (result.has_same_base(puOhm.base_units()))
            {
                return 1.0 / val;
            }
        }
    }
    else if (start.is_per_unit() || result.is_per_unit())
    {
        if (puHz == unit_cast(result) || puHz == unit_cast(start))
        {  // assume 60 Hz
            return convert(val, start, result, 60.0);
        }
        if (puMW == unit_cast(result) || puMW == unit_cast(start))
        {  // assume 100MVA for power base
            return convert(val, start, result, 100.0);
        }
        // mach number
        if (unit_cast(precise::special::mach) == unit_cast(result) ||
            unit_cast(precise::special::mach) == unit_cast(start))
        {  // assume NASA mach number approximation conversions
            return convert(val, start, result, 341.25);
        }
        // other assumptions for PU base are probably dangerous so shouldn't be allowed
        return constants::invalid_conversion;
    }

    auto base_start = start.base_units();
    auto base_result = result.base_units();
    if (base_start.has_same_base(base_result))
    {  // ignore flag and e flag  special cases have been dealt with already, so those are just markers
        return val * start.multiplier() / result.multiplier();
    }
    // deal with some counting conversions
    if (base_start.equivalent_non_counting(base_result))
    {
        auto r1 = base_start.radian();
        auto r2 = base_result.radian();
        auto c1 = base_start.count();
        auto c2 = base_result.count();
        auto m1 = base_start.mole();
        auto m2 = base_result.mole();
        if (m1 == m2 && r1 == r2 && (c1 == 0 || c2 == 0))
        {
            val = val * start.multiplier() / result.multiplier();
            return val;
        }

        if (m1 == m2 && (((r1 == 0) && (c1 == r1 || c1 == 0)) || ((r2 == 0) && (c2 == r1 || c2 == 0))))
        {
            static const double mux[5]{1.0 / (4.0 * constants::pi * constants::pi), 1.0 / (2.0 * constants::pi), 0,
                                       2.0 * constants::pi, 4.0 * constants::pi * constants::pi};
            int muxIndex = r2 - r1 + 2;
            if (muxIndex < 0 || muxIndex > 4)
            {
                return constants::invalid_conversion;
            }
            val *= mux[muxIndex];
            // either 1 or the other is 0 in this equation other it would have triggered before or not gotten here
            val = val * start.multiplier() / result.multiplier();
            return val;
        }
        if (r1 == r2 && (((m1 == 0) && (c1 == m1 || c1 == 0)) || ((m2 == 0) && (c2 == m1 || c2 == 0))))
        {
            static const double muxmol[3]{6.02214076e23, 0, 1.0 / 6.02214076e23};

            int muxIndex = m2 - m1 + 1;
            if (muxIndex < 0 || muxIndex > 2)
            {
                return constants::invalid_conversion;
            }
            val *= muxmol[muxIndex];
            // either 1 or the other is 0 in this equation other it would have triggered before or not gotten here
            val = val * start.multiplier() / result.multiplier();
            return val;
        }
        // radians converted to mole is kind of dumb, theoretically possible but probably shouldn't be
        // supported
    }
    // check for inverse units
    if (base_start.has_same_base(base_result.inv()))
    {  // ignore flag and e flag  special cases have been dealt with already, so those are just markers
        return result.multiplier() / (val * start.multiplier());
    }
    return constants::invalid_conversion;
}

/// Convert a value from one unit base to another potentially involving pu base values
template <typename UX, typename UX2>
double convert(double val, UX start, UX2 result, double baseValue)
{
    static_assert(std::is_same<UX, unit>::value || std::is_same<UX, precise_unit>::value,
                  "convert argument types must be unit or precise_unit");
    static_assert(std::is_same<UX2, unit>::value || std::is_same<UX2, precise_unit>::value,
                  "convert argument types must be unit or precise_unit");
    if (start == result || start.is_default() || result.is_default())
    {
        return val;
    }
    if (start.base_units() == result.base_units())
    {
        return val * start.multiplier() / result.multiplier();
    }
    /// if it the per unit is equivalent no baseValue is needed so give to first function
    if (start.is_per_unit() == result.is_per_unit())
    {
        return convert(val, start, result);
    }

    if (start.has_same_base(result.base_units()) || pu == unit_cast(start) || pu == unit_cast(result))
    {
        if (start.is_per_unit())
        {
            val = val * baseValue;
        }
        val = val * start.multiplier() / result.multiplier();
        if (result.is_per_unit())
        {
            val /= baseValue;
        }
        return val;
    }
    // this would require two base values which power might be assumed but assuming a base voltage is dangerous
    return constants::invalid_conversion;
}

// compute a base value for a particular value
inline double generate_base(detail::unit_data unit, double basePower, double baseVoltage)
{
    if (unit.has_same_base(W.base_units()))
    {
        return basePower;
    }
    if (unit.has_same_base(V.base_units()))
    {
        return baseVoltage;
    }
    if (unit.has_same_base(A.base_units()))
    {
        return basePower / baseVoltage;
    }
    if (unit.has_same_base(ohm.base_units()))
    {
        return baseVoltage * baseVoltage / basePower;
    }
    if (unit.has_same_base(S.base_units()))
    {
        return basePower / (baseVoltage * baseVoltage);
    }
    return constants::invalid_conversion;
}
/// Convert a value from one unit base to another involving power system units
/// the basePower and base voltage are used as the basis values
template <typename UX, typename UX2>
double convert(double val, UX start, UX2 result, double basePower, double baseVoltage)
{
    static_assert(std::is_same<UX, unit>::value || std::is_same<UX, precise_unit>::value,
                  "convert argument types must be unit or precise_unit");
    static_assert(std::is_same<UX2, unit>::value || std::is_same<UX2, precise_unit>::value,
                  "convert argument types must be unit or precise_unit");
    if (start.is_default() || result.is_default())
    {
        return val;
    }
    /// if it isn't per unit or both are per unit give it to the other function since bases aren't needed
    if (start.is_per_unit() == result.is_per_unit())
    {
        auto base = generate_base(start.base_units(), basePower, baseVoltage);
        if (std::isnan(base))
        {
            if (start.is_per_unit() && start == result)
            {
                return val * basePower / baseVoltage;
            }
            if (start.is_per_unit() && start.has_same_base(result.base_units()))
            {
                return val * basePower * start.multiplier() / baseVoltage / result.multiplier();
            }
        }
        return convert(val, start, result);
    }
    /// now we get into some power system specific conversions
    // deal with situations of same base in different quantities
    if (start.has_same_base(result.base_units()))
    {  // We have the same base now we just need to figure out which base to use
        auto base = generate_base(result.base_units(), basePower, baseVoltage);
        if (start.is_per_unit())
        {
            val = val * base;
        }
        val = val * start.multiplier() / result.multiplier();
        if (result.is_per_unit())
        {
            val /= base;
        }
        return val;
    }
    if (result.is_per_unit())
    {
        auto base = generate_base(start.base_units(), basePower, baseVoltage);
        auto puVal = val / base;
        if (pu == unit_cast(result))
        {  // if result is generic pu
            return puVal * start.multiplier();
        }
        // let the first function deal with both as PU
        return convert(puVal, start * pu, result) / result.multiplier();
    }
    // start must be per unit
    auto base = generate_base(result.base_units(), basePower, baseVoltage);
    base *= start.multiplier();
    if (pu == unit_cast(start))
    {  // if start is generic pu
        return val * base;
    }
    return convert(val, start, result * pu) * base;
}

/// Class defining a measurement (value+unit)
template <class X>
class measurement_type
{
  public:
    /// Default constructor
    constexpr measurement_type() = default;
    /// construct from a value and unit
    constexpr measurement_type(X val, unit base) : value_(val), units_(base) {}
    /// Get the base value with no units
    constexpr X value() const { return value_; }

    constexpr measurement_type operator*(measurement_type other) const
    {
        return measurement_type(value_ * other.value_, units_ * other.units_);
    }
    constexpr measurement_type operator*(double val) const { return measurement_type(value_ * val, units_); }
    constexpr measurement_type operator/(measurement_type other) const
    {
        return measurement_type(value_ / other.value_, units_ / other.units_);
    }
    constexpr measurement_type operator/(double val) const { return measurement_type(value_ / val, units_); }

    measurement_type operator+(measurement_type other) const
    {
        return measurement_type(value_ + other.value_as(units_), units_);
    }
    measurement_type operator-(measurement_type other) const
    {
        return measurement_type(value_ - other.value_as(units_), units_);
    }

    constexpr measurement_type operator+(double val) const { return measurement_type(value_ + val, units_); }
    constexpr measurement_type operator-(double val) const { return measurement_type(value_ - val, units_); }
    /// Convert a unit to have a new base
    measurement_type convert_to(unit newUnits) const
    {
        return measurement_type(units::convert(value_, units_, newUnits), newUnits);
    }

    /// Convert a unit into its base units
    constexpr measurement_type convert_to_base() const
    {
        return measurement_type(value_ * units_.multiplier(), unit(units_.base_units()));
    }

    constexpr unit units() const { return units_; }

    // convert the measurement to a single unit
    constexpr unit as_unit() const { return unit(value_, units_); }
    /// Equality operator
    bool operator==(measurement_type other) const
    {
        return detail::cround(static_cast<float>(value_)) ==
               detail::cround(static_cast<float>(other.value_as(units_)));
    }
    bool operator>(measurement_type other) const { return value_ > other.value_as(units_); }
    bool operator<(measurement_type other) const { return value_ < other.value_as(units_); }
    bool operator>=(measurement_type other) const
    {
        return detail::cround(static_cast<float>(value_)) >=
               detail::cround(static_cast<float>(other.value_as(units_)));
    }
    bool operator<=(measurement_type other) const
    {
        return detail::cround(static_cast<float>(value_)) <=
               detail::cround(static_cast<float>(other.value_as(units_)));
    }
    /// Not equal operator
    bool operator!=(measurement_type other) const { return !operator==(other); }
    /// Get the numerical value as a particular unit type
    double value_as(unit units) const
    {
        return (units_ == units) ? value_ : units::convert(value_, units_, units);
    }

  private:
    X value_{0.0};
    unit units_;
};

/// measurement using a double a value type
using measurement = measurement_type<double>;
/// Measurement using a float as the value type
using measurement_f = measurement_type<float>;

/// The design requirement is for this to fit in the space of 2 doubles
static_assert(sizeof(measurement) <= 16, "Measurement class is too large");

constexpr inline measurement operator*(double val, unit unit_base) { return {val, unit_base}; }
constexpr inline measurement operator*(unit unit_base, double val) { return {val, unit_base}; }

constexpr inline measurement operator/(double val, unit unit_base) { return {val, unit_base.inv()}; }
constexpr inline measurement operator/(unit unit_base, double val) { return {1.0 / val, unit_base}; }

/// Class defining a measurement (value+unit) with a fixed unit type
template <class X>
class fixed_measurement_type
{
  public:
    /// construct from a value and unit
    constexpr fixed_measurement_type(X val, unit base) : value_(val), units_(base) {}

    explicit constexpr fixed_measurement_type(measurement_type<X> val) : value_(val.value()), units_(val.units())
    {
    }

    constexpr fixed_measurement_type(const fixed_measurement_type &val) : value_(val.value()), units_(val.units())
    {
    }

    fixed_measurement_type &operator=(measurement_type<X> val)
    {
        value_ = (units_ == val.units()) ? val.value() : val.value_as(units_);
        return *this;
    }
    // direct conversion operator
    operator measurement_type<X>() { return measurement_type<X>(value_, units_); }
    /// Get the base value with no units
    constexpr X value() const { return value_; }

    constexpr measurement_type<X> operator*(measurement_type<X> other) const
    {
        return measurement_type<X>(value_ * other.value(), units_ * other.units());
    }
    constexpr measurement_type<X> operator*(double val) const { return measurement_type<X>(value_ * val, units_); }
    constexpr measurement_type<X> operator/(measurement_type<X> other) const
    {
        return measurement_type<X>(value_ / other.value(), units_ / other.units());
    }
    constexpr measurement_type<X> operator/(double val) const { return measurement_type<X>(value_ / val, units_); }

    measurement_type<X> operator+(measurement_type<X> other) const
    {
        return measurement_type<X>(value_ + other.value_as(units_), units_);
    }
    measurement_type<X> operator-(measurement_type<X> other) const
    {
        return measurement_type<X>(value_ - other.value_as(units_), units_);
    }

    constexpr measurement_type<X> operator+(double val) const { return measurement_type<X>(value_ + val, units_); }
    constexpr measurement_type<X> operator-(double val) const { return measurement_type<X>(value_ - val, units_); }

    /// Convert a unit to have a new base
    measurement_type<X> convert_to(unit newUnits) const
    {
        return measurement_type<X>(units::convert(value_, units_, newUnits), newUnits);
    }

    constexpr unit units() const { return units_; }

    // convert the measurement to a single unit
    constexpr unit as_unit() const { return unit(value_, units_); }

    /// Get the numerical value as a particular unit type
    double value_as(unit units) const
    {
        return (units_ == units) ? value_ : units::convert(value_, units_, units);
    }

  private:
    X value_{0.0};
    const unit units_;
};

/// measurement using a double a value type
using fixed_measurement = fixed_measurement_type<double>;
/// Measurement using a float as the value type
using fixed_measurement_f = fixed_measurement_type<float>;

/// Class using precise units and double precision
class precision_measurement
{
  public:
    /// Default constructor
    constexpr precision_measurement() = default;
    constexpr precision_measurement(double val, precise_unit base) : value_(val), units_(base) {}
    constexpr double value() const { return value_; }
    constexpr precise_unit units() const { return units_; }

    // convert the measurement to a single unit
    constexpr precise_unit as_unit() const { return {value_, units_}; }

    constexpr precision_measurement operator*(precision_measurement other) const
    {
        return {value_ * other.value_, units_ * other.units_};
    }
    constexpr precision_measurement operator*(precise_unit other) const { return {value_, units_ * other}; }
    constexpr precision_measurement operator*(double val) const { return {value_ * val, units_}; }
    constexpr precision_measurement operator/(precision_measurement other) const
    {
        return {value_ / other.value_, units_ / other.units_};
    }
    constexpr precision_measurement operator/(precise_unit other) const { return {value_, units_ / other}; }
    constexpr precision_measurement operator/(double val) const { return {value_ / val, units_}; }

    precision_measurement operator+(precision_measurement other) const
    {
        return {value_ + other.value_as(units_), units_};
    }
    precision_measurement operator-(precision_measurement other) const
    {
        return {value_ - other.value_as(units_), units_};
    }
    /// Add a double assuming the same units
    constexpr precision_measurement operator+(double val) const { return {value_ + val, units_}; }
    /// Subtract a double assuming the same units
    constexpr precision_measurement operator-(double val) const { return {value_ - val, units_}; }
    /// Convert a unit to have a new base
    precision_measurement convert_to(precise_unit newUnits) const
    {
        return {units::convert(value_, units_, newUnits), newUnits};
    }

    /// Convert a unit into its base units
    constexpr precision_measurement convert_to_base() const
    {
        return {value_ * units_.multiplier(), precise_unit(units_.base_units())};
    }

    /// Equality operator
    bool operator==(precision_measurement other) const
    {
        return detail::cround_precise(value_) == detail::cround_precise(other.value_as(units_));
    }
    /// Not equal operator
    bool operator!=(precision_measurement other) const { return !operator==(other); }

    bool operator>(precision_measurement other) const { return value_ > other.value_as(units_); }
    bool operator<(precision_measurement other) const { return value_ < other.value_as(units_); }
    bool operator>=(precision_measurement other) const
    {
        return detail::cround_precise(value_) >= detail::cround_precise(other.value_as(units_));
    }
    bool operator<=(precision_measurement other) const
    {
        return detail::cround_precise(value_) <= detail::cround_precise(other.value_as(units_));
    }
    /// Get the numerical value as a particular unit type
    double value_as(precise_unit units) const
    {
        return (units_ == units) ? value_ : units::convert(value_, units_, units);
    }

  private:
    double value_{0.0};
    precise_unit units_;
};

constexpr inline precision_measurement operator*(double val, precise_unit unit_base) { return {val, unit_base}; }
constexpr inline precision_measurement operator*(precise_unit unit_base, double val) { return {val, unit_base}; }

constexpr inline precision_measurement operator/(double val, precise_unit unit_base)
{
    return {val, unit_base.inv()};
}
constexpr inline precision_measurement operator/(precise_unit unit_base, double val)
{
    return {1.0 / val, unit_base};
}

/// Design requirement this must fit in space of 3 doubles
static_assert(sizeof(precision_measurement) <= 24, "precision measurement is too large");

/// Class using precise units and double precision
class fixed_precision_measurement
{
  public:
    constexpr fixed_precision_measurement(double val, precise_unit base) : value_(val), units_(base) {}

    explicit constexpr fixed_precision_measurement(precision_measurement val)
        : value_(val.value()), units_(val.units())
    {
    }

    constexpr fixed_precision_measurement(const fixed_precision_measurement &val)
        : value_(val.value()), units_(val.units())
    {
    }

    fixed_precision_measurement &operator=(precision_measurement val)
    {
        value_ = (units_ == val.units()) ? val.value() : val.value_as(units_);
        return *this;
    }

    // direct conversion operator
    operator precision_measurement() { return precision_measurement(value_, units_); }

    constexpr double value() const { return value_; }
    constexpr precise_unit units() const { return units_; }

    // convert the measurement to a single unit
    constexpr precise_unit as_unit() const { return {value_, units_}; }

    constexpr precision_measurement operator*(precision_measurement other) const
    {
        return {value_ * other.value(), units_ * other.units()};
    }
    constexpr precision_measurement operator*(double val) const { return {value_ * val, units_}; }
    constexpr precision_measurement operator/(precision_measurement other) const
    {
        return {value_ / other.value(), units_ / other.units()};
    }
    constexpr precision_measurement operator/(double val) const { return {value_ / val, units_}; }

    precision_measurement operator+(precision_measurement other) const
    {
        return {value_ + other.value_as(units_), units_};
    }
    precision_measurement operator-(precision_measurement other) const
    {
        return {value_ - other.value_as(units_), units_};
    }
    /// Add a double assuming the same units
    constexpr precision_measurement operator+(double val) const { return {value_ + val, units_}; }
    /// Subtract a double assuming the same units
    constexpr precision_measurement operator-(double val) const { return {value_ - val, units_}; }
    /// Convert a unit to have a new base
    precision_measurement convert_to(precise_unit newUnits) const
    {
        return {units::convert(value_, units_, newUnits), newUnits};
    }

    /// Convert a unit into its base units
    constexpr precision_measurement convert_to_base() const
    {
        return {value_ * units_.multiplier(), precise_unit(units_.base_units())};
    }

    /// Equality operator
    bool operator==(precision_measurement other) const
    {
        return detail::cround_precise(value_) == detail::cround_precise(other.value_as(units_));
    }
    /// Not equal operator
    bool operator!=(precision_measurement other) const { return !operator==(other); }

    bool operator>(precision_measurement other) const { return value_ > other.value_as(units_); }
    bool operator<(precision_measurement other) const { return value_ < other.value_as(units_); }
    bool operator>=(precision_measurement other) const
    {
        return detail::cround_precise(value_) >= detail::cround_precise(other.value_as(units_));
    }
    bool operator<=(precision_measurement other) const
    {
        return detail::cround_precise(value_) <= detail::cround_precise(other.value_as(units_));
    }
    /// Get the numerical value as a particular unit type
    double value_as(precise_unit units) const
    {
        return (units_ == units) ? value_ : units::convert(value_, units_, units);
    }

  private:
    double value_{0.0};
    const precise_unit units_;
};

#ifndef UNITS_HEADER_ONLY

enum unit_conversion_flags : uint32_t
{
    case_insensitive = 1u,  //!< perform case insensitive matching for UCUM case insensitive matching
    single_slash =
      2u,  //!< specify that there is a single numerator and denominator only a single slash in the unit operations
    recursion_depth1 = (1u << 15),  // skip checking for SI prefixes
    // don't put anything at   16, 15 through 17 are connected to limit recursion depth
    no_recursion = (1u << 17),  // skip checking for SI prefixes
    not_first_pass = (1u << 18),  // indicate that is not the first pass
    per_operator1 = (1u << 21),  // skip matching per
    no_per_operators = (1u << 23),  // skip matching per
    no_locality_modifiers = (1u << 24),  // skip locality modifiers
    no_of_operator = (1u << 25),  // skip dealing with "of"
    no_commodities = (1u << 26),  // skip commodity checks
    partition_check1 = (1u << 27),  // skip checking for SI prefixes
    // don't put anything at 28, 27 and 28 are connected to limit partition depth
    skip_partition_check = (1u << 29),  // skip checking for SI prefixes
    skip_si_prefix_check = (1u << 30),  // skip checking for SI prefixes
    skip_code_replacements = (1u << 31),  // don't do some code and sequence replacements
};
/// Generate a string representation of the unit
std::string to_string(precise_unit units, uint32_t match_flags = 0);

/// Generate a string representation of the unit
inline std::string to_string(unit units, uint32_t match_flags = 0)
{
    // For naming precision doesn't matter
    return to_string(precise_unit(units), match_flags);
}

/** Generate a unit object from a string representation of it
@param unit_string the string to convert
@param match_flags see /ref unit_conversion_flags to control the matching process somewhat
@return a precise unit corresponding to the string if no match was found the unit will be an error unit
*/
precise_unit unit_from_string(std::string unit_string, uint32_t match_flags = 0);

/** Generate a unit object from the string definition of a type of measurement
@param unit_type  string representing the type of measurement
@return a precise unit corresponding to the SI unit for the measurement specified in unit_type
*/
precise_unit default_unit(std::string unit_type);

/** Generate a measurement from a string
@param measurement_string the string to convert
@param match_flags see / ref unit_conversion_flags to control the matching process somewhat
  @ return a precise unit corresponding to the string if no match was found the unit will be an error unit
    */
precision_measurement measurement_from_string(std::string measurement_string, uint32_t match_flags = 0);
/// Convert a precision measurement to a string (with some extra decimal digits displayed
std::string to_string(precision_measurement measure, uint32_t match_flags = 0);
/// Convert a measurement to a string
std::string to_string(measurement measure, uint32_t match_flags = 0);
/// Convert a floating point measurement to a string
std::string to_string(measurement_f measure, uint32_t match_flags = 0);
/// Add a custom unit to be included in any string processing
void addUserDefinedUnit(std::string name, precise_unit un);
// Clear all user defined units from memory
void clearUserDefinedUnits();

/// Turn off the ability to add custom commodities for later access
bool disableUserDefinedUnits();
/// Enable the ability to add custom commodities for later access
bool enableUserDefinedUnits();

/// get the code to use for a particular commodity
uint32_t getCommodity(std::string comm);

/// get the code to use for a particular commodity
std::string getCommodityName(uint32_t commodity);

/// add a custom commodity for later retrieval
void addCustomCommodity(std::string comm, uint32_t code);
/// clear all custom commodities
void clearCustomCommodities();
/// Turn off the ability to add custom commodities for later access
bool disableCustomCommodities();
/// Enable the ability to add custom commodities for later access
bool enableCustomCommodities();

#define EXTRA_UNIT_STANDARDS
// Some specific unit code standards
#ifdef EXTRA_UNIT_STANDARDS
/// generate a unit from a string as defined by the X12 standard
precise_unit x12_unit(std::string x12_string);
/// generate a unit from a string as defined by the US DOD
precise_unit dod_unit(std::string dod_string);
/// generate a unit from a string as defined by the r20 standard
precise_unit r20_unit(std::string r20_string);
#endif

#endif  // UNITS_HEADER_ONLY
/// Physical constants in use with associated units
namespace constants
{
    /// Standard gravity
    constexpr precision_measurement g0(9.80665, precise::m / precise::s / precise::s);
    /// Gravitational Constant
    constexpr precision_measurement
      G(6.6740831e-11, precise_unit(detail::unit_data(3, -1, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)));
    /// Speed of light
    constexpr precision_measurement c{299792458.0, precise::m / precise::s};
    /// Elementary Charge (2019 redefinition)
    constexpr precision_measurement e(1.602176634e-19, precise::C);
    ///  hyperfine structure transition frequency of the cesium-133 atom
    constexpr precision_measurement fCs(9192631770.0, precise::Hz);
    /// Planck constant (2019 redefinition)
    constexpr precision_measurement h{6.62607015e-34, precise::J *precise::second};
    /// Boltzman constant (2019 redefinition)
    constexpr precision_measurement k{1.380649e-23, precise::J / precise::K};
    /// Avogadros constant (2019 redefinition)
    constexpr precision_measurement Na{6.02214076e23, precise::one / precise::mol};
    /// Luminous efficiency
    constexpr precision_measurement Kcd{683.0, precise::lm / precise::W};
    /// Permittivity of free space
    constexpr precision_measurement eps0{8.854187817e-12, precise::F / precise::m};
    /// Permeability of free space
    constexpr precision_measurement mu0{12.566370614e-7, precise::N / (precise::A * precise::A)};
    /// Gas Constant
    constexpr precision_measurement R{8.314459848, precise::J / (precise::mol * precise::K)};
    /// Stephan Boltzmann constant
    constexpr precision_measurement s{5.67036713e-8,
                                      precise_unit(detail::unit_data(0, 1, -3, 0, -4, 0, 0, 0, 0, 0, 0, 0, 0, 0))};
    /// hubble constant AKA 69.3 km/s/Mpc
    constexpr precision_measurement H0{2.25e-18, precise::Hz};
    /// Mass of an electron
    constexpr precision_measurement me{9.1093835611e-31, precise::kg};
    /// Mass of a proton
    constexpr precision_measurement mp{1.67262189821e-27, precise::kg};
    /// Planck units
    namespace planck
    {
        constexpr precision_measurement length{1.61622938e-35, precise::m};
        constexpr precision_measurement mass{2.17647051e-8, precise::kg};
        constexpr precision_measurement time{5.3911613e-44, precise::s};
        constexpr precision_measurement charge{1.87554595641e-18, precise::C};
        constexpr precision_measurement temperature{1.41680833e32, precise::K};
    }  // namespace planck

    namespace atomic
    {  // https://www.bipm.org/en/publications/si-brochure/table7.html
        constexpr precision_measurement length{0.5291772109217e-10, precise::m};
        constexpr precision_measurement mass = me;
        constexpr precision_measurement time{2.41888432650212e-17, precise::s};
        constexpr precision_measurement charge = e;
        constexpr precision_measurement energy{4.3597443419e-18, precise::J};
        constexpr precision_measurement action{1.05457172647e-34, precise::J *precise::s};
    }  // namespace atomic
}  // namespace constants

namespace detail
{
    namespace testing
    {
        // generate a number from a number sequence
        double testLeadingNumber(const std::string &test, size_t &index);
    }  // namespace testing
}  // namespace detail

}  // namespace units
