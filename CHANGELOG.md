# Changelog

All notable changes to this project after the 0.2.0 release will be documented in this file

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/).
This project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.6.0][] ~ 2020-09-30

### Changed

### Fixed

- The "hartree" string produced an incorrect value

### Added

- Added a series of constants to make the available constants match the Nist [Common Constants](https://physics.nist.gov/cgi-bin/cuu/Category?view=html&Frequently+used+constants.x=87&Frequently+used+constants.y=18) list
- added "Eh" as a valid string value for "hartree"

### Removed

## [0.5.0][] - 2020-08-17

This release includes some changes to the CMake builds and Targets. Also includes support for 64 bit base types for units with expanded power support, and support for a custom namespace for better integration with other applications. And additional units for natural gas and particle physics.

### Changed

- update the physical constants to latest values and updated related documentation on Physical constants [#56][]
- CMake targets units::units and units::header_only are the only two public targets [#155][]
- Updated code checks [#85][]

### Fixed

- Fixed `-Wshadow` warnings for GCC 4.9 and earlier [#66][]
- Documentation Fixes and updates [#152][], [#146][], [#111][], [#82][]
- Fixed pow_overflows not handling 0 correctly [#139][], [#134][]
- Fixed a few issues with the output strings and some SI units [#130][]
- Some cleanup for multiplier string generation [#93][]
- Fixed an issue with plural two character units like `lb`, `oz` [#91][]
- Fix identified Fuzzing issue [#84][]
- Fixed some shadow warnings on some compilers [#72][]
- Fixed some issues with `std::abs` usage [#70][]

### Added

- Added version output to converter app and the correct version info on the webserver [#54][]
- Added markdown linter to pre-commit [#64][]
- CMake format check to pre-commit [#65][]
- Added tests for the shared library build [#155][]
- Added support for 64 bit base unit [#150][], [#142][]
- CMake Variable for manipulating output namespace [#136][], [#141][]
- Added some support for compiling with C++20 [#125][]
- Added some additional units for supporting particle physics [#120][],[#115][], [#112][], [#109][]
- Added support for exposing the main unit maps for string conversions [#113][]
- Added support for natural gas related units [#74][]

### Removed

- Travis-CI builds have been migrated to Azure and Circle-CI [#159][]
- removed units::units-static, and units::units-shared CMake targets[#155][]
- Removed Becquerel from the generated unit strings as it was producing some very odd units at times [#82][]

## [0.4.0][] - 2020-03-30

Add a converter command line application and fix a few slow conversion issues and some other fuzz issues that came up recently, add isnormal operation for measurements, better test coverage for fixed_precise_measurement, some additional tests and static analysis checks.

### Changed

- Added several tests run under Azure to remove deprecated image and add some new tests [#40][]
- clears up several warnings from clang-tidy [#41][]
- the fuzzer now uses fuzz_measurement [#42][]
- update clang format to limit line length to 80 and allow reflowing comments [#43][]
- add clang-tidy checks for the tests and fix a few issues. [#47][]

### Fixed

- A few timeouts on the fuzzer- the fix was to generalize the multiplier insertion after ^ to accept multiple digits after the ^ instead of just ignoring it after more than one. [#34][]
- An asymmetry was observed in the unit equality from on the fuzzers, this resulted in some modifications of the `cround_equal` and `cround_precice_equals` functions. Also noted that the functions weren't aborting on exact floating point equality so were doing quite a bit of extra calculations. [#34][]
- A timeout issue from fuzzing having to do with not injecting multiplies after `[]` in some circumstances. The fix was to be a little more refined as to which point to not inject the `*` and to do it in multiple stages so as to not rely on the partitioner so much. [#35][]
- `fixed_measurement` and `fixed_precise_measurement` had incorrect subtraction operation in a few overloads. [#39][]
- fixed a few initial issues from fuzz_measurement [#42][]
- Some more fuzzing generated issues with cascading powers [#45][]
- A number of additional clang-tidy checks were added and the resulting warnings fixed [#46][]
- An issue from the fuzzer dealing with equation type units [#51][]

### Added

- added a [converter](https://units.readthedocs.io/en/latest/introduction/converter.html) command line application that can convert units on the command line [#35][]
- Added a file operation that can load user defined units from a file [#36][]
- Added `is_valid` methods for all measurement types [#36][]
- Added addUserDefinedInputUnit to add user defined units only on the input [#36][]
- The webserver gained a `to_string` option to use the internal to_string operations to simplify the measurement and units [#37][]
- The webserver and the converter gained an ability to handle `*` and `<base>` as the input unit to convert the measurement to base units. [#37][]
- Added `to_string` operation for uncertain_measurements [#38][]
- Added `isnormal` operation for measurement types [#39][]
- Added `UNITS_CLANG_TIDY` option to run tests with Clang tidy [#41][]
- Added fuzz_measurement fuzzer to test measurement_from_string [#42][]
- Added cpplint test to azure [#43][]
- Added a number of additional units from UDunits [#44][]
- Added several CI checks including CPPlint, and a series of CMAKE checks, and formatting checks [#48][],[#49][]
- Added a series of additional tests for coverage [#50][]

## [0.3.0][] - 2020-01-28

Continued work on cleaning up the library and starting to add main documentation, as well as adding more units and cleaning up string conversions and some additional tests. Additional fuzzing fixes and add a webserver for exploring conversions.

### Changed

- Change the unit_data operators from '+', '-' to '\*' and '/' so they actually match the operation they are performing [#12][]
- Pow on measurements is a free function instead of operator [#12][]
- Update CMake policy configuration so it works with newer CMakes [#31][]

### Fixed

- Several issues that came from the fuzzer tests ([#14][], [#18][], [#16][], [#19][], [#24][], [#28][], [#30][])
- Fixed the `UNITS_HEADER_ONLY` target so it actually works and add some tests for it [#23][]
- Update the CMake code so it correctly deals with and uses the `CMAKE_CXX_STANDARD` option [#22][]
- Some strict aliasing warnings on GCC 6 [#26][]

### Added

- Added pow and root functions to measurements [#7][]
- Add sqrt function which is a wrapper function around the root function for measurements and units [#8][]
- Added uncertain measurement class for dealing with uncertainties [#9][], later modified in the primary method of uncertainty propagation[#32][]
- Added a webserver for doing conversions through an HTML based interface [#11][]
- Added a docker file for doing fuzzing [#16][]
- Added initial set of [documentation](https://units.readthedocs.io/en/latest/) on readthedocs.io [#25][],[#27][]

### Removed

- member methods of pow and root for measurements

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
[#34]: https://github.com/LLNL/units/pull/34
[#35]: https://github.com/LLNL/units/pull/35
[#36]: https://github.com/LLNL/units/pull/36
[#37]: https://github.com/LLNL/units/pull/37
[#38]: https://github.com/LLNL/units/pull/38
[#39]: https://github.com/LLNL/units/pull/39
[#40]: https://github.com/LLNL/units/pull/40
[#41]: https://github.com/LLNL/units/pull/41
[#42]: https://github.com/LLNL/units/pull/42
[#43]: https://github.com/LLNL/units/pull/43
[#44]: https://github.com/LLNL/units/pull/44
[#45]: https://github.com/LLNL/units/pull/45
[#46]: https://github.com/LLNL/units/pull/46
[#47]: https://github.com/LLNL/units/pull/47
[#48]: https://github.com/LLNL/units/pull/48
[#49]: https://github.com/LLNL/units/pull/49
[#50]: https://github.com/LLNL/units/pull/50
[#51]: https://github.com/LLNL/units/pull/51
[#54]: https://github.com/LLNL/units/pull/54
[#56]: https://github.com/LLNL/units/pull/56
[#64]: https://github.com/LLNL/units/pull/64
[#65]: https://github.com/LLNL/units/pull/65
[#66]: https://github.com/LLNL/units/pull/66
[#70]: https://github.com/LLNL/units/pull/70
[#72]: https://github.com/LLNL/units/pull/72
[#74]: https://github.com/LLNL/units/pull/74
[#82]: https://github.com/LLNL/units/pull/82
[#84]: https://github.com/LLNL/units/pull/84
[#85]: https://github.com/LLNL/units/pull/85
[#91]: https://github.com/LLNL/units/pull/91
[#93]: https://github.com/LLNL/units/pull/93
[#109]: https://github.com/LLNL/units/pull/109
[#111]: https://github.com/LLNL/units/pull/111
[#112]: https://github.com/LLNL/units/pull/112
[#113]: https://github.com/LLNL/units/pull/113
[#115]: https://github.com/LLNL/units/pull/115
[#120]: https://github.com/LLNL/units/pull/120
[#125]: https://github.com/LLNL/units/pull/125
[#130]: https://github.com/LLNL/units/pull/130
[#134]: https://github.com/LLNL/units/pull/134
[#136]: https://github.com/LLNL/units/pull/136
[#139]: https://github.com/LLNL/units/pull/139
[#141]: https://github.com/LLNL/units/pull/141
[#142]: https://github.com/LLNL/units/pull/142
[#146]: https://github.com/LLNL/units/pull/146
[#150]: https://github.com/LLNL/units/pull/150
[#152]: https://github.com/LLNL/units/pull/152
[#159]: https://github.com/LLNL/units/pull/159
[#155]: https://github.com/LLNL/units/pull/155
[0.5.0]: https://github.com/LLNL/units/releases/tag/v0.5.0
[0.4.0]: https://github.com/LLNL/units/releases/tag/v0.4.0
[0.3.0]: https://github.com/LLNL/units/releases/tag/v0.3.0
[0.2.0]: https://github.com/LLNL/units/releases/tag/v0.2.0
