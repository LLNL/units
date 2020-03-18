
----------------------------
Installation and Linking
----------------------------

The units library supports a header only mode and a compiled mode.  One of the strengths of the library is the string processing which is only available in the compiled mode.  Other additions from the compiled mode are the root operations on units and measurements.    The header only mode includes the unit and measurement classes conversions between them and the definition library.

Header Only Use
-----------------

The header only portion of the library can simply be copied and used. There are 3 headers `units_decl.hpp` declares the underlying classes. `unit_defintions.hpp` declares constants for many of the units, and `units.hpp` which is the primary public interface to units. If `units.hpp` is included in another file and the variable `UNITS_HEADER_ONLY` is defined then none of the functions that require the cpp files are defined. These header files can simply be included in your project and used with no additional building required.  The `UNITS_HEADER_ONLY` definition is needed otherwise linking errors will result.

Compiled Usage
----------------

The second part is a few cpp files that can add some additional functionality. The primary additions from the cpp file are an ability to take roots of units and measurements and convert to and from strings. These files can be built as a standalone static library, a shared library or an object library, or included in the source code of whatever project want to use them. The code should build with an C++11 compiler. C++14 is recommended if possible to allow some additional function to be `constexpr`.  Most of the library is tagged with constexpr so can be run at compile time to link units that are known at compile time. General Unit numerical conversions are not at compile time, so will have a run-time cost. A `quick_convert` function is available to do simple conversions. with a requirement that the units have the same base and not be an equation unit. The cpp code also includes some functions for commodities and will eventually have r20 and x12 conversions, though this is not complete yet.

Standalone Library
-------------------
The units library can be built as a standalone library with either the static or shared library and installed like a typical package.


.. toctree::
   :maxdepth: 1

   cmake_variables
