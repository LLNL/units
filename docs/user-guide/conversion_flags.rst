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
