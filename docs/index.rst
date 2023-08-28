.. units documentation master file, created by
   sphinx-quickstart on Fri Jan 17 16:04:22 2020.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

*************************************************************
Welcome to The units library user guide and documentation!
*************************************************************

The Units library provides a means of working with units of measurement at runtime, including conversion to and from strings. It provides a small number of types for working with units and measurements and operations necessary for user input and output with units.

This software was developed for use in `LLNL/GridDyn <https://github.com/LLNL/GridDyn>`_, and `HELICS <https://github.com/GMLC-TDC/HELICS>`_ and is currently a work in progress (though getting close). Namespaces, function names, and code organization is subject to change though is fairly stable at this point, input is welcome.

|codecov| |Azure| |Circle CI| |License|

.. |codecov| image:: https://codecov.io/gh/LLNL/units/branch/main/graph/badge.svg
   :target: https://codecov.io/gh/LLNL/units

.. |Azure| image:: https://dev.azure.com/phlptp/units/_apis/build/status/LLNL.units?branchName=main
    :target: https://dev.azure.com/phlptp/units/_build/latest?definitionId=1&branchName=main

.. |Circle CI| image:: https://circleci.com/gh/LLNL/units.svg?style=svg
    :target: https://circleci.com/gh/LLNL/units

.. |License| image:: https://img.shields.io/badge/License-BSD-blue.svg
    :target: https://github.com/GMLC-TDC/HELICS-src/blob/main/LICENSE

The :ref:`Introduction` is a discussion about the why? and How the library came together and a generally what it does and how it was tested.
The :ref:`Installation and Linking` guide is a discussion about linking and using the library, and the :ref:`User Guide` is the
how-to about how to use the software library.  For the details see :ref:`Details`  and to try out some of the string conversions check out :ref:`Units on the Web`
Finally :ref:`Application Notes` contains some discussions on particular applications and usages.

.. toctree::
   :maxdepth: 1
   :caption: Basics

   introduction/index
   installation/index
   user-guide/index
   application_notes/index
   details/index
   web/index
