=====================
Conversion Flags
=====================

The `units_from_string` and `to_string` operations take an optional flags argument. This controls certain aspects of the conversion process.  In most cases this can be left to default unless very specific needs arise.

Unit_from_string flags
-------------------------
-    `default_conversions`  -- no_flags, so using the default operations
-    `case_insensitive` --perform case insensitive matching for UCUM case insensitive matching
-    `single_slash` --specify that there is a single numerator and denominator only a single slash in the unit operations
-    `strict_si` --input units are strict SI

-    `strict_ucum` --input units are matching UCUM standard
-    `cooking_units` --input units for cooking and recipes are prioritized
-    `astronomy_units` --input units for astronomy are prioritized
-    `surveying_units` --input units for surveying are prioritized
-    `nuclear_units` --input units for nuclear physics and radiation are prioritized
-    `climate_units` --input units for climate sciences
-    `us_customary_units` -- input units for us customary measurements are prioritized(same as `cooking_units | surveying_units`)

-    `numbers_only` --indicate that only numbers should be matched in the first segments, mostly applies only to power operations
-    `no_recursion` --don't recurse through the string
-    `not_first_pass` --indicate that is not the first pass

-    `no_per_operators` --skip matching "per"
-    `no_locality_modifiers` --skip locality modifiers
-    `no_of_operator` --skip dealing with "of"
-    `no_addition` -- skip trying unit addition

-    `no_commodities` --skip commodity checks

-    `skip_partition_check` --skip the partition check algorithm
-    `skip_si_prefix_check` --skip checking for SI prefixes
-    `skip_code_replacements` --don't do some code and sequence replacements

-    `minimum_partition_size2` --specify that any unit partitions must be greater or equal to 2 characters
-    `minimum_partition_size3` --specify that any unit partitions must be greater or equal to 3 characters
-    `minimum_partition_size4` --specify that any unit partitions must be greater or equal to 4 characters
-    `minimum_partition_size5` --specify that any unit partitions must be greater or equal to 5 characters
-    `minimum_partition_size6` --specify that any unit partitions must be greater or equal to 6 characters
-    `minimum_partition_size7` --specify that any unit partitions must be greater or equal to 7 characters

Indications for use
=========================
The `case_insensitive` flag should be used to ignore capitalization completely.  It is targeted at the UCUM upper case specification but is effective for all situations where case should be ignored.

The library is by nature somewhat flexible in capitalization patterns, because of this some strings are allowed that otherwise would not be if SI were strictly followed.  For example:  `Um` would match to micro meters which should not if being exacting to the SI standard.  The `strict_si` flag prevents some not all of these instances, and whether others can be disabled is being investigated.

The `single_slash` flag is targeted at a few specific programs which use the format of a single slash marking the separation of numerator from denominator.

`strict_ucum`, `cooking_units`, `astronomy_units`, `surveying_units`, `nuclear_units`, `climate_units` and  `us_customary_units` are part of the domain system and can change the unit matched.

The remainder of the flags are somewhat self explanatory and are primarily used as part of the string conversion program to prevent infinite recursion.  The `no_commodities` or `no_per_operator` may be used if it is known those do not apply for a slight increase in performance.  The `no_recursion` or `skip_partition_check` can be use if only simple strings are passed to speed up the process somewhat.

The minimum partition size flags can be used to restrict how much partitioning it does which can reduce the possibility of "false positives" or unwanted unit matches on the strings

All the flags can be "or"ed to make combinations  such as `minimum_partition_size4|astronomy_units|no_of_operator`

to_string Flags
---------------------

- `disable_large_power_strings` - if the units definition allows large powers this flag can disable the use of them in the output string.

The to_string flags can be combined with the other conversion flags without issue.

Default flags
====================
Flags will normally default to `0U` however they can be modified through `setDefaultFlags`.  This function returns the previous value in case it is needed to swap them temporarily.
The flags can be retrieved via `getDefaultFlags()`  This function is automatically called if no flag argument is passed.  The initial value can be set through a compile time or build time option `UNITS_DEFAULT_MATCH_FLAGS`.
