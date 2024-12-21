/*
Copyright (c) 2019-2024,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <nanobind/stl/string.h>

#include "units/units.hpp"
namespace nb = nanobind;

using namespace nb::literals;

NB_MODULE(units_llnl_ext, mod)
{
    mod.doc() =
        "A minimal Python extension for the llnl/units C++ library.\n"
        "The main use case is to represent any unit in simple way, enable conversions between them \n"
        "and with string representations and allow math operations on those units and measurements\n"
        "all classes are immutable.  More details can be found at github.com/LLNL/units";

    nb::class_<units::precise_unit>(
        mod,
        "unit",
        "a unit is a basic building block for unit conversion this library operates mainly on strings and can interpret those strings as units or measurements")
        .def(nb::init<>())
        .def(
            "__init__",
            [](units::precise_unit* type, const char* arg0) {
                new (type) units::precise_unit(
                    units::unit_from_string(std::string(arg0)));
            })
        .def(
            "__init__",
            [](units::precise_unit* type,
               const char* arg0,
               const char* commodity) {
                new (type) units::precise_unit{
                    units::unit_from_string(std::string(arg0)),
                    units::getCommodity(std::string(commodity))};
            })
        .def("multiplier", &units::precise_unit::multiplier)
        .def(
            "commodity",
            [](const units::precise_unit& type1) {
                return units::getCommodityName(type1.commodity());
            })
        .def(
            "set_commodity",
            [](units::precise_unit* unit, const char* commodity) {
                return units::precise_unit(
                    unit->multiplier(),
                    unit->base_units(),
                    units::getCommodity(std::string(commodity)));
            })
        .def(
            "set_multiplier",
            [](units::precise_unit* unit, double mult) {
                return units::precise_unit(
                    mult, unit->base_units(), unit->commodity());
            })
        .def("inv", &units::precise_unit::inv)
        .def(nb::self * nb::self)
        .def(nb::self / nb::self)
        .def(float() * nb::self)
        .def(nb::self * float())
        .def(float() / nb::self)
        .def(nb::self / float())
        .def(nb::self == nb::self)
        .def(nb::self != nb::self)
        .def(
            "__pow__",
            [](const units::precise_unit& a, int pow) { return a.pow(pow); },
            nb::is_operator())
        .def(
            "is_exactly_the_same",
            [](const units::precise_unit& type1,
               const units::precise_unit& type2) {
                return type1.is_exactly_the_same(type2);
            })
        .def(
            "has_same_base",
            [](const units::precise_unit& type1,
               const units::precise_unit& type2) {
                return type1.has_same_base(type2);
            },
            "check if two units have the same base units as each other")
        .def(
            "equivalent_non_counting",
            [](const units::precise_unit& type1,
               const units::precise_unit& type2) {
                return type1.equivalent_non_counting(type2);
            },
            "check if two units are equivalent in the non-counting units portion of the units (moles|radians|count)")
        .def(
            "is_convertible_to",
            [](const units::precise_unit& type1,
               const units::precise_unit& type2) {
                return type1.is_convertible(type2);
            },
            "check if two units are convertible to each other")
        .def(
            "is_convertible_to",
            [](const units::precise_unit& type1, const char* desired_units) {
                return type1.is_convertible(
                    units::unit_from_string(std::string(desired_units)));
            },
            "check if the unit can be converted to the desired unit")
        .def(
            "convert",
            [](units::precise_unit* unit,
               double value,
               const units::precise_unit& convert_to_units) {
                return units::convert(value, *unit, convert_to_units);
            },
            "value"_a,
            "unit_out"_a,
            "value represented by one unit in terms of another")
        .def(
            "convert",
            [](units::precise_unit* unit,
               double value,
               const char* convert_to_units) {
                return units::convert(
                    value,
                    *unit,
                    units::unit_from_string(std::string(convert_to_units)));
            },
            "value"_a,
            "unit_out"_a,
            "value represented by one unit in terms of another")
        .def(
            "is_default",
            &units::precise_unit::is_default,
            "check whether a unit is the default unit definition")
        .def("is_per_unit", &units::precise_unit::is_per_unit)
        .def(
            "has_i_flag",
            &units::precise_unit::has_i_flag,
            "check whether a unit is setting the i(imaginary) flag")
        .def(
            "has_e_flag",
            &units::precise_unit::has_e_flag,
            "check whether a unit is setting the e(extra) flag")
        .def(
            "is_valid",
            [](const units::precise_unit& type) {
                return units::is_valid(type);
            })
        .def(
            "is_normal",
            [](const units::precise_unit& type) {
                return units::isnormal(type);
            })
        .def(
            "is_error",
            [](const units::precise_unit& type) {
                return units::is_error(type);
            },
            "return true if the unit has the error flags set or is infinite")
        .def(
            "is_finite",
            [](const units::precise_unit& type) {
                return units::isfinite(type);
            })
        .def(
            "isinf",
            [](const units::precise_unit& type) { return units::isinf(type); })
        .def(
            "root",
            [](const units::precise_unit& type, int root) {
                return units::root(type, root);
            })
        .def(
            "sqrt",
            [](const units::precise_unit& type) {
                return units::root(type, 2);
            })
        .def(
            "__repr__",
            [](const units::precise_unit& type) {
                return units::to_string(type);
            })
        .def("to_string", [](const units::precise_unit& type) {
            return units::to_string(type);
        });

    nb::class_<units::precise_measurement>(
        mod,
        "measurement",
        "a measurement object consists of a measurement(value) and a unit and allows conversion to other units")
        .def(nb::init<>())
        .def(
            "__init__",
            [](units::precise_measurement* measurement, const char* arg0) {
                new (measurement) units::precise_measurement(
                    units::measurement_from_string(std::string(arg0)));
            })
        .def(
            "__init__",
            [](units::precise_measurement* measurement,
               double value,
               const char* arg0) {
                new (measurement) units::precise_measurement(
                    value, units::unit_from_string(std::string(arg0)));
            })
        .def(
            "__init__",
            [](units::precise_measurement* measurement,
               double value,
               const units::precise_unit& unit) {
                new (measurement) units::precise_measurement(value, unit);
            })
        .def("value", &units::precise_measurement::value)
        .def(
            "set_value",
            [](units::precise_measurement* measurement, double value) {
                return units::precise_measurement(value, measurement->units());
            })
        .def("units", &units::precise_measurement::units)
        .def(
            "set_units",
            [](units::precise_measurement* measurement,
               const units::precise_unit& unit) {
                return units::precise_measurement(measurement->value(), unit);
            })
        .def(
            "set_units",
            [](units::precise_measurement* measurement, const char* unit) {
                return units::precise_measurement(
                    measurement->value(),
                    units::unit_from_string(std::string(unit)));
            })
        .def(
            "value_as",
            [](const units::precise_measurement& measurement,
               const units::precise_unit& unit) {
                return measurement.value_as(unit);
            })
        .def(
            "value_as",
            [](const units::precise_measurement& measurement,
               const char* units) {
                return measurement.value_as(
                    units::unit_from_string(std::string(units)));
            })
        .def(
            "convert_to",
            [](const units::precise_measurement& measurement,
               const units::precise_unit& unit) {
                return measurement.convert_to(unit);
            })
        .def(
            "convert_to",
            [](const units::precise_measurement& measurement,
               const char* units) {
                return measurement.convert_to(
                    units::unit_from_string(std::string(units)));
            })
        .def(
            "convert_to_base",
            &units::precise_measurement::convert_to_base,
            "convert a measurement to a measurement using the base si units")
        .def("as_unit", &units::precise_measurement::as_unit)
        .def(nb::self * nb::self)
        .def(nb::self / nb::self)
        .def(nb::self + nb::self)
        .def(nb::self - nb::self)
        .def(float() * nb::self)
        .def(nb::self * float())
        .def(float() / nb::self)
        .def(nb::self / float())
        .def(nb::self == nb::self)
        .def(nb::self != nb::self)
        .def(nb::self > nb::self)
        .def(nb::self >= nb::self)
        .def(nb::self <= nb::self)
        .def(nb::self < nb::self)
        .def(
            "__pow__",
            [](const units::precise_measurement& measurement, int power) {
                return pow(measurement, power);
            },
            nb::is_operator())
        .def(
            "is_valid",
            [](const units::precise_measurement& measurement) {
                return units::is_valid(measurement);
            })
        .def(
            "is_normal",
            [](const units::precise_measurement& measurement) {
                return units::isnormal(measurement);
            })
        .def(
            "root",
            [](const units::precise_measurement& measurement, int root) {
                return units::root(measurement, root);
            })
        .def(
            "sqrt",
            [](const units::precise_measurement& measurement) {
                return units::root(measurement, 2);
            })
        .def(
            "__repr__",
            [](const units::precise_measurement& measurement) {
                return units::to_string(measurement);
            })
        .def("to_string", [](const units::precise_measurement& measurement) {
            return units::to_string(measurement);
        });

    mod.def(
        "convert",
        [](double val,
           const units::precise_unit& unitIn,
           const units::precise_unit& unitOut) {
            return units::convert(val, unitIn, unitOut);
        },
        "value"_a,
        "unit_in"_a,
        "unit_out"_a,
        "value represented by one unit in terms of another");
    mod.def(
        "convert",
        [](double val, const char* unitIn, const char* unitOut) {
            return units::convert(
                val,
                units::unit_from_string(std::string(unitIn)),
                units::unit_from_string(std::string(unitOut)));
        },
        "value"_a,
        "unit_in"_a,
        "unit_out"_a,
        "generate a value represented by one unit in terms of another");
    mod.def(
        "convert_pu",
        [](double val,
           const units::precise_unit& unitIn,
           const units::precise_unit& unitOut,
           double base_value) {
            return units::convert(val, unitIn, unitOut, base_value);
        },
        "value"_a,
        "unit_in"_a,
        "unit_out"_a,
        "base"_a,
        "generate a value represented by one unit in terms of another if one of the units is in per-unit, the base_value is used in part of the conversion");
    mod.def(
        "convert_pu",
        [](double val,
           const char* unitIn,
           const char* unitOut,
           double base_value) {
            return units::convert(
                val,
                units::unit_from_string(std::string(unitIn)),
                units::unit_from_string(std::string(unitOut)),
                base_value);
        },
        "value"_a,
        "unit_in"_a,
        "unit_out"_a,
        "base"_a,
        "generate a value represented by one unit in terms of another if one of the units is in per-unit, the base_value is used in part of the conversion");
    mod.def(
        "default_unit",
        &units::default_unit,
        "get the default unit to use for a particular type of measurement");
}
