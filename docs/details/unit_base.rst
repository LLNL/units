---------------------------
Unit base class
---------------------------

The unit base class is a bitmap comprising segments of a 32 bit number.  all the bits are defined

the underlying definition is a set of bit fields that cover a full 32 bit unsigned integer

.. code-block:: c++

   // needs to be defined for the full 32 bits
        signed int meter_ : 4;
        signed int second_ : 4; // 8
        signed int kilogram_ : 3;
        signed int ampere_ : 3;
        signed int candela_ : 2; // 16
        signed int kelvin_ : 3;
        signed int mole_ : 2;
        signed int radians_ : 3; // 24
        signed int currency_ : 2;
        signed int count_ : 2; // 28
        unsigned int per_unit_ : 1;
        unsigned int i_flag_ : 1; // 30
        unsigned int e_flag_ : 1; //
        unsigned int equation_ : 1; // 32


The default constructor sets all the fields to 0.  But this is private and only accessible from friend classes like units.

The main constructor looks like

.. code-block:: c++

    constexpr unit_data(
            int meter,
            int kilogram,
            int second,
            int ampere,
            int kelvin,
            int mole,
            int candela,
            int currency,
            int count,
            int radians,
            unsigned int per_unit,
            unsigned int flag,
            unsigned int flag2,
            unsigned int equation)

an alternative constructor

.. code-block:: c++

   explicit constexpr unit_data(std::nullptr_t);

sets all the fields to 1

Math operations
-------------------

When multiplying two base units the powers are added.
For the flags.  The e_flag and i_flag are added, effectively an Xor while the pu and equation are ORed.

For division the units are subtracted, while the operations on the flags are the same.

Power and Root functions
+++++++++++++++++++++++++++++

For power operations all the individual powers of the base units are multiplied by the power number.
The pu and equation flags are passed through.  For even powers the i_flag and e_flag are set to 0, and odd powers they left as is.
For root operations, First a check if the root is valid, if not the error unit is returned.  If it is a valid root all the powers are divided by the root power.  The Pu flag is left as is, the i_flag and e_flag are treated the same is in the pow function and the equations flag is set to 0.

There is one exception to the above rules.  If the e_flag and i_flag is active and seconds have a non-zero power,  the e_flag&i_flag modify the second so instead of seconds they represent sqrt(seconds).  The purpose of this is to allow rootHertz to work properly when a power of it is taken and allow units like amplitude spectral density to work correctly which have a fractional power of seconds in them.    Multiplication and division of a unit with seconds value any with the e_flag and i_flag is not going to work correctly.  
