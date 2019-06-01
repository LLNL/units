# Contributors

### Lawrence Livermore National Lab
- Corey McNeish
- Philip Top

## Used Libraries or Code
### [GridDyn](https://github.com/LLNL/GridDyn)  
This library was based on some code used inside GridDyn and was developed when that code reached the limits of its capabilties.  It was pulled out as the containers are not core to HELICS and it was useful as a standalone library so it could have better testing and separation of concerns.  HELICS is released with a BSD-3-Clause license.

### [googleTest](https://github.com/google/googletest)  
  The tests are written to use google test and mock frameworks and is included as a submodule.  Googletest is released with a BSD-3-clause licensed

### cmake scripts
Several cmake scripts came from other sources and were either used or modified for use in HELICS.
- Lars Bilke [CodeCoverage.cmake](https://github.com/bilke/cmake-modules/blob/master/CodeCoverage.cmake)
- CLI11 [CLI11](https://github.com/CLIUtils/CLI11)  while CLI11 was not used directly many of the CI scripts and structure were borrowed to set up the CI builds.  
