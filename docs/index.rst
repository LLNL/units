.. units documentation master file, created by
   sphinx-quickstart on Fri Jan 17 16:04:22 2020.
   You can adapt this file completely to your liking, but it should at least
   contain the root `toctree` directive.

*************************************************************
Welcome to The units library user guide and documentation!
*************************************************************

A library that provides runtime unit values, instead of individual unit types, for the purposes of working with units of measurement at run time possibly from user input or configuration files.  It supports conversion and interpretation of units to and from string representations.

This software was developed for use in LLNL/GridDyn, and is currently a work in progress. Namespaces, function names, and code organization is subject to change, input is welcome.

|Build Status| |codecov| |Codacy| |Azure| |Circle CI| |License|

.. |Build Status| image:: https://travis-ci.com/LLNL/units.svg?branch=master
   :target: https://travis-ci.com/LLNL/units

.. |codecov| image:: https://codecov.io/gh/LLNL/units/branch/master/graph/badge.svg
   :target: https://codecov.io/gh/LLNL/units

.. |Codacy| image:: https://api.codacy.com/project/badge/Grade/c0b5367026f34c4a9dc94ca4c19c770a
   :target: https://app.codacy.com/app/phlptp/units?utm_source=github.com&utm_medium=referral&utm_content=LLNL/units&utm_campaign=Badge_Grade_Settings

.. |Azure| image:: https://dev.azure.com/phlptp/units/_apis/build/status/LLNL.units?branchName=master
    :target: https://dev.azure.com/phlptp/units/_build/latest?definitionId=1&branchName=master

.. |Circle CI| image:: https://circleci.com/gh/LLNL/units.svg?style=svg
    :target: https://circleci.com/gh/LLNL/units

.. |License| image:: https://img.shields.io/badge/License-BSD-blue.svg
    :target: https://github.com/GMLC-TDC/HELICS-src/blob/master/LICENSE

The :ref:`Introduction` is a discussion about the why? and How the library came together and a generally what it does and how it was tested.
The :ref:`Installation and Linking` guide is a discussion about linking and using the library, and the :ref:`User Guide` is the
how-to about how to use the software library.  For the details see :ref:`Details`  and to try out some of the string conversions check out :ref:`Units on the Web`

.. toctree::
   :maxdepth: 1
   :caption: Basics

   introduction/index
   installation/index
   user-guide/index
   details/index
   web/index

Indices and tables
==================

* :ref:`genindex`
* :ref:`modindex`
* :ref:`search`
