----------------------------
Unit Library CMake Reference
----------------------------

There are a few CMake variables that control the build process, they can be altered to change how the units library is built and what exactly is built.

CMake variables
----------------

-  `BUILD_TESTING` : Generate CMake Variable controlling whether to build the tests or not
-  `UNITS_ENABLE_TESTS` :  Does the same thing as `BUILD_TESTING`
-  `UNITS_BUILD_STATIC_LIBRARY`:  Controls whether a static library should be built or not
-  `UNITS_BUILD_SHARED_LIBRARY`:  Controls whether to build a shared library or not, only one or none of `UNITS_BUILD_STATIC_LIBRARY` and `UNITS_BUILD_SHARED_LIBRARY` can be enabled at one time.
-  `BUILD_SHARED_LIBS`:  Controls the defaults for the previous two options, overriding them takes precedence
-  `UNITS_BUILD_FUZZ_TARGETS`:  If set to `ON`, the library will try to compile the fuzzing targets for clang libFuzzer, default `OFF`
-  `UNITS_BUILD_WEB_SERVER`:  If set to `ON`,  build a webserver,  This uses boost::beast and requires boost 1.70 or greater to build it also requires CMake 3.12 or greater, default `ON`
-  `UNITS_USE_EXTERNAL_GTEST`: Defaults to `OFF` only used if `UNIT_ENABLE_TESTS` is also on, but if set to `ON` will search for an external Gtest and GMock libraries
-  `UNITS_BUILD_CONVERTER_APP`: enables building a simple command line converter application that can convert units from the command line
-  `UNITS_ENABLE_EXTRA_COMPILER_WARNINGS`: Turn on bunch of extra compiler warnings, on by default
-  `UNITS_ENABLE_ERROR_ON_WARNINGS`:  Mostly useful in some testing contexts but will turn on `Werror` so any normal warnings generate an error.
-  `CMAKE_CXX_STANDARD`:  Compile with a particular C++ standard, valid values are `11`, `14`, `17`, `20`, and likely `23` though that isn't broadly supported.  Will set to `14` by default if not otherwise specified
-  `UNITS_BINARY_ONLY_INSTALL`:  Just install shared libraries and executables,  no headers or static libs or packaging information
-  `UNITS_CLANG_TIDY`:  Enable the clang tidy tests as part of the build
-  `UNITS_CLANG_TIDY_OPTIONS`:  options that get passed to clang tidy when enabled
-  `UNITS_BASE_TYPE`:  Set to `uint64_t` for expanded base-unit power support. This increases the size of a unit by 4 Bytes.
-  `UNITS_DOMAIN`:  Specify a default domain to use for string conversions.  Can be either a name from the domains namespace such as `domains::surveying` or one of 'COOKING', 'ASTRONOMY', 'NUCLEAR', 'SURVEYING', 'USE_CUSTOMARY', 'CLIMATE', or 'UCUM'.
-  `UNITS_DEFAULT_MATCH_FLAGS`: Specify an integer value for the default match flags to be used for conversion
-  `UNITS_DISABLE_NON_ENGLISH_UNITS`: the library includes a number of non-english units that can be converted from strings, these can be disabled by setting `UNITS_DISABLE_NON_ENGLISH_UNITS` to ON or setting the definition in the C++ code.
-  `UNITS_DISABLE_EXTRA_UNIT_STANDARDS`: If set to `ON` disables UN recommendation 12, X12(not implemented yet), DOD(not implemented yet), from being included in the compilation and generated from strings.
-  `UNITS_BUILD_PYTHON_LIBRARY`: If set to true builds the python wrapper to the units library using nanobind
-  `UNITS_PYTHON_INSTALL_SHARED_LIBRARY`: If set to true and `UNITS_BUILD_SHARED_LIBRARY` is also true will install the shared library alongside the python wrapper binary
-  `UNITS_PYTHON_ONLY_INSTALL`: defaults to true if built with scikit otherwise false, if true only installs the components required for the python library otherwise normal installation occurs
-  `UNITS_NAMESPACE`:  The top level namespace of the library, defaults to `units`.
   When compiling with C++17 (or higher), this can be set to, e.g., `mynamespace::units` to avoid name clashes with other libraries defining `units`.
-  `UNITS_INSTALL`:  This is set to `ON` normally but defaults to `OFF` if used as a subproject.  This controls whether anything gets installed by the install target.
-  `UNITS_CMAKE_PROJECT_NAME`:  This is set to `UNITS` by default.   If using this in a package manager or wish to rename the project this variable can be set to another name to change the name of the package.  This will change the install path and cmake target names. For example setting `-DUNITS_CMAKE_PROJECT_NAME=LLNL-UNITS` will create cmake project llnl-units::units, and llnl-units::header_only and will install in a llnl-units directory with appropriate cmake files.
-  `${UNITS_CMAKE_PROJECT_NAME}_ENABLE_SUBMODULE_UPDATE` this can be set to off to disable submodule updates which impacts Gtest, in which case if this is off and the submodule is not preset it will try to find and external Gtest, similar to `UNITS_USE_EXTERNAL_GTEST`
If compiling as part of a subproject then a few other options are useful

-  `UNITS_HEADER_ONLY`:  Only generate the header only target, sets `UNITS_BUILD_STATIC_LIBRARY` and `UNITS_BUILD_SHARED_LIBRARY` to OFF
-  `UNITS_INSTALL`:  enable the install instructions of the library
-  `UNITS_BUILD_OBJECT_LIBRARY`:  Default `OFF`,Generate an object library that can be used as part of other builds.  Only one of `UNITS_BUILD_SHARED_LIBRARY`, `UNITS_BUILD_STATIC_LIBRARY`, or `UNITS_BUILD_OBJECT_LIBRARY` can be set to `ON`.  If more than one are set,  the shared library and object library settings take precedence over the static library.
-  `UNITS_LIBRARY_EXPORT_COMMAND`:  If desired the targets for the units library can be merged into an root project target list by modifying this variable.  The use cases for this are rare, but if this is something you want to do this variable should be set to something like `EXPORT rootProjectTargets`.  It defaults to `"EXPORT unitsTargets"`

CMake Targets
--------------

If you are using the library as a submodule or importing the package there are a couple targets that can be used depending on the build.  NOTE:  these can be changed using `UNITS_CMAKE_PROJECT_NAME`.

-  `units::units`  will be set to the library being built, either the shared, static, or object
-  `units::header_only` is a target for the headers if `UNITS_HEADER_ONLY` CMake variable is set, then only this target is generated.  This target is always created.


Example
---------

As part of the `HELICS <https://github.com/GMLC-TDC/HELICS>`_ library the units library is used as a submodule it is included by the following code

.. code-block:: cmake

   # so units cpp exports to the correct target export
   set(UNITS_INSTALL OFF CACHE INTERNAL "")

   if(NOT CMAKE_CXX_STANDARD)
        set(CMAKE_CXX_STANDARD 17) # Supported values are ``11``, ``14``, and ``17``.
   endif()

   set(UNITS_BUILD_OBJECT_LIBRARY OFF CACHE INTERNAL "")
   set(UNITS_BUILD_STATIC_LIBRARY ON CACHE INTERNAL "")
   set(UNITS_BUILD_SHARED_LIBRARY OFF CACHE INTERNAL "")
   set(UNITS_BUILD_CONVERTER_APP OFF CACHE INTERNAL "")
   set(UNITS_BUILD_WEBSERVER OFF CACHE INTERNAL "")
   set(UNITS_CLANG_TIDY_OPTIONS "" CACHE INTERNAL "")
   set(UNITS_BUILD_FUZZ_TARGETS OFF CACHE INTERNAL "")

   add_subdirectory(
        "${PROJECT_SOURCE_DIR}/ThirdParty/units" "${PROJECT_BINARY_DIR}/ThirdParty/units"
   )

   set_target_properties(units PROPERTIES FOLDER Extern)

   hide_variable(UNITS_HEADER_ONLY)
   hide_variable(UNITS_BUILD_OBJECT_LIBRARY)
   hide_variable(UNITS_NAMESPACE)

Then the target linked by

.. code-block:: cmake

   target_link_libraries(helics_common PUBLIC HELICS::utilities units::units)
