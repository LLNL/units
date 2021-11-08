==================
Physical constants
==================

The units library comes with a number of physical constants with appropriate units defined.
All the physical constants are specified as :ref:`precise measurements` and in the namespace `units::constants`
In general the most recent definition was chosen which includes the 2019 redefinition of some SI units this matches with the rest of the library and the defined units.
Inspiration for the different constants was taken from `wikipedia <https://en.wikipedia.org/wiki/List_of_physical_constants>`_ and `NIST <https://physics.nist.gov/cuu/Constants/index.html>`_.
Defined constants.  The `2019 redefinition <https://www.nist.gov/si-redefinition/meet-constants>`_ of the SI system was used where applicable.  All `common constants <https://physics.nist.gov/cgi-bin/cuu/Category?view=html&Frequently+used+constants.x=87&Frequently+used+constants.y=18>`_ listed from NIST are included

Defined constants
----------------------
Values are taken from `NIST 2018 CODATA <https://physics.nist.gov/cuu/Constants/Table/allascii.txt>`_ unless otherwise noted

-   Standard gravity - `g0`
-   Gravitational constant - `G`
-   Speed of light - `c`
-   Elementary Charge (2019 redefinition) - `e`
-   hyperfine structure transition frequency of the caesium-133 atom - `fCs`
-   Planck constant (2019 redefinition) - `h`
-   Reduced Planck constant (2019 redefinition) - `hbar`
-   Boltzman constant (2019 redefinition) - `k`
-   Avogadros constant (2019 redefinition) - `Na`
-   Luminous efficiency - `kcd`
-   Permittivity of free space - `eps0`
-   Permeability of free space - `u0`
-   Gas Constant - `R`
-   Stephan Boltzmann constant -`s`
-   Hubble constant 69.8 km/s/Mpc - `H0`
-   Mass of an electron - `me`
-   Mass of a proton - `mp`
-   Bohr Radius - `a0`
-   Faraday's constant - `F`
-   Atomic mass constant - `mu`
-   Conductance quantum - `G0`
-   Josephson constant - `Kj`
-   Magnetic flux quantum - `phi0`
-   von Kiltzing constant - `Rk`
-   Rydberg constant - `Rinf`

Planck Units
------------------
These units are found in the `units::constants::planck` namespace and include
`length`, `mass`, `time`, `charge`, and `temperature`.

Atomic units
----------------
These physical constants are values related to an electron or `atomic measurements <https://www.bipm.org/en/publications/si-brochure/table7.html>`_
They include `length`, `mass`, `time`, `charge`- same as `e` above, `energy`, and `action`. The atomic constants are defined in the `units::constants::atomic` namespace.

Numbers
-----------
There are a few numbers that are used in the library and include definitions in the `units::constants` namespace.  They are represented as doubles and are defined as constexpr

-   `pi` (3.14159265358979323846)
-   `tau` (2.0*pi)
-   `invalid_conversion` (NaN)
-   `infinity`

From Strings
-----------------
All constants listed here are available for conversion from strings by wrapping in brackets  For example the luminous efficiency would be converted to a unit by using `[kcd]`  The planck constants are available as `[planck::XXXXX]` or `planckXXXXXX and the atomic constants are available as `[atomic::XXXX]`
