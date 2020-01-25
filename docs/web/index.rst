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

The conversion also supports mathematical operations  see :ref:`Units From Strings` for additional details on string conversions

Rest API
-----------

The units web server does not serve files, it generates all responses on the fly.  There are 3 URI indicators it responds to beyond the root page.

-  `/convert`  : respond with an html page
-  `/convert_trivial` : respond with the results as a simple text
-  `/convert_json` : respond with a json string containing the requested conversions and the results.

For example in linux or anything with curl

.. code-block:: bash

   $ curl -s "13.52.135.81/convert_trivial?measurement=10%20tons&units=lb"
   20000

   $ curl -s "13.52.135.81/convert_json?measurement=10%20tons&units=lb"
   {
   "measurement":"10 tons",
   "units":"lb",
   "value":"20000"
   }

This works with POST or GET methods
