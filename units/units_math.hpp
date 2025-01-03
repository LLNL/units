/*
Copyright (c) 2019-2025,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#pragma once

#include "units.hpp"

#include <cmath>
#include <type_traits>

/** @file defines some math operations on the units types*/

namespace UNITS_NAMESPACE {

/** declare the type traits*/

/// type_trait for detecting any measurement type
template<typename T>
struct is_measurement : std::false_type {};

template<>
struct is_measurement<measurement> : std::true_type {};
template<>
struct is_measurement<precise_measurement> : std::true_type {};
template<>
struct is_measurement<fixed_measurement> : std::true_type {};

template<>
struct is_measurement<fixed_precise_measurement> : std::true_type {};

template<>
struct is_measurement<uncertain_measurement> : std::true_type {};

/// type_trait for detecting a precise measurement type
template<typename T>
struct is_precise_measurement : std::false_type {};

template<>
struct is_precise_measurement<fixed_precise_measurement> : std::true_type {};
template<>
struct is_precise_measurement<precise_measurement> : std::true_type {};

/// type_trait for detecting a unit type
template<typename T>
struct is_unit : std::false_type {};

template<>
struct is_unit<unit> : std::true_type {};
template<>
struct is_unit<precise_unit> : std::true_type {};

// cmath overloads

template<
    typename X,
    typename = typename std::enable_if<is_measurement<X>::value>::type>
X cbrt(const X& measure)
{
    return root(measure, 3);
}

template<
    typename X,
    typename = typename std::enable_if<is_measurement<X>::value>::type>
X floor(const X& measure)
{
    return X((std::floor)(measure.value()), measure.units());
}

template<
    typename X,
    typename = typename std::enable_if<is_measurement<X>::value>::type>
X ceil(const X& measure)
{
    return X((std::ceil)(measure.value()), measure.units());
}

template<
    typename X,
    typename = typename std::enable_if<is_measurement<X>::value>::type>
X round(const X& measure)
{
    return X((std::round)(measure.value()), measure.units());
}

template<
    typename X,
    typename = typename std::enable_if<is_measurement<X>::value>::type>
X trunc(const X& measure)
{
    return X((std::trunc)(measure.value()), measure.units());
}

template<
    typename X,
    typename Y,
    typename = typename std::enable_if<
        is_measurement<X>::value || is_measurement<Y>::value>::type>
auto hypot(const X& measure1, const Y& measure2)
    -> decltype(measure1 + measure2)
{
    return sqrt(pow(measure1, 2) + pow(measure2, 2));
}

template<
    typename X,
    typename Y,
    typename Z,
    typename = typename std::enable_if<
        is_measurement<X>::value || is_measurement<Y>::value ||
        is_measurement<Z>::value>::type>
auto hypot(const X& measure1, const Y& measure2, const Z& measure3)
    -> decltype(measure1 + measure2 + measure3)
{
    return sqrt(pow(measure1, 2) + pow(measure2, 2) + pow(measure3, 2));
}

template<
    typename X,
    typename Y,
    typename = typename std::enable_if<
        is_measurement<X>::value || is_measurement<Y>::value>::type>
auto fmod(const X& measure1, const Y& measure2)
    -> decltype(measure1 - trunc(measure1 / measure2) * measure2)
{
    return measure1 - trunc(measure1 / measure2) * measure2;
}

template<
    typename X,
    typename = typename std::enable_if<is_measurement<X>::value>::type>
double sin(const X& measure)
{
    return (std::sin)(measure.value_as(precise::rad));
}

template<
    typename X,
    typename = typename std::enable_if<is_measurement<X>::value>::type>
double cos(const X& measure)
{
    return (std::cos)(measure.value_as(precise::rad));
}

template<
    typename X,
    typename = typename std::enable_if<is_measurement<X>::value>::type>
double tan(const X& measure)
{
    return (std::tan)(measure.value_as(precise::rad));
}

template<
    typename X,
    typename Y,
    typename = typename std::enable_if<
        is_measurement<X>::value || is_measurement<Y>::value>::type>
auto multiplies(const X& measure1, const Y& measure2)
    -> decltype(measure1 * measure2)
{
    auto res = measure1 * measure2;
    if (measure1.units().is_per_unit() != measure2.units().is_per_unit() &&
        measure1.units().has_same_base(measure2.units())) {
        using mtype = decltype(measure1 * measure2);
        using utype = typename std::conditional<
            is_precise_measurement<mtype>::value,
            precise_unit,
            unit>::type;
        utype nu = (measure1.units().is_per_unit()) ? utype(measure2.units()) :
                                                      utype(measure1.units());
        double nval = (measure1.units().is_per_unit()) ?
            measure1.units().multiplier() :
            measure2.units().multiplier();
        nu.set_flags(0, nu.has_i_flag(), nu.has_e_flag());
        res = mtype(res.value() * nval, nu);
    }
    return res;
}

template<
    typename X,
    typename Y,
    typename = typename std::enable_if<
        is_measurement<X>::value || is_measurement<Y>::value>::type>
auto divides(const X& measure1, const Y& measure2)
    -> decltype(measure1 / measure2)
{
    auto res = measure1 / measure2;
    if (res.units().has_same_base(one) &&
        !measure1.units().has_same_base(one)) {
        auto nu = measure2.units();
        nu = nu.add_per_unit();
        res = decltype(measure1 / measure2)(
            res.value() * res.units().multiplier() / nu.multiplier(), nu);
    }
    return res;
}

}  // namespace UNITS_NAMESPACE
