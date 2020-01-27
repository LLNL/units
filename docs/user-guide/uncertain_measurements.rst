=======================
Uncertain Measurements
=======================

The units library supports a class of measurements including an uncertainty measurement.

For Example `3.0±0.2m`  would indicate a measurement of 3.0 meters with an uncertainty of 0.2 m.  

All operations are supported 
The propagation of uncertainty follow the root sum of squares methods outlined `Here<http://lectureonline.cl.msu.edu/~mmp/labs/error/e2.htm>`_.  
There are methods available such as `simple_divide`, `simple_product`, `simple_sum` and `simple_subtract` that just sum the uncertainties.  The method in use in the regular operators assume that the mesurements used in the mathematical operation are independent.  A more thorough explanation can be found `Here<http://web.mit.edu/fluids-modules/www/exper_techniques/2.Propagation_of_Uncertaint.pdf>`_.


The structure of an uncertain measurement consists of a float for the measurement value and a float for the uncertainty, and `unit` for the unit of the measurement.  

Constructors
----------------

There are a number of different constructors for an uncertain measurement aimed at specify the uncertatinty and measurement in different ways.  

-   `constexpr uncertain_measurement()`   default constructor with 0 values for the value and uncertainty and a one for the unit.  

-   `constexpr uncertain_measurement(<float|double> val, <float|double> uncertainty, unit base)` : specify the parameters with values
    
-   `constexpr uncertain_measurement(<float|double> val, unit base)`:  Just specify the value and unit, assume 0.0 uncertainty.  

-    `constexpr uncertain_measurement(measurement val, float uncertainty) noexcept` : construct from a measurement and uncertainty value
   
-   `uncertain_measurement(measurement val, measurement uncertainty) noexcept`:  construct from a measurement value and uncertainty measurement.  The uncertainty is converted to the same units as the value measurement.  
      

Additional operators
----------------------

Beyond the operations used in :ref:`Measurements`, there are some specific functions related to getting and setting the uncertainty.

-   `uncertain_measurement& uncertainty(<double|float> newUncertainty)` :  Will set the uncertainty value as a numerical value 
-   `uncertain_measurement& uncertainty(const measurement &newUncerrtainty)`: will set the uncertainty as a measurement in specific units.  
-   `double uncertainty()`:  Will get the current numerical value of the uncertainty 
-   `double uncertainty_as(units)`:  will get the value of the uncertainty in specific units.  
-   `float uncertainty_f()`: will get the value of the uncertainty as a single precision floating point value 
-   `constexpr measurement uncertainty_measurement()`:  will return a measurement containing the uncertainty.  
-   `double fractional_uncertainty()`: will get the fractional uncertainty value. which is uncertainty/\|value\|.  


