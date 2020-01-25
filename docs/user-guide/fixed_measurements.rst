====================
Fixed Measurements
====================

The primary difference between `fixed_measurement` and `measurement` is the idea that in a `fixed_measurement` the unit part is a constant.  It does not change.  Therefore any addition or subtraction operation will produce another measurement with the same units.  It also allows for interaction and comparisons with numerical types since the unit is known.  This is unlike measurements where comparison and addition and subtraction operations with numbers are not allowed.  Otherwise the behavior and operations are identical between `measurement` and `fixed_measurement` and likewise between `fixed_precise_measurement` and `precise_measurement`
