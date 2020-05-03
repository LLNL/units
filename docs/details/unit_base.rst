---------------------------
Unit base class
---------------------------

The unit base class is a bitmap comprising segments of a 32 bit number.  all the bits are defined

the underlying definition is a set of bit fields that cover a full 32 bit unsigned integer

.. code-block:: c++

   // needs to be defined for the full 32 bits
        signed int meter_ : 4;
        signed int second_ : 4; // 8
        signed int kilogram_ : 3;
        signed int ampere_ : 3;
        signed int candela_ : 2; // 16
        signed int kelvin_ : 3;
        signed int mole_ : 2;
        signed int radians_ : 3; // 24
        signed int currency_ : 2;
        signed int count_ : 2; // 28
        unsigned int per_unit_ : 1;
        unsigned int i_flag_ : 1; // 30
        unsigned int e_flag_ : 1; //
        unsigned int equation_ : 1; // 32


The default constructor sets all the fields to 0.  But this is private and only accessible from friend classes like units.

The main constructor looks like

.. code-block:: c++

    constexpr unit_data(
            int meter,
            int kilogram,
            int second,
            int ampere,
            int kelvin,
            int mole,
            int candela,
            int currency,
            int count,
            int radians,
            unsigned int per_unit,
            unsigned int flag,
            unsigned int flag2,
            unsigned int equation)

an alternative constructor

.. code-block:: c++

   explicit constexpr unit_data(std::nullptr_t);

sets all the fields to 1

Math operations
-------------------

When multiplying two base units the powers are added.
For the flags.  The e_flag and i_flag are added, effectively an Xor while the pu and equation are ORed.

For division the units are subtracted, while the operations on the flags are the same.

Power and Root and Inv functions
++++++++++++++++++++++++++++++++++

For power operations all the individual powers of the base units are multiplied by the power number.
The pu and equation flags are passed through.  For even powers the i_flag and e_flag are set to 0, and odd powers they left as is.
For root operations, First a check if the root is valid, if not the error unit is returned.  If it is a valid root all the powers are divided by the root power.  The Pu flag is left as is, the i_flag and e_flag are treated the same is in the pow function and the equations flag is set to 0.

There is one exception to the above rules.  There is a special unit for √Hz   it is a combination of some i_flag and e_flag and a high power of the seconds unit.  This unit is used in amplitude spectral density and comes up on occasion in some engineering contexts.  There is some special logic in the power function that does the appropriate things such the square of √Hz= Hz.    If a low power of seconds is multiplied or divided by the special unit it still does the appropriate thing.  But  `√Hz*√Hz` will not generate the expected result.  √Hz is a singular unique unit and the only coordinated result is a power operation to remove it.  √Hz unit base itself uses a power of (-5) on the seconds and sets the i_flag and e_flag.

The inverse function is equivalent to pow(-1), and just inverts the unit_data.

Getters
-----------

The unit data type supports getters for the different fields all these are constexpr methods

-  `meter()`:  return the meter power
-  `kg()`:  return the kilogram power
-  `second()`:  return the seconds power
-  `ampere()`:  return the ampere power
-  `kelvin()`:  return the kelvin power
-  `mole()`:  return the mole power
-  `candela()`:  return the candela power
-  `currency()`:  return the currency power
-  `count()`:  return the count power
-  `radian()`:  return the radian power
-  `is_per_unit()`: returns true if the unit_base has the per_unit flag set
-  `is_equation()`: returns true if the unit_base has the equation field set
-  `has_i_flag()`: returns true if the i_flag is set
-  `has_e_flag()`: returns true if the e_flag is set

-  `empty()`:  will check if the unit_data has any of the base units set,  flags are ignored.
-  `unit_type_count`:  will count the number of base units with a non-zero power

Modifiers
--------------

there are a few methods will generate a new unit based on an existing one the methods are constexpr

-  `add_per_unit()`: will set the per_unit flag
-  `add_i_flag()`: will set the i_flag
-  `add_e_flag()`: will set the e_flag

The method `clear_flags` is the only non-const method that modifies a unit_data in place.

Comparisons
---------------
Unit data support the `==` and `!=` operators.  these check all fields.

There are a few additional comparison functions that are also available.

- `equivalent_non_counting(unit_base other)` : will return true if all the units but the counting units are equal,  the counting units are mole, radian, and count.
- `has_same_base(unit_base other)`: will return true if all the units bases are equivalent.  So the flags can be different.
