#Copyright (c) 2019-2024,
#Lawrence Livermore National Security, LLC;
#See the top-level NOTICE for additional details. All rights reserved.
#SPDX-License-Identifier: BSD-3-Clause

import units_llnl as u

def test_basic_measurement():
    m1=u.measurement('10 m')
    m2=u.measurement('2.5 s')
    m3=m1/m2
    m4=u.measurement('4.0 m/s')
    assert(m3==m4)
    
def test_basic_measurement2():
    m1=u.measurement(10,'m')
    m2=u.measurement(2.5, 's')
    m3=m1/m2
    m4=u.measurement(4.0, 'm/s')
    assert(m3==m4)
    
def test_basic_measurement3():
    
    u1=u.unit('m')
    u2=u.unit('s')
    
    m1=u.measurement(10,u1)
    m2=u.measurement(2.5, u2)
    
    m4=u.measurement(4.0, u1/u2)
    assert(m1/m2==m4)

def test_conditions():
    m1=u.measurement(1.0,'m')
    m2=u.measurement(3.0,u.unit('error'))
    m3=u.measurement(2.4,"infinity")
    assert(not m2.is_valid())
    assert(m1.is_normal())
    assert(not m2.is_normal())

    assert(m1.is_valid())

    assert(not m3.is_normal())
    assert(m3.is_valid())
    
def test_root():
    m1=u.measurement('100 m^6 per second squared')
    m2=m1.root(2)
    m3=m1.sqrt()
    m4=u.measurement('10 m*m*m/s')
    assert(m2==m3)
    assert(m3==m4)
    
def test_pow():
    m1=u.measurement('10 m')
    m2=m1**2
    m3=m1**3
    assert(m2==m1*m1)
    assert(m3==m1*m1*m1)
    
def test_comparisons():
    m1=u.measurement('10 m')
    m2=u.measurement('11 m')
    assert(m1<m2)
    assert(m2>m1)
    assert(m1>=m1)
    assert(m2<=m2)
    assert(m2>=m1)
    assert(m1<=m2)
    assert(m1!=m2)
