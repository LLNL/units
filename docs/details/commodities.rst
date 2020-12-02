==================
Commodity Details
==================

The `precise_unit` class includes an unsigned 32-bit unsigned integer that represents a commodity of some kind.

This is a 32 bit code representing a commodity and possibly containers or form factor.

So while there is some predefined structure to the commodities. Any user is free to use it however they like as it can be manipulated as 32 bit code however a user might wish to use it.  The conversion to and from string is governed by the following rules.

The high order bit(31) is a power, either 1 or -1.  So a 1 in high bit represents and inverse commodity,  for example a unit of $/oz of gold would have an inverse power of gold, while the $/oz would be in the `precise_unit`.  Upon division all bits in the commodity are inverted.  


Control code
----------------

bits 29 and 30 are control codes
`00` is a normal commodity
`01` is a normal commodity with form factor code
`10` is a predefined direct string.
`11` is a custom commodity defined in a map storage

Direct String Commodities
============================
To avoid always having to do a map lookup, many commodities or commodity codes can be represented by short string of 5 or fewer characters.  These cannot be case sensitive so '_' is a space or null character and if at the end will be removed for display purposes.  The very limited character set includes '_', `a-z', '`' and, '{|}~'.  This is meant to simplify a chunk of the use cases.  custom commodity string which are not captured in this mode fall into the custom commodity bin

Custom Commodity
=======================
String which can't be represented by the very simplistic short string mode are placed into a hash table for lookup and assigned a hash code generated from the string.  The string is converted to a 29-bit hash placed in the lower 29 bits of the commodity code.  

Commodity with Container 
=============================
Frequently commodities come in a specific form factor.  With a form factor code in place this can represent a form factor independent of the actual commodity material.  For example a drum of oil vs a drum of gasoline.  
  the container is wrapped in a 8-bit code bits 21-28.  The commodity itself is contained in bits 0-19.  

Normal Commodity 
============================

The codes used for normal commodity are the same as those used with a container with the exception that the additional 8 bit can be used for more specific codes of that commodity used for international trade.  The codes used are based on the harmonized system for international trade <https://www.trade.gov/harmonized-system-hs-codes>`_  The 0-20 bits contain the harmonized system 6 digit code.  The chapter is contained in bits 14-20, the section in bits 7-15, and the subsection in bits 0-6.   This allows structure that can act as a mask on specific types of commodities.  Common commodities are mapped to chapter and section mostly, though some exceptions go to the subsection for commodity to string translation.   The 6 digit harmonized commodity code is the same between using with a container and without.    If no container is used.  the additional 8 bits, can represent the country specific codes.  

  In the normalized code 7 bit sections, the codes for 100-127 represent other commodities that cannot be represented in the allowable 8 bits of space.  These are stored in a hash map when used for later reference as needed.  This allows representation of a large percentage of codes with no additional overhead and an additional 5.6 million codes through the hash structure.  This is far more codes than are presently in use.  