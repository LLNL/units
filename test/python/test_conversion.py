# Copyright (c) 2019-2025,
# Lawrence Livermore National Security, LLC;
# See the top-level NOTICE for additional details. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause

import units_llnl as u


def test_convert_units():
    u1 = u.Unit("m")
    u2 = u.Unit("cm")
    v1 = u.convert(10, u1, u2)
    assert v1 == 10 * 100

    v2 = u.convert(unit_in=u1, unit_out=u2, value=20)
    assert v2 == 2000


def test_convert_string():
    v1 = u.convert(10, "m", "mm")
    assert v1 == 10 * 1000

    v2 = u.convert(unit_out="mm", unit_in="m", value=20)
    assert v2 == 20000


def test_convert_units_pu():
    u1 = u.Unit("puMW")
    u2 = u.Unit("kW")
    v1 = u.convert_pu(0.5, u1, u2, 100)
    assert v1 == 50000

    v2 = u.convert_pu(unit_in=u1, unit_out=u2, value=1.2, base=100)
    assert v2 == 120000


def test_convert_string_pu():
    v1 = u.convert_pu(0.5, "puMW", "kW", 100)
    assert v1 == 50000

    v2 = u.convert_pu(unit_in="puMW", unit_out="kW", value=1.2, base=100)
    assert v2 == 120000
