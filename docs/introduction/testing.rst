=====================================
Testing
=====================================

The Code for the units library is put through a series of CI tests before being merged.
Current tests include running on CI systems.

CI systems
=================

Travis-CI
-----------
1. Clang 5.0 build using C++11, and C++14 using header only
2. Clang format style check
3. Clang 3.5 C++11,
4. GCC 7 C++11, C++14, C++17, Code coverage
5. GCC 4.8 C++11

Azure
---------
1.  GCC 5.4
2.  MSVC 2017
3.  MACOS

Circle-CI
-----------
1.  Clang 9,  Thread Sanitizer
2.  Clang 9,  Address, undefined behavior sanitizer
3.  Clang 9,  Memory Sanitizer
4.  Clang 8,  Fuzzing library -- run a couple of defined fuzzing tests from scratch to check for any anomalous situations. It has been a while since this found anything but when started there were quite a few issues.  The Fuzzers run the from_string operations then convert back to a string and back again and check for mismatches or other issues with malformed strings.

Codacy
---------
C++ static analysis and checks

Codecov
----------
Try to maintain the library at 100% coverage. Currently showing at 100% coverage but a few missing operations that just don't show up on code coverage reports


Unit Tests
===========
The units library has a series of units tests that are executed as part of the CI builds and when developing.  They are built using Google test module.  The module is downloaded if the tests are built.  It uses Release 1.10 from the google test github repository

1.  examples_test  A simple executable that loads up some different types of measurements and does a few checks directly,  it is mainly to test linking and has some useful features for helping with the code coverage measures.
2.  fuzz_issue_tests  tests a set of past fuzzing failures, including, errors, glitches, timeouts, round trip failures, and some round trip failures with particular flags.
3.  test_all_unit_base **DON'T RUN THIS TEST** it will take a very long time it does an exhaustive test of all possible unit bases to make sure the string conversion round trip works. I haven't actually executed it all yet.
4.  test_commodities  Run test using the commodity related functions and operations on precise_unit's 
5.  test_conversions1
6.  test_conversions2
7.  test_equation_units
8.  test_leadingNumbers
9.  test_measurement
10.  test_measurement_strings
11.  test_pu
12.  test_random_round_trip
13.  test_ucum
14.  test_udunits
15.  test_uncertain_measurements
16.  test_unit_ops
17.  test_unit_strings
