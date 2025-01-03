==================
Defined Units
==================

The units library comes with two sets of units predefined.  They are all located in `src/units/unit_definitions.hpp <https://github.com/LLNL/units/blob/master/units/unit_definitions.hpp>`_.
The definitions come in two flavors a precise_unit and a regular unit.  All the precise units are defined in the namespace `units::precise`

All the units are defined as a constexpr values.  The choice of which units to define is somewhat arbitrary and guided by the authors experience and the origins of the library in power systems and electrical engineering in the US.
Units that the author has actually encountered in work or life are included and in cases where there might be conflicts depending on the location preference was given to the US customary definition, though international systems take priority.

Listing of Units
--------------------

The most common units are defined in the namespace `units::precise` and others are defined in subnamespaces.

Base Units
================
Most base units have two definitions that are equivalent

- meter, m
- kilogram, kg
- second, s
- Ampere, A
- Kelvin, K
- mol
- candela, cd
- currency
- count
- pu
- iflag
- eflag
- radian, rad

Specialized units
==================
Some specialized units are defined for use in conversion applications or for making handling string conversions a little easier

- defunit - special unit that signifies conversion to any other units is possible
- invalid - special unit that conversion has failed
- error - an error unit

Numerical Units
===================
Sometimes it is useful to have pure numerical units, often for multiplication with other units such as `hundred*kg` or something like that which becomes a single unit with 100 kg.

- one
- ten
- hundred
- percent  (0.01)
- infinite
- nan

Also included in this category are `SI prefixes <https://physics.nist.gov/cuu/Units/prefixes.html>`_
deci, centi,milli, micro, nano, pico, femto, atto, zepto, yocto, ronto, quecto, deka, hecto, kilo, mega, giga, tera, peta, exa, hecto, zetta, yotta,rotta, quetta.

and SI data prefixes
kibi, mebi, gibi, tebi, pebi, exbi, zebi, yobi

Derived SI Units
====================
There are many units that used in combination with the SI system that are derived from the base units

- hertz, Hz
- volt, V
- Pa  (pascal,  on some systems this is defined as something else so the definition(`pascal`) is skipped)
- newton, N
- joule, J
- watt, W
- coulomb, C
- farad, F
- siemens, S
- weber, Wb
- tesla, T
- henry, H
- lumen, lm
- lux, lx
- becquerel, Bq
- gray, Gy
- sievert, Sv
- katal, kat
- sr

Extra SI related units
===========================
A few units that are not officially part of the SI but are `accepted <https://physics.nist.gov/cuu/Units/outside.html>`_ for use with the SI system, along with a few other SI units with prefixes that are commonly used.

- mg
- g
- mL
- L
- nm
- mm
- km
- cm
- bar

Centimeter-Gram-Second system
==================================
The `CGS <https://en.wikipedia.org/wiki/Centimetre%E2%80%93gram%E2%80%93second_system_of_units>`_ system is a variant on the metric system.  Units from the CGS system are included under the namespace `units::precise::cgs`.

- eng
- dyn
- barye
- gal
- poise
- stokes
- kayser
- oersted
- gauss
- debye
- maxwell
- biot
- gilbert
- stilb
- lambert
- phot
- curie
- roentgen
- REM
- RAD
- emu
- langley
- unitpole
- statC_charge
- statC_flux
- abOhm
- abFarad
- abHenry
- abVolt
- statV
- statT
- statHenry
- statOhm
- statFarad

Conventional Electrical Units
==================================
defined in namespace `units::precise::conventional`

- volt90
- ampere90
- watt90
- henry90
- coulomb90
- farad90
- ohm90

Meter Gram Force System
============================
defined in namespace `units::precise::gm`

- pond
- hyl
- at
- poncelet
- PS

Meter Tonne Second system
=================================
Defined in namespace `units::precise::MTS`

- sthene
- pieze
- thermie

Additional Time units
=============================
Defined in namespace `units::precise::time`,  units marked with * are also defined in the `units::precise`.

- min*
- ms*
- ns*
- hr*
- h*
- day*
- week
- yr* (8760 hr)
- fortnight
- sday - sidereal day
- syr - sidereal year
- at - mean tropical year
- aj - mean julian year
- ag - mean gregorian year
- year - aliased to median calendar year (365 days) which is the standard for SI
- mos - synodal (lunar) month
- moj - mean julian month
- mog - mean gregorian month

International customary Units
=======================================
These are traditional units that have some level of international definition
Defined in namespace `units::precise::i`

- grain
- point
- pica
- inch
- foot
- yard
- mile
- league
- hand
- cord
- board_foot
- mil
- circ_mil

A few units have short symbols defined in `unit::precise`
in, ft, yd, mile.  These alias to the international definition.

Avoirdupois units
====================
Avoirdupois units are another common international standard of units for weight and volumes.  The units are defined in `units::precise::av`

- dram
- ounce
- pound
- hundredweight
- longhundredweight
- ton
- longton
- stone
- lbf
- ozf
- slug
- poundal

A few common units have symbols defined in `units::precise` lb, ton, oz, lbf and these alias to the Avoirdupois equivalent.

Troy Units
===============
Most commonly for precious metals a few units are defined in `units::precise::troy`, with a basis in the international grain.

- pennyweight
- oz
- pound

United States Customary Units
======================================
These are traditional units defined in the United States, for survey or common usage.
Defined in `unit::precise::us`.

- foot
- inch
- mil
- yard
- rod
- chain
- link
- furlong
- mile
- league
- acre*
- homestead
- section
- township
- minim
- dram
- floz
- tbsp
- tsp
- pinch
- dash
- shot
- gill
- cup
- pint
- quart
- gallon
- flbarrel - liquid barrel
- barrel
- hogshead
- cord
- fifth

A few US customary units are defined in specific namespaces to distinguish them from other forms
US customary dry measurements are defined in `units::precise::us::dry`

- pint
- quart
- gallon
- peck
- bushel
- barrel
- sack
- strike

Some grain measures used in markets and commodities are defined in `units::precise::us::grain`.
When commodities are a little more developed this will be defined with appropriate commodity included.

- bushel_corn
- bushel_wheat
- bushel_barley
- bushel_oats

Some survey units are defined in `units::precise::us::engineers` to distinguish them from others

- chain
- link

The unit gal (gallon) is also defined in `units::precise` since that is pretty common to use.

FDA and metric measures
========================
The food and drug administration has defined some customary units in metric terms for use in medicine.  These are defined in `units::precise::metric`
Also included are some other customary units that have a metric definition.

- tbsp
- tsp
- floz
- cup
- cup_uslegal
- carat

Canadian Units
======================
Some Canadian definitions of customary units defined in `units::precise::canada`

- tbsp
- tsp
- cup
- cup_trad
- gallon
- grain::bushel_oats

Australia Units
=======================
Traditional Australian units defined in `units::precise::australia`

- tbsp
- tsp
- cup

Imperial or British Units
=============================
Traditional british or imperial units, defined in `units::precise::imp`.

- inch
- foot
- thou
- barleycorn
- rod
- chain
- link
- pace
- yard
- furlong
- league
- mile
- nautical_mile
- knot
- acre
- perch
- rood
- gallon
- quart
- pint
- gill
- cup
- floz
- tbsp
- tsp
- barrel
- peck
- bushel
- dram
- minim
- drachm
- stone
- hundredweight
- ton
- slug

Apothecaries System
===========================
Used in pharmaceutical contexts the apothecaries system of units is defined in `units::precise::apothecaries`.

- floz ( same as imperial version )
- minim
- scruple
- drachm
- ounce
- pound
- pint
- gallon
- metric_ounce

Nautical Units
======================
Some units defined in context of marine travel defined in `units::precise::nautical`

- fathom
- cable
- mile
- knot
- league

Japanese traditional Units
=================================
Some traditional Japanese units are included for historical interest in `units::precise::japan`

- shaku
- sun
- ken
- tsubo
- sho
- kan
- go
- cup

Chinese Traditional Units
===================================
Some traditional Chinese units are included for historical interest in `units::precise::chinese`

- jin
- liang
- qian
- li
- cun
- chi
- zhang

Typographic units
==============================
Units used in typesetting and typography are included in `units::precise::typographic`
Subsets of the units depending on the location are in subnamespaces

`units::precise::typographic::american`
+++++++++++++++++++++++++++++++++++++++++++

- line
- point
- pica
- twip

`units::precise::typographic::printers`
+++++++++++++++++++++++++++++++++++++++++++

- point
- pica

`units::precise::typographic::french`
+++++++++++++++++++++++++++++++++++++++++++

- point
- ligne
- pouce
- didot
- cicero
- pied
- toise

`units::precise::typographic::metric`
+++++++++++++++++++++++++++++++++++++++++++

- point
- quart

`units::precise::typographic::IN`
+++++++++++++++++++++++++++++++++++++++++++
l'Imprimerie nationale

- point
- pica


`units::precise::typographic::tex`
+++++++++++++++++++++++++++++++++++++++++++

- point
- pica

`units::precise::typographic::postscript`
+++++++++++++++++++++++++++++++++++++++++++

- point
- pica

`units::precise::typographic::dtp`
+++++++++++++++++++++++++++++++++++++++++++
This is the modern standard or as close to such a thing as exists

- point
- pica
- twip
- line

`units::precise::typographic`
+++++++++++++++++++++++++++++++++++++++++++
Values taken from dtp namespace

- point
- pica

Distance Units
===============================
Some additional distance units are defined in `units::precise::distance`

- ly
- au
- au_old
- angstrom
- parsec
- `smoot`
- cubit
- longcubit
- arpent_us
- arpent_fr
- xu


Area Units
==========================
Some additional units defining an area `units::precise::area`

- are
- hectare
- barn
- arpent

Mass Units
==========================
Some additional units defining a mass `units::precise::mass`

- quintal
- ton_assay
- longton_assay
- Da
- u
- tonne

t is included in the `units::precise` namespace as a metric tonne

Volume Units
==========================
Some additional units defining a volume `units::precise::volume`

- stere
- acre_foot
- drum


Angle Units
=========================
A few units defining angles are defined in `units::precise::angle`.

- deg*
- gon
- grad
- arcmin
- arcsec
- brad - binary radian (1/256 of a circle)


Directional Units
=========================
A few cardinal directional units are defined in `units::precise::direction`, these make use of the `i_flag` and a numerical value

- east
- north
- south
- west

Temperature Units
===========================
A few units related to temperature systems, defined in `units::precise::temperature`

- celsius, degC*
- fahrenheit, degF*
- rankine, degR
- reaumur

Pressure Units
=====================
Some units related to pressure are defined in `units::precise::pressure`

- psi
- inHg
- mmHg
- torr
- inH2O
- mmH2O
- atm - standard atmosphere
- att - technical atmosphere

Power Units
=====================
Some units related to power are defined in `units::precise::power`

- hpE - electric Horsepower
- hpI - international horsepower
- hpS - steam horesepower
- hpM - mechanical horsepower

the unit hp is aliased in the `units::precise` namespace to `power::hpI`

Energy Units
=====================
Some units related to energy are defined in `units::precise::energy`

- kWh
- MWh
- eV
- kcal
- cal_4 - calorie at 4 deg C
- cal_15 - calorie at 15 deg C
- cal_28 - calorie at 28 deg C
- cal_mean - mean calorie
- cal_it - international table calorie
- cal_th - thermochemical calorie
- btu_th - thermochemical BTU
- btu_39 - BTU at 39 deg C
- btu_59 - BTU at 59 deg C
- btu_60 - BTU at 60 deg C
- btu_mean - mean BTU
- btu_it - international table BTU
- btu_iso - rounded btu_it
- quad
- tonc - cooling ton
- therm_us
- therm_br
- therm_ec
- EER - energy efficiency ratio
- SG - specific gravity
- ton_tnt
- boe - barrel of oil equivalent
- foeb
- hartree
- tonhour
- tce - ton of coal equivalent
- lge - liter of gasoline equivalent

in the `units::precise` namespace

- btu = energy::but_it
- cal = energy::cal_th
- kWh = energy::kWh
- MWh = energy::MWh

Power system Units
=========================
Some additional units related to power systems and electrical engineering
in `units::precise::electrical` namespace

- MW
- VAR  - W*i_flag
- MVAR
- kW
- kVAR
- mW
- puMW
- puV
- puHz
- MJ
- puOhm
- puA
- kV
- mV
- mA

Equation type Units
==========================
Equation units are explained more thoroughly in :ref:`Equation Units`  Some of the specific common equation units are defined in the namespace `units::precise::log`.

- neper
- logE - natural logarithm
- neperA - neper of amplitude unit
- neperP - neper of a power unit
- logbase10
- bel
- belP - bel of a power based unit
- dBP
- belA - bel of an amplitude based unit
- dBA - dB of an amplitude based unit
- logbase2
- dB
- neglog10
- neglog100
- neglog1000
- neglog50000
- B_SPL
- B_V
- B_mV
- B_uV
- B_10nV
- B_W
- B_kW
- dB_SPL
- dB_V
- dB_mV
- dB_uV
- dB_10nV
- dB_W
- dB_kW
- dB_Z - radar reflectivity
- BZ - radar reflectivity


Textile related Units
========================
Units related to the textile industry in namespace `units::precise::textile`.

- tex
- denier
- span
- finger
- nail


Clinical Units
==================
Units related to clinical medicine in namespace `units::precise::clinical`.

- pru
- woodu
- diopter
- prism_diopter
- mesh
- charriere
- drop
- met
- hounsfield
- AHF


Laboratory Units
=======================
Units used in laboratory settings in namespace `units::precise::laboratory`.

- svedberg
- HPF
- LPF
- enzyme_unit
- IU
- arbU - arbitrary unit
- IR - index of reactivity
- Lf - Limit of flocculation
- PFU
- pH
- molarity
- molality


Data Units
============
Units related to computer data and storage in `units::precise::data`

- bit*
- nibble
- byte
- kB*
- MB*
- GB*
- kiB
- MiB
- GiB
- bit_s - Shannon bit for information theory
- shannon
- hartley
- ban
- dit
- deciban
- nat
- trit
- digits

`B` is defined as byte in `units::precise`

Computation units
===================
Units related to computation `units::precise::computation`.

- flop
- flops
- mips

Special units
======================
Some special units that were not otherwise characterized in namespace `units::precise::special`.

- ASD - amplitude spectral density
- moment_magnitude - moment magnitude for earthquake scales (related to richter scale)
- moment_energy
- sshws - saffir simpson hurricane wind scale
- beaufort - Beaufort wind scale
- fujita - Fujita scale for tornados
- mach - mach number(multiplier of the speed of sound)
- rootHertz - square root of Hertz, this is a special handling unit that triggers some specific behavior to handle it.
- rootMeter - square root of meter, this is a special handling unit that triggers some specific behavior to handle it.
- degreeAPI - API scale for measuring liquid density typically petroleum based products
- degreeBaumeLight - scale for measuring liquid density for liquids lighter than water
- debreeBaumeHeavy - scale for measuring liquid density for liquids heavier than water
- sone - unit of loudness


Other Units
====================
General purpose other units not otherwise categorical in namespace `units::precise::other`

- ppm - part per million
- ppb - part per billion
- candle
- faraday
- rpm* - revolution per minute
- CFM - cubic feet per minute
- MegaBuck - $1,000,000
- GigaBuck - $1,000,000,000

Climate Units
=======================
Units related to climate in namespace `units::precise::climate`

- gwp - global warming potential
- gtp - global temperature potential
- odp - ozone depletion unit

Speed Units
============================

mph and mps are defined in `units::precise` since they are pretty common

Units in the `units` namespace
-------------------------------------
Regular units are defined in the `units` namespace.  The general rule is that any units with a definition directly in `units::precise` has an analog `nonprecise` unit in the `units` namespace.
One addition is that any unit defined in `precise::electrical` also is defined in `units` this has to do with the origins of the library in power systems.
