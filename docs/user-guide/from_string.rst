=======================
Units From Strings
=======================

The units library contains a few functions to generate various types from string representations

-  `precise_unit unit_from_string(const std::string& ustring, std::uint32_t flags=0)` : will generate a precise_unit based on the data in the string
-  `unit unit_cast_from_string(const std::string& ustring, std::uint32_t flags=0)`: will generate a unit based on the data in the string
-  `precise_measurement measurement_from_string(const std::string& ustring, std::uint32_t flags=0)`: will generate a precise_measurement from the data in the string
-  `measurement measurement_cast_from_string(const std::string& ustring, std::uint32_t flags=0)`: will generate a measurement from the data in the string
-  `uncertain_measurement uncertain_measurement_from_string(const std::string& ustring, std::uint32_t flags=0)`: will generate an uncertain_measurement from the data in the string

The general form is to take a string and optionally a flag object.  See :ref:`Conversion Flags` for a detailed description of the flags.  Generally it is fine to leave off the flag argument.

Unit Strings
-------------

in general the unit string conversion is intended to be as flexible as possible.  just about any unit normally written can be converted.

For example

-  "m/s"
-  "meter/second"
-  "meter/s"
-  "metre/s"
-  "meters/s"
-  "meters per second"
-  "metres per second"
-  "m per sec"
-  "meterpersecond"
-  "METERS/s"
-  "m*s^-1"
-  "meters*seconds^(-1)"
-  "(second/meter)^(-1)"
-  "100 centimeters / 1000 ms"

Will all produce the unit of meters per second.   As a note there are quite a few more units that can be converted from strings than are listed in the :ref:`Defined Units`.
Numbers are supported and become part of the unit.  "99 feet" would create a new unit with a definition of 99 ft.  The multiplier stored would include the conversion from meters to feet*99.  This allows for arbitrary unit definitions.
The `+` operator also works if the units on both sides have the same base for example `3 ft + 2 in` would be the equivalent of `38 inches`.  If the units do not have the same base `+` is interpreted as a multiplication.

Measurement strings
--------------------

The conversion from a string to measurement looks for a leading number before the unit.  The "99 feet" in the previous example would then get a measurement value of 99 and the unit would be feet.  The measurement from string function also can interpret written numbers such as "three thousand four hundred and twenty-seven miles"  This should get correctly read as 3427 miles.

The conversion function also handles a few cases where the unit symbol is written before the value such as currency `$27.92`  would be a value of 27.92 with the currency unit.

Uncertain Measurements
---------------------------

Similarly to Measurement strings, uncertain measurements can also be converted from strings see :ref:`Uncertain Measurements` for additional details on the formats supported.
