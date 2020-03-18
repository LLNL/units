============
Units
============

Basic Unit Types
-----------------

There are two basic units classes
`units` and `precise_units`  They both include a `units_base`  see :ref:`Unit base class` for the details.
`units` has a single precision floating point multiplier and the `units_base` object.   The `precise_unit` type uses a double precision floating point multiplier and includes commodity.  The commodity is represented by a 32-bit code.  See :ref:`Commodities` for more details on how that is used and defined.
The simplest way to start is by using one of the :ref:`Defined units`  All standard units are defined and many non-standard ones as well.

The Basics of units are the seven SI base units:

-  the kilogram (kg), for mass.
-  the second (s), for time.
-  the kelvin (K), for temperature.
-  the ampere (A), for electric current.
-  the mole (mol), for the amount of a substance.
-  the candela (cd), for luminous intensity.
-  the meter (m), for distance.

In addition to the base SI units a couple additional bases are defined:

-  radian(rad), for angular measurement
-  Currency ($), for monetary values
-  Count (cnt),  for single object counting


Currency may seem like a unusual choice in units but numbers involving prices are encountered often enough in various disciplines that it is useful to include as part of a unit.
Technically count and radians are not units, they are representations of real things. A radian is a representation of rotation around a circle and is therefore distinct from a true unitless quantity even though there are no physical measurements associated with either.
And count and mole are theoretically equivalent though as a practical matter using moles for counts of things is a bit odd for example `1 GB` of data is `~1.6605*10^-15 mol` of data. So they are used in different context and don't mix very often, the convert functions can convert between them if necessary.

The structure also defines some flags:

-  `per-unit`, indicating per unit units
-  `i_flag`,  general flag and complex quantity
-  `e_flag`,  general unit discriminant
-  `equation`,  indicator that the unit is an equation unit.

Derived Units
+++++++++++++++
A vast majority of physical units can be constructed using these bases, as well as many non-physical units.  The entire structure for the units fits into 4 bytes to meet the design requirement for a compact type.  This required a detailed evaluation of what physical units and combinations of them were in use in different scientific and commercial disciplines,  The following list represents the range of allowed values chosen as the representation and those required by known and observed physical quantities.

-   meter:\[-8,+7\]  \:normal range \[-4,+4\], intermediate ops \[-6,+6\]
-   kilogram:\[-4,+3\] \:normal range \[-1,+1\], intermediate ops \[-2,+2\]
-   second:\[-8,+7\] \:normal range \[-4,+4\], intermediate ops \[-6,+6\]
-   ampere:\[-4,+3\] \:normal range \[-2,+2\]
-   kelvin:\[-4,+3\] \:normal range \[-4,+1\]
-   mole:\[-2,+1\] \:normal range \[-1,+1\]
-   candela:\[-2,+1\] \:normal range \[-1,+1\]
-   currency:\[-2,+1\] \:normal range \[-1,+1\]
-   count:\[-2,+1\] \:normal range \[-1,+1\]
-   radians:\[-4,+3\] \:normal range \[-2,+2\]

For example the kilogram is rarely used in a squared context, so it has a normal range of between -1 and 1.  But in intermediate mathematical operations it is squared on occasion, so we needed to be able represent that without overflow.  Since without getting extraordinary complex we are limited to whole bit representation that infers a two's complement notation of 2 bits is [-2,-1] for 3 bits [-4,+3], and for 4 bits [-8,+7].  So for kilograms 3 bits were used.    The pu flag was determined to be required by the initial design considerations, and a flag value also turned out to be required by library design requirements.  The equation and e_flag flags came a little later in the library development but turned out to be very useful in representing other kinds of units and discriminating between some units.


Basic operations
-----------------
Some mathematical operations between units are supported.  `*` and `/` with units produce a new unit.

.. code-block:: c++

   auto new_unit=m/s;
   auto another=new_unit*s;
   //another == m

produces a `new_unit` equivalent to meters/second.

Comparison Operators
----------------------

Units also support the comparison operators  `==`, and  `!=`.The other comparison operators are not supported as it is somewhat undefined whether m > kg or many other comparison like that.  The inequality is the inverse of equality, but the equality operator is an interesting subject.  The unit component is relatively straightforward that part is the base units,  if those are not equivalent then the answer is false.  However there is a floating point component to the unit representing a multiplier.  And floating point equality is treacherous.  What is done is a rounding operation with a range.  Basically `units` are assuming to have 6 decimal digits of precision, while `precise_units` have 13.  So units will result in equality as long as the first X significant digits in the multiplier are equivalent and the `unit_base` is equal.  this can't be a specific range since the power of the multiplier is wide ranging this parsecs to picometers and all the base of meters.

Methods
---------
Frequently units need to raised to some power.  Units have a `pow(int)` method to accomplish this.

.. code-block:: c++

   auto area_unit=m.pow(2);

The `^` will not work due to precedence rules in C++.  If an operator for '^' were defined an operation such as m/s^2  would produce meters squared per second squared which is probably not what is expected.  Therefore best not to define the operator and use a function instead.

.. toctree::
   :maxdepth: 1

   special_units
   custom_units
   equation_units
   custom_count_units
