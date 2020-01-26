/*
Copyright (c) 2019-2020,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#pragma once

#include <cmath>
#include <cstdint> // for std::uint32_t
#include <cstring> // for std::memcpy
#include <functional> //for std::hash

namespace units {
namespace detail {
    /** Class representing base unit data
    @details the seven SI base units https://en.m.wikipedia.org/wiki/SI_base_unit
    + currency, count, and radians, 4 flags: per_unit, flag1, flag2, equation
    */
    class unit_data {
      public:
        // construct from powers
        constexpr unit_data(
            int meter,
            int kilogram,
            int second,
            int ampere,
            int kelvin,
            int mole,
            int candela,
            int currency,
            int count,
            int radians,
            unsigned int per_unit,
            unsigned int flag,
            unsigned int flag2,
            unsigned int equation) :
            meter_(meter),
            second_(second), kilogram_(kilogram), ampere_(ampere), candela_(candela),
            kelvin_(kelvin), mole_(mole), radians_(radians), currency_(currency), count_(count),
            per_unit_(per_unit), i_flag_(flag), e_flag_(flag2), equation_(equation)
        {
        }
        /** Construct with the error flag triggered*/
        explicit constexpr unit_data(std::nullptr_t) :
            meter_(-8), second_(-8), kilogram_(-4), ampere_(-4), candela_(-2), kelvin_(-4),
            mole_(-2), radians_(-4), currency_(-2), count_(-2), per_unit_(1), i_flag_(1),
            e_flag_(1), equation_(1)
        {
        }

        // perform a multiply operation by adding the powers together
        constexpr unit_data operator*(unit_data other) const
        {
            return {
                meter_ + other.meter_,
                kilogram_ + other.kilogram_,
                second_ + other.second_,
                ampere_ + other.ampere_,
                kelvin_ + other.kelvin_,
                mole_ + other.mole_,
                candela_ + other.candela_,
                currency_ + other.currency_,
                count_ + other.count_,
                radians_ + other.radians_,
                static_cast<unsigned int>(per_unit_ | other.per_unit_),
                static_cast<unsigned int>(i_flag_ ^ other.i_flag_),
                static_cast<unsigned int>(e_flag_ ^ other.e_flag_),
                static_cast<unsigned int>(equation_ | other.equation_),
            };
        }
        /// Division equivalent operator
        constexpr unit_data operator/(unit_data other) const
        {
            return {
                meter_ - other.meter_,
                kilogram_ - other.kilogram_,
                second_ - other.second_,
                ampere_ - other.ampere_,
                kelvin_ - other.kelvin_,
                mole_ - other.mole_,
                candela_ - other.candela_,
                currency_ - other.currency_,
                count_ - other.count_,
                radians_ - other.radians_,
                static_cast<unsigned int>(per_unit_ | other.per_unit_),
                static_cast<unsigned int>(i_flag_ ^ other.i_flag_),
                static_cast<unsigned int>(e_flag_ ^ other.e_flag_),
                static_cast<unsigned int>(equation_ | other.equation_),
            };
        }
        /// invert the unit
        constexpr unit_data inv() const
        {
            return {-meter_,
                    -kilogram_,
                    -second_,
                    -ampere_,
                    -kelvin_,
                    -mole_,
                    -candela_,
                    -currency_,
                    -count_,
                    -radians_,
                    per_unit_,
                    i_flag_,
                    e_flag_,
                    equation_};
        }
        /// take a unit_data to some power
        constexpr unit_data pow(int power) const
        { // the +e_flag_ on seconds is to handle a few weird operations that generate a square_root hz operation,
            // the e_flag allows some recovery of that unit and handling of that peculiar situation
            return {meter_ * power,
                    kilogram_ * power,
                    (second_ * power) + rootHertzModifier(power),
                    ampere_ * power,
                    kelvin_ * power,
                    mole_ * power,
                    candela_ * power,
                    currency_ * power,
                    count_ * power,
                    radians_ * power,
                    per_unit_,
                    (power % 2 == 0) ? 0U : i_flag_,
                    (power % 2 == 0) ? 0U : e_flag_,
                    equation_};
        }
        constexpr unit_data root(int power) const
        {
            return (hasValidRoot(power)) ? unit_data(
                                               meter_ / power,
                                               kilogram_ / power,
                                               second_ / power,
                                               ampere_ / power,
                                               kelvin_ / power,
                                               0,
                                               0,
                                               0,
                                               0,
                                               radians_ / power,
                                               per_unit_,
                                               (power % 2 == 0) ? 0U : i_flag_,
                                               (power % 2 == 0) ? 0U : e_flag_,
                                               0) :
                                           unit_data(nullptr);
        }
        // comparison operators
        constexpr bool operator==(unit_data other) const
        {
            return equivalent_non_counting(other) && mole_ == other.mole_ &&
                count_ == other.count_ && radians_ == other.radians_ &&
                per_unit_ == other.per_unit_ && i_flag_ == other.i_flag_ &&
                e_flag_ == other.e_flag_ && equation_ == other.equation_;
        }
        constexpr bool operator!=(unit_data other) const { return !(*this == other); }

        // support for specific unitConversion calls
        constexpr bool is_per_unit() const { return per_unit_ != 0; }
        constexpr bool has_i_flag() const { return (i_flag_ != 0); }
        constexpr bool has_e_flag() const { return e_flag_ != 0; }
        constexpr bool is_equation() const { return equation_ != 0; }
        /// Check if the unit bases are the same
        constexpr bool has_same_base(unit_data other) const
        {
            return equivalent_non_counting(other) && mole_ == other.mole_ &&
                count_ == other.count_ && radians_ == other.radians_;
        }
        // Check equivalence for non-counting base units
        constexpr bool equivalent_non_counting(unit_data other) const
        {
            return meter_ == other.meter_ && second_ == other.second_ &&
                kilogram_ == other.kilogram_ && ampere_ == other.ampere_ &&
                candela_ == other.candela_ && kelvin_ == other.kelvin_ &&
                currency_ == other.currency_;
        }
        // Check if the unit is empty
        constexpr bool empty() const
        {
            return meter_ == 0 && second_ == 0 && kilogram_ == 0 && ampere_ == 0 && candela_ == 0 &&
                kelvin_ == 0 && mole_ == 0 && radians_ == 0 && currency_ == 0 && count_ == 0 &&
                equation_ == 0;
        }
        /// Get the number of different base units used
        constexpr int unit_type_count() const
        {
            return ((meter_ != 0) ? 1 : 0) + ((second_ != 0) ? 1 : 0) + ((kilogram_ != 0) ? 1 : 0) +
                ((ampere_ != 0) ? 1 : 0) + ((candela_ != 0) ? 1 : 0) + ((kelvin_ != 0) ? 1 : 0) +
                ((mole_ != 0) ? 1 : 0) + ((radians_ != 0) ? 1 : 0) + ((currency_ != 0) ? 1 : 0) +
                ((count_ != 0) ? 1 : 0);
        }
        /// Get the meter power
        constexpr int meter() const { return meter_; }
        /// Get the kilogram power
        constexpr int kg() const { return kilogram_; }
        /// Get the second power
        constexpr int second() const { return second_; }
        /// Get the ampere power
        constexpr int ampere() const { return ampere_; }
        /// Get the Kelvin power
        constexpr int kelvin() const { return kelvin_; }
        /// Get the mole power
        constexpr int mole() const { return mole_; }
        /// Get the candela power
        constexpr int candela() const { return candela_; }
        /// Get the currency power
        constexpr int currency() const { return currency_; }
        /// Get the count power
        constexpr int count() const { return count_; }
        /// Get the radian power
        constexpr int radian() const { return radians_; }

        /// set all the flags to 0;
        void clear_flags() { per_unit_ = i_flag_ = e_flag_ = equation_ = 0U; }
        /// generate a new unit_data but with per_unit flag
        constexpr unit_data add_per_unit() const
        {
            return {meter_,
                    kilogram_,
                    second_,
                    ampere_,
                    kelvin_,
                    mole_,
                    candela_,
                    currency_,
                    count_,
                    radians_,
                    1U,
                    i_flag_,
                    e_flag_,
                    equation_};
        }
        /// generate a new unit_data but with i flag
        constexpr unit_data add_i_flag() const
        {
            return {meter_,
                    kilogram_,
                    second_,
                    ampere_,
                    kelvin_,
                    mole_,
                    candela_,
                    currency_,
                    count_,
                    radians_,
                    per_unit_,
                    1U,
                    e_flag_,
                    equation_};
        }
        /// generate a new unit_data but with e flag
        constexpr unit_data add_e_flag() const
        {
            return {meter_,
                    kilogram_,
                    second_,
                    ampere_,
                    kelvin_,
                    mole_,
                    candela_,
                    currency_,
                    count_,
                    radians_,
                    per_unit_,
                    i_flag_,
                    1U,
                    equation_};
        }

      private:
        constexpr unit_data() :
            meter_(0), second_(0), kilogram_(0), ampere_(0), candela_(0), kelvin_(0), mole_(0),
            radians_(0), currency_(0), count_(0), per_unit_(0), i_flag_(0), e_flag_(0), equation_(0)
        {
        }

        constexpr bool hasValidRoot(int power) const
        {
            return meter_ % power == 0 && second_ % power == 0 && kilogram_ % power == 0 &&
                ampere_ % power == 0 && candela_ == 0 && kelvin_ % power == 0 && mole_ == 0 &&
                radians_ % power == 0 && currency_ == 0 && count_ == 0 && equation_ == 0 &&
                e_flag_ == 0;
        }
        constexpr int rootHertzModifier(int power) const
        {
            return (second_ * power == 0 || ((e_flag_ & i_flag_) == 0) || power % 2 != 0) ?
                0 :
                (power >> 1) * ((second_ < 0) || (power < 0) ? 9 : -9);
        }
        // needs to be defined for the full 32 bits
        signed int meter_ : 4;
        signed int second_ : 4; // 8
        signed int kilogram_ : 3;
        signed int ampere_ : 3;
        signed int candela_ : 2; // 16
        signed int kelvin_ : 3;
        signed int mole_ : 2;
        signed int radians_ : 3; // 24
        signed int currency_ : 2;
        signed int count_ : 2; // 28
        unsigned int per_unit_ : 1;
        unsigned int i_flag_ : 1; // 30
        unsigned int e_flag_ : 1; //
        unsigned int equation_ : 1; // 32
    };
    // We want this to be exactly 4 bytes by design
    static_assert(sizeof(unit_data) == 4, "Unit data is too large");

} // namespace detail
} // namespace units

namespace std {
/// Hash function for unit_data
template<>
struct hash<units::detail::unit_data> {
    size_t operator()(const units::detail::unit_data& x) const noexcept
    {
        unsigned int val;
        std::memcpy(&val, &x, sizeof(val));
        return hash<unsigned int>()(val);
    }
};
} // namespace std

namespace units {
namespace detail {
    /// constexpr operator to generate an integer power of a number
    template<typename X>
    constexpr X power_const(X val, int power)
    {
        return (power > 0) ? val * power_const(val, power - 1) :
                             (power < 0) ? X(1.0) / (val * power_const(val, -power - 1)) : X(1.0);
    }

    /// Round the multiplier to the expected level of precision
    inline float cround(float val)
    {
#ifdef UNITS_NO_IEEE754
        int exp;
        auto f = frexpf(val, &exp);
        f = roundf(f * 1e6F);
        return ldexpf(f * 1e-6F, exp);
#else
        //what this is doing is assuming IEEE 754 floating point definition
        // taking 20 bits out of 24(roughly 10^6), adding 8 first 0b1000 to do rounding

        std::uint32_t bits;
        std::memcpy(&bits, &val, sizeof(bits));
        bits += 8UL;
        bits &= 0xFFFFFFF0UL;
        std::memcpy(&val, &bits, sizeof(bits));
        return val;
#endif
    }

    /// Round a value to the expected level of precision of a double
    inline double cround_precise(double val)
    {
#ifdef UNITS_NO_IEEE754
        int exp;
        auto f = frexp(val, &exp);
        f = round(f * 1e12);
        return ldexp(f * 1e-12, exp);
#else
        //what this is doing is assuming IEEE 754 floating point (double precision) definition
        // taking 40 bits out of 52(roughly 10^12), adding 2^11 to do rounding
        // using memcpy to abide by strict aliasing rules
        // based on godbolt.org this gets compiled to 2 instructions + the register loads
        std::uint64_t bits;
        std::memcpy(&bits, &val, sizeof(bits));
        bits += 0x800ULL;
        bits &= 0xFFFFFFFFFFFFF000ULL;
        std::memcpy(&val, &bits, sizeof(bits));
        return val;
#endif
    }

    /// Do a rounding compare for equality on floats.
    inline bool compare_round_equals(float val1, float val2)
    {
        if (std::fpclassify(val1 - val2) == FP_SUBNORMAL) {
            return true;
        }
        auto c1 = cround(val2);
        if (cround(val1) == c1) {
            return true;
        }
        // yes these are magic numbers roughly half the value specified precision of 1e-6
        // they were arrived at by running a bunch of tests to meet the design requirements
        if (cround(val1 * (1.0F + 5.4e-7F)) == c1) {
            return true;
        }
        return (cround(val1 * (1.0F - 5.1e-7F)) == c1);
    }

    /// Do a rounding compare for equality on double
    inline bool compare_round_equals_precise(double val1, double val2)
    {
        if (std::fpclassify(val1 - val2) == FP_SUBNORMAL) {
            return true;
        }
        auto c1 = cround_precise(val2);

        if (cround_precise(val1) == c1) {
            return true;
        }
        // yes these are magic numbers half the value specified precision of 1e-12
        // and yes I am purposely using the floating point equality here
        if (cround_precise(val1 * (1.0 + 5.000e-13)) == c1) {
            return true;
        }
        return (cround_precise(val1 * (1.0 - 5.000e-13)) == c1);
    }
} // namespace detail

/**Class defining a basic unit module with float32 precision on the multiplier.
@details The class consists of a unit_base along with a 32 bit floating point number
*/
class unit {
  public:
    /// Default constructor
    constexpr unit() noexcept {};
    explicit constexpr unit(detail::unit_data base_unit) : base_units_(base_unit) {}
    /// Construct unit from base unit and a multiplier
    constexpr unit(detail::unit_data base_unit, double multiplier) :
        base_units_(base_unit), multiplier_(static_cast<float>(multiplier))
    {
    }
    /// Construct unit from base unit and a multiplier
    constexpr explicit unit(detail::unit_data base_unit, float multiplier) :
        base_units_(base_unit), multiplier_(multiplier)
    {
    }
    /// Take the double and unit in either order for simplicity
    constexpr unit(double multiplier, unit other) :
        unit(other.base_units_, multiplier * other.multiplier())
    {
    }
    /// Unit multiplication
    constexpr unit operator*(unit other) const
    {
        return {base_units_ * other.base_units_, multiplier() * other.multiplier()};
    }
    /// Division operator
    constexpr unit operator/(unit other) const
    {
        return {base_units_ / other.base_units_, multiplier() / other.multiplier()};
    }
    /// Invert the unit (take 1/unit)
    constexpr unit inv() const { return {base_units_.inv(), 1.0 / multiplier()}; }
    /// take a unit to an integral power
    constexpr unit pow(int power) const
    {
        return unit{base_units_.pow(power), detail::power_const(multiplier_, power)};
    }
    /// Test for unit equivalence to within nominal numerical tolerance (6 decimal digits)
    bool operator==(unit other) const
    {
        if (base_units_ != other.base_units_) {
            return false;
        }
        if (multiplier_ == other.multiplier_) {
            return true;
        }
        return detail::compare_round_equals(multiplier_, other.multiplier_);
    }
    bool operator!=(unit other) const { return !operator==(other); }
    // Test for exact numerical equivalence
    constexpr bool is_exactly_the_same(unit other) const
    {
        return base_units_ == other.base_units_ && multiplier_ == other.multiplier_;
    }
    /// Check if the units have the same base unit (ie they measure the same thing)
    constexpr bool has_same_base(unit other) const
    {
        return base_units_.has_same_base(other.base_units_);
    }
    constexpr bool has_same_base(detail::unit_data base) const
    {
        return base_units_.has_same_base(base);
    }
    /// Check if the units have the same base unit (ie they measure the same thing)
    constexpr bool equivalent_non_counting(unit other) const
    {
        return base_units_.equivalent_non_counting(other.base_units_);
    }
    constexpr bool equivalent_non_counting(detail::unit_data base) const
    {
        return base_units_.equivalent_non_counting(base);
    }
    /// Check if the units are in some way convertible to one another
    constexpr bool is_convertible(unit other) const
    {
        return base_units_.equivalent_non_counting(other.base_units_);
    }
    /// Check if the base units are in some way directly convertible to one another
    constexpr bool is_convertible(detail::unit_data base) const
    {
        return base_units_.equivalent_non_counting(base);
    }
    /// Get the number of different base units used
    constexpr int unit_type_count() const { return base_units_.unit_type_count(); }
    /// Check if the unit is a per_unit notation
    constexpr bool is_per_unit() const { return base_units_.is_per_unit(); }
    /// Check if the unit is a per_unit notation
    constexpr bool is_equation() const { return base_units_.is_equation(); }
    /// Check if the unit has the flag triggered
    constexpr bool has_i_flag() const { return base_units_.has_i_flag(); }
    /// Check if the unit has the e flag triggered
    constexpr bool has_e_flag() const { return base_units_.has_e_flag(); }
    /// Extract the base unit Multiplier
    constexpr double multiplier() const { return static_cast<double>(multiplier_); }
    /// Extract the base unit Multiplier as a single precision float
    constexpr float multiplier_f() const { return multiplier_; }
    /// generate a rounded version of the multiplier
    float cround() const { return detail::cround(multiplier_); }
    constexpr detail::unit_data base_units() const { return base_units_; }
    /// set all the flags to 0;
    void clear_flags() { base_units_.clear_flags(); }
    /// generate a new unit but with per_unit flag
    constexpr unit add_per_unit() const { return unit{base_units_.add_per_unit(), multiplier_}; }
    /// generate a new unit but with i flag
    constexpr unit add_i_flag() const { return unit{base_units_.add_i_flag(), multiplier_}; }
    /// generate a new unit but with e flag
    constexpr unit add_e_flag() const { return unit{base_units_.add_e_flag(), multiplier_}; }

  private:
    friend class precise_unit;
    detail::unit_data base_units_{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    float multiplier_{1.0};
};

/**Class defining a basic unit module with double precision on the multiplier.
@details The class consists of a unit_base along with a 64 bit floating point number
*/
class precise_unit {
  public:
    /// Default constructor
    constexpr precise_unit() noexcept {};
    explicit constexpr precise_unit(detail::unit_data base_unit) noexcept : base_units_(base_unit)
    {
    }
    /// copy constructor from a less precise unit
    explicit constexpr precise_unit(unit other) noexcept :
        base_units_(other.base_units_), multiplier_(other.multiplier())
    {
    }
    /// Construct from base_unit and multiplier
    constexpr precise_unit(detail::unit_data base_unit, double multiplier) noexcept :
        base_units_(base_unit), multiplier_(multiplier)
    {
    }
    /// Construct from base_unit, commodity and multiplier
    constexpr precise_unit(
        detail::unit_data base_unit,
        std::uint32_t commodity,
        double multiplier) noexcept :
        base_units_(base_unit),
        commodity_(commodity), multiplier_(multiplier)
    {
    }
    /// Copy constructor with a multiplier
    constexpr precise_unit(precise_unit other, double multiplier) noexcept :
        precise_unit(other.base_units_, other.commodity_, multiplier * other.multiplier_)
    {
    }
    /// Constructor for a double and other unit
    constexpr precise_unit(unit other, double multiplier) noexcept :
        precise_unit(other.base_units(), multiplier * other.multiplier())
    {
    }
    constexpr precise_unit(double multiplier, precise_unit other) noexcept :
        precise_unit(other.base_units_, other.commodity_, multiplier * other.multiplier_)
    {
    }
    /// Build a unit from another with a multiplier and commodity
    constexpr precise_unit(
        double multiplier,
        precise_unit other,
        std::uint32_t commodity) noexcept :
        precise_unit(other.base_units_, commodity, multiplier * other.multiplier_)
    {
    }
    /// Constructor for a double and other unit
    constexpr precise_unit(double multiplier, unit other) noexcept :
        precise_unit(other.base_units(), multiplier * other.multiplier())
    {
    }
    /// take the reciprocal of a unit
    constexpr precise_unit inv() const
    {
        return {base_units_.inv(), (commodity_ == 0) ? 0 : ~commodity_, 1.0 / multiplier()};
    }
    /// Multiply with another unit
    constexpr precise_unit operator*(precise_unit other) const
    {
        return {base_units_ * other.base_units_,
                (commodity_ == 0) ?
                    other.commodity_ :
                    ((other.commodity_ == 0) ? commodity_ : commodity_ & other.commodity_),
                multiplier() * other.multiplier()};
    }
    /// Multiplication operator with a lower precision unit
    constexpr precise_unit operator*(unit other) const
    {
        return {base_units_ * other.base_units_, commodity_, multiplier() * other.multiplier()};
    }
    /// Division operator
    constexpr precise_unit operator/(precise_unit other) const
    {
        return {base_units_ / other.base_units_,
                (commodity_ == 0) ?
                    ((other.commodity_ == 0) ? 0 : ~other.commodity_) :
                    ((other.commodity_ == 0) ? commodity_ : commodity_ & (~other.commodity_)),
                multiplier() / other.multiplier()};
    }
    /// Divide by a less precise unit
    constexpr precise_unit operator/(unit other) const
    {
        return {base_units_ / other.base_units_, commodity_, multiplier() / other.multiplier()};
    }
    /// take a unit to a power
    constexpr precise_unit pow(int power) const
    {
        return {base_units_.pow(power), commodity_, detail::power_const(multiplier_, power)};
    }
    /// Overloaded equality operator
    bool operator==(precise_unit other) const
    {
        if (base_units_ != other.base_units_ || commodity_ != other.commodity_) {
            return false;
        }
        if (multiplier_ == other.multiplier_) {
            return true;
        }
        return detail::compare_round_equals_precise(multiplier_, other.multiplier_);
    }
    ///  opposite of equality
    bool operator!=(precise_unit other) const { return !operator==(other); }
    // Test for exact numerical equivalence
    constexpr bool is_exactly_the_same(precise_unit other) const
    {
        return base_units_ == other.base_units_ && commodity_ == other.commodity_ &&
            multiplier_ == other.multiplier_;
    }
    /// Check if the units have the same base unit (i.e. they measure the same thing)
    constexpr bool has_same_base(precise_unit other) const
    {
        return base_units_.has_same_base(other.base_units_);
    }
    /// Check if the units have the same base unit (i.e. they measure the same thing)
    constexpr bool has_same_base(unit other) const
    {
        return base_units_.has_same_base(other.base_units_);
    }
    /// Check if the units has the same base units as a unit_data object
    constexpr bool has_same_base(detail::unit_data base) const
    {
        return base_units_.has_same_base(base);
    }

    /// Check rounded equality with another unit
    bool operator==(unit other) const
    {
        if (base_units_ != other.base_units_) {
            return false;
        }
        if (multiplier_ == other.multiplier()) {
            return true;
        }
        return detail::compare_round_equals(static_cast<float>(multiplier_), other.multiplier_);
    }
    bool operator!=(unit other) const { return !operator==(other); }

    /// Check if the units have the same base unit (i.e. they measure the same thing)
    constexpr bool equivalent_non_counting(precise_unit other) const
    {
        return base_units_.equivalent_non_counting(other.base_units_);
    }
    constexpr bool equivalent_non_counting(unit other) const
    {
        return base_units_.equivalent_non_counting(other.base_units_);
    }
    constexpr bool equivalent_non_counting(detail::unit_data base) const
    {
        return base_units_.equivalent_non_counting(base);
    }

    /// Check if the units are in some way convertible to one another
    constexpr bool is_convertible(precise_unit other) const
    {
        return commodity_ == other.commodity_ &&
            base_units_.equivalent_non_counting(other.base_units_);
    }
    constexpr bool is_convertible(unit other) const
    {
        return base_units_.equivalent_non_counting(other.base_units_);
    }
    constexpr bool is_convertible(detail::unit_data base) const
    {
        return base_units_.equivalent_non_counting(base);
    }
    /// Check unit equality (base units equal and equivalent multipliers to specified precision
    friend bool operator==(unit val1, precise_unit val2) { return (val2 == val1); }
    /// Get the number of different base units used
    constexpr int unit_type_count() const { return base_units_.unit_type_count(); }
    /// Check if the unit is the default unit
    constexpr bool is_default() const { return base_units_.empty() && base_units_.has_e_flag(); }
    /// Check if the unit is a per unit value
    constexpr bool is_per_unit() const { return base_units_.is_per_unit(); }
    /// Check if the unit is a per_unit notation
    constexpr bool is_equation() const { return base_units_.is_equation(); }
    /// Check if the unit has the flag triggered
    constexpr bool has_i_flag() const { return base_units_.has_i_flag(); }
    /// Check if the unit has the e flag triggered
    constexpr bool has_e_flag() const { return base_units_.has_e_flag(); }
    /// Get the commodity code
    constexpr std::uint32_t commodity() const { return commodity_; }
    /// Extract the base unit Multiplier
    constexpr double multiplier() const { return multiplier_; }
    /// Extract the base unit Multiplier as a single precision float
    constexpr float multiplier_f() const { return static_cast<float>(multiplier_); }
    /// Generate a rounded value of the multiplier rounded to the defined precision
    double cround() const { return detail::cround_precise(multiplier_); }
    /// Get the base units
    constexpr detail::unit_data base_units() const { return base_units_; }
    /// set all the flags to 0;
    void clear_flags() { base_units_.clear_flags(); }
    /// generate a new unit but with per_unit flag
    constexpr precise_unit add_per_unit() const
    {
        return {base_units_.add_per_unit(), commodity_, multiplier_};
    }
    /// generate a new unit but with i flag
    constexpr precise_unit add_i_flag() const
    {
        return {base_units_.add_i_flag(), commodity_, multiplier_};
    }
    /// generate a new unit but with e flag
    constexpr precise_unit add_e_flag() const
    {
        return {base_units_.add_e_flag(), commodity_, multiplier_};
    }
    /// Set the commodity
    precise_unit& commodity(unsigned int newCommodity)
    {
        commodity_ = newCommodity;
        return *this;
    }

  private:
    detail::unit_data base_units_{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
    std::uint32_t commodity_{0}; //!< a commodity specifier
    double multiplier_{1.0}; //!< unit multiplier
};

/// Check if a unit down cast is lossless
inline constexpr bool is_unit_cast_lossless(precise_unit val)
{
    return val.multiplier() == static_cast<double>(static_cast<float>(val.multiplier()));
}
/// Downcast a precise unit to the less precise version
constexpr unit unit_cast(precise_unit val)
{
    return {val.base_units(), val.multiplier()};
}
constexpr unit unit_cast(unit val)
{
    return val;
}

/// Check if the multiplier is nan
inline bool isnan(precise_unit u)
{
    return std::isnan(u.multiplier());
}

/// Check if the multiplier is nan
inline bool isnan(unit u)
{
    return std::isnan(u.multiplier_f());
}

/** check if unit multiplier is finite
@details checks that the multiplier is finite*/
inline bool isfinite(precise_unit utest)
{
    return std::isfinite(utest.multiplier());
}

/** check if the unit multiplier is finite
@details checks that the multiplier is finite*/
inline bool isfinite(unit utest)
{
    return std::isfinite(utest.multiplier_f());
}

/** check if unit multiplier is finite
@details checks that the multiplier is infinite*/
inline bool isinf(precise_unit utest)
{
    return std::isinf(utest.multiplier());
}

/** check if unit multiplier is infinite
@details checks that the multiplier is infinite*/
inline bool isinf(unit utest)
{
    return std::isinf(utest.multiplier_f());
}

/** generate a unit which is an integer power of another
@param u the unit to raise to a specific power
@param power the integral power, can be positive or negative
@return a new unit with the appropriate value
*/
inline constexpr unit pow(unit u, int power)
{
    return u.pow(power);
}

/** generate a precise unit which is an integer power of another
@param u the precise unit to raise to a specific power
@param power the integral power, can be positive or negative
@return a new precise unit with the appropriate value
*/
inline constexpr precise_unit pow(precise_unit u, int power)
{
    return u.pow(power);
}

#ifndef UNITS_HEADER_ONLY

/// take the root of a unit to some power
unit root(unit u, int power);

precise_unit root(precise_unit u, int power);

inline unit sqrt(unit u)
{
    return root(u, 2);
}

inline precise_unit sqrt(precise_unit u)
{
    return root(u, 2);
}
#endif

// Verify that the units are the expected sizes
static_assert(sizeof(unit) == 8, "Unit type is too large");
static_assert(sizeof(precise_unit) == 16, "precise unit type is too large");

} // namespace units

/// Defining the hash functions for a unit and precise_unit so they can be used in unordered_map
namespace std {
template<>
struct hash<units::unit> {
    size_t operator()(const units::unit& x) const
    {
        return hash<units::detail::unit_data>()(x.base_units()) ^ hash<float>()(x.cround());
    }
};

template<>
struct hash<units::precise_unit> {
    size_t operator()(const units::precise_unit& x) const
    {
        return hash<units::detail::unit_data>()(x.base_units()) ^ hash<double>()(x.cround());
    }
};
} // namespace std

// std::ostream &operator<<(std::ostream &stream, units::unit u);
