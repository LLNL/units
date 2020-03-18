=====================================
Testing
=====================================

The Code for the units library is put through a series of CI tests before being merged.
Current tests include running on CI systems.


Unit Tests
===========
The units library has a series of units tests that are executed as part of the CI builds and when developing.  They are built using Google test module.  The module is downloaded if the tests are built.  It uses Release 1.10 from the google test github repository

1.  examples_test  A simple executable that loads up some different types of measurements and does a few checks directly,  it is mainly to test linking and has some useful features for helping with the code coverage measures.
2.  fuzz_issue_tests  tests a set of past fuzzing failures, including, errors, glitches, timeouts, round trip failures, and some round trip failures with particular flags.
3.  test_all_unit_base **DO NOT RUN THIS TEST** it will take a very long time it does an exhaustive test of all possible unit bases to make sure the string conversion round trip works. I haven't actually executed it all yet.
4.  test_commodities  Run test using the commodity related functions and operations on precise_unit's
5.  test_conversions1 a series of tests about specific conversions, such as temperature, SI prefixes, extended SI units, and some other general operations about conversions
6.  test_conversions2 run through a series of test units and conversion from one of the converter websites,  there are number of files that get used that contain known conversions
7.  test_equation_units direct testing of the established equation units
8.  test_leadingNumbers run a bunch of checks on the leading number processing for units and measurements,  convert a leading string into a numerical value
9.  test_measurement a series of tests on measurement objects including operations and comparisons, and construction
10.  test_measurement_strings a few tests on the basic to and from string operations for measurements
11.  test_pu tests of pu units and conversions
12.  test_random_round_trip randomly pick a few 32 bit number, assume they are a unit and do a string conversion and interpretation on them and make sure they produce the same thing
13.  test_ucum a series of tests coming from `UCUM <https://github.com/lhncbc/ucum-lhc>`_  the units library tries to handle all official strings and a majority of the full names, and aliases
14.  test_udunits a series of tests and test files coming from  `UDUNITS-2 <https://github.com/Unidata/UDUNITS-2>`_  Not all the units convert, some never will since they are ambiguous but we will probably allow a few more over time
15.  test_uncertain_measurements test uncertain measurement operations using examples taken from web sources
16.  test_unit_ops test operations on units, including mathematical expressions and comparison operators
17.  test_unit_strings Unit strings test conversion to and from strings


CI systems
=================

Travis-CI
-----------
1. Clang 5.0 build using C\++11, and C\++14 using header only
2. Clang format style check
3. Clang 3.5 C++11,
4. GCC 7 C\++11, C\++14, C\++17, Code coverage
5. GCC 4.8 C++11

Azure
---------
1.  GCC 7.4 C++14 (Azure native Linux)
1.  AppleClang 11.0 (Xcode 11.3) C++17
1.  AppleClang 11.0 (Xcode 11.3) C++11
1.  MSVC 2017
1.  MSVC 2017 C++11
1.  MSVC 2019 C++17
1.  GCC 8.4 C++17
1.  GCC 4.8 C++11
1.  Clang 3.4 C++11
1.  Clang 8 C++14
1.  Clang-tidy (both main library and tests)
1.  CPPLINT


Circle-CI
-----------
1.  Clang 9,  Thread Sanitizer
2.  Clang 9,  Address, undefined behavior sanitizer
3.  Clang 9,  Memory Sanitizer
4.  Clang 8,  Fuzzing library -- run a couple of defined fuzzing tests from scratch to check for any anomalous situations. There are currently two fuzzers, the first test the units_from_string, and the second tests the measurement_from string.  It first converts the fuzzing sequence, then if it is a valid sequence, converts it to a string, then converts that string back to a measurement or unit and makes sure the two measurements or units are identical.  Any string sequence which doesn't work is captured and tested.

Codacy
---------
C++ static analysis and checks

Codecov
----------
Try to maintain the library at 100% coverage. Missing header only constexpr functions do not show up on the report, so periodic manual scans are done to make sure we have full coverage.
