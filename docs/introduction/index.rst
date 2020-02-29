==============
Introduction
==============

Why?
-----
So why have another units library?  This was something we poked at for a while before writing the library.
There are number of other alternatives in what are well designed C++ libraries but none of them met our needs. Some needs are pretty general, and others specific to power systems and electrical engineering.

Design Requirements
++++++++++++++++++++

-   have a units type that can be used in virtual function calls
-   Handle unit conversions
-   handle Per Unit operations and unit conversions
-   handle complex units easily like `$/puMw/hr`
-   Operate on strings with conversion to and from strings

Previously the library we were using met these requirements but only for a very limited set of units.  This library functioned but was nearing the limits of maintainability and operation as new units were needed and other conversions were required which required adding direct conversions between the classes of units and the code was getting to be a mess.  So looking around many of the existing unit libraries in C++ represent units as a type.  This works wonderfully if you can know all the types you want to use ahead of time.  In our case many of the conversions depended on configuration or input files so the units being converted were not known at compile time.  A few were but in general they were now.  And that led to an issue of how to do you pass that unit to a function that is meant to hide the internal unit in use, So having a type per unit did not seem functional from a coding or structural perspective.

Many of the dimensional analysis libraries, actually all as far as I can tell in C++ do not support string conversions.  There are a few examples in Java or Python, but our code is written in C++ so that didn't seem workable either.  Which led to the conclusion that I guess this is needed and some additional design considerations.

-  A small single compact type to represent all units (no bigger than a double) if we wanted to use it in a number of contexts.
-  Another measurement type that was interoperable with doubles and numbers.  Many numerical calculations came from a `real` array and numeric solver libraries so there is no opportunity to modify those types.  Which means we need double operations with measurements if we want to use them.
-  `constexpr` as much as possible since many units are known at compile time and we need to be able to generate complex units from others simpler units.
-   The library should be compatible with a broad range of compilers including some older ones back to GCC 4.7.
-   a fairly expansive list of predefined units to simplify operation

Speaking with others, a few items and contexts came up such as recipes, trade documents, other software package unit representations,  standardized string representation.  And sometimes a lot of precision is needed other times this is not the case.

Down the road deal with uncertainties in measurements and commodities, and containers.

.. toctree::
   :maxdepth: 1

   how
   testing
   sources
   converter
