#Copyright (c) 2019-2024,
#Lawrence Livermore National Security, LLC;
#See the top-level NOTICE for additional details. All rights reserved.
#SPDX-License-Identifier: BSD-3-Clause

import units_llnl as u

def test_basic_unit():
    u1=u.unit('m')
    u2=u.unit('s')
    u3=u1/u2
    u4=u.unit('mph')
    assert(u3.is_convertible(u4))

def test_conditions():
    u1=u.unit('m')
    u2=u.unit('error')
    u3=u.unit("infinity")
    assert(not u1.is_error())
    assert(u2.is_error())
    assert(u1.is_normal())
    assert(not u2.is_normal())

    assert(not u2.is_default())

    assert(u1.is_valid())

    assert(u1.is_finite())
    assert(not u3.is_finite())