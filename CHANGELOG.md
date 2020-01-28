# Changelog

All notable changes to this project after the 0.2.0 release will be documented in this file

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/).  
This project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.3.0][] - 2020-01-28

Continued work on cleaning up the library and starting to add main documentation, as well as adding more units and cleaning up string conversions and some additional tests.  Additional fuzzing fixes and add a webserver for exploring conversions.  
### Changed
-   Change the unit_data operators from '+', '-' to '*' and '/' so they actually match the operation they are performing [#12][]
-   Pow on measurements is a free function instead of operator [#12][]
-   Update cmake policy configuration so it works with newer CMakes [#31][]

### Fixed
-   Several issues that came from the fuzzer tests ([#14][], [#18][], [#16][], [#19][], [#24][], [#28][], [#30][])
-   Fixed the `UNITS_HEADER_ONLY` target so it actually works and add some tests for it [#23][]
-   Update the cmake code so it correctly deals with and uses the `CMAKE_CXX_STANDARD` option [#22][]
-   Some strict aliasing warnings on gcc 6 [#26][]
  
### Added
-   Added pow and root functions to measurements [#7][]
-   Add sqrt function which is a wrapper function around the root function for measurements and units [#8][]
-   Added uncertain measurment class for dealing with uncertainties [#9][], later modified in the primary method of uncertainty propagation[#32][]  
-   Added a webserver for doing conversions through an html based interface [#11][]
-   Added a docker file for doing fuzzing [#16][]
-   Added initial set of [documention](https://units.readthedocs.io/en/latest/) on readthedocs.io [#25][],[#27][]

### Removed
-   member methods of pow and root for measurements

## [0.2.0][] - 2019-12-25

[#7]: https://github.com/LLNL/units/pull/7
[#8]: https://github.com/LLNL/units/pull/8
[#9]: https://github.com/LLNL/units/pull/9
[#11]: https://github.com/LLNL/units/pull/11
[#12]: https://github.com/LLNL/units/pull/12
[#14]: https://github.com/LLNL/units/pull/14
[#16]: https://github.com/LLNL/units/pull/16
[#18]: https://github.com/LLNL/units/pull/18
[#19]: https://github.com/LLNL/units/pull/19
[#22]: https://github.com/LLNL/units/pull/22
[#23]: https://github.com/LLNL/units/pull/23
[#24]: https://github.com/LLNL/units/pull/24
[#25]: https://github.com/LLNL/units/pull/25
[#26]: https://github.com/LLNL/units/pull/26
[#27]: https://github.com/LLNL/units/pull/27
[#28]: https://github.com/LLNL/units/pull/28
[#30]: https://github.com/LLNL/units/pull/30
[#31]: https://github.com/LLNL/units/pull/31
[#32]: https://github.com/LLNL/units/pull/32
[0.3.0]: https://github.com/LLNL/units/releases/tag/v0.3.0

[0.2.0]: https://github.com/LLNL/units/releases/tag/v0.2.0