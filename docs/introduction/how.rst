=====================================
How It Works
=====================================

Given the design requirements the choice was how to make a class that could represent physical units.  The desire for it to be a compact class drove the decision to somewhat limit what could be represented to physically realizable units.

Unit Representation
--------------------

The unit class consists of a multiplier and a representation of base units.
The seven SI units + radians + currency units + count units.  In addition a unit has 4 flags,  per-unit for per unit or ratio units. One flag\[i_flag\] that is a representation of imaginary units, one flags for a variety of purposes and to differentiate otherwise similar units\[e_flag\]. And a flag to indicate an equation unit. Due to the requirement that the base units fit into a 4 byte type the represented powers of the units are limited.  The list below shows the bit representation range and observed range of use in equations and observed usage

-   meter:\[-8,+7\]  :normal range \[-4,+4\], intermediate ops \[-6,+6\]
-   kilogram:\[-4,+3\] :normal range \[-1,+1\], intermediate ops \[-2,+2\]
-   second:\[-8,+7\] :normal range \[-4,+4\], intermediate ops \[-6,+6\]
-   ampere:\[-4,+3\] :normal range \[-2,+2\]
-   kelvin:\[-4,+3\] :normal range \[-4,+1\]
-   mole:\[-2,+1\] :normal range \[-1,+1\]
-   candela:\[-2,+1\] :normal range \[-1,+1\]
-   currency:\[-2,+1\] :normal range \[-1,+1\]
-   count:\[-2,+1\] :normal range \[-1,+1\]
-   radians:\[-4,+3\] :normal range \[-2,+2\]

These ranges were chosen to represent nearly all physical quantities that could be found in various disciplines we have encountered.


- `SI units Publication guidelines <https://physics.nist.gov/cuu/pdf/sp811.pdf>`_
