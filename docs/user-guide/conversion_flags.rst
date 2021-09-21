=====================
Conversion Flags
=====================

The `units_from_string` and `to_string` operations take an optional flags argument. This controls certain aspects of the conversion process.  In most cases this can be left to default unless very specific needs arise.

Unit_from_string flags
-------------------------

-    `case_insensitive` --perform case insensitive matching for UCUM case insensitive matching
-    `single_slash` --specify that there is a single numerator and denominator only a single slash in the unit operations
-    `strict_si` --input units are strict SI

-    `strict_ucum` --input units are matching UCUM standard
-    `cooking_units` --input units for cooking and recipes are prioritized
-    `astronomy_units` --input units for astronomy are prioritized
-    `surveying_units` --input units for surveying are prioritized
-    `nuclear_units` --input units for nuclear physics and radiation are prioritized
-    `us_customary_units` -- input units for us customary measurements are prioritized(same as `cooking_units | surveying_units`)

-    `numbers_only` --indicate that only numbers should be matched in the first segments, mostly applies only to power operations
-    `no_recursion` --don't recurse through the string
-    `not_first_pass` --indicate that is not the first pass

-    `no_per_operators` --skip matching "per"
-    `no_locality_modifiers` --skip locality modifiers
-    `no_of_operator` --skip dealing with "of"

-    `no_commodities` --skip commodity checks

-    `skip_partition_check` --skip the partition check algorithm
-    `skip_si_prefix_check` --skip checking for SI prefixes
-    `skip_code_replacements` --don't do some code and sequence replacements

Indications for use
=========================
The `case_insensitive` flag should be used to ignore capitalization completely.  It is targeted at the UCUM upper case specification but is effective for all situations where case should be ignored.

The library is by nature somewhat flexible in capitalization patterns, because of this some strings are allowed that otherwise would not be if SI were strictly followed.  For example:  `Um` would match to micro meters which should not if being exacting to the SI standard.  The `strict_si` flag prevents some not all of these instances, and whether others can be disabled is being investigated.

The `single_slash` flag is targeted at a few specific programs which use the format of a single slash marking the separation of numerator from denominator.

`strict_ucum`, `cooking_units`, `astronomy_units`, `surveying_units`, `nuclear_units`, and  `us_customary_units` are part of the domain system and can change the unit matched.

The remainder of the flags are somewhat self explanatory and are primarily used as part of the string conversion program to prevent infinite recursion.  The `no_commodities` or `no_per_operator` may be used if it is known those do not apply for a slight increase in performance.  The `no_recursion` or `skip_partition_check` can be use if only simple strings are passed to speed up the process somewhat.

to_string Flags
---------------------
No flags currently affect the output though some are planned.
