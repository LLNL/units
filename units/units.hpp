/*
Copyright (c) 2019-2020,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#pragma once
#include "unit_definitions.hpp"

#include <cmath>
#include <string>
#include <type_traits>

#if __cplusplus >= 201402L || (defined(_MSC_VER) && _MSC_VER >= 1910)
#define UNITS_CPP14_CONSTEXPR constexpr
#else
#define UNITS_CPP14_CONSTEXPR const
#endif

namespace units {
/// Generate a conversion factor between two units in a constexpr function, the units will only convert if they
/// have the same base unit
template<typename UX, typename UX2>
constexpr double quick_convert(UX start, UX2 result)
{
    return quick_convert(1.0, start, result);
}

/// Generate a conversion factor between two units in a constexpr function, the units will only convert if they
/// have the same base unit
template<typename UX, typename UX2>
constexpr double quick_convert(double val, UX start, UX2 result)
{
    static_assert(
        std::is_same<UX, unit>::value || std::is_same<UX, precise_unit>::value,
        "convert argument types must be unit or precise_unit");
    static_assert(
        std::is_same<UX2, unit>::value || std::is_same<UX2, precise_unit>::value,
        "convert argument types must be unit or precise_unit");
    return (start.base_units() == result.base_units() && !start.is_equation() &&
            !result.is_equation()) ?
        val * start.multiplier() / result.multiplier() :
        constants::invalid_conversion;
}

/// Generate a conversion factor between two units
template<typename UX, typename UX2>
double convert(UX start, UX2 result)
{
    return convert(1.0, start, result);
}

/// Convert a value from one unit base to another
template<typename UX, typename UX2>
double convert(double val, UX start, UX2 result)
{
    static_assert(
        std::is_same<UX, unit>::value || std::is_same<UX, precise_unit>::value,
        "convert argument types must be unit or precise_unit");
    static_assert(
        std::is_same<UX2, unit>::value || std::is_same<UX2, precise_unit>::value,
        "convert argument types must be unit or precise_unit");
    if (start == result || is_default(start) || is_default(result)) {
        return val;
    }
    if ((is_temperature(start) || is_temperature(result)) &&
        start.has_same_base(result.base_units())) {
        return detail::convertTemperature(val, start, result);
    }
    if (start.is_equation() || result.is_equation()) {
        if (!start.base_units().equivalent_non_counting(result.base_units())) {
            return constants::invalid_conversion;
        }
        double keyval = precise::equations::convert_equnit_to_value(val, start.base_units());
        keyval = keyval * start.multiplier() / result.multiplier();
        return precise::equations::convert_value_to_equnit(keyval, result.base_units());
    }
    if (start.base_units() == result.base_units()) {
        return val * start.multiplier() / result.multiplier();
    }
    // check if both are pu since this doesn't require knowing a base unit
    if (start.is_per_unit() && result.is_per_unit()) { // we know they have different unit basis
        if (unit_cast(start) == pu ||
            unit_cast(result) ==
                pu) { // generic pu just means the units are equivalent since the the other is puXX already
            return val;
        }
        double converted_val =
            puconversion::knownConversions(val, start.base_units(), result.base_units());
        if (!std::isnan(converted_val)) {
            return converted_val;
        }
    } else if (start.is_per_unit() || result.is_per_unit()) {
        double genBase = puconversion::assumedBase(unit_cast(start), unit_cast(result));
        if (!std::isnan(genBase)) {
            return convert(val, start, result, genBase);
        }
        // other assumptions for PU base are probably dangerous so shouldn't be allowed
        return constants::invalid_conversion;
    }

    auto base_start = start.base_units();
    auto base_result = result.base_units();
    if (base_start.has_same_base(
            base_result)) { // ignore i flag and e flag,  special cases have been dealt with already, so those are just markers
        return val * start.multiplier() / result.multiplier();
    }
    // deal with some counting conversions
    if (base_start.equivalent_non_counting(base_result)) {
        double converted_val = detail::convertCountingUnits(val, start, result);
        if (!std::isnan(converted_val)) {
            return converted_val;
        }
    }
    // check for inverse units
    if (base_start.has_same_base(
            base_result
                .inv())) { // ignore flag and e flag  special cases have been dealt with already, so those are just markers
        return result.multiplier() / (val * start.multiplier());
    }
    return constants::invalid_conversion;
}

/// Convert a value from one unit base to another potentially involving pu base values
template<typename UX, typename UX2>
double convert(double val, UX start, UX2 result, double baseValue)
{
    static_assert(
        std::is_same<UX, unit>::value || std::is_same<UX, precise_unit>::value,
        "convert argument types must be unit or precise_unit");
    static_assert(
        std::is_same<UX2, unit>::value || std::is_same<UX2, precise_unit>::value,
        "convert argument types must be unit or precise_unit");
    if (start == result || is_default(start) || is_default(result)) {
        return val;
    }
    if (start.base_units() == result.base_units()) {
        return val * start.multiplier() / result.multiplier();
    }
    /// if it the per unit is equivalent no baseValue is needed so give to first function
    if (start.is_per_unit() == result.is_per_unit()) {
        return convert(val, start, result);
    }

    if (start.has_same_base(result.base_units()) || pu == unit_cast(start) ||
        pu == unit_cast(result)) {
        if (start.is_per_unit()) {
            val = val * baseValue;
        }
        val = val * start.multiplier() / result.multiplier();
        if (result.is_per_unit()) {
            val /= baseValue;
        }
        return val;
    }
    // this would require two base values which power might be assumed but assuming a base voltage is dangerous
    return constants::invalid_conversion;
}

/// Convert a value from one unit base to another involving power system units
/// the basePower and base voltage are used as the basis values
template<typename UX, typename UX2>
double convert(double val, UX start, UX2 result, double basePower, double baseVoltage)
{
    static_assert(
        std::is_same<UX, unit>::value || std::is_same<UX, precise_unit>::value,
        "convert argument types must be unit or precise_unit");
    static_assert(
        std::is_same<UX2, unit>::value || std::is_same<UX2, precise_unit>::value,
        "convert argument types must be unit or precise_unit");
    if (is_default(start) || is_default(result)) {
        return val;
    }
    /// if it isn't per unit or both are per unit give it to the other function since bases aren't needed
    if (start.is_per_unit() == result.is_per_unit()) {
        auto base = puconversion::generate_base(start.base_units(), basePower, baseVoltage);
        if (std::isnan(base)) { // no known base conversions so this means we are converting bases
            if (start.is_per_unit() && start == result) {
                return val * basePower / baseVoltage;
            }
            if (start.is_per_unit() && start.has_same_base(result.base_units())) {
                return val * basePower * start.multiplier() / baseVoltage / result.multiplier();
            }
        }
        return convert(val, start, result);
    }
    /// now we get into some power system specific conversions
    // deal with situations of same base in different quantities
    if (start.has_same_base(
            result
                .base_units())) { // We have the same base now we just need to figure out which base to use
        auto base = puconversion::generate_base(result.base_units(), basePower, baseVoltage);
        if (start.is_per_unit()) {
            val = val * base;
        }
        val = val * start.multiplier() / result.multiplier();
        if (result.is_per_unit()) {
            val /= base;
        }
        return val;
    }
    if (result.is_per_unit()) {
        auto base = puconversion::generate_base(start.base_units(), basePower, baseVoltage);
        auto puVal = val / base;
        if (pu == unit_cast(result)) { // if result is generic pu
            return puVal * start.multiplier();
        }
        // let the first function deal with both as PU
        return convert(puVal, start * pu, result) / result.multiplier();
    }
    // start must be per unit
    auto base = puconversion::generate_base(result.base_units(), basePower, baseVoltage);
    base *= start.multiplier();
    if (pu == unit_cast(start)) { // if start is generic pu
        return val * base;
    }
    return convert(val, start, result * pu) * base;
}

/// Class defining a measurement (value+unit)
class measurement {
  public:
    /// Default constructor
    constexpr measurement() = default;
    /// construct from a value and unit
    constexpr measurement(double val, unit base) : value_(val), units_(base) {}
    /// Get the base value with no units
    constexpr double value() const { return value_; }

    constexpr measurement operator*(measurement other) const
    {
        return measurement(value_ * other.value_, units_ * other.units_);
    }
    constexpr measurement operator*(unit other) const { return {value_, units_ * other}; }
    constexpr measurement operator*(double val) const { return measurement(value_ * val, units_); }
    constexpr measurement operator/(measurement other) const
    {
        return measurement(value_ / other.value_, units_ / other.units_);
    }
    constexpr measurement operator/(unit other) const { return {value_, units_ / other}; }

    constexpr measurement operator/(double val) const { return measurement(value_ / val, units_); }
    measurement operator%(measurement other) const
    {
        return measurement(fmod(value_, other.value_as(units_)), units_);
    }
    measurement operator%(double val) const { return measurement(fmod(value_, val), units_); }
    measurement operator+(measurement other) const
    {
        return measurement(value_ + other.value_as(units_), units_);
    }
    measurement operator-(measurement other) const
    {
        return measurement(value_ - other.value_as(units_), units_);
    }

    // double multiplier
    friend constexpr inline measurement operator*(double val, measurement meas)
    {
        return meas * val;
    }
    // divide measurement into a double
    friend constexpr inline measurement operator/(double val, measurement meas)
    {
        return {val / meas.value_, meas.units_.inv()};
    }

    friend constexpr measurement pow(const measurement& meas, int power)
    {
        return measurement{detail::power_const(meas.value_, power), meas.units_.pow(power)};
    }
    /// Convert a unit to have a new base
    measurement convert_to(unit newUnits) const
    {
        return measurement(units::convert(value_, units_, newUnits), newUnits);
    }

    /// Convert a unit into its base units
    constexpr measurement convert_to_base() const
    {
        return measurement(value_ * units_.multiplier(), unit(units_.base_units()));
    }

    /// extract the current units from the measurement
    constexpr unit units() const { return units_; }

    // convert the measurement to a single unit
    constexpr unit as_unit() const { return unit(value_, units_); }
    /// Equality operator
    bool operator==(measurement other) const
    {
        auto val = other.value_as(units_);
        return (value_ == val) ?
            true :
            detail::compare_round_equals(static_cast<float>(value_), static_cast<float>(val));
    }
    bool operator>(measurement other) const { return value_ > other.value_as(units_); }
    bool operator<(measurement other) const { return value_ < other.value_as(units_); }
    bool operator>=(measurement other) const
    {
        auto val = other.value_as(units_);
        return (value_ >= val) ?
            true :
            detail::compare_round_equals(static_cast<float>(value_), static_cast<float>(val));
    }
    bool operator<=(measurement other) const
    {
        auto val = other.value_as(units_);
        return (value_ <= val) ?
            true :
            detail::compare_round_equals(static_cast<float>(value_), static_cast<float>(val));
    }
    /// Not equal operator
    bool operator!=(measurement other) const { return !operator==(other); }
    /// Get the numerical value as a particular unit type
    double value_as(unit units) const
    {
        return (units_ == units) ? value_ : units::convert(value_, units_, units);
    }

  private:
    double value_{0.0}; //!< the numerical quantity of the unit
    unit units_; //!< the actual unit represented
};

/// The design requirement is for this to fit in the space of 2 doubles
static_assert(sizeof(measurement) <= 16, "Measurement class is too large");

constexpr inline measurement operator*(double val, unit unit_base)
{
    return {val, unit_base};
}
constexpr inline measurement operator*(unit unit_base, double val)
{
    return {val, unit_base};
}

constexpr inline measurement operator/(double val, unit unit_base)
{
    return {val, unit_base.inv()};
}
constexpr inline measurement operator/(unit unit_base, double val)
{
    return {1.0 / val, unit_base};
}

/// Class defining a measurement (value+unit) with a fixed unit type
class fixed_measurement {
  public:
    /// construct from a value and unit
    constexpr fixed_measurement(double val, unit base) : value_(val), units_(base) {}
    /// construct from a regular measurement
    explicit constexpr fixed_measurement(measurement val) noexcept :
        value_(val.value()), units_(val.units())
    {
    }
    // define copy constructor
    constexpr fixed_measurement(const fixed_measurement& val) noexcept :
        value_(val.value()), units_(val.units())
    {
    }
    /// assignment operator
    fixed_measurement& operator=(measurement val)
    {
        value_ = (units_ == val.units()) ? val.value() : val.value_as(units_);
        return *this;
    }
    /// assignment operator treat it the same as a measurement
    fixed_measurement& operator=(fixed_measurement val)
    {
        value_ = (units_ == val.units()) ? val.value() : val.value_as(units_);
        return *this;
    }
    /// Assignment from number,  allow direct numerical assignment since the units are fixes and known at
    /// construction time
    fixed_measurement& operator=(double val)
    {
        value_ = val;
        return *this;
    }
    /// direct conversion operator
    operator measurement() { return measurement(value_, units_); }

    /// Get the base value with no units
    constexpr double value() const { return value_; }

    constexpr measurement operator*(measurement other) const
    {
        return measurement(value_ * other.value(), units_ * other.units());
    }
    constexpr measurement operator*(unit other) const
    {
        return measurement(value_, units_ * other);
    }
    constexpr fixed_measurement operator*(double val) const
    {
        return fixed_measurement(value_ * val, units_);
    }
    constexpr measurement operator/(measurement other) const
    {
        return measurement(value_ / other.value(), units_ / other.units());
    }
    constexpr measurement operator/(unit other) const
    {
        return measurement(value_, units_ / other);
    }
    constexpr fixed_measurement operator/(double val) const
    {
        return fixed_measurement(value_ / val, units_);
    }

    fixed_measurement operator+(measurement other) const
    {
        return fixed_measurement(value_ + other.value_as(units_), units_);
    }
    fixed_measurement operator-(measurement other) const
    {
        return fixed_measurement(value_ - other.value_as(units_), units_);
    }

    constexpr fixed_measurement operator+(double val) const
    {
        return fixed_measurement(value_ + val, units_);
    }
    constexpr fixed_measurement operator-(double val) const
    {
        return fixed_measurement(value_ - val, units_);
    }

    /// take the measurement to some power
    constexpr friend fixed_measurement pow(const fixed_measurement& meas, int power)
    {
        return fixed_measurement{detail::power_const(meas.value_, power), meas.units_.pow(power)};
    }
    /// Convert a unit to have a new base
    fixed_measurement convert_to(unit newUnits) const
    {
        return fixed_measurement(units::convert(value_, units_, newUnits), newUnits);
    }
    /// Get the underlying units value
    constexpr unit units() const { return units_; }

    /// convert the measurement to a single unit
    constexpr unit as_unit() const { return unit(value_, units_); }

    /// Get the numerical value as a particular unit type
    double value_as(unit units) const
    {
        return (units_ == units) ? value_ :
                                   units::convert(static_cast<double>(value_), units_, units);
    }

    fixed_measurement& operator+=(double val)
    {
        value_ += val;
        return *this;
    }
    fixed_measurement& operator-=(double val)
    {
        value_ -= val;
        return *this;
    }
    fixed_measurement& operator*=(double val)
    {
        value_ *= val;
        return *this;
    }
    fixed_measurement& operator/=(double val)
    {
        value_ /= val;
        return *this;
    }
    /// comparison operators
    bool operator==(double val) const
    {
        return (value_ == val) ?
            true :
            detail::compare_round_equals(static_cast<float>(value_), static_cast<float>(val));
    };
    bool operator!=(double val) const { return !operator==(val); };
    constexpr bool operator>(double val) const { return value_ > val; };
    constexpr bool operator<(double val) const { return value_ < val; };
    bool operator>=(double val) const { return (value_ > val) ? true : operator==(val); };
    bool operator<=(double val) const { return value_ < val ? true : operator==(val); };

    bool operator==(measurement val) const
    {
        return operator==((units_ == val.units()) ? val.value() : val.value_as(units_));
    };
    bool operator!=(measurement val) const
    {
        return operator!=((units_ == val.units()) ? val.value() : val.value_as(units_));
    };
    bool operator>(measurement val) const
    {
        return operator>((units_ == val.units()) ? val.value() : val.value_as(units_));
    };
    bool operator<(measurement val) const
    {
        return operator<((units_ == val.units()) ? val.value() : val.value_as(units_));
    };
    bool operator>=(measurement val) const
    {
        return operator>=((units_ == val.units()) ? val.value() : val.value_as(units_));
    };
    bool operator<=(measurement val) const
    {
        return operator<=((units_ == val.units()) ? val.value() : val.value_as(units_));
    };

    friend bool operator==(double val, const fixed_measurement& v2) { return v2 == val; };
    friend bool operator!=(double val, const fixed_measurement& v2) { return v2 != val; };
    friend constexpr bool operator>(double val, const fixed_measurement& v2)
    {
        return val > v2.value();
    };
    friend constexpr bool operator<(double val, const fixed_measurement& v2)
    {
        return val < v2.value();
    };
    friend bool operator>=(double val, const fixed_measurement& v2)
    {
        return (val > v2.value()) ? true : (v2 == val);
    };
    friend bool operator<=(double val, const fixed_measurement& v2)
    {
        return (val < v2.value()) ? true : (v2 == val);
    };

    // + and - are allowed for fixed_measurement since the units are known
    /// friend operators for math operators
    friend constexpr inline fixed_measurement operator+(double v1, const fixed_measurement& v2)
    {
        return {v1 + v2.value(), v2.units()};
    }
    friend constexpr inline fixed_measurement operator-(double v1, const fixed_measurement& v2)
    {
        return {v1 - v2.value(), v2.units()};
    }
    friend constexpr inline fixed_measurement operator*(double v1, const fixed_measurement& v2)
    {
        return {v1 * v2.value(), v2.units()};
    }
    friend constexpr inline fixed_measurement operator/(double v1, const fixed_measurement& v2)
    {
        return {v1 / v2.value(), v2.units().inv()};
    }

  private:
    double value_{0.0}; //!< the unit value
    const unit units_; //!< a fixed unit of measurement
};

/// Design requirement this must fit in space of 2 doubles
static_assert(sizeof(fixed_measurement) <= 16, "fixed measurement is too large");

/** Class defining a measurement with tolerance (value+tolerance+unit) with a fixed unit type
the tolerance is assumed to be some statistical reference either standard deviation or 95% confidence bounds or something like that

uncertainty propagation equations from
http://www.geol.lsu.edu/jlorenzo/geophysics/uncertainties/Uncertaintiespart2.html
*/
class uncertain_measurement {
  public:
    constexpr uncertain_measurement() = default;
    /// construct from a single precision value, uncertainty, and unit
    constexpr uncertain_measurement(float val, float uncertainty, unit base) noexcept :
        value_(val), uncertainty_(uncertainty), units_(base)
    {
    }
    /// construct from a single precision value, and unit assume uncertainty is 0
    explicit constexpr uncertain_measurement(float val, unit base) noexcept :
        value_(val), units_(base)
    {
    }

    /// construct from a double precision value, and unit assume uncertainty is 0
    explicit constexpr uncertain_measurement(double val, unit base) noexcept :
        value_(static_cast<float>(val)), units_(base)
    {
    }
    /// construct from a regular measurement and uncertainty value
    explicit constexpr uncertain_measurement(measurement val, float uncertainty) noexcept :
        value_(static_cast<float>(val.value())), uncertainty_(uncertainty), units_(val.units())
    {
    }
    /// construct from a regular measurement and an uncertainty measurement
    explicit uncertain_measurement(measurement val, measurement uncertainty) noexcept :
        value_(static_cast<float>(val.value())),
        uncertainty_(static_cast<float>(uncertainty.value_as(val.units()))), units_(val.units())
    {
    }
    /// construct from a double precision value, uncertainty, and unit
    explicit constexpr uncertain_measurement(double val, double uncertainty, unit base) noexcept :
        value_(static_cast<float>(val)), uncertainty_(static_cast<float>(uncertainty)), units_(base)
    {
    }
    /// construct from a regular measurement
    explicit constexpr uncertain_measurement(measurement val, double uncertainty) noexcept :
        value_(static_cast<float>(val.value())), uncertainty_(static_cast<float>(uncertainty)),
        units_(val.units())
    {
    }

    /// Get the base value with no units
    constexpr double value() const { return static_cast<double>(value_); }
    /// Get the uncertainty with no units
    constexpr double uncertainty() const { return static_cast<double>(uncertainty_); }
    /// Get the base value with no units as a single precision float
    constexpr float value_f() const { return value_; }
    /// Get the uncertainty with no units as a single precision float
    constexpr float uncertainty_f() const { return uncertainty_; }

    /// Set the uncertainty
    uncertain_measurement& uncertainty(float newUncertainty)
    {
        uncertainty_ = newUncertainty;
        return *this;
    }
    /// Set the uncertainty
    uncertain_measurement& uncertainty(double newUncertainty)
    {
        uncertainty_ = static_cast<float>(newUncertainty);
        return *this;
    }
    /// Set the uncertainty
    uncertain_measurement& uncertainty(const measurement& newUncertainty)
    {
        uncertainty_ = static_cast<float>(newUncertainty.value_as(units_));
        return *this;
    }

    /// Get the fractional uncertainty with no units
    constexpr double fractional_uncertainty() const
    {
        return uncertainty() / static_cast<double>((value_ >= 0.0F) ? value_ : -value_);
    }

    /// Get the uncertainty as a separate measurement
    constexpr measurement uncertainty_measurement() const
    {
        return measurement(uncertainty(), units_);
    }

    /// Cast operator to a measurement
    constexpr operator measurement() const { return measurement(value(), units_); }
    /** Compute a product and calculate the new uncertainties using the root sum of squares(rss) method*/
    uncertain_measurement operator*(uncertain_measurement other) const
    {
        float tval1 = uncertainty_ / value_;
        float tval2 = other.uncertainty_ / other.value_;
        float ntol = std::sqrt(tval1 * tval1 + tval2 * tval2);
        float nval = value_ * other.value_;
        return uncertain_measurement(nval, nval * ntol, units_ * other.units());
    }

    /** Perform a multiplication with uncertain measurements using the simple method for uncertainty propagation*/
    UNITS_CPP14_CONSTEXPR uncertain_measurement simple_product(uncertain_measurement other) const
    {
        float ntol = uncertainty_ / value_ + other.uncertainty_ / other.value_;
        float nval = value_ * other.value_;
        return uncertain_measurement(nval, nval * ntol, units_ * other.units());
    }
    /** Multiply with another measurement 
	equivalent to uncertain_measurement multiplication with 0 uncertainty*/
    constexpr uncertain_measurement operator*(measurement other) const
    {
        return uncertain_measurement(
            static_cast<float>(value() * other.value()),
            static_cast<float>(other.value() * uncertainty()),
            units_ * other.units());
    }
    constexpr uncertain_measurement operator*(unit other) const
    {
        return uncertain_measurement(value_, uncertainty_, units_ * other);
    }
    constexpr uncertain_measurement operator*(float val) const
    {
        return uncertain_measurement(value_ * val, uncertainty_ * val, units_);
    }

    constexpr uncertain_measurement operator*(double val) const
    {
        return uncertain_measurement(value() * val, uncertainty() * val, units_);
    }
    /** compute a unit division and calculate the new uncertainties using the root sum of squares(rss) method*/
    uncertain_measurement operator/(uncertain_measurement other) const
    {
        float tval1 = uncertainty_ / value_;
        float tval2 = other.uncertainty_ / other.value_;
        float ntol = std::sqrt(tval1 * tval1 + tval2 * tval2);
        float nval = value_ / other.value_;
        return uncertain_measurement(nval, nval * ntol, units_ / other.units());
    }

    /** division operator propagate uncertainty using simple method allowing constexpr in C++14
	*/
    UNITS_CPP14_CONSTEXPR uncertain_measurement simple_divide(uncertain_measurement other) const
    {
        float ntol = uncertainty_ / value_ + other.uncertainty_ / other.value_;
        float nval = value_ / other.value_;
        return uncertain_measurement(nval, nval * ntol, units_ / other.units());
    }

    constexpr uncertain_measurement operator/(measurement other) const
    {
        return uncertain_measurement(
            static_cast<float>(value() / other.value()),
            static_cast<float>(uncertainty() / other.value()),
            units_ / other.units());
    }
    constexpr uncertain_measurement operator/(unit other) const
    {
        return uncertain_measurement(value_, uncertainty_, units_ / other);
    }
    constexpr uncertain_measurement operator/(float val) const
    {
        return uncertain_measurement(value_ / val, uncertainty_ / val, units_);
    }
    constexpr uncertain_measurement operator/(double val) const
    {
        return uncertain_measurement(value() / val, uncertainty() / val, units_);
    }

    /** compute a unit addition and calculate the new uncertainties using the root sum of squares(rss) method*/
    uncertain_measurement operator+(uncertain_measurement other) const
    {
        float cval = static_cast<float>(convert(other.units_, units_));
        float ntol = std::sqrt(
            uncertainty_ * uncertainty_ + cval * cval * other.uncertainty_ * other.uncertainty_);
        return uncertain_measurement(value_ + cval * other.value_, ntol, units_);
    }

    uncertain_measurement simple_add(uncertain_measurement other) const
    {
        float cval = static_cast<float>(convert(other.units_, units_));
        float ntol = uncertainty_ + other.uncertainty_ * cval;
        return uncertain_measurement(value_ + cval * other.value_, ntol, units_);
    }

    /** compute a unit subtraction and calculate the new uncertainties using the root sum of squares(rss) method*/
    uncertain_measurement operator-(uncertain_measurement other) const
    {
        float cval = static_cast<float>(convert(other.units_, units_));
        float ntol = std::sqrt(
            uncertainty_ * uncertainty_ + cval * cval * other.uncertainty_ * other.uncertainty_);
        return uncertain_measurement(value_ - cval * other.value_, ntol, units_);
    }

    /** compute a unit subtraction and calculate the new uncertainties using the simple uncertainty summation method*/
    uncertain_measurement simple_subtract(uncertain_measurement other) const
    {
        float cval = static_cast<float>(convert(other.units_, units_));
        float ntol = uncertainty_ + other.uncertainty_ * cval;
        return uncertain_measurement(value_ - cval * other.value_, ntol, units_);
    }

    uncertain_measurement operator+(measurement other) const
    {
        float cval = static_cast<float>(other.value_as(units_));
        return uncertain_measurement(value_ + cval, uncertainty_, units_);
    }

    uncertain_measurement operator-(measurement other) const
    {
        float cval = static_cast<float>(other.value_as(units_));
        return uncertain_measurement(value_ - cval, uncertainty_, units_);
    }

    /// take the measurement to some power
    friend UNITS_CPP14_CONSTEXPR uncertain_measurement
        pow(const uncertain_measurement& meas, int power)
    {
        auto new_value = detail::power_const(meas.value_, power);
        auto new_tol =
            ((power >= 0) ? power : -power) * new_value * meas.uncertainty_ / meas.value_;
        return uncertain_measurement{new_value, new_tol, meas.units_.pow(power)};
    }

    /// Convert a unit to have a new base
    uncertain_measurement convert_to(unit newUnits) const
    {
        float cval = static_cast<float>(convert(units_, newUnits));
        return uncertain_measurement(cval * value_, uncertainty_ * cval, newUnits);
    }
    /// Get the underlying units value
    constexpr unit units() const { return units_; }

    /// Get the numerical value as a particular unit type
    double value_as(unit units) const
    {
        return (units_ == units) ? static_cast<double>(value_) :
                                   units::convert(static_cast<double>(value_), units_, units);
    }
    /// Get the numerical value of the uncertainty as a particular unit
    double uncertainty_as(unit units) const
    {
        return (units_ == units) ? static_cast<double>(uncertainty_) :
                                   units::convert(static_cast<double>(uncertainty_), units_, units);
    }

    /// comparison operators
    /** the equality operator reverts to the measurement comparison if the uncertainty is 0 otherwise it returns true if the 
	measurement that is being compared has a value within the 1 uncertainty of the value*/
    bool operator==(const measurement& other) const
    {
        auto val = static_cast<float>(other.value_as(units_));
        if (uncertainty_ == 0.0F) {
            return (value_ == val) ? true : detail::compare_round_equals(value_, val);
        } else {
            return (val >= (value_ - uncertainty_) && val <= (value_ + uncertainty_));
        }
    }
    bool operator>(const measurement& other) const
    {
        return static_cast<double>(value_) > other.value_as(units_);
    }
    bool operator<(const measurement& other) const
    {
        return static_cast<double>(value_) < other.value_as(units_);
    }
    bool operator>=(const measurement& other) const
    {
        auto val = other.value_as(units_);
        return (value() >= val) ? true : operator==(measurement(val, units_));
    }
    bool operator<=(const measurement& other) const
    {
        auto val = other.value_as(units_);
        return (value() <= val) ? true : operator==(measurement(val, units_));
    }
    /// Not equal operator
    bool operator!=(const measurement& other) const { return !operator==(other); }

    bool operator==(const uncertain_measurement& other) const
    {
        auto zval = simple_subtract(other);
        return (zval == measurement(0.0, units_));
    }
    bool operator>(const uncertain_measurement& other) const
    {
        return static_cast<double>(value_) > other.value_as(units_);
    }
    bool operator<(const uncertain_measurement& other) const
    {
        return static_cast<double>(value_) < other.value_as(units_);
    }
    bool operator>=(const uncertain_measurement& other) const
    {
        auto zval = simple_subtract(other);
        return (zval.value_ >= 0.0F) ? true : (zval == measurement(0.0, units_));
    }
    bool operator<=(const uncertain_measurement& other) const
    {
        auto zval = simple_subtract(other);
        return (zval.value_ <= 0.0F) ? true : (zval == measurement(0.0, units_));
    }
    /// Not equal operator
    bool operator!=(const uncertain_measurement& other) const { return !operator==(other); }

    /// operator for measurement =
    friend bool operator==(const measurement& other, const uncertain_measurement& v2)
    {
        return v2 == other;
    };
    friend bool operator!=(const measurement& other, const uncertain_measurement& v2)
    {
        return v2 != other;
    };
    friend constexpr bool operator>(const measurement& other, const uncertain_measurement& v2)
    {
        return other.value() > v2.value();
    };
    friend constexpr bool operator<(const measurement& other, const uncertain_measurement& v2)
    {
        return other.value() < v2.value();
    };
    friend bool operator>=(const measurement& other, const uncertain_measurement& v2)
    {
        return (other > v2) ? true : (v2 == other);
    };
    friend bool operator<=(const measurement& other, const uncertain_measurement& v2)
    {
        return (other < v2) ? true : (v2 == other);
    };

    // + and - are allowed for fixed_measurement since the units are known
    /// friend operators for math operators
    friend inline uncertain_measurement
        operator+(const measurement& v1, const uncertain_measurement& v2)
    {
        double cval = convert(v2.units_, v1.units());
        double ntol = v2.uncertainty() * cval;
        return uncertain_measurement(v1.value() + cval * v2.value(), ntol, v1.units());
    }
    friend inline uncertain_measurement
        operator-(const measurement& v1, const uncertain_measurement& v2)
    {
        double cval = convert(v2.units_, v1.units());
        double ntol = v2.uncertainty() * cval;
        return uncertain_measurement(v1.value() - cval * v2.value(), ntol, v1.units());
    }
    friend constexpr inline uncertain_measurement
        operator*(const measurement& v1, const uncertain_measurement& v2)
    {
        return v2.operator*(v1);
    }

    friend UNITS_CPP14_CONSTEXPR inline uncertain_measurement
        operator/(const measurement& v1, const uncertain_measurement& v2)
    {
        double ntol = v2.uncertainty() / v2.value();
        double nval = v1.value() / v2.value();
        return uncertain_measurement(nval, nval * ntol, v1.units() / v2.units_);
    }

    friend constexpr inline uncertain_measurement
        operator*(double v1, const uncertain_measurement& v2)
    {
        return v2.operator*(v1);
    }

    friend constexpr inline uncertain_measurement
        operator*(float v1, const uncertain_measurement& v2)
    {
        return v2.operator*(v1);
    }

    friend UNITS_CPP14_CONSTEXPR inline uncertain_measurement
        operator/(double v1, const uncertain_measurement& v2)
    {
        double ntol = v2.uncertainty() / v2.value();
        double nval = v1 / v2.value();
        return uncertain_measurement(nval, nval * ntol, v2.units_.inv());
    }

    friend UNITS_CPP14_CONSTEXPR inline uncertain_measurement
        operator/(float v1, const uncertain_measurement& v2)
    {
        float ntol = v2.uncertainty_ / v2.value_;
        float nval = v1 / v2.value_;
        return uncertain_measurement(nval, nval * ntol, v2.units_.inv());
    }

    friend UNITS_CPP14_CONSTEXPR inline uncertain_measurement
        operator/(int v1, const uncertain_measurement& v2)
    {
        float ntol = v2.uncertainty_ / v2.value_;
        float nval = static_cast<float>(v1) / v2.value_;
        return uncertain_measurement(nval, nval * ntol, v2.units_.inv());
    }

  private:
    float value_{0.0F}; //!< the unit value
    float uncertainty_{0.0F};
    unit units_; //!< a fixed unit of measurement
};

/// Design requirement this must fit in space of 2 doubles
static_assert(sizeof(uncertain_measurement) <= 16, "uncertain measurement is too large");

/// Class using precise units and double precision
class precise_measurement {
  public:
    /// Default constructor
    constexpr precise_measurement() = default;
    constexpr precise_measurement(double val, precise_unit base) : value_(val), units_(base) {}
    /// implicit conversion from a lower precision measurement
    constexpr precise_measurement(const measurement& other) :
        value_(other.value()), units_(other.units())
    {
    }
    /// getter for the numerical component of the measurement
    constexpr double value() const { return value_; }
    /// getter for the unit component of a measurement
    constexpr precise_unit units() const { return units_; }

    // convert the measurement to a single unit
    constexpr precise_unit as_unit() const { return {value_, units_}; }

    constexpr precise_measurement operator*(const precise_measurement& other) const
    {
        return {value_ * other.value_, units_ * other.units_};
    }
    constexpr precise_measurement operator*(precise_unit other) const
    {
        return {value_, units_ * other};
    }
    constexpr precise_measurement operator*(double val) const { return {value_ * val, units_}; }
    constexpr precise_measurement operator/(const precise_measurement& other) const
    {
        return {value_ / other.value_, units_ / other.units_};
    }
    constexpr precise_measurement operator/(precise_unit other) const
    {
        return {value_, units_ / other};
    }
    constexpr precise_measurement operator/(double val) const { return {value_ / val, units_}; }

    precise_measurement operator+(const precise_measurement& other) const
    {
        return {value_ + other.value_as(units_), units_};
    }
    precise_measurement operator-(const precise_measurement& other) const
    {
        return {value_ - other.value_as(units_), units_};
    }

    /// take the measurement to some power
    constexpr friend precise_measurement pow(const precise_measurement& meas, int power)
    {
        return precise_measurement{detail::power_const(meas.value_, power), meas.units_.pow(power)};
    }

    /// Convert a unit to have a new base
    precise_measurement convert_to(precise_unit newUnits) const
    {
        return {units::convert(value_, units_, newUnits), newUnits};
    }

    /// Convert a unit into its base units
    constexpr precise_measurement convert_to_base() const
    {
        return {value_ * units_.multiplier(), precise_unit(units_.base_units())};
    }

    /// Equality operator
    bool operator==(const precise_measurement& other) const
    {
        return valueEqualityCheck(
            (units_ == other.units()) ? other.value() : other.value_as(units_));
    }
    /// Not equal operator
    bool operator!=(const precise_measurement& other) const
    {
        return !valueEqualityCheck(
            (units_ == other.units()) ? other.value() : other.value_as(units_));
    }

    bool operator>(const precise_measurement& other) const
    {
        return value_ > other.value_as(units_);
    }
    bool operator<(const precise_measurement& other) const
    {
        return value_ < other.value_as(units_);
    }
    bool operator>=(const precise_measurement& other) const
    {
        double val = other.value_as(units_);
        return (value_ > val) ? true : valueEqualityCheck(val);
    }
    bool operator<=(const precise_measurement& other) const
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
    friend constexpr inline precise_measurement
        operator*(double val, const precise_measurement& meas)
    {
        return meas * val;
    }
    // divide measurement into a double
    friend constexpr inline precise_measurement
        operator/(double val, const precise_measurement& meas)
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

constexpr inline precise_measurement operator*(double val, precise_unit unit_base)
{
    return {val, unit_base};
}
constexpr inline precise_measurement operator*(precise_unit unit_base, double val)
{
    return {val, unit_base};
}

constexpr inline precise_measurement operator/(double val, precise_unit unit_base)
{
    return {val, unit_base.inv()};
}
constexpr inline precise_measurement operator/(precise_unit unit_base, double val)
{
    return {1.0 / val, unit_base};
}

/// Design requirement this must fit in space of 3 doubles
static_assert(sizeof(precise_measurement) <= 24, "precise measurement is too large");

/// Class using precise units and double precision
class fixed_precise_measurement {
  public:
    constexpr fixed_precise_measurement(double val, precise_unit base) : value_(val), units_(base)
    {
    }

    explicit constexpr fixed_precise_measurement(const precise_measurement& val) :
        value_(val.value()), units_(val.units())
    {
    }

    constexpr fixed_precise_measurement(const fixed_precise_measurement& val) :
        value_(val.value()), units_(val.units())
    {
    }

    fixed_precise_measurement& operator=(const precise_measurement& val)
    {
        value_ = (units_ == val.units()) ? val.value() : val.value_as(units_);
        return *this;
    }

    /// Assignment from double,  allow direct numerical assignment since the units are fixes and known at
    /// construction time
    fixed_precise_measurement& operator=(double val)
    {
        value_ = val;
        return *this;
    }
    // direct conversion operator
    operator precise_measurement() { return precise_measurement(value_, units_); }

    constexpr double value() const { return value_; }
    constexpr precise_unit units() const { return units_; }

    // convert the measurement to a single unit
    constexpr precise_unit as_unit() const { return {value_, units_}; }

    /// Get the numerical value as a particular unit type
    double value_as(precise_unit units) const
    {
        return (units_ == units) ? value_ : units::convert(value_, units_, units);
    }

    constexpr precise_measurement operator*(const precise_measurement& other) const
    {
        return {value_ * other.value(), units_ * other.units()};
    }
    constexpr precise_measurement operator*(precise_unit other) const
    {
        return {value_, units_ * other};
    }
    constexpr fixed_precise_measurement operator*(double val) const
    {
        return {value_ * val, units_};
    }
    constexpr precise_measurement operator/(const precise_measurement& other) const
    {
        return {value_ / other.value(), units_ / other.units()};
    }
    constexpr precise_measurement operator/(precise_unit other) const
    {
        return {value_, units_ / other};
    }
    constexpr fixed_precise_measurement operator/(double val) const
    {
        return {value_ / val, units_};
    }

    fixed_precise_measurement operator+(const precise_measurement& other) const
    {
        return {value_ + other.value_as(units_), units_};
    }
    fixed_precise_measurement operator-(const precise_measurement& other) const
    {
        return {value_ - other.value_as(units_), units_};
    }
    /// Add a double assuming the same units
    constexpr fixed_precise_measurement operator+(double val) const
    {
        return {value_ + val, units_};
    }
    /// Subtract a double assuming the same units
    constexpr fixed_precise_measurement operator-(double val) const
    {
        return {value_ - val, units_};
    }

    fixed_precise_measurement& operator+=(double val)
    {
        value_ += val;
        return *this;
    }
    fixed_precise_measurement& operator-=(double val)
    {
        value_ -= val;
        return *this;
    }
    fixed_precise_measurement& operator*=(double val)
    {
        value_ *= val;
        return *this;
    }
    fixed_precise_measurement& operator/=(double val)
    {
        value_ /= val;
        return *this;
    }

    /// take the measurement to some power
    constexpr friend fixed_precise_measurement pow(const fixed_precise_measurement& meas, int power)
    {
        return fixed_precise_measurement{detail::power_const(meas.value_, power),
                                         meas.units_.pow(power)};
    }

    /// Convert a unit to have a new base
    precise_measurement convert_to(precise_unit newUnits) const
    {
        return {units::convert(value_, units_, newUnits), newUnits};
    }

    /// Convert a unit into its base units
    constexpr precise_measurement convert_to_base() const
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
    bool operator==(const precise_measurement& val) const
    {
        return operator==((units_ == val.units()) ? val.value() : val.value_as(units_));
    }
    /// Not equal operator
    bool operator!=(const precise_measurement& val) const
    {
        return operator!=((units_ == val.units()) ? val.value() : val.value_as(units_));
    }

    bool operator>(const precise_measurement& val) const
    {
        return operator>((units_ == val.units()) ? val.value() : val.value_as(units_));
    }
    bool operator<(const precise_measurement& val) const
    {
        return operator<((units_ == val.units()) ? val.value() : val.value_as(units_));
    }
    bool operator>=(const precise_measurement& val) const
    {
        return operator>=((units_ == val.units()) ? val.value() : val.value_as(units_));
    }
    bool operator<=(const precise_measurement& val) const
    {
        return operator<=((units_ == val.units()) ? val.value() : val.value_as(units_));
    }

    friend bool operator==(double val, const fixed_precise_measurement& v2) { return v2 == val; };
    friend bool operator!=(double val, const fixed_precise_measurement& v2) { return v2 != val; };
    friend constexpr bool operator>(double val, const fixed_precise_measurement& v2)
    {
        return val > v2.value();
    };
    friend constexpr bool operator<(double val, const fixed_precise_measurement& v2)
    {
        return val < v2.value();
    };
    friend bool operator>=(double val, const fixed_precise_measurement& v2)
    {
        return (val >= v2.value()) ? true : (v2 == val);
    };
    friend bool operator<=(double val, const fixed_precise_measurement& v2)
    {
        return (val <= v2.value()) ? true : (v2 == val);
    };

    /// friend operators for math operators
    friend constexpr double operator+(double v1, const fixed_precise_measurement& v2)
    {
        return v1 + v2.value();
    }
    friend constexpr double operator-(double v1, const fixed_precise_measurement& v2)
    {
        return v1 - v2.value();
    }
    friend constexpr double operator*(double v1, const fixed_precise_measurement& v2)
    {
        return v1 * v2.value();
    }
    friend constexpr double operator/(double v1, const fixed_precise_measurement& v2)
    {
        return v1 / v2.value();
    }

  private:
    double value_{0.0}; //!< the quantity of units measured
    const precise_unit units_; //!< the units associated with the quantity
};

/// perform a down-conversion from a precise measurement to a measurement
constexpr measurement measurement_cast(const precise_measurement& measure)
{
    return measurement(measure.value(), unit_cast(measure.units()));
}

/// perform a down-conversion from a precise measurement to a measurement
constexpr fixed_measurement measurement_cast(const fixed_precise_measurement& measure)
{
    return fixed_measurement(measure.value(), unit_cast(measure.units()));
}

/// define measurement cast on measurement so it works in templates
constexpr measurement measurement_cast(measurement measure)
{
    return measure;
}

#ifndef UNITS_HEADER_ONLY

measurement root(const measurement& meas, int power);

fixed_measurement root(const fixed_measurement& fm, int power);

uncertain_measurement root(const uncertain_measurement& um, int power);

precise_measurement root(const precise_measurement& pm, int power);

fixed_precise_measurement root(const fixed_precise_measurement& fpm, int power);

inline measurement sqrt(const measurement& meas)
{
    return root(meas, 2);
}

inline precise_measurement sqrt(const precise_measurement& meas)
{
    return root(meas, 2);
}

inline fixed_measurement sqrt(const fixed_measurement& meas)
{
    return root(meas, 2);
}

inline uncertain_measurement sqrt(const uncertain_measurement& meas)
{
    return root(meas, 2);
}

inline fixed_precise_measurement sqrt(const fixed_precise_measurement& meas)
{
    return root(meas, 2);
}

/** The unit conversion flag are some modifiers for the string conversion operations,
some are used internally some are meant for external use, though all are possible to use externally
*/
enum unit_conversion_flags : std::uint32_t {
    case_insensitive = 1u, //!< perform case insensitive matching for UCUM case insensitive matching
    single_slash =
        2u, //!< specify that there is a single numerator and denominator only a single slash in the unit operations
    recursion_depth1 = (1u << 15), // skip checking for SI prefixes
    // don't put anything at   16, 15 through 17 are connected to limit recursion depth
    no_recursion = (1u << 17), // don't recurse through the string
    not_first_pass = (1u << 18), // indicate that is not the first pass
    per_operator1 = (1u << 19), // skip matching per
    // don't put anything at 20, 19 and 21 are connected to limit per operations
    no_per_operators = (1u << 21), // skip matching per
    no_locality_modifiers = (1u << 22), // skip locality modifiers
    no_of_operator = (1u << 23), // skip dealing with "of"
    commodity_check1 = (1u << 24), // counter for skipping commodity check vi of
    // don't put anything at 25 24 and 26 are connected
    no_commodities = (1u << 26), // skip commodity checks
    partition_check1 = (1u << 27), // skip checking for SI prefixes
    // don't put anything at 28, 27 and 28 are connected to limit partition depth
    skip_partition_check = (1u << 29), // skip checking for SI prefixes
    skip_si_prefix_check = (1u << 30), // skip checking for SI prefixes
    skip_code_replacements = (1u << 31), // don't do some code and sequence replacements
};
/// Generate a string representation of the unit
std::string to_string(precise_unit units, std::uint32_t match_flags = 0);

/// Generate a string representation of the unit
inline std::string to_string(unit units, std::uint32_t match_flags = 0)
{
    // For naming, precision doesn't matter
    return to_string(precise_unit(units), match_flags);
}

/** Generate a precise unit object from a string representation of it
@param unit_string the string to convert
@param match_flags see /ref unit_conversion_flags to control the matching process somewhat
@return a precise unit corresponding to the string if no match was found the unit will be an error unit
*/
precise_unit unit_from_string(std::string unit_string, std::uint32_t match_flags = 0);

/** Generate a unit object from a string representation of it
@details uses a unit_cast to convert the precise_unit to a unit
@param unit_string the string to convert
@param match_flags see /ref unit_conversion_flags to control the matching process somewhat
@return a unit corresponding to the string if no match was found the unit will be an error unit
*/
inline unit unit_cast_from_string(std::string unit_string, std::uint32_t match_flags = 0)
{
    return unit_cast(unit_from_string(std::move(unit_string), match_flags));
}

/** Generate a unit object from the string definition of a type of measurement
@param unit_type  string representing the type of measurement
@return a precise unit corresponding to the SI unit for the measurement specified in unit_type
*/
precise_unit default_unit(std::string unit_type);

/** Generate a precise_measurement from a string
@param measurement_string the string to convert
@param match_flags see / ref unit_conversion_flags to control the matching process somewhat
  @ return a precise unit corresponding to the string if no match was found the unit will be an error unit
    */
precise_measurement
    measurement_from_string(std::string measurement_string, std::uint32_t match_flags = 0);

/** Generate a measurement from a string
@param measurement_string the string to convert
@param match_flags see / ref unit_conversion_flags to control the matching process somewhat
  @ return a precise unit corresponding to the string if no match was found the unit will be an error unit
	*/
inline measurement
    measurement_cast_from_string(std::string measurement_string, std::uint32_t match_flags = 0)
{
    return measurement_cast(measurement_from_string(std::move(measurement_string), match_flags));
}

/** Generate an uncertain_measurement from a string
the string should contain some symbol of the form +/- in one of the various forms what comes after will determine the uncertainty
for example "3.0+/-0.4 m" or "2.5 m +/- 2 cm"
@param measurement_string the string to convert
@param match_flags see / ref unit_conversion_flags to control the matching process somewhat
  @ return a precise unit corresponding to the string if no match was found the unit will be an error unit
	*/
uncertain_measurement uncertain_measurement_from_string(
    std::string measurement_string,
    std::uint32_t match_flags = 0);

/// Convert a precise measurement to a string (with some extra decimal digits displayed
std::string to_string(precise_measurement measure, std::uint32_t match_flags = 0);
/// Convert a measurement to a string
std::string to_string(measurement measure, std::uint32_t match_flags = 0);

/// Add a custom unit to be included in any string processing
void addUserDefinedUnit(std::string name, precise_unit un);
/// Clear all user defined units from memory
void clearUserDefinedUnits();

/// Turn off the ability to add custom units for later access
void disableUserDefinedUnits();
/// Enable the ability to add custom units for later access
void enableUserDefinedUnits();

/// get the code to use for a particular commodity
std::uint32_t getCommodity(std::string comm);

/// get the code to use for a particular commodity
std::string getCommodityName(std::uint32_t commodity);

/// add a custom commodity for later retrieval
void addCustomCommodity(std::string comm, std::uint32_t code);
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

#endif // UNITS_HEADER_ONLY
/// Physical constants in use with associated units
namespace constants {
    /// Standard gravity
    constexpr precise_measurement g0(9.80665, precise::m / precise::s / precise::s);
    /// Gravitational Constant
    constexpr precise_measurement
        G(6.6740831e-11,
          precise_unit(detail::unit_data(3, -1, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0)));
    /// Speed of light
    constexpr precise_measurement c{299792458.0, precise::m / precise::s};
    /// Elementary Charge (2019 redefinition)
    constexpr precise_measurement e(1.602176634e-19, precise::C);
    ///  hyperfine structure transition frequency of the cesium-133 atom
    constexpr precise_measurement fCs(9192631770.0, precise::Hz);
    /// Planck constant (2019 redefinition)
    constexpr precise_measurement h{6.62607015e-34, precise::J* precise::second};
    /// Boltzman constant (2019 redefinition)
    constexpr precise_measurement k{1.380649e-23, precise::J / precise::K};
    /// Avogadros constant (2019 redefinition)
    constexpr precise_measurement Na{6.02214076e23, precise::one / precise::mol};
    /// Luminous efficiency
    constexpr precise_measurement Kcd{683.0, precise::lm / precise::W};
    /// Permittivity of free space
    constexpr precise_measurement eps0{8.854187817e-12, precise::F / precise::m};
    /// Permeability of free space
    constexpr precise_measurement mu0{12.566370614e-7, precise::N / (precise::A * precise::A)};
    /// Gas Constant
    constexpr precise_measurement R{8.314459848, precise::J / (precise::mol * precise::K)};
    /// Stephan Boltzmann constant
    constexpr precise_measurement s{
        5.67036713e-8,
        precise_unit(detail::unit_data(0, 1, -3, 0, -4, 0, 0, 0, 0, 0, 0, 0, 0, 0))};
    /// hubble constant AKA 69.3 km/s/Mpc
    constexpr precise_measurement H0{2.25e-18, precise::Hz};
    /// Mass of an electron
    constexpr precise_measurement me{9.1093835611e-31, precise::kg};
    /// Mass of a proton
    constexpr precise_measurement mp{1.67262189821e-27, precise::kg};
    /// Planck units
    namespace planck {
        constexpr precise_measurement length{1.61622938e-35, precise::m};
        constexpr precise_measurement mass{2.17647051e-8, precise::kg};
        constexpr precise_measurement time{5.3911613e-44, precise::s};
        constexpr precise_measurement charge{1.87554595641e-18, precise::C};
        constexpr precise_measurement temperature{1.41680833e32, precise::K};
    } // namespace planck
    /// measurements related to an electron or atomic measurements
    namespace atomic { // https://www.bipm.org/en/publications/si-brochure/table7.html
        constexpr precise_measurement length{0.5291772109217e-10, precise::m};
        constexpr precise_measurement mass = me;
        constexpr precise_measurement time{2.41888432650212e-17, precise::s};
        constexpr precise_measurement charge = e;
        constexpr precise_measurement energy{4.3597443419e-18, precise::J};
        constexpr precise_measurement action{1.05457172647e-34, precise::J* precise::s};
    } // namespace atomic
} // namespace constants

namespace detail {
    /// A namespace specifically for unit testing some components
    namespace testing {
        // generate a number from a number sequence
        double testLeadingNumber(const std::string& test, size_t& index);
        // generate a number from words
        double testNumericalWords(const std::string& test, size_t& index);
    } // namespace testing
} // namespace detail

} // namespace units
