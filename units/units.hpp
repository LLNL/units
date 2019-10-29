/*
Copyright (c) 2019,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#pragma once
#include "unit_definitions.hpp"
#include <cmath>
#include <string>
#include <type_traits>

namespace units
{
/// Generate a conversion factor between two units in a constexpr function, the units will only convert if they
/// have the same base unit
template <typename UX, typename UX2>
constexpr double quick_convert(UX start, UX2 result)
{
    return quick_convert(1.0, start, result);
}

/// Generate a conversion factor between two units in a constexpr function, the units will only convert if they
/// have the same base unit
template <typename UX, typename UX2>
constexpr double quick_convert(double val, UX start, UX2 result)
{
    static_assert(std::is_same<UX, unit>::value || std::is_same<UX, precise_unit>::value,
                  "convert argument types must be unit or precise_unit");
    static_assert(std::is_same<UX2, unit>::value || std::is_same<UX2, precise_unit>::value,
                  "convert argument types must be unit or precise_unit");
    return (start.base_units() == result.base_units() && !start.is_equation() && !result.is_equation()) ?
             val * start.multiplier() / result.multiplier() :
             constants::invalid_conversion;
}

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
    if ((is_temperature(start) || is_temperature(result)) && start.has_same_base(result.base_units()))
    {
        return detail::convertTemperature(val, start, result);
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
        double converted_val = puconversion::knownConversions(val, start.base_units(), result.base_units());
        if (!std::isnan(converted_val))
        {
            return converted_val;
        }
    }
    else if (start.is_per_unit() || result.is_per_unit())
    {
        double genBase = puconversion::assumedBase(unit_cast(start), unit_cast(result));
        if (!std::isnan(genBase))
        {
            return convert(val, start, result, genBase);
        }
        // other assumptions for PU base are probably dangerous so shouldn't be allowed
        return constants::invalid_conversion;
    }

    auto base_start = start.base_units();
    auto base_result = result.base_units();
    if (base_start.has_same_base(base_result))
    {  // ignore i flag and e flag,  special cases have been dealt with already, so those are just markers
        return val * start.multiplier() / result.multiplier();
    }
    // deal with some counting conversions
    if (base_start.equivalent_non_counting(base_result))
    {
        double converted_val = detail::convertCountingUnits(val, start, result);
        if (!std::isnan(converted_val))
        {
            return converted_val;
        }
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
        auto base = puconversion::generate_base(start.base_units(), basePower, baseVoltage);
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
        auto base = puconversion::generate_base(result.base_units(), basePower, baseVoltage);
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
        auto base = puconversion::generate_base(start.base_units(), basePower, baseVoltage);
        auto puVal = val / base;
        if (pu == unit_cast(result))
        {  // if result is generic pu
            return puVal * start.multiplier();
        }
        // let the first function deal with both as PU
        return convert(puVal, start * pu, result) / result.multiplier();
    }
    // start must be per unit
    auto base = puconversion::generate_base(result.base_units(), basePower, baseVoltage);
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

    constexpr measurement_type operator%(double val) const { return measurement_type(fmod(value_, val), units_); }
    measurement_type operator+(measurement_type other) const
    {
        return measurement_type(value_ + other.value_as(units_), units_);
    }
    measurement_type operator-(measurement_type other) const
    {
        return measurement_type(value_ - other.value_as(units_), units_);
    }

    // double multiplier
    friend constexpr inline measurement_type operator*(double val, measurement_type meas) { return meas * val; }
    // divide measurement into a double
    friend constexpr inline measurement_type operator/(double val, measurement_type meas)
    {
        return {val / meas.value_, meas.units_.inv()};
    }

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

    /// extract the current units from the measurement
    constexpr unit units() const { return units_; }

    // convert the measurement to a single unit
    constexpr unit as_unit() const { return unit(value_, units_); }
    /// Equality operator
    bool operator==(measurement_type other) const
    {
        auto val = other.value_as(units_);
        return (value_ == val) ? true :
                                 detail::compare_round_equals(static_cast<float>(value_), static_cast<float>(val));
    }
    constexpr bool operator>(measurement_type other) const { return value_ > other.value_as(units_); }
    constexpr bool operator<(measurement_type other) const { return value_ < other.value_as(units_); }
    bool operator>=(measurement_type other) const
    {
        auto val = other.value_as(units_);
        return (value_ >= val) ? true :
                                 detail::compare_round_equals(static_cast<float>(value_), static_cast<float>(val));
    }
    bool operator<=(measurement_type other) const
    {
        auto val = other.value_as(units_);
        return (value_ <= val) ? true :
                                 detail::compare_round_equals(static_cast<float>(value_), static_cast<float>(val));
    }
    /// Not equal operator
    bool operator!=(measurement_type other) const { return !operator==(other); }
    /// Get the numerical value as a particular unit type
    double value_as(unit units) const
    {
        return (units_ == units) ? value_ : units::convert(value_, units_, units);
    }

  private:
    X value_{0.0};  //!< the numerical quantity of the unit
    unit units_;  //!< the actual unit represented
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

    explicit constexpr fixed_measurement_type(measurement_type<X> val) noexcept
        : value_(val.value()), units_(val.units())
    {
    }
    // define copy constructor but purposely leave off copy assignment and move since that would be pointless
    constexpr fixed_measurement_type(const fixed_measurement_type &val) noexcept
        : value_(val.value()), units_(val.units())
    {
    }
    /// assignment operator
    fixed_measurement_type &operator=(measurement_type<X> val)
    {
        value_ = (units_ == val.units()) ? val.value() : static_cast<X>(val.value_as(units_));
        return *this;
    }
    /// Assignment from number,  allow direct numerical assignment since the units are fixes and known at
    /// construction time
    fixed_measurement_type &operator=(X val)
    {
        value_ = val;
        return *this;
    }
    /// direct conversion operator
    operator measurement_type<X>() { return measurement_type<X>(value_, units_); }
    /// Get the base value with no units
    constexpr X value() const { return value_; }

    constexpr measurement_type<X> operator*(measurement_type<X> other) const
    {
        return measurement_type<X>(value_ * other.value(), units_ * other.units());
    }
    constexpr fixed_measurement_type<X> operator*(X val) const
    {
        return fixed_measurement_type<X>(value_ * val, units_);
    }
    constexpr measurement_type<X> operator/(measurement_type<X> other) const
    {
        return measurement_type<X>(value_ / other.value(), units_ / other.units());
    }
    constexpr fixed_measurement_type<X> operator/(X val) const
    {
        return fixed_measurement_type<X>(value_ / val, units_);
    }

    fixed_measurement_type<X> operator+(measurement_type<X> other) const
    {
        return fixed_measurement_type<X>(value_ + other.value_as(units_), units_);
    }
    fixed_measurement_type<X> operator-(measurement_type<X> other) const
    {
        return fixed_measurement_type<X>(value_ - other.value_as(units_), units_);
    }

    constexpr fixed_measurement_type<X> operator+(X val) const
    {
        return fixed_measurement_type<X>(value_ + val, units_);
    }
    constexpr fixed_measurement_type<X> operator-(X val) const
    {
        return fixed_measurement_type<X>(value_ - val, units_);
    }

    /// Convert a unit to have a new base
    fixed_measurement_type<X> convert_to(unit newUnits) const
    {
        return fixed_measurement_type<X>(units::convert(value_, units_, newUnits), newUnits);
    }
    /// Get the underlying units value
    constexpr unit units() const { return units_; }

    /// convert the measurement to a single unit
    constexpr unit as_unit() const { return unit(value_, units_); }

    /// Get the numerical value as a particular unit type
    double value_as(unit units) const
    {
        return (units_ == units) ? value_ : units::convert(static_cast<double>(value_), units_, units);
    }

    fixed_measurement_type<X> &operator+=(X val)
    {
        value_ += val;
        return *this;
    }
    fixed_measurement_type<X> &operator-=(X val)
    {
        value_ -= val;
        return *this;
    }
    fixed_measurement_type<X> &operator*=(X val)
    {
        value_ *= val;
        return *this;
    }
    fixed_measurement_type<X> &operator/=(X val)
    {
        value_ /= val;
        return *this;
    }
    /// comparison operators
    bool operator==(X val) const
    {
        return (value_ == val) ? true :
                                 detail::compare_round_equals(static_cast<float>(value_), static_cast<float>(val));
    };
    bool operator!=(X val) const { return !operator==(val); };
    constexpr bool operator>(X val) const { return value_ > val; };
    constexpr bool operator<(X val) const { return value_ < val; };
    bool operator>=(X val) const { return (value_ > val) ? true : operator==(val); };
    bool operator<=(X val) const { return value_ < val ? true : operator==(val); };

    bool operator==(measurement_type<X> val) const
    {
        return operator==((units_ == val.units()) ? val.value() : static_cast<X>(val.value_as(units_)));
    };
    bool operator!=(measurement_type<X> val) const
    {
        return operator!=((units_ == val.units()) ? val.value() : static_cast<X>(val.value_as(units_)));
    };
    bool operator>(measurement_type<X> val) const
    {
        return operator>((units_ == val.units()) ? val.value() : static_cast<X>(val.value_as(units_)));
    };
    bool operator<(measurement_type<X> val) const
    {
        return operator<((units_ == val.units()) ? val.value() : static_cast<X>(val.value_as(units_)));
    };
    bool operator>=(measurement_type<X> val) const
    {
        return operator>=((units_ == val.units()) ? val.value() : static_cast<X>(val.value_as(units_)));
    };
    bool operator<=(measurement_type<X> val) const
    {
        return operator<=((units_ == val.units()) ? val.value() : static_cast<X>(val.value_as(units_)));
    };

    friend bool operator==(X val, const fixed_measurement_type<X> &v2) { return v2 == val; };
    friend bool operator!=(X val, const fixed_measurement_type<X> &v2) { return v2 != val; };
    friend constexpr bool operator>(X val, const fixed_measurement_type<X> &v2) { return val > v2.value(); };
    friend constexpr bool operator<(X val, const fixed_measurement_type<X> &v2) { return val < v2.value(); };
    friend bool operator>=(X val, const fixed_measurement_type<X> &v2)
    {
        return (val > v2.value()) ? true : (v2 == val);
    };
    friend bool operator<=(X val, const fixed_measurement_type<X> &v2)
    {
        return (val < v2.value()) ? true : (v2 == val);
    };

    // + and - are allowed for fixed_measurement since the units are known
    /// friend operators for math operators
    friend constexpr inline fixed_measurement_type<X> operator+(X v1, const fixed_measurement_type<X> &v2)
    {
        return {v1 + v2.value(), v2.units()};
    }
    friend constexpr inline fixed_measurement_type<X> operator-(X v1, const fixed_measurement_type<X> &v2)
    {
        return {v1 - v2.value(), v2.units()};
    }
    friend constexpr inline fixed_measurement_type<X> operator*(X v1, const fixed_measurement_type<X> &v2)
    {
        return {v1 * v2.value(), v2.units()};
    }
    friend constexpr inline fixed_measurement_type<X> operator/(X v1, const fixed_measurement_type<X> &v2)
    {
        return {v1 / v2.value(), v2.units().inv()};
    }

  private:
    X value_{0.0};  //!< the unit value
    const unit units_;  //!< a fixed unit of measurement
};

/// measurement using a double as the value type
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
        return valueEqualityCheck((units_ == other.units()) ? other.value() : other.value_as(units_));
    }
    /// Not equal operator
    bool operator!=(precision_measurement other) const
    {
        return !valueEqualityCheck((units_ == other.units()) ? other.value() : other.value_as(units_));
    }

    bool operator>(precision_measurement other) const { return value_ > other.value_as(units_); }
    bool operator<(precision_measurement other) const { return value_ < other.value_as(units_); }
    bool operator>=(precision_measurement other) const
    {
        double val = other.value_as(units_);
        return (value_ > val) ? true : valueEqualityCheck(val);
    }
    bool operator<=(precision_measurement other) const
    {
        double val = other.value_as(units_);
        return (value_ < val) ? true : valueEqualityCheck(val);
    }
    /// Get the numerical value as a particular unit type
    double value_as(precise_unit units) const
    {
        return (units_ == units) ? value_ : units::convert(value_, units_, units);
    }
    // double multiplier
    friend constexpr inline precision_measurement operator*(double val, precision_measurement meas)
    {
        return meas * val;
    }
    // divide measurement into a double
    friend constexpr inline precision_measurement operator/(double val, precision_measurement meas)
    {
        return {val / meas.value_, meas.units_.inv()};
    }

  private:
    double value_{0.0};
    precise_unit units_;
    /// does a numerical equality check on the value accounting for tolerances
    bool valueEqualityCheck(double otherval) const
    {
        return (value_ == otherval) ? true : detail::compare_round_equals_precise(value_, otherval);
    }
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

    /// Assignment from double,  allow direct numerical assignment since the units are fixes and known at
    /// construction time
    fixed_precision_measurement &operator=(double val)
    {
        value_ = val;
        return *this;
    }
    // direct conversion operator
    operator precision_measurement() { return precision_measurement(value_, units_); }

    constexpr double value() const { return value_; }
    constexpr precise_unit units() const { return units_; }

    // convert the measurement to a single unit
    constexpr precise_unit as_unit() const { return {value_, units_}; }

    /// Get the numerical value as a particular unit type
    double value_as(precise_unit units) const
    {
        return (units_ == units) ? value_ : units::convert(value_, units_, units);
    }

    constexpr precision_measurement operator*(precision_measurement other) const
    {
        return {value_ * other.value(), units_ * other.units()};
    }
    constexpr fixed_precision_measurement operator*(double val) const { return {value_ * val, units_}; }
    constexpr precision_measurement operator/(precision_measurement other) const
    {
        return {value_ / other.value(), units_ / other.units()};
    }
    constexpr fixed_precision_measurement operator/(double val) const { return {value_ / val, units_}; }

    fixed_precision_measurement operator+(precision_measurement other) const
    {
        return {value_ + other.value_as(units_), units_};
    }
    fixed_precision_measurement operator-(precision_measurement other) const
    {
        return {value_ - other.value_as(units_), units_};
    }
    /// Add a double assuming the same units
    constexpr fixed_precision_measurement operator+(double val) const { return {value_ + val, units_}; }
    /// Subtract a double assuming the same units
    constexpr fixed_precision_measurement operator-(double val) const { return {value_ - val, units_}; }

    fixed_precision_measurement &operator+=(double val)
    {
        value_ += val;
        return *this;
    }
    fixed_precision_measurement &operator-=(double val)
    {
        value_ -= val;
        return *this;
    }
    fixed_precision_measurement &operator*=(double val)
    {
        value_ *= val;
        return *this;
    }
    fixed_precision_measurement &operator/=(double val)
    {
        value_ /= val;
        return *this;
    }
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

    /// comparison operators
    bool operator==(double val) const
    {
        return (value_ == val) ? true : detail::compare_round_equals_precise(value_, val);
    };
    bool operator!=(double val) const { return !operator==(val); };
    constexpr bool operator>(double val) const { return value_ > val; };
    constexpr bool operator<(double val) const { return value_ < val; };
    bool operator>=(double val) const { return (value_ >= val) ? true : operator==(val); };
    bool operator<=(double val) const { return value_ <= val ? true : operator==(val); };

    /// Equality operator
    bool operator==(precision_measurement val) const
    {
        return operator==((units_ == val.units()) ? val.value() : val.value_as(units_));
    }
    /// Not equal operator
    bool operator!=(precision_measurement val) const
    {
        return operator!=((units_ == val.units()) ? val.value() : val.value_as(units_));
    }

    bool operator>(precision_measurement val) const
    {
        return operator>((units_ == val.units()) ? val.value() : val.value_as(units_));
    }
    bool operator<(precision_measurement val) const
    {
        return operator<((units_ == val.units()) ? val.value() : val.value_as(units_));
    }
    bool operator>=(precision_measurement val) const
    {
        return operator>=((units_ == val.units()) ? val.value() : val.value_as(units_));
    }
    bool operator<=(precision_measurement val) const
    {
        return operator<=((units_ == val.units()) ? val.value() : val.value_as(units_));
    }

    friend bool operator==(double val, const fixed_precision_measurement &v2) { return v2 == val; };
    friend bool operator!=(double val, const fixed_precision_measurement &v2) { return v2 != val; };
    friend constexpr bool operator>(double val, const fixed_precision_measurement &v2)
    {
        return val > v2.value();
    };
    friend constexpr bool operator<(double val, const fixed_precision_measurement &v2)
    {
        return val < v2.value();
    };
    friend bool operator>=(double val, const fixed_precision_measurement &v2)
    {
        return (val >= v2.value()) ? true : (v2 == val);
    };
    friend bool operator<=(double val, const fixed_precision_measurement &v2)
    {
        return (val <= v2.value()) ? true : (v2 == val);
    };

    /// friend operators for math operators
    friend constexpr double operator+(double v1, const fixed_precision_measurement &v2) { return v1 + v2.value(); }
    friend constexpr double operator-(double v1, const fixed_precision_measurement &v2) { return v1 - v2.value(); }
    friend constexpr double operator*(double v1, const fixed_precision_measurement &v2) { return v1 * v2.value(); }
    friend constexpr double operator/(double v1, const fixed_precision_measurement &v2) { return v1 / v2.value(); }

  private:
    double value_{0.0};  //!< the quantity of units measured
    const precise_unit units_;  //!< the units associated with the quantity
};

#ifndef UNITS_HEADER_ONLY

enum unit_conversion_flags : uint32_t
{
    case_insensitive = 1u,  //!< perform case insensitive matching for UCUM case insensitive matching
    single_slash =
      2u,  //!< specify that there is a single numerator and denominator only a single slash in the unit operations
    recursion_depth1 = (1u << 15),  // skip checking for SI prefixes
    // don't put anything at   16, 15 through 17 are connected to limit recursion depth
    no_recursion = (1u << 17),  // don't recurse through the string
    not_first_pass = (1u << 18),  // indicate that is not the first pass
    per_operator1 = (1u << 19),  // skip matching per
    // don't put anything at 20, 19 and 21 are connected to limit per operations
    no_per_operators = (1u << 21),  // skip matching per
    no_locality_modifiers = (1u << 22),  // skip locality modifiers
    no_of_operator = (1u << 23),  // skip dealing with "of"
    commodity_check1 = (1u << 24),  // counter for skipping commodity check vi of
    // don't put anything at 25 24 and 26 are connected
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

/** Generate a precise unit object from a string representation of it
@param unit_string the string to convert
@param match_flags see /ref unit_conversion_flags to control the matching process somewhat
@return a precise unit corresponding to the string if no match was found the unit will be an error unit
*/
precise_unit unit_from_string(std::string unit_string, uint32_t match_flags = 0);

/** Generate a unit object from a string representation of it
@details uses a unit_cast to convert the precise_unit to a unit
@param unit_string the string to convert
@param match_flags see /ref unit_conversion_flags to control the matching process somewhat
@return a unit corresponding to the string if no match was found the unit will be an error unit
*/
inline unit unit_cast_from_string(std::string unit_string, uint32_t match_flags = 0)
{
    return unit_cast(unit_from_string(unit_string, match_flags));
}

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

/// Turn off the ability to add custom units for later access
void disableUserDefinedUnits();
/// Enable the ability to add custom units for later access
void enableUserDefinedUnits();

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
    /// A namespace specifically for unit testsing some components
    namespace testing
    {
        // generate a number from a number sequence
        double testLeadingNumber(const std::string &test, size_t &index);
    }  // namespace testing
}  // namespace detail

}  // namespace units
