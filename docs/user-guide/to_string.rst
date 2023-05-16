=======================
Units To Strings
=======================

All the class in the units library can be given as an argument to a `to_string` function.  This function converts the units or value into a `std::string` that is representative of the unit or measurement.  In all cases the primary aim of the `to_string` is to generate a string that the corresponding `*_from_string` function will recognize and convert back to the original unit.  The secondary aim is to generate string that is human readable in standard notation.  While this is achieved for many common units there is some work left to do to make it better.

For example

.. code-block:: c++

   measurement density=10.0*kg/m.pow(3);
   measurement meas2=2.7*puMW;

   auto str1=to_string(density);
   auto str2=to_string(meas2);

   // from google tests
   EXPECT_EQ(str1, "10 kg/m^3");
   EXPECT_EQ(str2, "2.7 puMW");


.. code-block:: c++

   uncertain_measurement um1(10.0, 0.4, m);
   auto str = to_string(um1);
   EXPECT_EQ(str, "10+/-0.4 m");

Uncertain measurement string conversions make some attempt to honor significant digits based on the uncertainty.

Advanced Usage
----------------
The `to_string` function also takes a second argument which is a `std::uint64_t match_flags` in all cases this default to 0,  it is currently unused though will be used in the future to allow some fine tuning of the output in specific cases.  In the near future a flag to allow utf 8 output strings will convert certain units to more common utf8 symbols such as unit Powers and degree symbols, and a few others.  The output string would default to ascii only characters.

Stream Operators
----------------

Output stream operators are NOT included in the library.  It was debatable to include them or not but there would be a lot of additional overloads that would add quite a bit of code to the header files, that in most cases is not necessary so the decision was made to exclude them.  The `to_string` operations provide most of the capabilities with some additional flexibility, and if needed for a particular use case can be added to the user code in a simple fashion

.. code-block:: c++

   namespace units{
      std::ostream& operator<<(std::ostream& os, const precise_unit& u)
      {
          os << to_string(u);
          return os;
      }
   } // namespace units


Any of the types in the units library with a `to_string` operation can be handled in the same way.  Depending on the compiler, placing the operator in the namespace may or may not be necessary.

Underlying Conversion Map Access
----------------------------------

The underlying conversion maps may be accessed by users if desired.
To access them a compile time definition needs to be added to the build `ENABLE_UNIT_MAP_ACCESS`

.. code-block:: c++

   #ifdef ENABLE_UNIT_MAP_ACCESS
   namespace detail {
       const std::unordered_map<std::string, precise_unit>& getUnitStringMap();
       const std::unordered_map<unit, const char*>& getUnitNameMap();
   }
   #endif

These may be useful for building a GUI or smart lookup or some other operations.  `getUnitStringMap()` returns a map of known unit strings, and `getUnitNameMap()` is a mapping of common units back to strings as a building block for the `to_string` operation.
