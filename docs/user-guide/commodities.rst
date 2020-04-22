=======================
Commodities
=======================

the `precise_unit` class can represent commodities as well as units.  The commodity is represented by a 32 bit unsigned number that codes a variety of commodities.
The actual representation is still undergoing some change so expect this to change going forward. See :ref:`Commodity Details` for more details.


Methods
==============
There are a few available methods for dealing with commodity codes and string translation

- `std::uint32_t getCommodity(std::string comm)` - will get a commodity from a string.
- `std::string getCommodityName(std::uint32_t commodity)` - will translate a commodity code into a string


Custom Commodities
++++++++++++++++++++

- `void addCustomCommodity(std::string comm, std::uint32_t code)` - Add a custom commodity code using a string and code
- `void clearCustomCommodities()` - clear all current user defined commodities
- `void disableCustomCommodities()` - Turn off the use of custom commodities
- `void enableCustomCommodities()` - Turn on the ability to add and check custom commodities for later access


Commodities to names
=====================
The `getCommodityName` methods has 4 stages and will return from any successful stage.

1.  Check custom commodities if allowed
2.  Check standardized commodity names
3.  Check for special codes for name storage (short names <=5 ascii lower case characters are stored directly in the code)
4.  generate string "CXCOMM[<code>]"

String To Commodities
======================
The `getCommodity` method works nearly the opposite of `getCommodityName`.

1.  Check custom commodities if allowed
2.  Check standardized commodity codes
3.  Check for string "CXCOMM[<code>]"
4.  Check for special codes for name storage (short names <=5 ascii lower case characters are stored directly in the code)
5.  Generate a hash code of the string and if allowed store it as a custom commodity

Defined Commodities
=====================
The list of commodities is still in development.  Generally `traded commodities<https://en.wikipedia.org/wiki/List_of_traded_commodities>`_ are available as well as a few others that are used in clinical definitions or other uses as part of unit definition standards.  In the future this list will more generally expand to match international trade tables.  See `commodities.cpp<https://github.com/LLNL/units/blob/master/units/commodities.cpp>`_ for details on the exact list.
