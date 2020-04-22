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

Advanced Usage
----------------
The `to_string` function also takes a second argument which is a `std::uint32_t match_flags` in all cases this default to 0,  it is currently unused though will be used in the future to allow some fine tuning of the output in specific cases.  In the near future a flag to allow utf 8 output strings will convert certain units to more common utf8 symbols such as unit Powers and degree symbols, and a few others.  The output string would default to ascii only characters.
