==============================
Parsing of squared and cubic
==============================

When units are written there are a few terms that modify the powers of a unit.
The two primary terms are `square` and `cubic`

These are rules the library follows when parsing terms such as this

-  `square` or `sq` or `sq.` will apply to the unit immediately following the term
-  `cubic` or `cu` or `cu.` will apply the unit immediately following the term
-  `squared` will apply to the unit immediately preceding the term
-  `cubed` will apply to the unit immediately preceding the term
