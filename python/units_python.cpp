#include <nanobind/nanobind.h>
#include <nanobind/stl/string.h>
#include <nanobind/operators.h>

#include "units/units.hpp"
namespace nb=nanobind;

NB_MODULE(units_llnl, mod) {
    nb::class_<units::precise_unit>(mod, "unit")
        .def(nb::init<>())
        .def("__init__",[](units::precise_unit *type,const char *arg0){new(type) units::precise_unit(units::unit_from_string(std::string(arg0)));})
        .def("multiplier", &units::precise_unit::multiplier)
        .def(nb::self + nb::self)
        .def(nb::self - nb::self)
        .def(nb::self * nb::self)
        .def(nb::self / nb::self)
        .def(float() * nb::self)
        .def( nb::self*float())
        .def(float() / nb::self)
        .def( nb::self/float())
        .def(nb::self == nb::self)
        .def(nb::self < nb::self)
        .def(nb::self > nb::self)
        .def(nb::self <= nb::self)
        .def(nb::self >= nb::self)
        .def(nb::self != nb::self)
        .def("__repr__", [](const units_precise_unit& type) {return units::to_string(type); })
        .def("to_string", [](const units_precise_unit& type) {return units::to_string(type); });

}