==================
Special Units
==================

There are a few defined units that are special in some fashion, and can be used as sentinel values or have special operations associated with them.

Default Unit
-------------

The `defunit` unit is allowed to be converted to any other unit.  it is equivalent of `per-unit*i_flag`  The main use case is in the convert functions and makes a good


Error Unit
-----------

.. code-block:: c++

   auto error_unit=unit(detail::unit_data(nullptr));

Invalid Unit
-------------

An invalid unit is any unit that is either the error unit or has a NaN in the multiplier.  This is the unit returned from a string conversion if the string does not describe a unit or measurement.

one
------

The default constructor for `unit` and `precise_unit` is empty unit data and 1.0 in the multiplier.


There are also precise versions of these values in the `precise` namespace
