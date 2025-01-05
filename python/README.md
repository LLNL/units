# Units

[![codecov](https://codecov.io/gh/LLNL/units/branch/main/graph/badge.svg)](https://codecov.io/gh/LLNL/units)
[![Build Status](https://dev.azure.com/phlptp/units/_apis/build/status/LLNL.units?branchName=main)](https://dev.azure.com/phlptp/units/_build/latest?definitionId=1&branchName=main)
[![CircleCI](https://circleci.com/gh/LLNL/units.svg?style=svg)](https://circleci.com/gh/LLNL/units)
[![](https://img.shields.io/badge/License-BSD-blue.svg)](https://github.com/GMLC-TDC/HELICS-src/blob/main/LICENSE)
[![](https://img.shields.io/pypi/pyversions/units-llnl)](https://pypi.org/project/units-llnl/)
[![](https://img.shields.io/pypi/v/units-llnl)](https://pypi.org/project/units-llnl/)
[![Documentation Status](https://readthedocs.org/projects/units/badge/?version=latest)](https://units.readthedocs.io/en/latest/?badge=latest)
[![pre-commit.ci status](https://results.pre-commit.ci/badge/github/LLNL/units/main.svg)](https://results.pre-commit.ci/latest/github/LLNL/units/main)

[What's new](./CHANGELOG.md)

[Documentation](https://units.readthedocs.io/en/latest/)

The Units library provides a means of working with units of measurement at runtime, including conversion to and from strings. It provides a small number of types for working with units and measurements and operations necessary for user input and output with units. For additional description and discussion see [Readme](https://github.com/LLNL/units/blob/main/README.md). The Python library is a wrapper around the C++ library using [nanobind](https://github.com/wjakob/nanobind).

## Table of contents

- [Units](#units)
  - [Table of contents](#table-of-contents)
  - [Purpose](#purpose)
    - [Basic use case](#basic-use-case)
  - [Try it out](#try-it-out)
    - [Unit methods](#unit-methods)
      - [Constructors](#constructors)
      - [Methods](#methods)
      - [Properties](#properties)
      - [Operators](#operators)
    - [Measurements](#measurements)
      - [Constructors](#constructors-1)
      - [Methods](#methods-1)
      - [Properties](#properties-1)
      - [Operators](#operators-1)
    - [Other library methods](#other-library-methods)
    - [Future plans](#future-plans)
  - [Contributions](#contributions)
  - [Project Using the Units Library](#project-using-the-units-library)
  - [Release](#release)

## Purpose

A units library was needed to be able to represent units from a wide range of disciplines and be able to separate them from the numerical values for use in calculations when needed. The main drivers are

1. converting units, often represented by strings, to a standardized unit set when dealing with user input and output.
2. Being able to use the unit as a singular type that could contain any unit, and not introduce a huge number of types to represent all possible units.
3. Being able to associate a completely arbitrary unit given by users with a generic interface and support conversions between those user defined units and other units.
4. The library has its origins in power systems so support for per-unit operations was also lacking in the alternatives.

The python wrapper around the library is mainly intended to be able to handle various string representations and easily handle conversions, along with some support for commodities and packaging.

### Basic use case

The primary use case for the library is string operations and conversion. For example if you have a library that does some computations with physical units. In the library code itself the units are standardized and well defined. For example take a velocity, internally everything is in meters per second, but there is a configuration file that takes in the initial data and you would like to broadly support different units on the input

```python
from units_llnl import Unit

u1 = Unit("m")
u2 = Unit("cm")
v1 = u1.convert(10, u2)
assert v1 == 10 * 100

v2 = u1.convert(unit_out=u2, value=20)
assert v2 == 2000
```

```python
from units_llnl import Measurement

m1 = Measurement("10 m")
m2 = Measurement("2.5 s")
m3 = m1 / m2
m4 = Measurement("4.0 m/s")
assert m3 == m4
```

## Try it out

If you want to try out the string conversion components. There is server running that can do the string conversions

[Unit String Conversions](https://units.readthedocs.io/en/latest/_static/convert.html)

For more details see the [documentation](https://units.readthedocs.io/en/latest/web/index.html)

### Unit methods

These operations apply the `Units` object in Python. It maps to a `precise_unit` in C++. The Unit object is immutable like a python string so a new one is created for methods that modify the unit in some way.

#### Constructors

- `Unit(unit_str:str)` construct from a string
- `Unit(unit_str:str,commodity_str:str)` construct a unit from a unit string and commodity string
- `Unit(float multiplier, unit:Unit)` construct a unit using another unit as a base along with a multiplier

#### Methods

- `is_exactly_the_same(other:Unit)->bool` compare two units and check for exact equivalence in both the unit_data and the multiplier.
- `has_same_base(other:Unit)->bool` check if the units have the same base units.
- `equivalent_non_counting(other:Unit)->bool` check if the units are equivalent ignoring the counting bases.
- `is_convertible_to(other:Unit)->bool` check if the units are convertible to each other, currently checks `equivalent_non_counting()`, but some additional conditions might be allowed in the future to better match convert.
- `convert(value:float,unit_out:Unit|str)->float` convert a value from the existing unit to another, can also be a string.
- `is_per_unit()->bool` true if the unit has the per_unit flag active.
- `is_equation()->bool` true if the unit has the equation flag active.
- `is_valid()->bool` true if the unit is a valid unit.
- `is_normal()->bool` true if the unit is a normal unit (not error, nan, or subnormal).
- `is_error()->bool` true if the unit is an error unit (e.g invalid conversion).
- `isfinite()->bool` true if the unit does not have an infinite multiplier.
- `isinf()->bool` true if the unit does have an infinite multiplier.
- `root(power:int)->Unit` return a new unit taken to the root power.
- `sqrt()->Unit` returns a new unit which is the square root of the current unit.
- `set_multiplier(mult:float)->Unit` generate a new Unit with the set multiplier.
- `set_commodity(int commodity)` generate a new unit with the assigned commodity.

#### Properties

- `multiplier->float` return the unit multiplier as a floating point number
- `commodity->str` get the commodity of the unit
- `base_units`->Unit gets the base units (no multiplier) associated with a unit

#### Operators

- `*`,`/` with other units produces a new unit
- `~` produces the inverse of the unit
- `**` is an exponentiation operator and produces a new unit
- `*`, `/` with a floating point generates a `Measurement`
- `==` and `!=` produce the appropriate comparison operators
- f string formatting also works with units and returns the string representation of the unit. This string is guaranteed to produce the same unit as the current unit, but may not be the same string as was used to create it.
- `str`,`bool` are defined, `bool` indicates that the unit is valid, and non-zero
- `Units` may also be used as the indexing element in a dictionary

### Measurements

#### Constructors

- `Measurement(measurement_str:str)` construct from a string
- `Measurement(value:float, unit:Unit|str)` construct a `Measurement` from a value and a `Unit` or string representing a `Unit`

#### Methods

- `is_normal()->bool` true if the unit is a normal unit (not error, nan, or subnormal)
- `is_valid()->bool` true if the `Measurement` is a valid Measurement (not error)
- `root(power:int)->Measurement` return a new unit taken to the root power
- `sqrt()->Unit` returns a new unit which is the square root of the current unit
- `set_value(value:float)->Measurement` generate a new `Measurement` with the new Value
- `set_units(unit:Unit|str)` generate a new `Measurement` with the new units
- `value_as(unit:Unit|str)->float` convert the value of the `Measurement` to a new `Unit`
- `convert_to(unit:Unit|str)->Measurement` create a new `Measurement` with the new units and the value converted to those units
- `convert_to_base()->Measurement` create a new `Measurement` with the units as the base measurement units
- `is_close(other:Measurement)->bool` return true if the two measurements are close (both converted to non precise measurement and compared)

#### Properties

- `value->float` return the numerical portion of a `Measurement`
- `units->Unit` get the `Unit` associated with a `Measurement`

#### Operators

- `*`,`/` with other `Measurements` produces a new Measurement
- `~` inverts the measurement equivalent to `1/measurement`
- `+`,`-` with other `Measurements` ensures the units are in the same base unit and performs the appropriate action
- `**` is an exponentiation operator and produces a new `Measurement` (NOTE: beware of limits on power representations of some units, things will always wrap so it is defined but may not produce what you expect). Can be negative.
- `*`, `/`,`%` with a floating point generates a `Measurement`
- `//` produces the floor of the resulting unit of division
- `==`,`!=`,`>`,`<`,`>=`,`<=` produce the appropriate comparison operators
- `str`,`float`,`bool` are defined, `bool` indicates that the measurement is non zero and is valid
- `round`, `math.ceil`,`math.floor`, and `math.trunc` work as expected
- f string formatting also works with measurement. Some special formatters are available `f"{m1:-}"` will remove the unit and just display the value. `f"{m1:new_unit}"` will convert to a new unit before displaying. `f"{m1:-new_unit}"` will do the conversion but just display the numerical value after the conversion.

### Other library methods

- `convert(value:float,unit_in:Unit|str,unit_out:Unit|str)->float` generate a value represented by one unit in terms of another
- `convert_pu(value:float,unit_in:Unit|str,unit_out:Unit|str, base:float)->float` "generate a value represented by one unit in terms of another if one of the units is in per-unit, the base_value is used in part of the conversion"
- `default_unit(unit_type:str)->Unit` generate a unit used for a particular type of measurement
- `add_user_defined_unit(unit_name|str,unit_definition:str|Unit)` add a custom string representing a particular unit to use in future string translations
- `add_units_from_file(file|str)` inject a list of user defined units from a file

### Future plans

Uncertain measurements will likely be added, potentially some trig functions on measurements. Also some more commodity operations, and x12 and r20 unit types.

## Contributions

Contributions are welcome. See [Contributing](./CONTRIBUTING.md) for more details and [Contributors](./CONTRIBUTORS.md) for a list of the current and past Contributors to this project.

## Project Using the Units Library

Anyone else using the units library? Please let us know.

- [HELICS](www.helics.org)
- [GridDyn](https://github.com/LLNL/GridDyn)
- [scipp](https://scipp.github.io/)

## Release

This units library is distributed under the terms of the BSD-3 clause license. All new
contributions must be made under this license. [LICENSE](./LICENSE)

SPDX-License-Identifier: BSD-3-Clause

LLNL-CODE-773786
