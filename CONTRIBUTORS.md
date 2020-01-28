# Contributors

## Lawrence Livermore National Lab
-   [Corey McNeish](https://github.com/mcneish1)
-   [Philip Top](https://github.com/phlptp)

## Other
-   [Greg Sjaardema](https://github.com/gsjaardema)
-   [JensMunkHansen](https://github.com/JensMunkHansen)

## Used Libraries or Code

### [GridDyn](https://github.com/LLNL/GridDyn)  
This library was based on some code used inside GridDyn and was developed when that code reached the limits of its capabilities.  It was pulled out as the units are not core to GridDyn and could serve useful purposes in other applications.  GridDyn is licensed with a BSD-3-Clause license

### [googleTest](https://github.com/google/googletest)  
The tests are written to use google test and mock frameworks and is included as a submodule.  Googletest is released with a BSD-3-clause licensed.

### [BOOST](https://www.boost.org)
Boost is used for the webserver code.  There is no dependency elsewhere In the webeserver Boost\::Beast is used for the http processing and that also uses boost\::ASIO and boost\::flat_map.  Boost is licensed under the Boost Software License.  The Webserver is not built by default so boost is not required for most builds or to use Units as a library.  

## References for Unit definitions

-   [UDUNITS-2](https://github.com/Unidata/UDUNITS-2) The UDUNITS package supports units of physical quantities. Its C library provides for arithmetic manipulation of units and for conversion of numeric values between compatible units.	The package contains an extensive unit database, which is in XML format	and user-extendable. The package also contains a command-line utility for investigating units and converting values.  Some of the unit definition files are used to test the library to ensure compatibility with the definitions in this library.  Some of the definitions files are included.  UDUNITS-2 is licensed with a [BSD-3-clause](https://github.com/Unidata/UDUNITS-2/blob/master/COPYRIGHT) with an additional clause revoking the license on allegations of patent infringement.  No code from UDUNITS-2 is used, only the unit definitions files.  
-   [ucum-lhc](https://github.com/lhncbc/ucum-lhc) This is the LHC implementation of validation and conversion services based on the [Unified Code for Units of Measure](http://unitsofmeasure.org/) (UCUM) code system created by the Regenstrief Institute, Inc. The repository is licensed under a [BSD](https://github.com/lhncbc/ucum-lhc/blob/master/LICENSE.md) license.  No code is used in units, but the unit definition files are used for testing purposes and some of the json files are included in the repository.  

### cmake scripts
Several cmake scripts came from other sources and were either used or modified for use in HELICS.
-   Lars Bilke [CodeCoverage.cmake](https://github.com/bilke/cmake-modules/blob/master/CodeCoverage.cmake)
-   [CLI11](https://github.com/CLIUtils/CLI11)  while CLI11 was not used directly many of the CI scripts and structure were borrowed to set up the CI builds.  
-   [HELICS](https://github.com/GMLC-TDC/HELICS)  Some of the code for cmake and some of the build scripts and a few of the docker images used for testing are maintained through the HELICS library.  HELICS also uses the units library for unit translations.  
