==================
User defined units
==================

The units library has support for user defined units and :ref:`Commodities`.  These interact with the `*_from_string` and `to_string` operations to allow custom conversions and definitions.

Defining a custom unit
--------------------------

The basic function for adding a custom unit is `addUserDefinedUnit(string name, precise_unit un)`

For example from a test in the library

.. code-block:: c++

   precise_unit idgit(4.754, mol/m.pow(2));
   addUserDefinedUnit("idgit", idgit);

   auto ipm=unit_from_string("idgit/min");
   EXPECT_EQ(ipm, idgit / min);

   auto str = to_string(ipm);
   EXPECT_EQ(str, "idgit/min");

   str = to_string(ipm.inv());
   EXPECT_EQ(str, "min/idgit");

Basically user defined units can interact with the string conversion functions just like any other unit defined in the library.

If only an ability to interpret strings is needed the `addUserDefinedInputUnit` can be used

.. code-block:: c++

   precise_unit idgit(4.754, mol/m.pow(2));
   addUserDefinedInputUnit("idgit", idgit);

   auto ipm = unit_from_string("idgit/min");
   EXPECT_EQ(ipm, idgit / min);

   auto str = to_string(ipm);
   EXPECT_EQ(str.find("idgit"), std::string::npos);
   EXPECT_NE(str.find("kat") , std::string::npos);

Input File
------------------
Sometimes it is useful to have a larger library of units in this case the `std::string definedUnitsFromFile(const std::string& filename)` can be used to load a number of units at once.

The file format is quite simple.
`#` at the beginning of a line indicates a comment
other wise ::

   # comment
   meeter == meter
   meh == meeter per hour
   # => indicates input only unit
        mehmeh => meh/s

or ::

   # comment
   yodles=73 counts

   # comment
   "yeedles", 19 yodles

   yimdles; dozen yeedles

or ::

   # test the quotes for inclusion
   "bl==p"=18.7 cups

   # test single quotes for inclusion
   'y,,p',9 tons

   # ignore just one quote
   'np==14 kg

   # escaped quotes
   "j\"\""= 13.5 W

   # escaped quotes
   'q""'= 15.5 W

The basic rule is that one of `[=,;]` will separate a definition name from a unit definition.  If the next character after the separator is an '=' it is ignored.  If it is a '>' it implies input only definition.  Otherwise it calls `addUserDefinedUnit` for each definition.  The function is declared `noexcept` and will return a string with each error separated by a newline.  So if the result string is `empty()` there were no errors.

Other Library Operations
---------------------------

*   `clearUserDefinedUnits()`  will erase all previously defined units
*   `disableUserDefinedUnits()`   will disable the use of user defined units
*   `enableUserDefinedUnits()`  will enable their use if they had been disabled,  they are enabled by default.

Notes on units and threads
----------------------------
The user defined units usage flag is an atomic variable but the modification of the user defined library are not thread safe, so if threads are needed make all the changes in one thread before using it in other threads, or protect the calls with a separate mutex.  The disable and enable functions trigger an atomic variable that enables the use of user defined units in the string translation functions.  disableUserDefinedUnits() also turns off the ability to specify new user defined units but does not erase those already defined.
