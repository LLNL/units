/*
Copyright (c) 2019-2021,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#pragma once

#include "units_decl.hpp"

namespace UNITS_NAMESPACE {
namespace detail {

    /** Helper to check whether operations on bitfield members of given width
     * would overflow */
    template<int Bits>
    struct bitfield {
        static constexpr int32_t min = -(static_cast<int32_t>(1U << Bits) / 2);
        static constexpr int32_t max = static_cast<int32_t>(1U << Bits) / 2 - 1;
        template<class T>
        static constexpr bool plus_overflows(const T& a, const T& b)
        {
            return ((b > 0) && (a > max - b)) || ((b < 0) && (a < min - b));
        }
        template<class T>
        static constexpr bool minus_overflows(const T& a, const T& b)
        {
            return ((b < 0) && (a > max + b)) || ((b > 0) && (a < min + b));
        }
        template<class T>
        static constexpr bool times_overflows(const T& a, const T& b)
        {
            return (a * b < min) || (a * b > max);
        }
    };

    /** Return a base of unit_data via index, forwarding to underlying method.
     * The index `Base` should be from enum unit_data:base or an equivalent
     * integer. */
    template<int Base>
    constexpr int get_base(const unit_data& u);
    template<>
    constexpr int get_base<unit_data::base::Meter>(const unit_data& u)
    {
        return u.meter();
    }
    template<>
    constexpr int get_base<unit_data::base::Second>(const unit_data& u)
    {
        return u.second();
    }
    template<>
    constexpr int get_base<unit_data::base::Kilogram>(const unit_data& u)
    {
        return u.kg();
    }
    template<>
    constexpr int get_base<unit_data::base::Ampere>(const unit_data& u)
    {
        return u.ampere();
    }
    template<>
    constexpr int get_base<unit_data::base::Candela>(const unit_data& u)
    {
        return u.candela();
    }
    template<>
    constexpr int get_base<unit_data::base::Kelvin>(const unit_data& u)
    {
        return u.kelvin();
    }
    template<>
    constexpr int get_base<unit_data::base::Mole>(const unit_data& u)
    {
        return u.mole();
    }
    template<>
    constexpr int get_base<unit_data::base::Radians>(const unit_data& u)
    {
        return u.radian();
    }
    template<>
    constexpr int get_base<unit_data::base::Currency>(const unit_data& u)
    {
        return u.currency();
    }
    template<>
    constexpr int get_base<unit_data::base::Count>(const unit_data& u)
    {
        return u.count();
    }

    /** Helper to check whether operations on members of unit_data identified by
     * `Base` ID would overflow. Base should be from enum unit_data::base or
     * equivalent integer. */
    template<int Base>
    struct base_access {
        template<class T>
        static constexpr bool plus_overflows(const T& a, const T& b)
        {
            return bitfield<unit_data::bits[Base]>::plus_overflows(
                get_base<Base>(a), get_base<Base>(b));
        }
        template<class T>
        static constexpr bool minus_overflows(const T& a, const T& b)
        {
            return bitfield<unit_data::bits[Base]>::minus_overflows(
                get_base<Base>(a), get_base<Base>(b));
        }
        template<class T>
        static constexpr bool times_overflows(const T& a, const int power)
        {
            return bitfield<unit_data::bits[Base]>::times_overflows(
                get_base<Base>(a), power);
        }
    };

    // Explicitly spelling out all bases in an error-prone way can be avoided,
    // e.g., in C++17 by defining `get_base<N>()` for `unit_data`, then use
    // `std::index_sequence` and fold in a helper function, something like:
    //     return (bitfield<Is>::plus_overflows(get_base<Is>(a),
    //             get_base<Is>(b)) || ...)
    // Getting this to work in C++11 requires to much code here, so for now we
    // do it the manual way.
    constexpr bool times_overflows(const unit_data& a, const unit_data& b)
    {
        return (
            base_access<0>::plus_overflows(a, b) ||
            base_access<1>::plus_overflows(a, b) ||
            base_access<2>::plus_overflows(a, b) ||
            base_access<3>::plus_overflows(a, b) ||
            base_access<4>::plus_overflows(a, b) ||
            base_access<5>::plus_overflows(a, b) ||
            base_access<6>::plus_overflows(a, b) ||
            base_access<7>::plus_overflows(a, b) ||
            base_access<8>::plus_overflows(a, b) ||
            base_access<9>::plus_overflows(a, b));
    }

    constexpr bool divides_overflows(const unit_data& a, const unit_data& b)
    {
        return (
            base_access<0>::minus_overflows(a, b) ||
            base_access<1>::minus_overflows(a, b) ||
            base_access<2>::minus_overflows(a, b) ||
            base_access<3>::minus_overflows(a, b) ||
            base_access<4>::minus_overflows(a, b) ||
            base_access<5>::minus_overflows(a, b) ||
            base_access<6>::minus_overflows(a, b) ||
            base_access<7>::minus_overflows(a, b) ||
            base_access<8>::minus_overflows(a, b) ||
            base_access<9>::minus_overflows(a, b));
    }

    constexpr bool inv_overflows(const unit_data& a)
    {
        return divides_overflows(
            unit_data{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, a);
    }

    constexpr bool pow_overflows(const unit_data& a, const int power)
    {
        return (
            base_access<0>::times_overflows(a, power) ||
            base_access<1>::times_overflows(a, power) ||
            base_access<2>::times_overflows(a, power) ||
            base_access<3>::times_overflows(a, power) ||
            base_access<4>::times_overflows(a, power) ||
            base_access<5>::times_overflows(a, power) ||
            base_access<6>::times_overflows(a, power) ||
            base_access<7>::times_overflows(a, power) ||
            base_access<8>::times_overflows(a, power) ||
            base_access<9>::times_overflows(a, power));
    }

}  // namespace detail

/** Return true if multiplying `a` and `b` would lead to base unit exponent
 * over- or underflow. */
template<class T1, class T2>
constexpr bool times_overflows(const T1& a, const T2& b)
{
    return times_overflows(a.base_units(), b.base_units());
}

/** Return true if dividing `a` and `b` would lead to base unit exponent over-
 * or underflow. */
template<class T1, class T2>
constexpr bool divides_overflows(const T1& a, const T2& b)
{
    return divides_overflows(a.base_units(), b.base_units());
}

/** Return true if inverting `a` would lead to base unit exponent over- or
 * underflow. */
template<class T>
constexpr bool inv_overflows(const T& a)
{
    return inv_overflows(a.base_units());
}

/** Return true if `a**power` would lead to base unit exponent over- or
 * underflow. */
template<class T>
constexpr bool pow_overflows(const T& a, const int power)
{
    return pow_overflows(a.base_units(), power);
}

}  // namespace UNITS_NAMESPACE
