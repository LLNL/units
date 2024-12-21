# Units

[![codecov](https://codecov.io/gh/LLNL/units/branch/main/graph/badge.svg)](https://codecov.io/gh/LLNL/units)
[![Build Status](https://dev.azure.com/phlptp/units/_apis/build/status/LLNL.units?branchName=main)](https://dev.azure.com/phlptp/units/_build/latest?definitionId=1&branchName=main)
[![CircleCI](https://circleci.com/gh/LLNL/units.svg?style=svg)](https://circleci.com/gh/LLNL/units)
[![](https://img.shields.io/badge/License-BSD-blue.svg)](https://github.com/GMLC-TDC/HELICS-src/blob/main/LICENSE)
[![Documentation Status](https://readthedocs.org/projects/units/badge/?version=latest)](https://units.readthedocs.io/en/latest/?badge=latest)
[![pre-commit.ci status](https://results.pre-commit.ci/badge/github/LLNL/units/main.svg)](https://results.pre-commit.ci/latest/github/LLNL/units/main)

[What's new](./CHANGELOG.md)

[Documentation](https://units.readthedocs.io/en/latest/)

The Units library provides a means of working with units of measurement at runtime, including conversion to and from strings. It provides a small number of types for working with units and measurements and operations necessary for user input and output with units. For additional description and discussion see [Readme](https://github.com/LLNL/units/blob/main/README.md)

## Table of contents

- [Units](#units)
  - [Table of contents](#table-of-contents)
  - [Purpose](#purpose)
    - [Basic use case](#basic-use-case)
  - [Try it out](#try-it-out)
    - [Unit methods](#unit-methods)
      - [Unit Operators](#unit-operators)
    - [Measurement Operations](#measurement-operations)
      - [Measurement operators](#measurement-operators)
  - [Contributions](#contributions)
  - [Project Using the Units Library](#project-using-the-units-library)
  - [Release](#release)

## Purpose

A units library was needed to be able to represent units from a wide range of disciplines and be able to separate them from the numerical values for use in calculations when needed. The main drivers are

1. converting units, often represented by strings, to a standardized unit set when dealing with user input and output.
2. Being able to use the unit as a singular type that could contain any unit, and not introduce a huge number of types to represent all possible units.
3. Being able to associate a completely arbitrary unit given by users with a generic interface and support conversions between those user defined units and other units.
4. The library has its origins in power systems so support for per-unit operations was also lacking in the alternatives.
5. Capture uncertainty and uncertainty calculations directly with a measurement

The python wrapper around the library is mainly intended to be able to handle various string representations and easily handle conversions, along with some support for commodities and packaging.

### Basic use case

The primary use case for the library is string operations and conversion. For example if you have a library that does some computations with physical units. In the library code itself the units are standardized and well defined. For example take a velocity, internally everything is in meters per second, but there is a configuration file that takes in the initial data and you would like to broadly support different units on the input

```python
from units_llnl import unit

u1 = unit("m")
u2 = unit("cm")
v1 = u1.convert(10, u2)
assert v1 == 10 * 100

v2 = u1.convert(unit_out=u2, value=20)
assert v2 == 2000
```

```python
from units_llnl import measurement

m1 = measurement("10 m")
m2 = measurement("2.5 s")
m3 = m1 / m2
m4 = measurement("4.0 m/s")
assert m3 == m4
```

## Try it out

If you want to try out the string conversion components. There is server running that can do the string conversions

[Unit String Conversions](https://units.readthedocs.io/en/latest/_static/convert.html)

For more details see the [documentation](https://units.readthedocs.io/en/latest/web/index.html)

### Unit methods

These operations apply to units and precise_units

- `<unit>(<unit_data>)` construct from a base unit_data
- `<unit>(<unit_data>, double multiplier)` construct a unit from a base data and a multiplier
- `<unit>(double multiplier, <unit>)` construct from a multiplier and another unit
- also available are copy constructor and copy assignments
- `<unit> inv()` generate a new unit containing the inverse unit `m.inv()= 1/m`
- `<unit> pow(int power)` take a unit to power(NOTE: beware of limits on power representations of some units, things will always wrap so it is defined but may not produce what you expect). `power` can be negative.
- `bool is_exactly_the_same(<unit>)` compare two units and check for exact equivalence in both the unit_data and the multiplier, NOTE: this uses double equality
- `bool has_same_base(<unit>|<unit_data>)` check if the <unit_data> is the same
- `equivalent_non_counting(<unit>|<unit_data>)` check if the units are equivalent ignoring the counting bases
- `bool is_convertible(<unit>)` check if the units are convertible to each other, currently checks `equivalent_non_counting()`, but some additional conditions might be allowed in the future to better match convert.
- `int unit_type_count()` count the number of unit bases used, (does not take into consideration powers, just if the dimension is used or not.
- `bool is_per_unit()` true if the unit has the per_unit flag active
- `bool is_equation()` true if the unit has the equation flag active
- `bool has_i_flag()` true if the i_flag is marked active
- `bool has_e_flag()` true if the e_flag is marked active
- `double multiplier()` return the unit multiplier as a double

- `commodity()` get the commodity of the unit
- `commodity(int commodity)` assign a commodity to the precise_unit.

#### Unit Operators

There are also several operator overloads that apply to units and precise_units.

- `<unit>=<unit>*<unit>` generate a new unit with the units multiplied ie `m*m` does what you might expect and produces a new unit with `m^2`
- `<unit>=<unit>/<unit>` generate a new unit with the units divided ie `m/s` does what you might expect and produces a new unit with meters per second. NOTE: `m/m` will produce `1` it will not automatically produce a `pu` though we are looking at how to make a 'pu_m\*m=m' so units like strain might work smoothly.

- `bool <unit>==<unit>` compare two units. this does a rounding compare so there is some tolerance to roughly 7 significant digits for \<unit> and 13 significant digits for <precise_unit>.
- `bool <unit>!=<unit>` the opposite of `==`

precise_units can usually operate with a `precise_unit` or `unit`, `unit` usually can't operate on `precise_unit`.

### Measurement Operations

- `<measurement>(val, <unit>)` construct a unit from a value and unit object.
- `double value() const` get the measurement value as a double.
- `<measurement> convert_to(<unit>) const` convert the value in the measurement to another unit base
- `<measurement> convert_to_base() const` convert to a base unit, i.e. a unit whose multiplier is 1.0
- `<unit> units() const` get the units used as a basis for the measurement
- `<unit> as_unit() const` take the measurement as is and convert it into a single unit. For Examples say a was 10 m. calling as_unit() on that measurement would produce a unit with a multiplier of 10 and a base of meters.
- `double value_as(<unit>)` get the value of a measurement as if it were measured in \<unit\>

#### Measurement operators

There are several operator overloads which work on measurements or units to produce measurements.

- `'*', '/', '+','-'` are all defined for mathematical operations on a measurement and produce another measurement.
- `%` `*`, and `/` are defined for \<measurement>\<op>\<double>
- `*`, and `/` are defined for \<double>\<op>\<measurement>

Notes: for regular measurements, `+` and `-` are not defined for doubles due to ambiguity of what that operation means. For `fixed_measurement` types this is defined as the units are known at construction and cannot change. For `fixed_measurement` types if the operator would produce a new measurement with the same units it will be a fixed measurement, if not it reverts to a regular measurement.

- `==`, `!=`, `>`, `<`, `>=`, `<=` are defined for all measurement comparisons
- `<measurement>=<double>*<unit>`
- `<measurement>=<unit>*<double>`
- `<measurement>=<unit>/<double>`
- `<measurement>=<double>/<unit>` basically calling a number multiplied or divided by a `<unit>` produces a measurement, specifically `unit` produces a measurement and `precise_unit` produces a precise_measurement.

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
