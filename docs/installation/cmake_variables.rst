----------------------------
Unit Library CMake Reference
----------------------------

There are a few cmake variables that control the build process, they can be altered to change how the units library is built and what exactly is built.

CMake variables
----------------

-  `BUILD_TESTING` : Generate CMake Variable controlling whether to build the tests or not
-  `UNITS_ENABLE_TESTS` :  Does the same thing as `BUILD_TESTING`
-  `UNITS_BUILD_STATIC_LIBRARY`:  Controls whether a static library should be built or not
-  `UNITS_BUILD_SHARED_LIBRARY`:  Controls whether to build a shared library or not
-  `UNITS_BUILD_FUZZ_TARGETS`:  If set to `ON`, the library will try to compile the fuzzing targets for clang libFuzzer
-  `UNITS_BUILD_WEB_SERVER`:  If set to `ON`,  build a webserver,  This uses boost::beast and requires boost 1.70 or greater to build it also requires cmake 3.12 or greater
-  `UNITS_BUILD_CONVERTER_APP`: enables building a simple command line converter application that can convert units from the command line
-  `UNITS_ENABLE_EXTRA_COMPILER_WARNINGS`: Turn on bunch of extra compiler warnings, on by default
-  `UNITS_ENABLE_ERROR_ON_WARNINGS`:  Mostly useful in some testing contexts but will turn on Werror so any normal warnings generate an error.
-  `CMAKE_CXX_STANDARD`:  Compile with a particular C++ standard, valid values are `11`, `14`, `17`, and likely `20` though that isn't broadly supported.
-  `UNITS_BINARY_ONLY_INSTALL`:  Just install shared librarys and executables,  no headers or static libs or packaging information

If compiling as part of a subproject then a few other options are useful

-  `UNITS_HEADER_ONLY`:  Only generate the header only target
-  `UNITS_INSTALL`:  enable the install instructions of the library
-  `UNITS_WITH_CMAKE_PACKAGE`:  Generate the cmake package variables for an installation or package
-  `UNITS_BUILD_OBJECT_LIBRARY`:  Generate an object library that can be used as part of other builds

CMake Targets
--------------

If you are using the library as a submodule or importing the package there are a couple targets that can be used depending on the build

-  `units::static`  will be set to the static library if built
-  `units::shared`  will be set to the shared library if built
-  `units::object`  will be set to the object library if enabled
-  `units::units`  will be set to the static library if built or the shared library if built and the static is not
-  `units::units-header-only` is a target if `UNITS_HEADER_ONLY` cmake variable is set


Example
---------

As part of the HELICS library the units library is used as a submodule it is included

.. code-block:: cmake

   # so units cpp exports to the correct target export
   set(UNITS_INSTALL OFF CACHE INTERNAL "")

   if(NOT CMAKE_CXX_STANDARD)
      set(CMAKE_CXX_STANDARD 14) # Supported values are ``11``, ``14``, and ``17``.
   endif()

   set(UNITS_BUILD_OBJECT_LIBRARY OFF CACHE INTERNAL "")
   set(UNITS_BUILD_STATIC_LIBRARY ON CACHE INTERNAL "")
   set(UNITS_BUILD_SHARED_LIBRARY OFF CACHE INTERNAL "")
   set(UNITS_BUILD_WEBSERVER OFF CACHE INTERNAL "")

   add_subdirectory("${PROJECT_SOURCE_DIR}/ThirdParty/units"
                 "${PROJECT_BINARY_DIR}/ThirdParty/units")

   set_target_properties(units-static PROPERTIES FOLDER Extern)

   hide_variable(UNITS_HEADER_ONLY)
   hide_variable(UNITS_BUILD_OBJECT_LIBRARY)

Then the target linked by

.. code-block:: cmake

   target_link_libraries(helics_common PUBLIC HELICS::utilities units::units)
