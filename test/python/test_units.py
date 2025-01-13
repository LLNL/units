# Copyright (c) 2019-2025,
# Lawrence Livermore National Security, LLC;
# See the top-level NOTICE for additional details. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause

import units_llnl as u


def test_basic_unit():
    u1 = u.Unit("m")
    u2 = u.Unit("s")
    u3 = u1 / u2
    u4 = u.Unit("mph")
    assert u3.is_convertible_to(u4)


def test_unit_constructor():
    u1 = u.Unit("cm")
    u2 = u.Unit(100, u1)

    assert u2 == u.Unit("m")
    assert u2


def test_unit_constructor2():
    u1 = u.Unit(unit="cm")
    u2 = u.Unit(100, u1)

    assert u2 == u.Unit(unit="m")
    assert u2


def test_unit_constructor3():
    u1 = u.Unit(unit="kg", multiplier=10, commodity="gold")

    assert u1.multiplier == 10.0
    assert u1.commodity == "gold"
    assert u1.base_units == u.Unit("kg")


def test_basic_multiplication():
    u1 = u.Unit("m")
    u2 = u.Unit("s")
    u3 = u1 / (u2 * u2)
    assert u3 == u.Unit("m/s^2")


def test_conditions():
    u1 = u.Unit("m")
    u2 = u.Unit("error")
    u3 = u.Unit("infinity")
    assert not u1.is_error()
    assert bool(u1)
    assert u2.is_error()
    assert u1.is_normal()
    assert not u2.is_normal()
    assert not bool(u2)
    assert u1.is_valid()

    assert u1.isfinite()
    assert not u1.isinf()
    assert u3.is_valid()
    assert not u3.isfinite()
    assert u3.isinf()

    u4 = u.Unit("puMW")
    assert u4.is_per_unit()
    assert not u3.is_per_unit()


def test_bad_unit():
    ue = u.Unit("qdfgqtegqgqg")
    assert not ue
    assert ue.is_error()


def test_zero():
    u1 = u.Unit("0")
    u2 = u.Unit("nan")
    u3 = u.Unit("m/s")
    u4 = u.Unit(0, u3)
    assert not bool(u1)
    assert not u1
    assert not bool(u2)
    assert not u2
    assert u3
    assert bool(u3)
    assert not bool(u4)
    assert not u4


def test_root():
    u1 = u.Unit("m^6 per second squared")
    u2 = u1.root(2)
    u3 = u1.sqrt()
    u4 = u.Unit("m*m*m/s")
    assert u2 == u3
    assert u3 == u4


def test_base():
    u1 = u.Unit("3 ft")
    u2 = u.Unit("yd")
    u3 = u.Unit("mm")
    assert u1.is_exactly_the_same(u1)
    assert u1 == u2
    assert u1.has_same_base(u2)
    assert u2.has_same_base(u3)


def test_multiplier():
    u1 = u.Unit("nm")
    assert u1.multiplier == 1e-9
    u2 = u1.set_multiplier(1e-6)
    assert u2 == u.Unit("um")
    assert u2.multiplier == 1e-6
    assert u1 != u2


def test_commodity():
    u1 = u.Unit("lb", "gold")
    assert u1.commodity == "gold"
    u2 = u1.set_commodity("silver")
    assert u2.commodity == "silver"


def test_string():
    u1 = u.Unit("lb")
    assert str(u1) == "lb"
    s3 = f"the unit is {u1}"
    assert s3 == "the unit is lb"
    u3 = u.Unit()
    assert str(u3) == ""


def test_inv():
    u1 = u.Unit("s")
    assert ~u1 == u.Unit("Hz")
    u3 = ~(~u1)
    assert u3 == u1


def test_hash():
    u1 = u.Unit("25.6 in")
    u2 = u1
    u3 = u.Unit("11.3 m")
    h1 = hash(u1)
    h2 = hash(u2)
    h3 = hash(u3)
    assert h1 == h2
    assert h1 != h3


def test_dictionary():
    d1 = {}

    u1 = u.Unit("25.6 in")
    u2 = u1
    u3 = u.Unit("11.3 m")
    d1[u1] = "in"
    d1[u3] = "m"

    assert d1[u2] == "in"
    assert d1[u3] == "m"


def test_to_dict():
    u1 = u.Unit(10.0, "m")
    v1 = u1.to_dict()
    assert v1["unit"] == "10m"


def test_float_mult():
    u1 = u.Unit("m")
    m3 = 10 * u1
    assert type(m3).__name__ == "Measurement"
    assert m3.value == 10

    m4 = u1 * 12
    assert type(m4).__name__ == "Measurement"
    assert m4.value == 12


def test_mult_vect():
    u1 = u.Unit("m")

    v1 = [10, 20, 40]
    mv = v1 * u1
    assert type(mv[0]).__name__ == "Measurement"
    print(mv)
    assert mv[1].value == 20

    mv = u1 * v1
    assert type(mv[2]).__name__ == "Measurement"
    print(mv)
    assert mv[2].value == 40


def test_convert_units():
    u1 = u.Unit("m")
    u2 = u.Unit("cm")
    v1 = u1.convert(10, u2)
    assert v1 == 10 * 100

    v2 = u1.convert(unit_out=u2, value=20)
    assert v2 == 2000

    v1 = u1.to(10, u2)
    assert v1 == 10 * 100

    v2 = u1.to(unit_out=u2, value=20)
    assert v2 == 2000


def test_convert_string():
    u1 = u.Unit("m")
    v1 = u1.convert(10, "mm")
    assert v1 == 10 * 1000

    v2 = u1.convert(unit_out="mm", value=20)
    assert v2 == 20000

    v1 = u1.to(10, "mm")
    assert v1 == 10 * 1000

    v2 = u1.to(unit_out="mm", value=20)
    assert v2 == 20000
