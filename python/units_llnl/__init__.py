from typing import TypeAlias
from .units_llnl_ext import (
    Unit,
    Measurement,
    Dimension,
    convert,
    convert_pu,
    default_unit,
    add_user_defined_unit,
    defined_units_from_file,
    __doc__,
)

Quantity: TypeAlias = Measurement


def asdimension(arg) -> Dimension:
    return Dimension(arg)


def asunit(*args) -> Unit:
    return Unit(*args)


def asquantity(*args) -> Quantity:
    return Measurement(*args)
