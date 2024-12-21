# Copyright (c) 2019-2024,
# Lawrence Livermore National Security, LLC;
# See the top-level NOTICE for additional details. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause

import units_llnl as u


def test_basic_measurement():
    m1 = u.measurement("10 m")
    m2 = u.measurement("2.5 s")
    m3 = m1 / m2
    m4 = u.measurement("4.0 m/s")
    assert m3 == m4


def test_basic_measurement2():
    m1 = u.measurement(10, "m")
    m2 = u.measurement(2.5, "s")
    m3 = m1 / m2
    m4 = u.measurement(4.0, "m/s")
    assert m3 == m4


def test_basic_measurement3():

    u1 = u.unit("m")
    u2 = u.unit("s")

    m1 = u.measurement(10, u1)
    m2 = u.measurement(2.5, u2)

    m4 = u.measurement(4.0, u1 / u2)
    assert m1 / m2 == m4
    assert m4.units() == u1 / u2


def test_conditions():
    m1 = u.measurement(1.0, "m")
    m2 = u.measurement(3.0, u.unit("error"))
    m3 = u.measurement(2.4, "infinity")
    assert not m2.is_valid()
    assert m1.is_normal()
    assert not m2.is_normal()

    assert m1.is_valid()

    assert not m3.is_normal()
    assert m3.is_valid()


def test_root():
    m1 = u.measurement("100 m^6 per second squared")
    m2 = m1.root(2)
    m3 = m1.sqrt()
    m4 = u.measurement("10 m*m*m/s")
    assert m2 == m3
    assert m3 == m4


def test_pow():
    m1 = u.measurement("10 m")
    m2 = m1**2
    m3 = m1**3
    assert m2 == m1 * m1
    assert m3 == m1 * m1 * m1


def test_comparisons():
    m1 = u.measurement("10 m")
    m2 = u.measurement("11 m")
    assert m1 < m2
    assert m2 > m1
    assert m1 >= m1
    assert m2 <= m2
    assert m2 >= m1
    assert m1 <= m2
    assert m1 != m2


def test_set_value():
    m1 = u.measurement("100 months")
    assert m1.value() == 100
    m2 = m1.set_value(14)
    assert m2.value() == 14


def test_set_units():
    m1 = u.measurement("100 months")
    assert m1.units() == u.unit("month")
    m2 = m1.set_units(u.unit("day"))
    assert m2.units() == u.unit("day")


def test_value_as():
    m1 = u.measurement("20 weeks")
    assert m1.value_as("day") == 20 * 7
    u1 = u.unit("hr")
    assert m1.value_as(u1) == 20 * 7 * 24


def test_convert_to():
    m1 = u.measurement("20 weeks")
    m2 = m1.convert_to("day")
    assert m2.value() == 20 * 7
    u1 = u.unit("hr")
    m3 = m1.convert_to("hr")
    assert m3.value() == 20 * 7 * 24

    m4 = m1.convert_to_base()
    assert m4.units() == u.unit("s")
    assert m4.units().multiplier() == 1.0
    assert m4.value() == 20 * 7 * 24 * 3600


def test_as_unit():
    m1 = u.measurement("15 seconds")
    m2 = u.measurement(4, m1.as_unit())

    assert m2.value() == 4
    assert m2.value_as("s") == 60


def test_add_sub():
    m1 = u.measurement("15 seconds")
    m2 = u.measurement(1, "minute")
    m3 = m2 - m1
    assert m3.value() == 0.75

    m4 = m3 + m2 + m1
    assert m4 == u.measurement(120, "second")


def test_mult():
    m1 = u.measurement("2 meters")
    m2 = u.measurement(3, "meters")
    m3 = m2 * m1
    assert m3.value() == 6

    m4 = 3 * m3
    assert m4 == u.measurement(18, "meters squared")

    m5 = m3 * 3
    assert m5 == u.measurement(18, "meters squared")


def test_div():
    m1 = u.measurement("10 meters")
    m2 = u.measurement(2, "seconds")
    m3 = m1 / m2
    assert m3.value() == 5

    m4 = 10 / m3
    assert m4 == u.measurement(2, "s/m")

    m5 = m3 / 2.5
    assert m5 == u.measurement(2, "m/s")


def test_string():
    m1 = u.measurement("10 lb")
    assert m1.to_string() == "10 lb"
    s3 = f"the measurement is {m1}"
    assert s3 == "the measurement is 10 lb"
