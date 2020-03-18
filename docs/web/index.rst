==================
Units on the Web
==================

You can try out the string conversions through the units
`Webserver <../_static/convert.html>`_

This page allows you to enter a measurement string and a unit string for conversion.

The measurement string can be of any form with a number and units

-   `10 m`
-   `hundred pounds`
-   `45.673 GB`
-   `dozen feet`

the unit string should be some unit that is convertible from the measurement units:

-   `inches`
-   `troy oz`
-   `kiB`
-   `british fathoms`

The conversion also supports mathematical operations  see :ref:`Units From Strings` for additional details on string conversions.  The units can also be set to `*`or `<base>` to convert the measurement to base units.

Rest API
-----------

The units web server does not serve files, it generates all responses on the fly.  There are 3 URI indicators it responds to beyond the root page.

-  `/convert`  : responds with an html page
-  `/convert_trivial` : responds with the results as a simple text
-  `/convert_json` : responds with a json string containing the requested conversions and the results.

For example in Linux or anything with curl

.. code-block:: bash

   $ curl -s "13.52.135.81/convert_trivial?measurement=10%20tons&units=lb"
   20000

   $ curl -s "13.52.135.81/convert_json?measurement=10%20tons&units=lb"
   {
   "request_measurement":"",
   "request_units":"lb"",
   "measurement":"",
   "units":"lb"",
   "value":"nan"
   }

    $ curl -s "13.52.135.81/convert_json?measurement=ten%20meterspersecond&units=feetperminute&caction=to_string"
   {
   "request_measurement":"ten meterspersecond",
   "request_units":"feetperminute",
   "measurement":"10 m/s",
   "units":"ft/min",
   "value":"1968.5"
   }

This works with POST or GET methods.  The `caction` field can be set to "to_string" this will "simplify" the units in the result or at least use the internal to_string operations to convert to an interpretable string in more accessible units.
