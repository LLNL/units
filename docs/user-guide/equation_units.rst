==================
Equation Units
==================

The use of an equation flag in the unit_base defines a set of equation units.  These are specific units where the relationship with other units is defined through an equation rather than a specific multiplier.  There are 31 available equation units.  Equation units use up the flags, count, and radian fields.  All other units are left alone for defining the underlying units of the equation unit.  So the equation specifier defines and equation rather than a specific unit.
equation types 0-15 deal with logarithms in some way, 16-31 are undefined or represent some common scale type units


to extract the equation type
-  unsigned short precise::custom::eq_type(detail::unit_data udata);

Current equation definitions

-   0: log10(x)
-   1: nepers
-   2: bels
-   3: decibels
-   4: -log10(x)
-   5: -log10(x)/2.0
-   6: -log10(x)/3.0
-   7: -log10(x)/log10(50000)
-   8: log2(x)
-   9: ln(x)
-   10: log10(x)
-   11: 10*log10(x)
-   12: 2*log10(x)
-   13: 20*log10(x)
-   14: log10(x)/log10(3)
-   15: 0.5*ln(x)
-   16: API Gravity
-   17: Degree Baume Light
-   18: Degree Baume Heavy
-   19: UNDEFINED
-   20: UNDEFINED
-   21: UNDEFINED
-   22: saffir-simpson hurricane wind scale
-   23: Beaufort wind scale
-   24: Fujita scale
-   25: UNDEFINED
-   26: UNDEFINED
-   27: Prism diopter-100.0*tan(x)
-   28: UNDEFINED
-   29: Moment magnitude scale for earthquakes (richter)
-   30: Energy magnitude scale for earthquakes
-   31: UNDEFINED


The wind scales are not very accurate since they match up a slightly fuzzier notion to actual wind speed.  There are general charts and the equations in use utilize a polynomial to approximate them to a continuous scale.  So the units when used are generally convertible to a velocity unit such as m/s.  There are currently 10 undefined equation units available if needed.
The density scales (API Gravity, Degree Baume Heavy and Light ) are based on handbook definitions at typical temperature scales.  A few more like this might be added in the near future.
Equation Value conversions
-----------------------------
The actual definitions of the equations are found in the `unit::precise::equation` namespace.  Two functions are provided that convert values from equation values to units and vice versa.

-   `double convert_equnit_to_value(double val, detail::unit_data UT)`
-   `double convert_value_to_equnit(double val, detail::unit_data UT)`

also since some equation unit definitions depend on whether the actual units are power or magnitude values, there is a helper function to help determine this.
`bool is_power_unit(detail::unit_data UT)`
This applies in the neper, bel, and decibel units.
