# Copyright (c) 2019-2025,
# Lawrence Livermore National Security, LLC;
# See the top-level NOTICE for additional details. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause

import units_llnl as u

from units_llnl import Unit, Measurement, Dimension, asdimension


def test_dimensions():
    d1 = Dimension("length")
    d2 = Dimension("distance")
    assert d1 == d2


def test_dimensions_with_bracketstring():
    d1 = Dimension("[length]")
    d2 = Dimension("[distance]")
    assert d1 == d2


def test_dimension_unit_constructor():
    d1 = Dimension(Unit("m"))
    d2 = Dimension(Unit("ft"))
    d3 = Dimension("[length]")
    assert d1 == d2
    assert d2 == d3


def test_dimension_unit_dim_prop():
    d1 = Unit("m").dimension
    d2 = Unit("ft").dimension
    d3 = Dimension("[length]")
    assert d1 == d2
    assert d2 == d3


def test_dimension_measure_constructor():
    d1 = Dimension(Measurement("10 m"))
    d2 = Dimension(Measurement("0.2586 ft"))
    d3 = Dimension("length")
    assert d1 == d2
    assert d3 == d2


def test_dimension_measure_dim_prop():
    d1 = Measurement("10 m").dimension
    d2 = Measurement("0.2586 ft").dimension
    d3 = Dimension("length")
    assert d1 == d2
    assert d3 == d2


def test_dimensions_multiply():
    d1 = Dimension("length")
    d2 = Dimension("distance")
    d3 = d1 * d2
    assert str(d3) == "[area]"


def test_dimensions_divide():
    d1 = Dimension("volume")
    d2 = Dimension("area")
    d3 = d1 / d2
    assert str(d3) == "[length]"


def test_dimensions_units():
    d1 = Dimension("volume")
    u1 = Unit("m^3")
    assert d1.default_unit == u1


def test_dimensions_decompose1():
    d1 = Dimension("volume")
    decomposition = d1.decompose()

    assert decomposition["meters"] == 3


def test_dimensions_decompose2():
    u1 = Unit("$ per watt radian")
    dim = u1.dimension
    decomposition = dim.decompose()

    dim2 = Dimension(decomposition)
    assert dim2 == dim
    assert dim2.default_unit == u1
