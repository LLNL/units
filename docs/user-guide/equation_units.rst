==================
Equation Units
==================

The use of an equation flag in the unit_base defines a set of equation units.  These are specific units where the relationship with other units is defined through an equation rather than a specific multiplier.  There are 31 available equation units.  Equation units use up the flags, and the cound and radian fields.  All other units are left alone for defining the underlying units of the equation unit.  So the equation specifier defines and equation rather than a specific unit.  
equation types 0-15 deal with logarithms in some way


to extract the equation type 
-  unsigned short precise::custom::eq_type(detail::unit_data udata);

Current equation definitions 

-   0:  
-   1:
-   2:
-   3:
-   4:
-   5:
-   6:
-   7:
-   8:
-   9:
-   10:
-   11:
-   12:
-   13:
-   14:
-   15:
-   16: UNDEFINED
-   17: UNDEFINED
-   18: UNDEFINED
-   19: UNDEFINED
-   20: UNDEFINED
-   21: UNDEFINED
-   22: saffir simpson hurricane wind scale
-   23: Beaufort wind scale
-   24: Fujita scale
-   25: UNDEFINED
-   26: UNDEFINED
-   27: Prism diopter
-   28: UNDEFINED
-   29: Moment magnitude scale for earthquakes
-   30: Energy magnitude scale for earthquakes
-   31: UNDEFINED


The wind scales are not very accurate since they match up a slightly fuzzer notion to actual wind speed.  There are general charts and the equations in use use a polynomial to approximate them to a continuous scale.  So the units when used are generally convertible to a velocity unit such as m/s.  


