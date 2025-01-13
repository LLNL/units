# Copyright (c) 2019-2025,
# Lawrence Livermore National Security, LLC;
# See the top-level NOTICE for additional details. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause

import units_llnl as u
import math


def test_basic_measurement():
    m1 = u.Measurement("10 m")
    m2 = u.Measurement("2.5 s")
    m3 = m1 / m2
    m4 = u.Measurement("4.0 m/s")
    assert m3 == m4


def test_Quantity_alias():
    m1 = u.Quantity("10 m")
    m2 = u.Quantity("2.5 s")
    m3 = m1 / m2
    m4 = u.Quantity("4.0 m/s")
    assert m3 == m4


def test_basic_measurement2():
    m1 = u.Measurement(10, "m")
    m2 = u.Measurement(2.5, "s")
    m3 = m1 / m2
    m4 = u.Measurement(4.0, "m/s")
    assert m3 == m4


def test_basic_measurement3():

    u1 = u.Unit("m")
    u2 = u.Unit("s")

    m1 = u.Measurement(10, u1)
    m2 = u.Measurement(2.5, u2)

    m4 = u.Measurement(4.0, u1 / u2)
    assert m1 / m2 == m4
    assert m4.units == u1 / u2


def test_basic_measurement3():

    u1 = u.Unit("m")
    u2 = u.Unit("s")

    m1 = u.Measurement(value=10, unit=u1)
    m2 = u.Measurement(unit=u2, value=2.5)

    m4 = u.Measurement(4.0, u1 / u2)
    assert m1 / m2 == m4
    assert m4.units == u1 / u2


def test_conditions():
    m1 = u.Measurement(1.0, "m")
    m2 = u.Measurement(3.0, u.Unit("error"))
    m3 = u.Measurement(2.4, "infinity")
    assert not m2.is_valid()
    assert m1.is_normal()
    assert not m2.is_normal()

    assert m1.is_valid()

    assert not m3.is_normal()
    assert m3.is_valid()


def test_root():
    m1 = u.Measurement("100 m^6 per second squared")
    m2 = m1.root(2)
    m3 = m1.sqrt()
    m4 = u.Measurement("10 m*m*m/s")
    assert m2 == m3
    assert m3 == m4


def test_pow():
    m1 = u.Measurement("10 m")
    m2 = m1**2
    m3 = m1**3
    assert m2 == m1 * m1
    assert m3 == m1 * m1 * m1


def test_comparisons():
    m1 = u.Measurement("10 m")
    m2 = u.Measurement("11 m")
    assert m1 < m2
    assert m2 > m1
    assert m1 >= m1
    assert m2 <= m2
    assert m2 >= m1
    assert m1 <= m2
    assert m1 != m2


def test_set_value():
    m1 = u.Measurement("100 months")
    assert m1.value == 100
    m2 = m1.set_value(14)
    assert m2.value == 14


def test_set_units():
    m1 = u.Measurement("100 months")
    assert m1.units == u.Unit("month")
    m2 = m1.set_units(u.Unit("day"))
    assert m2.units == u.Unit("day")


def test_value_as():
    m1 = u.Measurement("20 weeks")
    assert m1.value_as("day") == 20 * 7
    u1 = u.Unit("hr")
    assert m1.value_as(u1) == 20 * 7 * 24


def test_convert_to():
    m1 = u.Measurement("20 weeks")
    m2 = m1.convert_to("day")
    assert m2.value == 20 * 7
    u1 = u.Unit("hr")
    m3 = m1.convert_to("hr")
    assert m3.value == 20 * 7 * 24

    m2 = m1.to("day")
    assert m2.value == 20 * 7
    u1 = u.Unit("hr")
    m3 = m1.to("hr")
    assert m3.value == 20 * 7 * 24
    m4 = m1.convert_to_base()
    assert m4.units == u.Unit("s")
    assert m4.units.multiplier == 1.0
    assert m4.value == 20 * 7 * 24 * 3600


def test_as_unit():
    m1 = u.Measurement("15 seconds")
    m2 = u.Measurement(4, m1.as_unit())

    assert m2.value == 4
    assert m2.value_as("s") == 60
    assert float(m1) == 15
    assert m1


def test_add_sub():
    m1 = u.Measurement("15 seconds")
    m2 = u.Measurement(1, "minute")
    m3 = m2 - m1
    assert m3.value == 0.75

    m4 = m3 + m2 + m1
    assert m4 == u.Measurement(120, "second")


def test_negation():
    m1 = u.Measurement("15 seconds")
    m3 = -m1
    assert m3.value == -15.0


def test_conditions():
    m1 = u.Measurement(34.5, "fq2te1tg1fe")
    assert not m1
    assert not bool(m1)

    m2 = u.Measurement(0, "m")
    assert not m2
    assert not bool(m2)
    assert m2.is_normal()


def test_mod():
    m1 = u.Measurement("18 seconds")
    m2 = u.Measurement("1 min")
    m3 = (m2 % m1).to("s")
    assert math.floor(m3.value) == 6
    m4 = m1 % 5
    assert m4.value == 3


def test_floor_div():
    m1 = u.Measurement("18 seconds")
    m2 = u.Measurement("1 min")
    m3 = m2 // m1
    assert m3.value == 3
    m4 = m1 // 4
    assert m4.value == 4
    assert m4.units == u.Unit("s")


def test_math_func():
    m1 = u.Measurement("15.78 seconds")
    m2 = u.Measurement("15.48 seconds")
    assert math.floor(m1).value == 15
    assert math.floor(m2).value == 15
    assert math.ceil(m1).value == 16
    assert math.ceil(m2).value == 16
    assert round(m1).value == 16
    assert round(m2).value == 15
    assert math.trunc(m1).value == 15
    assert math.trunc(m2).value == 15


def test_mult():
    m1 = u.Measurement("2 meters")
    m2 = u.Measurement(3, "meters")
    m3 = m2 * m1
    assert m3.value == 6

    m4 = 3 * m3
    assert m4 == u.Measurement(18, "meters squared")

    m5 = m3 * 3
    assert m5 == u.Measurement(18, "meters squared")


def test_vector_mult():
    m1 = u.Measurement("2 meters")
    m2 = u.Measurement(3, "meters")
    v1 = [5, 10, 15, 20, 25]
    mv3 = v1 * m1
    mv4 = m2 * v1

    assert mv3[2].value == 30
    assert mv4[3].value == 60


def test_div():
    m1 = u.Measurement("10 meters")
    m2 = u.Measurement(2, "seconds")
    m3 = m1 / m2
    assert m3.value == 5

    m4 = 10 / m3
    assert m4 == u.Measurement(2, "s/m")

    m5 = m3 / 2.5
    assert m5 == u.Measurement(2, "m/s")


def test_string():
    m1 = u.Measurement("10 lb")
    assert str(m1) == "10 lb"
    s3 = f"the measurement is {m1}"
    assert s3 == "the measurement is 10 lb"


def test_format():
    m1 = u.Measurement("9.7552 lb")
    s1 = f"the measurement is {m1:kg}"
    assert "kg" in s1

    s2 = f"the measurement is {m1:-}"
    assert s2 == "the measurement is 9.7552 "

    s3 = f"the measurement is {m1:-kg}"
    assert "kg" not in s3


def test_close():
    m1 = u.Measurement("10 lb")
    m2 = u.Measurement("10.0000000001 lb")
    assert m1 != m2
    assert m1.isclose(m2)


def test_to_dict():
    m1 = u.Measurement(value=25, unit="lb")
    dv = m1.to_dict()
    assert dv["value"] == 25
    assert dv["unit"] == "lb"
