============
Measurements
============

The combination of a value and unit is known as a measurement.  In the units library they are constructed by multiplying or dividing a unit by a numerical value.

.. code-block:: c++

   measurement meas=10.0*m;
   measurement meas2=5.3/s;


They can also be constructed via the constructor

.. code-block:: c++

   measurement meas(10.0, kg);
   measurement meas2(2.7, MW);

There are two kinds of measurements the regular `measurement` which uses a double precision floating point for the value and a `precise_measurement` which uses a double and a `precise_unit`.
In terms of size the `measurement` class is 16 Bytes and the `precise_measurement` is 24 bytes.  
