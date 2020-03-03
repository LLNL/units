=======================
Units To Strings
=======================

All the class in the units library can be given as an argument to a `to_string` function.  This function converts the units or value into a `std::string` that is representative of the unit or measurement.  In all cases the primary aim of the `to_string` is to generate a string that the corresponding `*_from_string` function will recognize and convert back to the original unit.  The secondary aim is to generate string that is human readable in standard notation.  WHile this is achieved for many common units there is some work left to do to make it better.

For example




Advanced Usage
----------------
The `to_string` function also takes a second argument which is a `std::uint32_t match_flags` in all cases this default to 0,  it is currently unused though will be used in the future to allow some fine tuning of the output in specific cases.
