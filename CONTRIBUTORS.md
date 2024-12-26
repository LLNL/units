# Contributors

## Lawrence Livermore National Lab

- [Philip Top](https://github.com/phlptp)
- [Ryan Mast](https://github.com/nightlark)

## Other

- [Corey McNeish](https://github.com/mcneish1)
- [Greg Sjaardema](https://github.com/gsjaardema) - Documentation updates
- [JensMunkHansen](https://github.com/JensMunkHansen)
- [Simon Heybrock](https://github.com/SimonHeybrock) - Doc updates and help supporting 64 bit unit base types, some additional constants and suggestions related to particle physics
- [Kerim371](https://github.com/kerim371) - CMake builds for the shared library version
- [Owen Arnold](https://github.com/OwenArnold) - Documentation
- [Peter Steneteg](https://github.com/petersteneteg)
- [Jan-Lukas Wynen](https://github.com/jl-wynen) - Support for multi-digit power strings
- [Jiahao Wen](https://github.com/wkindling) - Fixes for cmake standard condition

## Used Libraries or Code

### [GridDyn](https://github.com/LLNL/GridDyn)

This library was based on some code used inside GridDyn and was developed when that code reached the limits of its capabilities. It was pulled out as the units are not core to GridDyn and could serve useful purposes in other applications, Many of the original definitions of units, particularly in power systems originate from GridDyn. GridDyn is licensed with a BSD-3-Clause license.

### [googleTest](https://github.com/google/googletest)

The tests are written to use google test and mock frameworks and is included as a submodule. Googletest is released with a BSD-3-clause licensed.

### [BOOST](https://www.boost.org)

Boost is used for the webserver code. There is no dependency elsewhere in the library. In the webserver Boost\::Beast is used for the http processing which includes use of boost\::ASIO; boost\::flat_map is also used in some of the processing to handle the REST API. Boost is licensed under the Boost Software License. The Webserver is not built by default so boost is not required for most builds or to use Units as a library.

### [CLI11](https://github.com/CLIUtils/CLI11)

The converter app uses CLI11 as part of the command line processing for the converter app and the single header file is included in the repository. It is not used in the units library itself. A number of the CI scripts and structures and github actions were borrowed from the CLI11 repository. CLI11 is released with a BSD-3-clause license.

### [nanobind](https://github.com/wjakob/nanobind)

Nanobind is a small binding library that exposes C++ types in Python and vice versa. It is used for creating the python bindings for the library. Nanobind is released with a BSD-3-Clause license.

## References for Unit definitions

- [UDUNITS-2](https://github.com/Unidata/UDUNITS-2) The UDUNITS package supports units of physical quantities. Its C library provides for arithmetic manipulation of units and for conversion of numeric values between compatible units. The package contains an extensive unit database, which is in XML format and user-extendable. The package also contains a command-line utility for investigating units and converting values. Some of the unit definition files are used to test the library to ensure compatibility with the definitions in this library. Some of the definitions files are included. UDUNITS-2 is licensed with a [BSD-3-clause](https://github.com/Unidata/UDUNITS-2/blob/master/COPYRIGHT) with an additional clause revoking the license on allegations of patent infringement. No code from UDUNITS-2 is used, only the unit definitions files.
- [ucum-lhc](https://github.com/lhncbc/ucum-lhc) This is the LHC implementation of validation and conversion services based on the [Unified Code for Units of Measure](http://unitsofmeasure.org/) (UCUM) code system created by the Regenstrief Institute, Inc. The repository is licensed under a [BSD](https://github.com/lhncbc/ucum-lhc/blob/master/LICENSE.md) license. No code is used in units, but the unit definition files are used for testing purposes and some of the JSON files are included in the repository.
- [google unit conversions](https://support.google.com/websearch/answer/3284611?hl=en#unitconverter) Google unit conversion support was examined to provide additional unit strings and some conversion values.
- [convert me](https://www.convert-me.com/en/) unit converter web site used as test and check for various conversions

### cmake scripts

Several CMake scripts came from other sources and were either used or modified for use in HELICS.

- Lars Bilke [CodeCoverage.cmake](https://github.com/bilke/cmake-modules/blob/master/CodeCoverage.cmake)
- [HELICS](https://github.com/GMLC-TDC/HELICS) Some of the code CMake and build scripts and a few of the docker images used for testing are maintained through the HELICS library. HELICS also uses the units library for unit translations.
- [Nanobind example](https://github.com/wjakob/nanobind_example) much of the scripts for the the Python binding and CI jobs was copied and modified from the nanobind example repo.
