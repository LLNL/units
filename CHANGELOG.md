# Changelog
All notable changes to this project after the 0.2.0 release will be documented in this file

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/).  
This project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.3.0][] - 2020-01-28

Continued work on cleaning up the library and starting to add main documentation
### Changed
-   Change the unit_data operators from '+', '-' to '*' and '/' so they actually match the operation they are performing (#12)
-   Pow on measurements is a free function instead of operator (#12)
-   Update cmake policy configuration so it works with newer CMakes (#31)

### Fixed
-   Several issues that came from the fuzzer tests (#14, #18, #16, #19, #24, #28, #30)
-   Fixed the `UNITS_HEADER_ONLY` target so it actually works and add some tests for it (#23)
-   Update the cmake code so it correctly deals with and uses the `CMAKE_CXX_STANDARD` option (#22)
-   Some strict aliasing warnings on gcc 6 (#26)
  
### Added
-   Added pow and root functions to measurements (#7)
-   Add sqrt function which is a helper function on top of root for measurements and units (#8)
-   Added uncertain measurment class for dealing with uncertainties (#9), later modified in the primary method of uncertainty propagation(#32)  
-   Added a webserver for doing conversions through an html based interface (#11)
-   Added a docker file for doing fuzzing (#16)
-   Added initial set of [documention](https://units.readthedocs.io/en/latest/) on readthedocs.io

### Removed
-   member methods of pow and root for measurements

## [0.2.0][] - 2019-12-25
