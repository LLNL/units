=====================================
Converter App
=====================================

As a simple example and potentially useful tool we made the converter app.  It is a command line application that can be built as part of the units library to convert units given on the command line.

.. code-block:: bash

   $ ./unit_convert 10 m ft
   32.8084

   $ ./unit_convert ten meters per second mph
   22.3694

   $ ./unit_convert --full ten meters per second mph
   ten meters per second = 22.3694 mph

   $ ./unit_convert --simplified ten meters per second miles/hour
   10 m/s = 22.3694 mph

   $ ./unit_convert -s four hundred seventy-three kilograms per hour pounds/min
   473 kg/hr = 17.3798 lb/min

   $ ./unit_convert -s 22 british fathoms *
   10 british fathoms = 18.288 m


basically there are two options `--full,-f` and `--simplified,-s`  a measurement which will take an arbitrary number of strings and a final string as a unit to convert to.  It outputs the conversion and if specified the surrounding measurement and units either simplified or in the original.  Using `*` or `<base>` in place of the unit string will result in converting the measurement to base units.

.. code-block:: bash

   $ ./unit_convert --help
   application to perform a conversion of a value from one unit to another
   Usage: unit_convert [OPTIONS] measure... convert

   Positionals:
     measure [TEXT ...] ... REQUIRED
                              measurement to convert .e.g '57.4 m', 'two thousand GB' '45.7*22.2 feet^3/s^2'
     convert TEXT REQUIRED       the units to convert the measurement to

   Options:
     -h,--help                   Print this help message and exit
     -f,--full                   specify that the output should include the measurement and units
     -s,--simplified             simplify the units using the units library to_string functions and print the conversion string like full. This option will take precedence over --full
     --measurement [TEXT ...] ... REQUIRED
                              measurement to convert .e.g '57.4 m', 'two thousand GB' '45.7*22.2 feet^3/s^2'
     --convert TEXT REQUIRED     the units to convert the measurement to
