==================
Unit Domains
==================

There are some ambiguous unit symbols.  Different fields use the same symbol to mean different things.  In the units library the definition has defaulted to SI standard definition with two known ambiguities.  the symbol 'a' is used for `are`,  the symbol `rad` refers to radians.

However there are occasions where the units from one field or another are desired.  The units library applies the notion of a unit domain which can be passed to the unit_flags argument for any string conversion, for a few select units this will change the resulting from a string.

Available Domains
----------------------
thus far 5 specific unit domains have been defined they are in the
`units::domains`  namespace.

-   `ucum`            -- THE UNIFIED CODE FOR UNITS OF MEASURE
-   `cooking`         -- units and symbols commonly used for recipes
-   `astronomy`       -- units and symbols used in astronomy
-   `nuclear`         -- units and symbols used in nuclear or particle physics
-   `surveying`       -- units and symbols used in surveying in the United states
-   `us_customary`    -- units and symbols traditionally used in the us(combination of cooking and surveying)
-   `climate`         -- units and symbols used in climate science
-   `allDomains`      -- this domain does all the above domains where not mutually exclusive. So mostly a combination of ucum and astronomy/nuclear with a few us customary units IT is not recommended to use this but provided for the combinations

The only units and symbols using the domain are those that might be ambiguous or contradictory to the ST definition.  The specific units affected are defined in the next section.

Domain Specific Units
-----------------------

These are unit definitions affected by specifying a specific unit domain

UCUM
++++++++++
-  `B`  -- bel vs Byte
-  `a`  -- julian year vs are

Astronomy
++++++++++++
-  `am`  -- arc minute vs attometer
-  `as`  -- arc second vs attosecond
-  `year` -- mean tropical year vs median calendar year

Cooking
++++++++++
-   `C`  -- cup vs coulomb
-   `T`  -- Tablespoon vs Tesla
-   `c`  -- cup vs speed of light
-   `t`  -- teaspoon vs metric tonne
-   `TB`  -- Tablespoon vs TeraByte
-   `smi` -- smidgen vs square mile
-   `scruple` -- slightly different definition when used in cooking context
-   `ds`  -- dessertspoon vs deci second


Surveying
++++++++++++
-   `'` and all variants refer to feet vs arcmin
-   `''` and all variants refer to inches vs arcsec

Nuclear
++++++++++

-   `rad`  radiation absorbed dose vs radian
-   `rd`   same as `rad` vs rod

Climate
++++++++++

-   `kt`  kilo-tonne vs karat

US customary
++++++++++++++++

Combination of surveying and cooking

All domains
++++++++++++

Combination of all of the above


More units will likely be added to this as the need arises

Specifying the domain
--------------------------

The domain can be specified in the unit_flag string supplied to the `unit_from_string` operation.

.. code-block:: c++

   auto unit1=units::unit_from_string(unitString,nuclear_units)

when used as part of the flags argument the definitions are in the `unit_conversion_flags` enumeration

-   `strict_ucum`
-   `cooking_units`
-   `astronomy_units`
-   `surveying_units`
-   `nuclear_units`
-   `climate_units`
-   `us_customary_units`


A default domain can also be specified though

.. code-block:: c++

   setUnitsDomain(code);


with the code using one of those found in the `units::domains` namespace.
this domain will be used unless another is specified through the match flags.  This function return the previous domain which can be used if only setting the value temporarily.

The default domain can be set at compile time through the `UNITS_DEFAULT_DOMAIN` definition

.. code-block:: c++

   #define UNITS_DEFAULT_DOMAIN units::domains::astronomy
   #include "units/units.hpp"

In CMake this field can be defined and will be directly translated.  The `UNITS_DOMAIN` CMake variable can also be used to specify a domain as a string like `UCUM` or `COOKING` and have it appropriately translate.
See :ref:`Unit Library CMake Reference` for more details.
