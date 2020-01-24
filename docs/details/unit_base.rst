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

