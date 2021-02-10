/*
Copyright (c) 2019-2021,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#pragma once

#include "units_decl.hpp"

namespace units {
namespace detail {

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
            return ((a == -1) && (b == min)) || ((b == -1) && (a == min)) ||
                (a > max / b) || ((a < min / b));
        }
    };

    constexpr bool times_overflows(const unit_data& a, const unit_data& b)
    {
        return (
            bitfield<4>::plus_overflows(a.meter(), b.meter()) ||
            bitfield<4>::plus_overflows(a.second(), b.second()) ||
            bitfield<3>::plus_overflows(a.kg(), b.kg()) ||
            bitfield<3>::plus_overflows(a.ampere(), b.ampere()) ||
            bitfield<2>::plus_overflows(a.candela(), b.candela()) ||
            bitfield<3>::plus_overflows(a.kelvin(), b.kelvin()) ||
            bitfield<2>::plus_overflows(a.mole(), b.mole()) ||
            bitfield<3>::plus_overflows(a.radian(), b.radian()) ||
            bitfield<2>::plus_overflows(a.currency(), b.currency()) ||
            bitfield<2>::plus_overflows(a.count(), b.count()));
    }

    constexpr bool divides_overflows(const unit_data& a, const unit_data& b)
    {
        return (
            bitfield<4>::minus_overflows(a.meter(), b.meter()) ||
            bitfield<4>::minus_overflows(a.second(), b.second()) ||
            bitfield<3>::minus_overflows(a.kg(), b.kg()) ||
            bitfield<3>::minus_overflows(a.ampere(), b.ampere()) ||
            bitfield<2>::minus_overflows(a.candela(), b.candela()) ||
            bitfield<3>::minus_overflows(a.kelvin(), b.kelvin()) ||
            bitfield<2>::minus_overflows(a.mole(), b.mole()) ||
            bitfield<3>::minus_overflows(a.radian(), b.radian()) ||
            bitfield<2>::minus_overflows(a.currency(), b.currency()) ||
            bitfield<2>::minus_overflows(a.count(), b.count()));
    }

    constexpr bool inv_overflows(const unit_data& a)
    {
        return divides_overflows(
            unit_data{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, a);
    }

    constexpr bool pow_overflows(const unit_data& a, const int power)
    {
        return (
            bitfield<4>::times_overflows(a.meter(), power) ||
            bitfield<4>::times_overflows(a.second(), power) ||
            bitfield<3>::times_overflows(a.kg(), power) ||
            bitfield<3>::times_overflows(a.ampere(), power) ||
            bitfield<2>::times_overflows(a.candela(), power) ||
            bitfield<3>::times_overflows(a.kelvin(), power) ||
            bitfield<2>::times_overflows(a.mole(), power) ||
            bitfield<3>::times_overflows(a.radian(), power) ||
            bitfield<2>::times_overflows(a.currency(), power) ||
            bitfield<2>::times_overflows(a.count(), power));
    }

}  // namespace detail

template<class T1, class T2>
constexpr bool times_overflows(const T1& a, const T2& b)
{
    return times_overflows(a.base_units(), b.base_units());
}

template<class T1, class T2>
constexpr bool divides_overflows(const T1& a, const T2& b)
{
    return divides_overflows(a.base_units(), b.base_units());
}

template<class T>
constexpr bool inv_overflows(const T& a)
{
    return inv_overflows(a.base_units());
}

template<class T>
constexpr bool pow_overflows(const T& a, const int power)
{
    return pow_overflows(a.base_units(), power);
}

}  // namespace units
