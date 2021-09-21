==================
Unit Domains
==================

There are some ambiguous unit symbols.  Different fields use the same symbol to mean different things.  In the units library the definition has defaulted to SI standard definition with two known ambiguities.  the symbol 'a' is used for `are`,  the symbol `rad` refers to radians.

However there are occasions where the units from one field or another are desired.  The units library applies the notion of a unit domain which can be passed to the unit_flags argument for any string conversion, for a few select units this will change the resulting from a string.

Available Domains
----------------------
thus far 5 specific unit domains have been defined they are in the
`units::domains`  namespace.

-   `ucum`  -- THE UNIFIED CODE FOR UNITS OF MEASURE
-   `cooking`  -- units and symbols commonly used for recipes
-   `astronomy`  -- units and symbols used in astronomy
-   `nuclear`    -- units and symbols used in nuclear or particle physics
-   `surveying`  -- units and symbols used in surveying in the United states
-   `us_customary`  --units and symbols traditionally used in the us(combination of cooking and surveying)

The only units and symbols using the domain are those that might be ambiguous or contradictory to the ST definition.  The specific units affected are defined in the next section.

Domain Specific Units
-----------------------

These are unit definitions affected by specifying a specific unit domain

UCUM
++++++++++
-  `B`  -- bel
-  `a`  -- julian year

Astronomy
++++++++++++
-  `am`  -- arc minute
-  `as`  -- arc second
-  `year` -- mean tropical year

Cooking
++++++++++
-   `C`  -- cup
-   `T`  -- Tablespoon
-   `c`  -- cup
-   `t`  -- teaspoon
-   `TB`  -- Tablespoon

Surveying
++++++++++++
-   `'` and all variants refer to feet
-   `''` and all variants refer to inches

Nuclear
++++++++++

-   `rad`  radiation absorbed dose
-   `rd`   same as `rad`

US customary
++++++++++++++++

Combination of surveying and cooking


More units will likely be added to this as the need arises

Specifying the domain
--------------------------

The domain can be specified in the unit_flag string supplied to the `unit_from_string` operation.

.. code-block:: c++

   auto unit1=units::unit_from_string(unitString,nuclear_units)

when used as part of the flags argument the definitions are in the `unit_conversion_flags` enumeration

-   strict_ucum
-   `cooking_units`
-   `astronomy_units`
-   `surveying_units`
-   `nuclear_units`
-   `us_customary_units`


A default domain can also be specified though

.. code-block:: c++

   setUnitsDomain(code);


with the code using one of those found in the `units::domains` namespace.
this domain will be used unless another is specified through the match flags.

The default domain can be set at compile time through the `UNITS_DEFAULT_DOMAIN` definition which is available through CMake options or specified before compilation.

.. code-block:: c++

   #define UNITS_DEFAULT_DOMAIN units::domains::astronomy
   #include "units/units.hpp"




