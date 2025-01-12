/*
Copyright (c) 2019-2025,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <nanobind/stl/bind_map.h>
#include <nanobind/stl/string.h>
#include <nanobind/stl/vector.h>

#include "units/units.hpp"
#include "units/units_math.hpp"
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
        "Unit",
        "a Unit is a basic building block for unit conversion this library operates mainly on strings and can interpret those strings as units or measurements")
        .def(nb::init<>())
        .def(
            "__init__",
            [](units::precise_unit* type, const char* arg0) {
                new (type) units::precise_unit(
                    units::unit_from_string(std::string(arg0)));
            },
            "unit"_a)
        .def(
            "__init__",
            [](units::precise_unit* type,
               double multiplier,
               const units::precise_unit& base) {
                new (type) units::precise_unit(multiplier, base);
            },
            "multiplier"_a,
            "unit"_a)
        .def(
            "__init__",
            [](units::precise_unit* type, double multiplier, const char* base) {
                new (type) units::precise_unit(
                    multiplier, units::unit_from_string(base));
            },
            "multiplier"_a,
            "unit"_a)
        .def(
            "__init__",
            [](units::precise_unit* type,
               const char* arg0,
               const char* commodity) {
                new (type) units::precise_unit{
                    units::unit_from_string(std::string(arg0)),
                    units::getCommodity(std::string(commodity))};
            },
            "unit"_a,
            "commodity"_a)
        .def(
            "__init__",
            [](units::precise_unit* type,
               double multiplier,
               const char* arg0,
               const char* commodity) {
                new (type) units::precise_unit{
                    multiplier,
                    units::unit_from_string(std::string(arg0)),
                    units::getCommodity(std::string(commodity))};
            },
            "multiplier"_a,
            "unit"_a,
            "commodity"_a)
        .def_prop_ro("multiplier", &units::precise_unit::multiplier)
        .def_prop_ro(
            "commodity",
            [](const units::precise_unit& unit) {
                return units::getCommodityName(unit.commodity());
            })
        .def_prop_ro(
            "base_units",
            [](const units::precise_unit& type1) {
                return units::precise_unit(type1.base_units());
            })
        .def(
            "set_commodity",
            [](const units::precise_unit* unit, const char* commodity) {
                return units::precise_unit(
                    unit->multiplier(),
                    unit->base_units(),
                    units::getCommodity(std::string(commodity)));
            })
        .def(
            "set_multiplier",
            [](const units::precise_unit* unit, double mult) {
                return units::precise_unit(
                    mult, unit->base_units(), unit->commodity());
            })
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
            [](const units::precise_unit& unit, int pow) {
                return unit.pow(pow);
            },
            nb::is_operator())
        .def(
            "__pow__",
            [](const units::precise_unit& unit, float pow) {
                if (pow < 1.0 && pow > 0.0) {
                    if (pow == 0.5) {
                        return units::root(unit, 2);
                    } else {
                        return units::root(unit, static_cast<int>(1.0 / pow));
                    }

                } else {
                    return unit.pow(int(pow));
                }
            },
            nb::is_operator())
        .def(
            "__mul__",
            [](const units::precise_unit& unit,
               const std::vector<double>& mult) {
                std::vector<units::precise_measurement> results;
                results.resize(mult.size());
                for (std::size_t ii = 0; ii < mult.size(); ++ii) {
                    results[ii] = mult[ii] * unit;
                }
                return results;
            },
            nb::is_operator())
        .def(
            "__rmul__",
            [](const units::precise_unit& unit,
               const std::vector<double>& mult) {
                std::vector<units::precise_measurement> results;
                results.resize(mult.size());
                for (std::size_t ii = 0; ii < mult.size(); ++ii) {
                    results[ii] = mult[ii] * unit;
                }
                return results;
            },
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
            "to",
            [](units::precise_unit* unit,
               double value,
               const units::precise_unit& convert_to_units) {
                return units::convert(value, *unit, convert_to_units);
            },
            "value"_a,
            "unit_out"_a,
            "value represented by one unit in terms of another")
        .def(
            "to",
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
        .def("is_per_unit", &units::precise_unit::is_per_unit)
        .def("is_equation", &units::precise_unit::is_equation)
        .def(
            "is_valid",
            [](const units::precise_unit& unit) {
                return units::is_valid(unit);
            })
        .def(
            "is_normal",
            [](const units::precise_unit& unit) {
                return units::isnormal(unit);
            })
        .def(
            "is_error",
            [](const units::precise_unit& unit) {
                return units::is_error(unit);
            },
            "return true if the unit has the error flags set or is infinite")
        .def(
            "isfinite",
            [](const units::precise_unit& unit) {
                return units::isfinite(unit);
            })
        .def(
            "isinf",
            [](const units::precise_unit& unit) { return units::isinf(unit); })
        .def(
            "root",
            [](const units::precise_unit& unit, int root) {
                return units::root(unit, root);
            })
        .def(
            "sqrt",
            [](const units::precise_unit& unit) {
                return units::root(unit, 2);
            })
        .def(
            "__invert__",
            [](const units::precise_unit& unit) { return unit.inv(); })
        .def(
            "__repr__",
            [](const units::precise_unit& unit) {
                return units::to_string(unit);
            })
        .def(
            "__bool__",
            [](const units::precise_unit& unit) {
                return (
                    is_valid(unit) && !is_error(unit) &&
                    unit.multiplier() != 0);
            })
        .def(
            "to_dict",
            [](const units::precise_unit& unit) {
                nb::dict dictionary;
                dictionary["unit"] = units::to_string(unit);
                return dictionary;
            })
        .def("__hash__", [](const units::precise_unit& unit) {
            return std::hash<units::precise_unit>()(unit);
        });

    nb::class_<units::precise_measurement>(
        mod,
        "Measurement",
        "a Measurement object consists of a measurement(value) and a unit and allows conversion to other units")
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
            },
            "value"_a,
            "unit"_a)
        .def(
            "__init__",
            [](units::precise_measurement* measurement,
               const std::string& value,
               const std::string& unit) {
                new (measurement) units::precise_measurement(
                    units::measurement_from_string(value + " " + unit));
            },
            "value"_a,
            "unit"_a)
        .def(
            "__init__",
            [](units::precise_measurement* measurement,
               double value,
               const units::precise_unit& unit) {
                new (measurement) units::precise_measurement(value, unit);
            },
            "value"_a,
            "unit"_a)
        .def_prop_ro(
            "value",
            [](const units::precise_measurement& measurement) {
                return measurement.value();
            })
        .def(
            "set_value",
            [](const units::precise_measurement* measurement, double value) {
                return units::precise_measurement(value, measurement->units());
            })
        .def_prop_ro(
            "units",
            [](const units::precise_measurement& measurement) {
                return measurement.units();
            })
        .def_prop_ro(
            "unit",
            [](const units::precise_measurement& measurement) {
                return measurement.units();
            })
        .def(
            "set_units",
            [](const units::precise_measurement* measurement,
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
            },
            "create a new `Measurement` with the new units and the value converted to those units")
        .def(
            "convert_to",
            [](const units::precise_measurement& measurement,
               const char* units) {
                return measurement.convert_to(
                    units::unit_from_string(std::string(units)));
            },
            "create a new `Measurement` with the new units and the value converted to those units")
        .def(
            "to",
            [](const units::precise_measurement& measurement,
               const units::precise_unit& unit) {
                return measurement.convert_to(unit);
            },
            "create a new `Measurement` with the new units and the value converted to those units")
        .def(
            "to",
            [](const units::precise_measurement& measurement,
               const char* units) {
                return measurement.convert_to(
                    units::unit_from_string(std::string(units)));
            },
            "create a new `Measurement` with the new units and the value converted to those units")
        .def(
            "convert_to_base",
            &units::precise_measurement::convert_to_base,
            "convert a measurement to a measurement using the base si units")
        .def("as_unit", &units::precise_measurement::as_unit)
        .def(nb::self * nb::self)
        .def(nb::self / nb::self)
        .def(nb::self % nb::self)
        .def(nb::self + nb::self)
        .def(nb::self - nb::self)
        .def(float() * nb::self)
        .def(nb::self * float())
        .def(float() / nb::self)
        .def(nb::self / float())
        .def(nb::self % float())
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
            "__floordiv__",
            [](const units::precise_measurement& measurement,
               const units::precise_measurement& other) {
                auto res1 = measurement / other;
                if (res1.units().unit_type_count() == 0) {
                    res1 = res1.convert_to_base();
                }
                return floor(res1);
            },
            nb::is_operator())
        .def(
            "__floordiv__",
            [](const units::precise_measurement& measurement, double divisor) {
                return floor(measurement / divisor);
            },
            nb::is_operator())

        .def(
            "__mul__",
            [](const units::precise_measurement& measurement,
               const std::vector<double>& mult) {
                std::vector<units::precise_measurement> results;
                results.resize(mult.size());
                for (std::size_t ii = 0; ii < mult.size(); ++ii) {
                    results[ii] = measurement * mult[ii];
                }
                return results;
            },
            nb::is_operator())
        .def(
            "__rmul__",
            [](const units::precise_measurement& measurement,
               const std::vector<double>& mult) {
                std::vector<units::precise_measurement> results;
                results.resize(mult.size());
                for (std::size_t ii = 0; ii < mult.size(); ++ii) {
                    results[ii] = mult[ii] * measurement;
                }
                return results;
            },
            nb::is_operator())
        .def(
            "is_valid",
            [](const units::precise_measurement& measurement) {
                return units::is_valid(measurement);
            },
            "true if the `Measurement` is a valid Measurement (not error)")
        .def(
            "is_normal",
            [](const units::precise_measurement& measurement) {
                return units::isnormal(measurement);
            },
            "true if the unit is a normal unit(not error, nan, or subnormal)")
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
            "isclose",
            [](const units::precise_measurement& measurement1,
               const units::precise_measurement& measurement2) {
                return units::measurement_cast(measurement1) ==
                    units::measurement_cast(measurement2);
            },
            "return true if the two measurements are close (both converted to non precise measurement and compared)")
        .def(
            "__repr__",
            [](const units::precise_measurement& measurement) {
                return units::to_string(measurement);
            })
        .def(
            "__format__",
            [](const units::precise_measurement& measurement,
               std::string fmt_string) {
                if (fmt_string.empty()) {
                    return units::to_string(measurement);
                }
                if (fmt_string == "-") {
                    return units::to_string(units::precise_measurement(
                        measurement.value(), units::precise::one));
                }
                if (fmt_string.front() == '-') {
                    return units::to_string(units::precise_measurement(
                        measurement.value_as(
                            units::unit_from_string(fmt_string.substr(1))),
                        units::precise::one));
                } else {
                    return units::to_string(measurement.convert_to(
                        units::unit_from_string(fmt_string)));
                }
            })
        .def(
            "to_dict",
            [](const units::precise_measurement& measurement) {
                nb::dict dictionary;
                dictionary["unit"] = units::to_string(measurement.units());
                dictionary["value"] = measurement.value();
                return dictionary;
            })
        .def(
            "__neg__",
            [](const units::precise_measurement& measurement) {
                return -measurement;
            })
        .def(
            "__invert__",
            [](const units::precise_measurement& measurement) {
                return 1.0 / measurement;
            })
        .def(
            "__trunc__",
            [](const units::precise_measurement& measurement) {
                return trunc(measurement);
            })
        .def(
            "__ceil__",
            [](const units::precise_measurement& measurement) {
                return ceil(measurement);
            })
        .def(
            "__floor__",
            [](const units::precise_measurement& measurement) {
                return floor(measurement);
            })
        .def(
            "__round__",
            [](const units::precise_measurement& measurement) {
                return round(measurement);
            })
        .def(
            "__float__",
            [](const units::precise_measurement& measurement) {
                return measurement.value();
            })
        .def("__bool__", [](const units::precise_measurement& measurement) {
            return (
                is_valid(measurement.units()) && (measurement.value() != 0.0) &&
                (measurement.units().multiplier() != 0.0) &&
                !is_error(measurement.units()));
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
        "generate a value represented by one unit in terms of another");
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
    mod.def(
        "add_user_defined_unit",
        [](const char* unit_name, const units::precise_unit& unit_definition) {
            units::addUserDefinedUnit(std::string(unit_name), unit_definition);
        },
        "unit_name"_a,
        "unit_definition"_a,
        "add a custom string to represent a user defined unit");
    mod.def(
        "add_user_defined_unit",
        [](const char* unit_name, const char* unit_definition) {
            units::addUserDefinedUnit(
                std::string(unit_name),
                units::unit_from_string(std::string(unit_definition)));
        },
        "unit_name"_a,
        "unit_definition"_a,
        "add a custom string to represent a user defined unit");
    mod.def(
        "defined_units_from_file",
        &units::definedUnitsFromFile,
        "inject a list of user defined units from a file");
}
