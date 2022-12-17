# Changelog

All notable changes to this project after the 0.2.0 release will be documented in this file

The format is based on [Keep a Changelog](http://keepachangelog.com/en/1.0.0/).
This project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.7.0][] - 2022-12-17

Added several math operations for units, restored coverage to 100%, added uncertain constants and cleaner string generation and interpretation around the use of '.' and uncertain measurements, and added support for new SI prefixes.

### Changed

- Changed the interpretation of '.' in some unit strings to allow more consistent use as abbreviation [#250][]
- Made the string generation for uncertain measurements obey rules for significant figures more closely [#249][]

### Fixed

- Get code coverage back to 100% [#256][],[#254][], [#253][], [#252][]
- Fixed hard coded namespaces in unit_conversion maps [#241][]
- Fixed a bug found by fuzzing [#240][]

### Added

- math operations from the standard library including: trunc, ceil, floor, round, fmod, sin, cos, tan. [#235][]
- added support for new SI prefixes: rotta, quetta, ronto, quecto. [#255][]
- Added a number of uncertain constants [#248][]

[#235]: https://github.com/LLNL/units/pull/235
[#240]: https://github.com/LLNL/units/pull/240
[#241]: https://github.com/LLNL/units/pull/241
[#248]: https://github.com/LLNL/units/pull/248
[#249]: https://github.com/LLNL/units/pull/249
[#250]: https://github.com/LLNL/units/pull/250
[#251]: https://github.com/LLNL/units/pull/251
[#252]: https://github.com/LLNL/units/pull/252
[#253]: https://github.com/LLNL/units/pull/253
[#254]: https://github.com/LLNL/units/pull/254
[#255]: https://github.com/LLNL/units/pull/255

## [0.6.0][] - 2022-05-16

This release has a number of fixes for specific unit strings, adds some other units and a new unit_conversion_maps structure to reduce stack usage.

### Changed

- Convert default branch to "main"
- The string maps are new defined in a constexpr array in `units_conversion_maps.hpp` this reduces the stack usage significantly and clears up some warnings about that on some compilers [#217][],[#226][]
- The country specific codes for China and Canada were changed from "ch" and `can` to `cn` and `ca` to match country top level domains [#229][]

### Fixed

- The "hartree" string produced an incorrect value [#163][]
- Fixed an issue with inverse multipliers for inverse conversions [#166][]
- changed the string conversions to better align with SI recommendations [#173][]
- Fix an issue with the string "0.2" [#188][]
- Fix asterisk operator typo [#194][]
- Update the docker images to use more recent base images [#206][],[#212][]
- User defined units were not being used in the string conversion operations properly [#213][]
- Update third party library links [#216][]
- Clean up some `if constexpr` warnings on some platforms [#227][]
- Fix an issue of misinterpreting short strings with spaces such as `kg m` [#224][]

### Added

- Added a series of constants to make the available constants match the Nist [Common Constants](https://physics.nist.gov/cgi-bin/cuu/Category?view=html&Frequently+used+constants.x=87&Frequently+used+constants.y=18) list [#163][]
- added "Eh" as a valid string value for "hartree" [#163][]
- Added support for larger power factors in units when using larger base [#184][]
- Added support for some climate related units [#210][],[#179][]
- Added mass to weight conversions [#229][]
- added domains to allow some domain specific units that might mean different things in different contexts [#173][]

[#217]: https://github.com/LLNL/units/pull/217
[#226]: https://github.com/LLNL/units/pull/226
[#166]: https://github.com/LLNL/units/pull/166
[#188]: https://github.com/LLNL/units/pull/188
[#229]: https://github.com/LLNL/units/pull/229
[#173]: https://github.com/LLNL/units/pull/173
[#210]: https://github.com/LLNL/units/pull/210
[#179]: https://github.com/LLNL/units/pull/179
[#184]: https://github.com/LLNL/units/pull/184
[#163]: https://github.com/LLNL/units/pull/163
[#224]: https://github.com/LLNL/units/pull/224
[#216]: https://github.com/LLNL/units/pull/216
[#227]: https://github.com/LLNL/units/pull/227
[#213]: https://github.com/LLNL/units/pull/213
[#212]: https://github.com/LLNL/units/pull/212
[#206]: https://github.com/LLNL/units/pull/206
[#194]: https://github.com/LLNL/units/pull/194

## [0.5.0][] - 2021-08-17

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
[0.7.0]: https://github.com/LLNL/units/releases/tag/v0.7.0
[0.6.0]: https://github.com/LLNL/units/releases/tag/v0.6.0
[0.5.0]: https://github.com/LLNL/units/releases/tag/v0.5.0
[0.4.0]: https://github.com/LLNL/units/releases/tag/v0.4.0
[0.3.0]: https://github.com/LLNL/units/releases/tag/v0.3.0
[0.2.0]: https://github.com/LLNL/units/releases/tag/v0.2.0
