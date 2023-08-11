=======================
Math Operations
=======================

Additional mathematical operations on measurements are available in the `unit_math.hpp` header these are header only so no additional compilation is required.  The intention of this header is to match operations from the `cmath` header available in the standard library.  These are all template functions which will work for any measurement type.

Type traits
----------------------

The header includes a few type traits used in the header file and potentially useful elsewhere including

-  `is_unit` : true if the type is a unit (unit or precise_unit)
-  `is_measurement` : true if the type is one of the defined measurement types
-  `is_precise_measurement` : true if the type is one of the defined precise measurement type

Rounding and Truncation
-------------------------
These operations will effect only the value part of the measurement

-   `round`
-   `trunc`
-   `ceil`
-   `floor`

Trigonometric functions
-------------------------

Trigonometric operations will operate only if the measurement is convertible to radians

-  `sin`
-  `cos`
-  `tan`

Multiplies and divides
-------------------------------

Division and multiplication operators for measurements that have support for per_unit measurement

- `multiplies` : works like `*` except when one of the measurements is per_unit and they have the same unit base, then they remove the per unit
- `divides` : works like `/` except if both measurements have the same base then the result has a `per_unit` unit

See :ref:`strain` for examples on usage

Others
---------

Other common mathematical expressions found in `<cmath>`

- `fmod` : return the floating point modulus of a division operation as long as division is a valid operation
- `hypot` : works for two and three measurements or floating point values as long as addition is a valid operation.
- `cbrt` :  works similarly to the `sqrt` operation
