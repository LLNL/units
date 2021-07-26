====================
Fixed Measurements
====================

The primary difference between `fixed_measurement` and `measurement` is the idea that in a `fixed_measurement` the unit part is a constant.  It does not change.  Therefore any addition or subtraction operation will produce another measurement with the same units.  It also allows for interaction and comparisons with numerical types since the unit is known.  This is unlike measurements where comparison and addition and subtraction operations with numbers are not allowed.  Otherwise the behavior and operations are identical between `measurement` and `fixed_measurement` and likewise between `fixed_precise_measurement` and `precise_measurement`

Relationship with numbers
--------------------------
Because the `unit` associated with a fixed measurement is fixed.  It becomes legitimate to work with singular real valued numbers.


.. code-block:: c++

    fixed_measurement dist(10, m);

    if (dist>10.0) //this has meaning because the units of dist is known.
    {
        //all other operators are defined with doubles
    }

    dist=dist+3.0;  // dist is now 13 meters

    dist-=2.0;  // dist is now 11 meters

    dist=5.0;  // dist is now 5 meters


Interactions with `measurement`
--------------------------------

Fixed measurements have an implicit conversion to :ref:`measurements`, so all the methods that work with measurement work with `fixed_measurements`.  The construction of a fixed_measurement from measurement is explicit.  Likewise fixed_precise_measurement` have an implicit conversion to `precise_measurement`, so all the methods that work with `precise_measurement` work with `fixed_precise_measurements`.  The construction of a fixed_measurement from measurement is explicit.
