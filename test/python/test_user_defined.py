# Copyright (c) 2019-2025,
# Lawrence Livermore National Security, LLC;
# See the top-level NOTICE for additional details. All rights reserved.
# SPDX-License-Identifier: BSD-3-Clause

from units_llnl import Unit, add_user_defined_unit, defined_units_from_file


def test_user_defined_unit():
    clucks = Unit("19.3 m*A")

    add_user_defined_unit("clucks", clucks)
    add_user_defined_unit("sclucks", "23 m*mol*$")

    assert Unit("clucks/A") == Unit("19.3 m")
    assert Unit("sclucks/$") == Unit("23 m*mol")

    assert str(clucks) == "clucks"
    assert str(~clucks) == "1/clucks"

    sclucks = Unit("sclucks")
    assert not sclucks.is_error()
    assert str(Unit("ug") / sclucks**3) == "ug/sclucks^3"


def test_user_defined_unit_from_file():
    import os.path

    script_dir = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
    outputstr = defined_units_from_file(
        os.path.join(script_dir, "files/test_unit_files/other_units2.txt")
    )
    assert not outputstr
    y1 = Unit("yodles")
    assert y1 == Unit("73.0 count")

    y2 = Unit("yeedles")
    assert y2 == Unit("19.0 yodles")
    y3 = Unit("yimdles")
    assert y3 == Unit("12 yeedles")
    assert y3 == Unit("19*yodles*12")
