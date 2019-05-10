# Units

[![Codacy Badge](https://api.codacy.com/project/badge/Grade/c0b5367026f34c4a9dc94ca4c19c770a)](https://app.codacy.com/app/phlptp/units?utm_source=github.com&utm_medium=referral&utm_content=LLNL/units&utm_campaign=Badge_Grade_Settings)

A library that provides runtime unit values, instead of individual unit types, for the purposes of working with units of measurement at run time possibly from user input.  

This software was developed for use in LLNL/GridDyn, and is currently a work in progress.  Namespaces, function names, and code organization is subject to change, input is welcome.    

## Purpose
A unit library was needed to be able to represent units of a wide range of disciplines and be able to separate them from the numerical values for use in calculations.  The main driver is converting units to a standardized unit set when dealing with user input and output.  And be able to use the unit as a singular type that could contain any unit, and not introduce a huge number of types to represent all possible units.  Because sometimes the unit type needs to be used inside virtual function calls which must strictly define a type.  The library also has its origin in power systems so support for per-unit operations was also lacking in the alternatives. 

It was desired that the unit representation be a compact type(<=8 bytes) that is typically passed by value, that can represent a wide assortment of units and arbitrary combinations of units.  The primary use of the conversions is at run-time to convert user input/output to/from internal units, it is not to provide strict type safety or dimensional analysis, though it can provide some of that.  It does NOT provide compile time checking of units.  The units library provides a library that supports units where many of the units in use are unknown at compile time and conversions and uses are dealt with at run time. 
### Limitations
  - The powers represented by units are limited see [Unit representation](#unit_representation) but only normal physical units are supported.
 - The library uses floating point and double precision for the multipliers which is generally good enough for most engineering contexts, but does come with the limits and associated loss of precision for long series of calculations.
 - Currency is supported as a unit but it is not recommended to use this for anything beyond basic financial calculations. So if you are doing a lot of financial calculations use something more specific for currency manipulations.  
 - Fractional unit powers are not supported in general.  While some mathematical operations on units are supported any root operations `sqrt` or `cbrt` will only produce valid results if the result is integral powers of the root.  One exception is limited support for root Hz operations in measurements of Amplitude spectral density.  so a specific definition of a unit representing square root of Hz is available.  
 - While conversions of various temperature definitions are supported, there is no generalized support for datums and bias shifts.  It may be possible to add some specific cases in the future for common uses cases but the space requirement limits such use.  Some of the other libraries have general support for this.
 - some number of equation like units is supported  these include logarithms, nepers, and some things like Saffir-Simpson, Beaufort, and Richter scales for wind and earthquakes.  There is capacity within the framework to add a few more equation like units if a need arises
 - The unit `rad` in the natures of absorbed dose is not recognized as it would conflicts with `rad` in terms of radians. So `rad` means radians
   

###  Alternatives
If you are looking for compile time and prevention of unit errors in equations for dimensional analysis one of these libraries might work for you.  
* [boost units](https://www.boost.org/doc/libs/1_69_0/doc/html/boost_units.html) - Zero-overhead dimensional analysis and unit/quantity manipulation and conversion in C++
* [Units](https://github.com/nholthaus/units) -A compile-time, header-only, dimensional analysis library built on `c++14` with no dependencies.
* [Units](https://github.com/VincentDucharme/Units) -Another compile time library
* [PhysUnits-CT](https://github.com/martinmoene/PhysUnits-CT-Cpp11) A C++ library for compile-time dimensional analysis and unit/quantity manipulation and conversion.
* [PhysUnits-RT](https://github.com/martinmoene/PhysUnits-RT)-A C++ library for run-time dimensional analysis and unit/quantity manipulation and conversion.
* [Libunits](https://sourceforge.net/projects/libunits/) - The ultimate shared library to do calculations(!) and conversions with any units!
Includes all SI and pseudo SI units and thousands of US, Imperial and other units.
* [unitscpp](http://code.google.com/p/unitscpp/) - A lightweight C++ library for physical calculation with units.

These libraries will work well if the number of units being dealt with is known at compile time.  Many also produce zero overhead operations and checking.  Therefore in situations where this is possible other libraries are a preferred alternative.  
### Types
There are only a few types in the library
 * `detail::unit_base` is the base representation of physical units and powers.  It uses a bitfield to store the base unit representation in a 4 byte representation.  It is mostly expected that unit_base will not be used in a standalone context but through one of other types.
 * `unit` is the primary type representing a physical unit it consists of a `float` multiplier along with a `unit_base` and contains this within an 8 byte type.  The float has an accuracy of around 6 decimal digits.  Units within that tolerance will compare equal.  
 * `precise_unit` is the a more accurate type representing a physical unit it consists of a `double` multiplier along with a `unit_base` and contains this within an 16 byte type.  The float has an accuracy of around 13 decimal digits.  Units within that tolerance will compare equal. 
 * `measurement` is a 16 byte type containing a double value along with a `unit` and mathematical operations can be performed on it usually producing a new measurement. `measurement` is an alias to a `measurement_base<double>` so the quantity type can be templated.  `measurement_f` is an alias for `measurement_base<float>` but others could be defined
 * `precise_measurement` is similar to measurement except using a double for the quantity and a `precise_unit` as the units.  

### Unit representation
The unit class consists of a multiplier and a representation of base units. 
The seven SI units + radians + currency units + count units.  in addition a unit has 4 flags,  per-unit for per unit or ratio units. two flags for a variety of purposes and to differentiate otherwise similar units. And a flag to indicate an equation unit. Due to the requirement that the base units fit into a 4 byte type the represented powers of the units are limited.  The list below shows the bit representation range and observed range of use in equations and observed usage

 - meter:[-8,+7]  :normal range [-4,+4], intermediate ops [-6,6]
 - kilogram:[-4,+3] :normal range [-1,+1], intermediate ops [-2,2]
 - second:[-8,+7] :normal range [-4,+4], intermediate ops [-6,6]
 - ampere:[-4,+3] :normal range [-2,+2]
 - kelvin:[-4,+3] :normal range [-4,+1]
 - mole:[-2,+1] :normal range [-1,+1]
 - candela:[-2,+1] :normal range [-1,+1]
 - currency:[-2,+1] :normal range [-1,+1]
 - count:[-2,+1] :normal range [-1,+1]
 - radians:[-4,+3] :normal range [-2,+2]

  These ranges were chosen to represent nearly all physical quantities that could be found in various disciplines we have encountered.  

#### Discussion points
 * Currency may seem like a unusual choice in units but numbers involving prices are encountered often enough in various disciplines that it is useful to include as part of a unit.  
 * Technically count and radians are not units, they are representations of real things. A radian is a representation of rotation around a circle and is therefore distinct from a true unitless quantity even though there are no physical measurements associated with either. 
 * And count and mole are theoretically equivalent though as a practical matter using moles for counts of things is a bit odd for example 1 GB of data is ~1.6605*10^-15 mol of data.  So they are used in different context and don't mix very often
 * This library CANNOT represent fractional unit powers, and it follows the order of operation in C++ so IF you have equations that any portion of the operation may exceed the numerical limits on powers even if the result does not BE CAREFUL. 
 * The normal rules about floating point operations losing precision also apply to unit representations with non integral multipliers.
 * In general with string conversions there are many units that can be interpreted in multiple ways.  In general the priority was given to units in more common use.  
 * The unit `yr` has different meanings in different contexts.  Currently the following notation has been adopted for string conversions `yr`=`365*day`=`8760*hr`,  `a`=`365.25*day`, `annum`=`365.25*day`, `syr`=`365.24*day`.  The typical usage was distinct in different contexts so this is the compromise.  

# Defined units
There are 2 sets of defined units
they are in the namespace `units`  these are the lower precision types
the high precision units are in the namespace `units::precise`.
Also defined are a number of physical constants in `units::constants`
```cpp
/// Standard gravity
constexpr precision_measurement g0(9.80665, precise::m / precise::s / precise::s);
/// Gravitational Constant
constexpr precision_measurement
  G(6.6740831e-11, precise_unit(detail::unit_data(3, -1, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0)));
/// Speed of light
constexpr precision_measurement c{299792458.0, precise::m / precise::s};
/// Elementary Charge (2019 redefinition)
constexpr precision_measurement e(1.602176634e-19, precise::C);
///  hyperfine structure transition frequency of the caesium-133 atom
constexpr precision_measurement fCs(9192631770.0, precise::Hz);
/// Planck constant (2019 redefinition)
constexpr precision_measurement h{6.62607015e-34, precise::J *precise::second};
/// Boltzman constant (2019 redefinition)
constexpr precision_measurement k{1.380649e-23, precise::J / precise::K};
/// Avogadros constant (2019 redefinition)
constexpr precision_measurement Na{6.02214076e23, precise::one / precise::mol};
/// Luminous efficiency
constexpr precision_measurement Kcd{683.0, precise::lm / precise::W};
/// Permittivity of free space
constexpr precision_measurement eps0{8.854187817e-12, precise::F / precise::m};
/// Permeability of free space
constexpr precision_measurement u0{12.566370614e-7, precise::N / (precise::A * precise::A)};
/// Gas Constant
constexpr precision_measurement R{8.314459848, precise::J / (precise::mol * precise::K)};
/// Stephan Boltzmann constant
constexpr precision_measurement sigma{5.67036713e-8,
                                      precise_unit(detail::unit_data(0, 1, -3, 0, -4, 0, 0, 0, 0, 0, 0, 0))};

```

# Building the library
There are two parts of the library  a header only portion that can simply be copied and used.  And a few cpp file that can add some additional functionality.  The primary additions from the cpp file are an ability to take roots of units and measurements and convert to and from strings.  These files can be built as a standalone static library or included in the source code of whatever project want to use them.  The code should build with an C++11 compiler.    Most of the library is tagged with constexpr so can be run at compile time to link units that are known at compile time.  Unit numerical conversions are not at compile time, so will have a run-time cost.   

# How to use the library

# Available function

`unit_from_string( string, flags)`: convert a string representation of units into a unit value.  


# Release
This units library is distributed under the terms of the BSD-3 clause license. All new
contributions must be made under this license. [LICENSE](LICENSE)

SPDX-License-Identifier: BSD-3-Clause

LLNL-CODE-773786
