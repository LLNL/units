==================
Python
==================
The Python wrapper for the units library is a simplified version of the library.  It is focused on the string operations of the library and conversions between units and measurements.
The two key classes are `Unit` which encapsulates a specific unit of measure and optional commodity if desired, and `Measurement` which captures a value + `Unit`.  Math operations are supported on the types.
The units library is available through a pypi_ package

.. code-block:: sh

    pip install units-llnl

Usage
-----------

.. code-block:: python

    from units_llnl import Unit

    u1 = Unit("m")
    u2 = Unit("s")
    u3 = u1 / u2
    speed = 20
    desired = "mph"
    # convert the unit to miles per hour
    result = u3.convert(speed, desired)
    print(f"{20} {u3} = {result} {desired}")

This will print a result `20 m/s = 44.73872584108805 mph`

.. code-block:: python

    from units_llnl import Measurement

    m1 = Measurement("220 m")
    m2 = Measurement("11 s")
    m3 = m1 / m2
    desired = "mph"
    # convert the unit to miles per hour
    result = m3.convert_to(desired)
    print(f"{m3} = {result}")

This will produce `20 m/s = 44.7387258411 mph`

See the pypi_ landing page for a complete description of all methods and functions.

.. _pypi: https://pypi.org/project/units-llnl/

Future expansions will include uncertain units and some additional math operations on measurements
