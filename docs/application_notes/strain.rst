----------------------------
Strain
----------------------------

Strain is an interesting unit in that it is a dimensionless unit.  The most common expression is in `in/in` or `mm/mm` often defined as ε.  The effects of strain are pretty small so µε is also pretty common.
It is the fractional change in length.   There are a several ways to represent this in the units library depending on the needs for a particular situation.

Method 1
=============

The first way is simply as a ratio.

.. code-block:: c++

   measurement deltaLength=0.00001*m;
   measurement length=1*m;

   auto strain=deltaLength/length;

   EXPECT_EQ(to_string(strain), "1e-05");

   //applied to a 10 ft bar
   auto distortion=strain*(10*ft);
   EXPECT_EQ(to_string(distortion), "0.0001 ft");

The main issue that there is no distinction between a strain measurement and any other ratio, but in many cases that is fine.

Method 2
=============

The default defined unit of strain in the units library uses per unit meters as a basis.  The multiplies and divides methods in the units math library can take per unit flag into account when doing the multiplication to get the original units back.
The advantages of this are that strain becomes a distinctive unit from all other ratio units.   Volumetric or area strain can be represented in the same way.  It does have the disadvantage of requiring the `multiplies`

.. code-block:: c++

   #include <units/units_math.hpp> //for multiplies
   precise_measurement strain=1e-05*default_unit("strain");
   EXPECT_EQ(to_string(strain), "1e-05 strain");

   //applied to a 10 ft bar
   auto distortion=multiplies(strain,(10*ft));
   EXPECT_EQ(to_string(distortion), "0.0001 ft");

.. code-block:: c++

   #include <units/units_math.hpp> //for multiplies divides
   measurement deltaLength=0.00001*m;
   measurement length=1*m;

   auto strain=divides(deltaLength,length);
   EXPECT_EQ(to_string(strain), "1e-05 strain");

   //applied to a 10 ft bar
   auto distortion=multiplies(strain,(10*ft));
   EXPECT_EQ(to_string(distortion), "0.0001 ft");

Method 3
============

The third potential method is to use one of the indicator flags to define a unit for strain.  This can work in cases where there is no other potential conflicts with the flag and you need the `*` operator to work.

.. code-block:: c++

    precise_unit ustrain(1e-6,eflag);  // microstrain

    addUserDefinedUnit("ustrain",ustrain);
    precise_measurement strain=45.7*ustrain;
    EXPECT_EQ(to_string(strain), "45.7 ustrain");

    //applied to a 10 m bar
    auto distortion=strain*(10*m);
    EXPECT_DOUBLE_EQ(distortion.value_as(precise::mm),0.457);

The advantages of this are that the there is no per unit values to handle.  The disadvantage is that the eflag needs to be handled particularly when dealing with strings.  If it is just dealing with computations this is less of an issue.
So this method can work fine in some cases.


Discussion
==================
There are several ways to represent strain or any other ratio unit that is derived from particular unit cancellations.  All have advantages and disadvantages in particular situations and the method of choice will come down to the expected use cases.
The library chooses the per unit method as it maintains the source units, but other choices are free to choose if they work better in particular situations.
