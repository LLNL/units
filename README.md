# Units

[![Build Status](https://travis-ci.com/LLNL/units.svg?branch=master)](https://travis-ci.com/LLNL/units)
[![codecov](https://codecov.io/gh/LLNL/units/branch/master/graph/badge.svg)](https://codecov.io/gh/LLNL/units)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/c0b5367026f34c4a9dc94ca4c19c770a)](https://app.codacy.com/app/phlptp/units?utm_source=github.com&utm_medium=referral&utm_content=LLNL/units&utm_campaign=Badge_Grade_Settings)
[![Build Status](https://dev.azure.com/phlptp/units/_apis/build/status/LLNL.units?branchName=master)](https://dev.azure.com/phlptp/units/_build/latest?definitionId=1&branchName=master)
[![CircleCI](https://circleci.com/gh/LLNL/units.svg?style=svg)](https://circleci.com/gh/LLNL/units)
[![](https://img.shields.io/badge/License-BSD-blue.svg)](https://github.com/GMLC-TDC/HELICS-src/blob/master/LICENSE)
[![Documentation Status](https://readthedocs.org/projects/units/badge/?version=latest)](https://units.readthedocs.io/en/latest/?badge=latest)

[What's new](./CHANGELOG.md) •
[Documentation](https://units.readthedocs.io/en/latest/)

A library that provides runtime unit values, instead of individual unit types, for the purposes of working with units of measurement at run time possibly from user input.

This software was developed for use in [LLNL/GridDyn](https://github.com/LLNL/GridDyn), and is currently a work in progress (though getting closer). Namespaces, function names, and code organization is subject to change, input is welcome. An \[in development\] set of [documentation](https://units.readthedocs.io/en/latest/) is available.

## Table of contents

- [Purpose](#purpose)
- [Limitations](#limitations)
- [Alternatives](#alternatives)
- [C++ Types](#types)
- [Unit Representation](#unit-representation)
- [Building The Library](#building-the-library)
- [Try it out](#try-it-out)
- [Usage](#usage)
  - [Units](#unit-methods)
  - [Measurements](#measurement-operations)
  - [String Conversions](#available-library-functions)
- [Contributions](#contributions)
- [Release](#release)

## Purpose

A unit library was needed to be able to represent units of a wide range of disciplines and be able to separate them from the numerical values for use in calculations. The main driver is converting units, often represented by strings, to a standardized unit set when dealing with user input and output. And be able to use the unit as a singular type that could contain any unit, and not introduce a huge number of types to represent all possible units. Sometimes the unit type needs to be used inside virtual function calls which must strictly define a type. The library also has its origin in power systems so support for per-unit operations was also lacking in the alternatives.

It was desired that the unit representation be a compact type(<=8 bytes) that is typically passed by value, that can represent a wide assortment of units and arbitrary combinations of units. The primary use of the conversions is at run-time to convert user input/output to/from internal units, it is not to provide strict type safety or dimensional analysis, though it can provide some of that. This library does **NOT** provide compile time checking of units. The units library provides a library that supports units and operations on them where many of the units in use are unknown at compile time and conversions and uses are dealt with at run time, and may be of a wide variety of units.

This library is an engineering library, created to represent a huge variety of units and measurements in a simple data type instead of a proliferation of templates. It supports conversion of units to and from strings. It supports mathematical operations on units and measurements which is `constexpr` where possible. It supports units used in power systems and electrical engineering, and conversions between them as well as some typical assumptions for supporting conversions. In some cases it also has some notion of commodities, and support for existing unit standards for strings and naming.

### Basic use case

The primary use case for the library is string operations and conversion. For example if you have a library that does some computations with physical units. In the library code itself the units are standardized and well defined. Say a velocity, internally everything is in meters per second. But there is a configuration file that takes in the initial data and you would like to broadly support different units on the input

```cpp
#include <units/units.hpp>

double GetInputValueAs(const std::string &input, precise_units out)
{
   auto meas=measurement_from_string(input);
   return meas.value_as(out);
}

```

The return value can be checked for validity as an invalid conversion would result in `constants::invalid_conversion` or `Nan` so can be checked by `std::isnan`
or

```cpp
if (!meas.units().is_convertible(out)
{
    throw(std::invalid_argument);
}
```

## Limitations

- The powers represented by units are limited see [Unit representation](#unit_representation) and only normal physical units or common operations are supported.
- The library uses floating point and double precision for the multipliers which is generally good enough for most engineering contexts, but does come with the limits and associated loss of precision for long series of calculations on floating point numbers.
- Currency is supported as a unit but it is not recommended to use this for anything beyond basic financial calculations. So if you are doing a lot of financial calculations or accounting use something more specific for currency manipulations.
- Fractional unit powers are not supported in general. While some mathematical operations on units are supported any root operations `sqrt` or `cbrt` will only produce valid results if the result is integral powers of the base units. One exception is limited support for √Hz operations in measurements of Amplitude spectral density. A specific definition of a unit representing square root of Hz is available and will work in combination with other units.
- While conversions of various temperature definitions are supported, there is no generalized support for datums and bias shifts. It may be possible to add some specific cases in the future for common uses cases but the space requirement limits such use. Some of the other libraries have general support for this.
- A few [equation](https://units.readthedocs.io/en/latest/user-guide/equation_units.html) like units are supported these include logarithms, nepers, and some things like Saffir-Simpson, Beaufort, and Fujita scales for wind, and Richter scales for earthquakes. There is capacity within the framework to add a few more equation like units if a need arises.
- The unit `rad` in the nature of radiation absorbed dose is not recognized as it would conflicts with `rad` in terms of radians. So `rad` means radians since that is the more common use in electrical engineering. There is support for custom unit strings so "rad" can be overridden if required using custom units for string conversion operations.

## Alternatives

If you are looking for compile time and prevention of unit errors in equations for dimensional analysis one of these libraries might work for you.

- [boost units](https://www.boost.org/doc/libs/1_69_0/doc/html/boost_units.html) -Zero-overhead dimensional analysis and unit/quantity manipulation and conversion in C++
- [Units](https://github.com/nholthaus/units) -A compile-time, header-only, dimensional analysis library built on `C++14` with no dependencies.
- [Units](https://github.com/VincentDucharme/Units) -Another compile time library
- [PhysUnits-CT](https://github.com/martinmoene/PhysUnits-CT-Cpp11) A C++ library for compile-time dimensional analysis and unit/quantity manipulation and conversion.
- [PhysUnits-RT](https://github.com/martinmoene/PhysUnits-RT) -A C++ library for run-time dimensional analysis and unit/quantity manipulation and conversion.
- [Libunits](https://sourceforge.net/projects/libunits/) -The ultimate shared library to do calculations(!) and conversions with any units! Includes all SI and pseudo SI units and thousands of US, Imperial and other units.
- [unitscpp](http://code.google.com/p/unitscpp/) -A lightweight C++ library for physical calculation with units.
- [mpusz/units](https://github.com/mpusz/units) -A compile-time enabled Modern C++ library that provides compile-time dimensional analysis and unit/quantity manipulation.
- [bernedom/SI](https://github.com/bernedom/SI) -A header only C++ library that provides type safety and user defined literals for handling physical values defined in the International System of Units

These libraries will work well if the number of units being dealt with is known at compile time. Many also produce zero overhead operations and checking. Therefore in situations where this is possible other libraries are a preferred alternative.

### Reasons to choose this units library vs. another option

1.  Conversions to and from regular strings are required
2.  The number of units in use is large
3.  A specific single unit or measurement type is required to handle many different kinds of units or measurements
4.  Uncertainties are needed to be included with the measurements
5.  Working with per unit values
6.  Dealing with commodities in addition to regular units. i.e. differentiate between a gallon of water and a gallon of gasoline
7.  Dealing with equation type units
8.  Complete C++ type safety is not a critical design requirement.
9.  Support is needed for some funky custom unit with bizarre base units.

### Reasons to choose something else

1.  Type safety and dimensional analysis is a design requirement
2.  Performance is absolutely critical (many other libraries are zero runtime overhead)
3.  You are only working with a small number of known units
4.  You cannot use C++11 yet.
5.  You need to operate on arbitrary powers of base units

## Types

There are only a few types in the library

- `detail::unit_base` is the base representation of physical units and powers. It uses a bitfield to store the base unit representation in a 4-byte representation. It is mostly expected that unit_base will not be used in a standalone context but through one of other types.
- `unit` is the primary type representing a physical unit it consists of a `float` multiplier along with a `unit_base` and contains this within an 8 byte type. The float has an accuracy of around 6 decimal digits. Units within that tolerance will compare equal.
- `precise_unit` is the a more accurate type representing a physical unit it consists of a `double` multiplier along with a `unit_base` and contains this within an 16 byte type. The double has an accuracy of around 13 decimal digits. Units within that tolerance will compare equal. The remaining 4 bytes are used to contain a commodity object code.
- `measurement` is a 16 byte type containing a double value along with a `unit` and mathematical operations can be performed on it usually producing a new measurement.
- `precise_measurement` is similar to measurement except using a double for the quantity and a `precise_unit` as the units.
- `fixed_measurement` is a 16 byte type containing a double value along with a constant `unit` and mathematical operations can be performed on it usually producing a new `measurement`. The distinction between `fixed_measurement` and `measurement` is that the unit definition of `fixed_measurement` is constant and any assignments get automatically converted, `fixed_measurement`'s are implicitly convertible to a `measurement` of the same value type. fixed_measurement also support some operation with pure numbers by assuming a unit that are not allowed on regular measurement types.
- `fixed_precise_measurement` is similar to `fixed_measurement` except it uses `precise_unit` as a base and uses a double for the measurement instead of a template, except it is 24 bytes instead of 16.
- `uncertain_measurement` is similar to `measurement` except it uses a 32-bit float for the value and contains an uncertainty field which is also 32-bits. Mathematical operations on `uncertain_measurements` will propagate the uncertainty and convert it as necessary. The class also includes functions for simple-uncertainty propagation like `simple_subtract` which just sums the uncertainties. The sum-of-squares methods is used in the overloaded math operators. Mathematical operations are supported on the type and it interoperates with measurements.

## Unit representation

The `unit` class consists of a multiplier and a representation of base units.
The seven [SI units](https://www.nist.gov/pml/weights-and-measures/metric-si/si-units) + radians + currency units + count units. In addition a `unit` has 4 flags, per-unit for per unit or ratio units. One flag\[i_flag\] that is a representation of imaginary units, one flags for a variety of purposes and to differentiate otherwise similar units\[e_flag\]. And a flag to indicate an equation unit. Due to the requirement that the base units fit into a 4 byte type the represented powers of the units are limited. The table below shows the bit representation range and observed range of use in equations and observed usage

| Base Unit | Bits | Representable range | Normal Range | Intermediate Operations |
| --------- | ---- | ------------------- | ------------ | ----------------------- |
| meter     | 4    | \[-8,+7\]           | \[-4,+4\]    | \[-6,+6\]               |
| kilogram  | 3    | \[-4,+3\]           | \[-1,+1\]    | \[-2,+2\]               |
| second    | 4    | \[-8,+7\]           | \[-4,+4\]    | \[-6,+6\]               |
| ampere    | 3    | \[-4,+3\]           | \[-2,+2\]    |                         |
| kelvin    | 3    | \[-4,+3\]           | \[-4,+1\]    |                         |
| mole      | 2    | \[-2,+1\]           | \[-1,+1\]    |                         |
| candela   | 2    | \[-2,+1\]           | \[-1,+1\]    |                         |
| currency  | 2    | \[-2,+1\]           | \[-1,+1\]    |                         |
| count     | 2    | \[-2,+1\]           | \[-1,+1\]    |                         |
| radians   | 3    | \[-4,+3\]           | \[-2,+2\]    |                         |

These ranges were chosen to represent nearly all physical quantities that could be found in various disciplines we have encountered.

### Discussion points

- Currency may seem like a unusual choice in units but numbers involving prices are encountered often enough in various disciplines that it is useful to include as part of a unit.
- Technically count and radians are not units, they are representations of real things. A radian is a representation of rotation around a circle and is therefore distinct from a true unitless quantity even though there are no physical measurements associated with either.
- And count and mole are theoretically equivalent though as a practical matter using moles for counts of things is a bit odd for example 1 GB of data is ~1.6605\*10^-15 mol of data. So they are used in different context and don't mix very often, the convert functions does convert between them if necessary.
- This library **CANNOT** represent fractional unit powers( except for sqrt Hz used in noise density units), and it follows the order of operation in C++ so **IF** you have equations that any portion of the operation may exceed the numerical limits on powers even if the result does not, **BE CAREFUL**.
- The normal rules about floating point operations losing precision also apply to unit representations with non-integral multipliers.
- With string conversions there are many units that can be interpreted in multiple ways. In general the priority was given to units in more common use in the United States, or in power systems and electrical engineering which was the origin of this library.
- The unit `yr` has different meanings in different contexts. Currently the following notation has been adopted for string conversions `yr`=`365*day`=`8760*hr`, `a`=`365.25*day`, `annum`=`365.25*day`, `syr`=`365.24*day`. The typical usage was distinct in different contexts so this is the compromise.
- The i_flag functions such that when squared it goes to 0, similar to the imaginary number `i*conj(i)=i^0`. This is useful for directional units such as compass directions and reactive power in power systems.
- Measurement/unit equality is an interesting question. The library takes a pragmatic approach vs. a precise mathematical approach. The precision of a float is taken to be roughly 7 decimal digit of precision. A double used in the 'precise' values to be 13 digits of precision. This is sufficient to run a few operations without going out of tolerance from floating point operations. This also comes into equality which is nominally taken to be values and units within this tolerance level. So numbers are rounded to a certain number of digits then compared to within a tolerance level. Some effort was made to make this uniform, but tolerance around the last digit is not exact. Comparison operators for the units and measurements are provided. Equality and inequality use the rounded comparison; greater and less than are exact, while `>=` and `<=` check first for > or < conditions then check for equality if needed. There are a few situations that are not totally consistent like `1.0000001*m==1.0*m` and `1.0000001*m>1.0*m`, but such is nature of floating point operations. So from a mathematical purity sense this isn't consistent but does mostly what was needed. If the difference between the two values is a subnormal number the comparison also evaluates to true.

## Defined units

There are 2 sets of defined units, many common units are defined in the `units` namespace, many others are defined in `units::precise` and subnamespaces.
See [Defined Units](https://units.readthedocs.io/en/latest/user-guide/defined_units.html) for details on the available units.

### Physics constants

A set of physical and numerical constants are defined in the `units::constants` namespace. More details and a list of available constants are described in [Physical Units](https://units.readthedocs.io/en/latest/user-guide/Physical_constants.html)

## Building the library

There are two parts of the library a header only portion that can simply be copied and used. There are 3 headers `units_decl.hpp` declares the underlying classes. `unit_defintions.hpp` declares constants for many of the units, and `units.hpp` which is the primary public interface to units. If `units.hpp` is included in another file and the variable `UNITS_HEADER_ONLY` is defined then none of the functions that require the cpp files are defined. These header files can simply be included in your project and used with no additional building required.

The second part is a few cpp files that can add some additional functionality. The primary additions from the cpp file are an ability to take roots of units and measurements and convert to and from strings. These files can be built as a standalone static library or included in the source code of whatever project want to use them. The code should build with an C++11 compiler. Most of the library is tagged with constexpr so can be run at compile time to link units that are known at compile time. Unit numerical conversions are not at compile time, so will have a run-time cost. A `quick_convert` function is available to do simple conversions. with a requirement that the units have the same base and not be an equation unit. The cpp code also includes some functions for commodities and will eventually have r20 and x12 conversions, though this is not complete yet.

## Try it out

If you want to try out the string conversion components. There is server running that can do the string conversions

[Unit String Conversions](https://units.readthedocs.io/en/latest/_static/convert.html)

For more details see the [documentation](https://units.readthedocs.io/en/latest/web/index.html)

### Converter Application

A [converter](https://units.readthedocs.io/en/latest/introduction/converter.html) command line application can be built as part the units library by setting
`UNITS_BUILD_CONVERTER_APP=ON` in the CMake build. This is a simple command line script that takes a measurement entered on the command line and a unit to convert to and returns the new value by itself or part of a string output with the units either simplified or in original form.

## Usage

Many units are defined as `constexpr` objects and can be used directly

```cpp
#include "units.hpp"
using namespace units

measurement length1=45.0*m;
measurement length2=20.0*m;

measurement area=length1*length2;

std::cout<<"the area is "<<area<< " or "<<area.convert_to(ft.pow(2))<<".\n";
```

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
- `double multiplier()` return the unit multiplier as a double(regardless of how it is actually stored)
- `<float|double> cround()` round the multiplier to an appropriate number of digits
- `<unit_data> base_units()` get the base units
- `void clear_flags()` clear any flags associated with the units

For precise_units only

- `commodity()` get the commodity of the unit
- `commodity(int commodity)` assign a commodity to the precise_unit.

#### Unit Operators

There are also several operator overloads that apply to units and precise_units.

- `<unit>=<unit>*<unit>` generate a new unit with the units multiplied ie `m*m` does what you might expect and produces a new unit with `m^2`
- `<unit>=<unit>/<unit>` generate a new unit with the units divided ie `m/s` does what you might expect and produces a new unit with meters per second. NOTE: `m/m` will produce `1` it will not automatically produce a `pu` though we are looking at how to make a 'pu_m\*m=m' so units like strain might work smoothly.

- `bool <unit>==<unit>` compare two units. this does a rounding compare so there is some tolerance to roughly 7 significant digits for \<unit> and 13 significant digits for <precise_unit>.
- `bool <unit>!=<unit>` the opposite of `==`

precise_units can usually operate with a precise unit or unit, unit usually can't operate on precise_unit.

#### Unit free functions

These functions are not class methods but operate on units

- `std::hash<unit>()` generate a hash code of a unit, for things like use in std::unordered_map or other purposes.
- `<unit> unit_cast(<unit>)` convert a unit into <unit>, mainly used to convert a precise_unit into a regular unit.
- `bool is_unit_cast_lossless(<precise_unit>)` returns true if the multiplier in a precise_unit can be converted exactly into a float.
- `bool isnan(<unit>)` true if the unit multiplier is a NaN.
- `bool isinf(<unit>)` true if the unit multiplier is infinite.
- `double quick_convert(<unit>, <unit>)` generate the conversion factor between two units. This function is constexpr.
- `double quick_convert(double factor, <unit>, <unit>)` convert a specific value from one unit to another, function is constexpr but does not cover all possible conversion.
- `double convert(<unit>, <unit>)` generate the conversion factor between two units.
- `double convert(double val, <unit>, <unit>)` convert a value from one unit to another.
- `double convert(double val, <unit>, <unit>, double baseValue)` do a conversion assuming a particular basevalue for per unit conversions.
- `double convert(double val, <unit>, <unit>, double basePower, double baseVoltage)` do a conversion using base units, specifically making assumptions about per unit values in power systems.
- `bool is_error(<unit>)` check if the unit is a special error unit.
- `bool is_default(<unit>)` check if the unit is a special default unit.
- `bool is_valid(<unit>)` check to make sure the unit is not an invalid unit( the multiplier is not a NaN) and the unit_data does not match the defined `invalid_unit`.
- `bool is_temperature(<unit>)` return true if the unit is a temperature unit such as `F` or `C` or one of the other temperature units.
- `bool is_normal(<unit>)` return true if the multiplier is a normal number, there is some defined unit base, not the identity unit, the multiplier is not negative, and not the default unit. basically a simple way to check if you have some non-special unit that will behave more or less how you expect it to.
- `<unit> root(<unit>, int power)` non constexpr, take the root of a unit, produces `error` unit if the root is not well defined. power can be negative.
- `<unit> sqrt(<unit>)` convenience function for taking the sqrt of a unit.

### Measurement Operations

- `<measurement>(val, <unit>)` construct a unit from a value and unit object.
- `X value() const` get the measurement value, depending on the type this could be a double or float, or another defined type if the template is used.
- `<measurement> convert_to(<unit>) const` convert the value in the measurement to another unit base
- `<measurement> convert_to_base() const` convert to a base unit, i.e. a unit whose multiplier is 1.0
- `<unit> units() const` get the units used as a basis for the measurement
- `<unit> as_unit() const` take the measurement as is and convert it into a single unit. For Examples say a was 10 m. calling as_unit() on that measurement would produce a unit with a multiplier of 10 and a base of meters.
- `double value_as(<unit>)` get the value of a measurement as if it were measured in \<unit\>

#### Uncertain measurement methods

Uncertatin measurements have a few additional functions to support the uncertainty calculations

- `rss_add`, `rss_subtract`, `rss_product`, `rss_divide` are equivalent to the associated operator but use the root-sum of squares method for propagating the uncertainty.
- `double uncertainty()` get the numerical value of the uncertainty.
- `double uncertainty_as(<unit>)` get the uncertainty in terms of a particular unit.
- `fractional_uncertainty()` get the uncertainty as a fraction of the value.

#### Measurement operators

There are several operator overloads which work on measurements or units to produce measurements.

- `'*', '/', '+','-'` are all defined for mathematical operations on a measurement and produce another measurement.
- `%` `*`, and `/` are defined for \<measurement>\<op>\<double>
- `*`, and `/` are defined for \<double>\<op>\<measurement>

Notes: for regular measurements, `+` and `-` are not defined for doubles due to uncertainty of what that means. For fixed_measurement types this is defined as the units are known at construction and cannot change. For fixed_measurement types if the operator would produce a new measurement with the same units it will be a fixed measurement, if not it reverts to a regular measurement.

- `==`, `!=`, `>`, `<`, `>=`, `<=` are defined for all measurement comparisons
- `<measurement>=<double>*<unit>`
- `<measurement>=<unit>*<double>`
- `<measurement>=<unit>/<double>`
- `<measurement>=<double>/<unit>` basically calling a number multiplied or divided by a `<unit>` produces a measurement, specifically `unit` produces a measurement and `precise_unit` produces a precise_measurement.

#### Measurement functions

These free functions work on any of different measurement types.

- `measurement measurement_cast(<measurement>)` convert a precise_measurement into measurement
- `fixed_measurement measurement_cast(<fixed_measurement>)` convert a fixed_precise_measurement or fixed_measurement into a fixed_measurement
- `<measurement> pow(<measurement>, int)` generate a measurement which is a specific power of another measurement
- `<measurement> root(<measurement>, int)` generate a root of a measurement
- `<measurement> sqrt(<measurement>)` take the square root of a measurement of any kind, the units need to have a valid root.
- `bool is_valid(<measurement>)` will result in true if the underlying unit is valid and the value is not a nan.
- `bool isnormal(<measurement>)` will result in true if the underlying unit is normal and the value is not a nan or infinity or subnormal - zero is allowed in the measurement value, but not the unit multiplier.

### Available library functions

#### String Conversions

- `precise_unit unit_from_string( string, flags)`: convert a string representation of units into a precise_unit value.
- `unit unit_cast_from_string( string, flags)`: convert a string representation of units into a unit value NOTE: same as previous function except has an included unit cast for convenience.
- `precise_unit default_unit( string)`: get a unit associated with a particular kind of measurement. for example `default_unit("length")` would return `precise::m`
- `precise_measurement measurement_from_string(string,flags)`: convert a string to a precise_measurement.
- `measurement measurement_cast_from_string(string,flags)`: convert a string to a measurement calls measurement_from_string and does a measurement_cast.
- `uncertain_measurement uncertain_measurement_from_string(string,flags)`: convert a string to an uncertain measurement. Typically the string will have some segment with a ±, `+/-` or the html equivalent in it to signify the uncertainty.
- `std::string to_string([unit|measurement],flags=0)` : convert a unit or measurement to a string, all defined units or measurements listed above are supported. The eventual plan is to support a couple different standards for the strings through the flags, But for now they don't do much.

#### Custom Units

- `addUserDefinedUnit(std::string name, precise_unit un)` add a new unit that can be used in the string operations.
- `clearUserDefinedUnits()` remove all user defined units from the library.
- `disableUserDefinedUnits()` there is a(likely small-an additional unordered map lookup) performance hit in the string conversions functions if custom units are used so they can be disabled completely if desired.
- `enableUserDefinedUnits()` enable the use of UserDefinedUnits. they are enabled by default.

#### Commodities

The units library has some support for commodities, more might be added in the future. Commodities are supported in precise_units.

- `std::uint32_t getCommodity(std::string commodity)` get a commodity code from a string.
- `std::string getCommodityName(std::uint32_t)` get the name of a commodity from its code
- `addUserDefinedCommodity(std::string name, std::uint32_t code)` add a new commodity that can be used in the string operations.
- `clearUserDefinedCommodities()` remove all user defined commodities from the library.
- `disableUserDefinedCommodities()` there is a (likely small) performance hit in string conversions if custom commodities are used so they can be disabled completely if desired.
- `enableUserDefinedCommodities()` enable the use of UserDefinedCommodities. User defined commodities are enabled by default. Defining user specified commodities is thread-safe.

#### Other unit definitions

These are all only partially implemented, not recommended for use yet

- `precise_unit x12_unit(string)` get a unit from an X12 string.
- `precise_unit dod_unit(string)` get a unit from a DOD code string.
- `precise_unit r20_unit(string)` get a unit from an r20 code string.

## Contributions

Contributions are welcome. See [Contributing](./CONTRIBUTING.md) for more details and [Contributors](./CONTRIBUTORS.md) for a list of the current and past Contributors to this project.

## Release

This units library is distributed under the terms of the BSD-3 clause license. All new
contributions must be made under this license. [LICENSE](./LICENSE)

SPDX-License-Identifier: BSD-3-Clause

LLNL-CODE-773786
