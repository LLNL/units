==================
User defined units
==================

The units library has support for user defined units and commodities.  These interact with the `*_from_string` and `to_string` operations to allow custom conversions and definitions.  

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
Sometimes it isful to have a larger library of units in this case the `std::string definedUnitsFromFile(const std::string& filename)` can be used to load a number of units at once. 

The file format is quite simple.  
`#` at the beginning of a line indicates a comment
other wise 

.. 

   # comment
   meeter == meter
   meh == meeter per hour

Other Library Operations
---------------------------

*   `clearUserDefinedUnits()`  will erase all previously defined units 
*   `disableUserDefinedUnits()   will disable the use of user defined units 
*   `enableUserDefinedUnits()`  will enable their use if they had been disabled,  they are enabled by default.  

Notes on units and threads
----------------------------
The user defined units usage is an atomic variable but the modification of the user defined library are not thread safe, so if threads are needed make all the changes in one thread before using it in other threads.   