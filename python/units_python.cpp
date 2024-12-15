#include <nanobind/nanobind.h>
#include <nanobind/operators.h>
#include <nanobind/stl/string.h>

#include "units/units.hpp"
namespace nb = nanobind;

NB_MODULE(units_llnl, mod)
{
    nb::class_<units::precise_unit>(mod, "unit")
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
                    *unit, units::getCommodity(std::string(commodity)));
            })
        .def(
            "set_multiplier",
            [](units::precise_unit* unit, double mult) {
                return units::precise_unit(mult, *unit);
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
            })
        .def(
            "equivalent_non_counting",
            [](const units::precise_unit& type1,
               const units::precise_unit& type2) {
                return type1.equivalent_non_counting(type2);
            })
        .def(
            "is_convertible",
            [](const units::precise_unit& type1,
               const units::precise_unit& type2) {
                return type1.is_convertible(type2);
            })
        .def("is_default", &units::precise_unit::is_default)
        .def("is_per_unit", &units::precise_unit::is_per_unit)
        .def("has_i_flag", &units::precise_unit::has_i_flag)
        .def("has_e_flag", &units::precise_unit::has_e_flag)
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
            })
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

    nb::class_<units::precise_measurement>(mod, "measurement")
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
        .def(
            "set_commodity",
            [](units::precise_measurement* measurement, const char* commodity) {
                return units::precise_measurement(
                    measurement->value(),
                    units::precise_unit(
                        measurement->units(),
                        units::getCommodity(std::string(commodity))));
            })
        .def("units", &units::precise_measurement::units)
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
        .def("convert_to_base", &units::precise_measurement::convert_to_base)
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
            "is_valid",
            [](const units::precise_measurement& type) {
                return units::is_valid(type);
            })
        .def(
            "is_normal",
            [](const units::precise_measurement& type) {
                return units::isnormal(type);
            })
        .def(
            "root",
            [](const units::precise_measurement& type, int root) {
                return units::root(type, root);
            })
        .def(
            "sqrt",
            [](const units::precise_measurement& type) {
                return units::root(type, 2);
            })
        .def(
            "__repr__",
            [](const units::precise_measurement& type) {
                return units::to_string(type);
            })
        .def("to_string", [](const units::precise_measurement& type) {
            return units::to_string(type);
        });

    mod.def(
        "convert",
        [](double val,
           const units::precise_unit& unitIn,
           const units::precise_unit& unitOut) {
            return units::convert(val, unitIn, unitOut);
        });
    mod.def("convert", [](double val, const char* unitIn, const char* unitOut) {
        return units::convert(
            val,
            units::unit_from_string(std::string(unitIn)),
            units::unit_from_string(std::string(unitOut)));
    });
    mod.def(
        "convert_pu",
        [](double val,
           const units::precise_unit& unitIn,
           const units::precise_unit& unitOut,
           double base_value) {
            return units::convert(val, unitIn, unitOut, base_value);
        });
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
        });
}
