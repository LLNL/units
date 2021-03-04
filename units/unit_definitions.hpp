/*
Copyright (c) 2019-2021,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#pragma once

#include "units_decl.hpp"

#include <algorithm>
#include <array>
#include <cstdlib>
#include <limits>

namespace UNITS_NAMESPACE {
/// Constants used in definitions of units

static_assert(
    std::numeric_limits<double>::has_signaling_NaN,
    "nan is used to signify invalid values");
static_assert(
    std::numeric_limits<double>::has_infinity,
    "nan is used to signify invalid values");
namespace constants {
    constexpr double pi = 3.14159265358979323846;
    constexpr double tau = 2.0 * pi;
    constexpr double invalid_conversion =
        std::numeric_limits<double>::signaling_NaN();
    constexpr double infinity = std::numeric_limits<double>::infinity();
}  // namespace constants

/// basic commodity definitions
namespace commodities {
    // https://en.wikipedia.org/wiki/List_of_traded_commodities
    enum commodity : std::uint32_t {
        water = 1,
        // metals
        gold = 2,
        copper = 4,
        silver = 6,
        platinum = 7,
        palladium = 8,
        zinc = 9,
        tin = 10,
        lead = 11,
        aluminum = 12,
        alluminum_alloy = 13,
        nickel = 14,
        cobolt = 15,
        molybdenum = 16,

        // energy
        oil = 101,
        heat_oil = 102,
        nat_gas = 103,
        brent_crude = 104,
        ethanol = 105,
        propane = 107,
        // grains
        wheat = 404,
        corn = 405,
        soybeans = 406,
        soybean_meal = 407,
        soybean_oil = 408,
        oats = 409,
        rice = 410,
        red_wheat = 411,
        spring_wheat = 412,
        canola = 413,
        rough_rice = 414,
        rapeseed = 415,
        adzuci = 418,
        barley = 420,
        // meats
        live_cattle = 601,
        feeder_cattle = 602,
        lean_hogs = 603,
        milk = 604,

        // soft
        cotton = 945,
        orange_juice = 947,
        sugar = 948,
        sugar_11 = 949,
        sugar_14 = 950,
        coffee = 952,
        cocoa = 961,
        palm_oil = 971,
        rubber = 999,
        wool = 946,
        lumber = 5007,

        // other common unit blocks
        people = 115125,
        particles = 117463,
        cars = 43567,
        flop = 215262,
        instruction = 452255,

        // clinical
        tissue = 4622626,
        cell = 45236884,
        embryo = 52632253,
        Hahnemann = 2352622,
        Korsakov = 262626562,
        protein = 325255,
        creatinine = 2566225,

        // computer
        pixel = 516115414,
        voxel = 516115415,
        errors = 516115418,
    };
}  // namespace commodities

namespace precise {
    // base units
    constexpr precise_unit
        meter(detail::unit_data(1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    constexpr precise_unit m = meter;
    constexpr precise_unit
        kilogram(detail::unit_data(0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    constexpr precise_unit kg = kilogram;
    constexpr precise_unit
        second(detail::unit_data(0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    constexpr precise_unit s = second;

    constexpr precise_unit
        Ampere(detail::unit_data(0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    constexpr precise_unit A = Ampere;

    constexpr precise_unit
        Kelvin(detail::unit_data(0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    constexpr precise_unit K = Kelvin;
    constexpr precise_unit
        mol(detail::unit_data(0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0));
    constexpr precise_unit
        candela(detail::unit_data(0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0));
    constexpr precise_unit cd = candela;
    constexpr precise_unit
        currency(detail::unit_data(0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0));
    constexpr precise_unit
        count(detail::unit_data(0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0));
    constexpr precise_unit
        pu(detail::unit_data(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0));
    constexpr precise_unit
        iflag(detail::unit_data(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0));
    constexpr precise_unit
        eflag(detail::unit_data(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0));
    constexpr precise_unit
        radian(detail::unit_data(0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0));
    constexpr precise_unit rad = radian;
    // define some specialized units
    constexpr precise_unit
        defunit(detail::unit_data(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0, 0));
    constexpr precise_unit
        invalid(detail::unit_data(nullptr), constants::invalid_conversion);
    constexpr precise_unit error(detail::unit_data(nullptr));

    /// Define some unitless numbers
    constexpr precise_unit one;
    constexpr precise_unit hundred = precise_unit(100.0, one);
    constexpr precise_unit ten = precise_unit(10.0, one);
    constexpr precise_unit percent(0.01, one);
    constexpr precise_unit ratio = one;

    constexpr precise_unit infinite(
        detail::unit_data(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
        constants::infinity);
    constexpr precise_unit
        nan(detail::unit_data(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0),
            constants::invalid_conversion);
    // SI prefixes as units
    constexpr precise_unit milli(1e-3, one);
    constexpr precise_unit micro(1e-6, one);
    constexpr precise_unit nano(1e-9, one);
    constexpr precise_unit pico(1e-12, one);
    constexpr precise_unit femto(1e-15, one);
    constexpr precise_unit atto(1e-18, one);
    constexpr precise_unit zepto(1e-21, one);
    constexpr precise_unit yocto(1e-24, one);

    constexpr precise_unit hecto(1e2, one);
    constexpr precise_unit kilo(1e3, one);
    constexpr precise_unit mega(1e6, one);
    constexpr precise_unit giga(1e9, one);
    constexpr precise_unit tera(1e12, one);
    constexpr precise_unit peta(1e15, one);
    constexpr precise_unit exa(1e18, one);
    constexpr precise_unit zetta(1e21, one);
    constexpr precise_unit yotta(1e24, one);

    constexpr precise_unit kibi(1024, one);
    constexpr precise_unit mebi = kibi * kibi;
    constexpr precise_unit gibi = mebi * kibi;
    constexpr precise_unit tebi = gibi * kibi;
    constexpr precise_unit pebi = tebi * kibi;
    constexpr precise_unit exbi = pebi * kibi;
    constexpr precise_unit zebi = exbi * kibi;
    constexpr precise_unit yobi = zebi * kibi;

    // Derived SI units:
    constexpr precise_unit
        hertz(detail::unit_data(0, 0, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));

    constexpr precise_unit
        volt(detail::unit_data(2, 1, -3, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));

    constexpr precise_unit
        newton(detail::unit_data(1, 1, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    constexpr precise_unit
        Pa(detail::unit_data(-1, 1, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    constexpr precise_unit
        joule(detail::unit_data(2, 1, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    constexpr precise_unit
        watt(detail::unit_data(2, 1, -3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    constexpr precise_unit
        coulomb(detail::unit_data(0, 0, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    constexpr precise_unit
        farad(detail::unit_data(-2, -1, 4, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    constexpr precise_unit
        ohm(detail::unit_data(2, 1, -3, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    constexpr precise_unit
        siemens(detail::unit_data(-2, -1, 3, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    constexpr precise_unit
        weber(detail::unit_data(2, 1, -2, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    constexpr precise_unit
        tesla(detail::unit_data(0, 1, -2, -1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    constexpr precise_unit
        henry(detail::unit_data(2, 1, -2, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    constexpr precise_unit
        lumen(detail::unit_data(0, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 0));
    constexpr precise_unit
        lux(detail::unit_data(-2, 0, 0, 0, 0, 0, 1, 0, 0, 2, 0, 0, 0, 0));
    constexpr precise_unit
        becquerel(detail::unit_data(0, 0, -1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0));
    constexpr precise_unit
        gray(detail::unit_data(2, 0, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    constexpr precise_unit
        sievert(detail::unit_data(2, 0, -2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0));
    constexpr precise_unit
        katal(detail::unit_data(0, 0, -1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0));

    constexpr precise_unit Hz = hertz;
    constexpr precise_unit V = volt;
    constexpr precise_unit N = newton;
#ifndef pascal
    // in some windows networking applications  pascal is a #define that will
    // cause all sorts of issues
    constexpr precise_unit pascal = Pa;
#endif
    constexpr precise_unit J = joule;
    constexpr precise_unit W = watt;
    constexpr precise_unit C = coulomb;
    constexpr precise_unit F = farad;
    constexpr precise_unit S = siemens;
    constexpr precise_unit Wb = weber;
    constexpr precise_unit T = tesla;
    constexpr precise_unit H = henry;
    constexpr precise_unit lm = lumen;
    constexpr precise_unit lx = lux;
    constexpr precise_unit Bq = becquerel;
    constexpr precise_unit Gy = gray;
    constexpr precise_unit Sv = sievert;
    constexpr precise_unit kat = katal;
    constexpr precise_unit sr = rad * rad;

    // Extra SI units
    constexpr precise_unit bar(100000.0, Pa);

    // Distance units
    constexpr precise_unit cm(0.01, m);
    constexpr precise_unit km(1000.0, m);
    constexpr precise_unit mm(0.001, m);
    constexpr precise_unit nm(1e-9, m);

    // Volume units
    constexpr precise_unit L{0.001, m* m* m};
    constexpr precise_unit mL{0.001, L};
    // mass units
    constexpr precise_unit g(0.001, kg);
    constexpr precise_unit mg(0.001, g);

    /// Units from the cgs system
    namespace cgs {
        constexpr double c_const = 29979245800.0;  // speed of light in cm/s
        constexpr precise_unit erg(1e-7, J);
        constexpr precise_unit dyn(1e-5, N);
        constexpr precise_unit barye(0.1, Pa);
        constexpr precise_unit gal = cm / s.pow(2);
        constexpr precise_unit poise{0.1, Pa* s};
        constexpr precise_unit stokes{1e-4, m.pow(2) / s};
        constexpr precise_unit kayser = one / cm;
        constexpr precise_unit oersted{1000.0 / 4.0 / constants::pi, A / m};
        constexpr precise_unit gauss{1e-4, T};
        constexpr precise_unit debye{1.0 / (c_const * 1e20), C* m};
        constexpr precise_unit maxwell{1e-8, Wb};
        constexpr precise_unit biot{10.0, A};
        constexpr precise_unit gilbert = oersted * cm;
        constexpr precise_unit stilb = cd / cm.pow(2);
        constexpr precise_unit lambert{1.0 / constants::pi, cd / cm.pow(2)};
        constexpr precise_unit phot{10000, lx};
        constexpr precise_unit curie(3.7e10, Bq);
        constexpr precise_unit roentgen{2.58e-4, C / kg};
        constexpr precise_unit REM{0.01, precise::Sv};
        constexpr precise_unit RAD{100, erg / g};
        constexpr precise_unit emu{0.001, A* m* m};
        constexpr precise_unit langley{41840.0, J / m.pow(2)};
        constexpr precise_unit unitpole{1.256637061436e-7, Wb};
        constexpr precise_unit statC_charge{10.0 / c_const, C};
        constexpr precise_unit statC_flux{10.0 /
                                              (4.0 * constants::pi * c_const),
                                          V* m};
        constexpr precise_unit abOhm{1e-9, ohm};
        constexpr precise_unit abFarad{1e9, F};
        constexpr precise_unit abHenry{1e-9, H};
        constexpr precise_unit abVolt{1e-8, V};
        constexpr precise_unit statV{c_const, abVolt};
        constexpr precise_unit statT{c_const * 1e-4, T};
        constexpr precise_unit statHenry{c_const * c_const, abHenry};
        constexpr precise_unit statOhm{c_const * c_const, abOhm};
        constexpr precise_unit statFarad{1.0 / (c_const * c_const), abFarad};

    }  // namespace cgs

    /// meter-gram-force system of units
    namespace gm {
        constexpr precise_unit pond{980.665, cgs::dyn};
        constexpr precise_unit hyl{9.80665, kg};
        constexpr precise_unit at{98066.5, Pa};
        constexpr precise_unit poncelet{980.665, W};
        constexpr precise_unit PS{735.49875, W};  // metric horsepower

    }  // namespace gm

    /// Meter tonne second system of units
    namespace MTS {
        constexpr precise_unit sthene{1000.0, N};
        constexpr precise_unit pieze{1000.0, Pa};
        constexpr precise_unit thermie{4.186, mega* J};

    }  // namespace MTS

    /// Units of time
    namespace time {
        // Time unit
        constexpr precise_unit min(60.0, s);
        constexpr precise_unit ms(0.001, s);
        constexpr precise_unit ns(1e-9, s);
        constexpr precise_unit hr(60.0, min);
        constexpr precise_unit h(60.0, min);
        constexpr precise_unit day(24.0, hr);
        constexpr precise_unit week(7.0, day);
        constexpr precise_unit yr(8760.0, hr);  // median calendar year;
        constexpr precise_unit fortnight(14, day);

        constexpr precise_unit sday{365.24 / 366.24, day};  // sidereal day
        constexpr precise_unit syr(365.256363004, day);  // sidereal year
        constexpr precise_unit at{365.24219, day* eflag};  // mean tropical year
        constexpr precise_unit aj{365.25, day};  // julian year
        constexpr precise_unit ag{365.2425, day};  // gregorian year
        constexpr precise_unit year = aj;  // standard year
        constexpr precise_unit mos{29.53059, day};  // synodal month
        constexpr precise_unit moj{1.0 / 12.0, aj};  // mean julian month
        constexpr precise_unit mog{1.0 / 12.0, ag};  // mean gregorian month
    }  // namespace time

    constexpr precise_unit min = time::min;
    constexpr precise_unit ms = time::ms;
    constexpr precise_unit ns = time::ns;
    constexpr precise_unit hr = time::hr;
    constexpr precise_unit h = time::h;
    constexpr precise_unit yr = time::yr;
    constexpr precise_unit day = time::day;

    // International units
    namespace i {
        constexpr precise_unit grain{64.79891, mg};

        constexpr precise_unit point(127.0 / 360.0, mm);
        constexpr precise_unit pica(127.0 / 30.0, mm);
        constexpr precise_unit inch(0.0254, m);
        constexpr precise_unit foot(0.3048, m);

        constexpr precise_unit yard(0.9144, m);
        constexpr precise_unit mile(1609.344, m);
        constexpr precise_unit league(3.0, mile);
        constexpr precise_unit hand(4.0, inch);

        constexpr precise_unit cord{128.0, foot.pow(3)};
        constexpr precise_unit board_foot{144, inch.pow(3)};
        constexpr precise_unit mil = milli * inch;
        constexpr precise_unit circ_mil = {constants::pi / 4.0, mil.pow(2)};

    }  // namespace i

    constexpr precise_unit in = i::inch;

    constexpr precise_unit ft = i::foot;
    constexpr precise_unit yd = i::yard;
    constexpr precise_unit mile = i::mile;

    // https://en.wikipedia.org/wiki/United_States_customary_units

    // Avoirdupois units, common international standard
    namespace av {
        constexpr precise_unit dram{1.7718451953125, g};
        constexpr precise_unit ounce{16.0, dram};
        constexpr precise_unit pound{453.59237, g};
        constexpr precise_unit hundredweight{100.0, pound};
        constexpr precise_unit longhundredweight{112, pound};
        constexpr precise_unit ton{2000.0, pound};
        constexpr precise_unit longton{2240.0, pound};
        constexpr precise_unit stone{14.0, pound};
        constexpr precise_unit lbf = precise_unit(4.4482216152605, precise::N);
        constexpr precise_unit ozf{1.0 / 16.0, lbf};
        constexpr precise_unit slug = lbf * s.pow(2) / ft;
        constexpr precise_unit poundal{0.138254954376, N};
    }  // namespace av

    constexpr precise_unit lb = av::pound;
    constexpr precise_unit ton = av::ton;
    constexpr precise_unit oz = av::ounce;
    constexpr precise_unit lbf = av::lbf;
    // troy units
    namespace troy {
        constexpr precise_unit pennyweight{24.0, i::grain};
        constexpr precise_unit oz{31.1034768, g};
        constexpr precise_unit pound{12.0, oz};
    }  // namespace troy

    // US customary units (survey)
    namespace us {
        constexpr precise_unit foot{1200.0 / 3937.0, m};
        constexpr precise_unit inch{1.0 / 12.0, foot};
        constexpr precise_unit mil{0.001, inch};
        constexpr precise_unit yard(3.0, foot);
        constexpr precise_unit rod(16.5, foot);
        constexpr precise_unit chain(4.0, rod);
        constexpr precise_unit link(0.01, chain);
        constexpr precise_unit furlong(10.0, chain);
        constexpr precise_unit mile(8.0, furlong);
        constexpr precise_unit league(3.0, mile);
        namespace engineers {
            constexpr precise_unit chain(100.0, foot);
            constexpr precise_unit link(0.01, chain);
        }  // namespace engineers
        // area
        constexpr precise_unit acre(43560.0, foot.pow(2));
        constexpr precise_unit homestead(160.0, acre);
        constexpr precise_unit section(640.0, acre);
        constexpr precise_unit township(36.0, section);
        // volume
        constexpr precise_unit minim{61.611519921875, micro* L};
        constexpr precise_unit dram{60.0, minim};
        constexpr precise_unit floz{29.5735295625e-6, m* m* m};
        constexpr precise_unit tbsp{0.5, floz};
        constexpr precise_unit tsp{1.0 / 6.0, floz};
        constexpr precise_unit pinch{0.125, tsp};
        constexpr precise_unit dash{0.5, pinch};
        constexpr precise_unit shot{3.0, tbsp};
        constexpr precise_unit gill{4.0, floz};
        constexpr precise_unit cup{8.0, floz};
        constexpr precise_unit pint{2.0, cup};
        constexpr precise_unit quart{2.0, pint};
        constexpr precise_unit gallon{3.785411784, L};
        constexpr precise_unit flbarrel{31.5, gallon};
        constexpr precise_unit barrel{42.0, gallon};
        constexpr precise_unit hogshead{63.0, gallon};
        constexpr precise_unit cord{128.0, i::foot.pow(3)};
        constexpr precise_unit fifth{0.2, gallon};

        /// Us customary dry measurements
        namespace dry {
            constexpr precise_unit pint{0.5506104713575, L};
            constexpr precise_unit quart{2.0, pint};
            constexpr precise_unit gallon{4.0, quart};
            constexpr precise_unit peck{2.0, gallon};
            constexpr precise_unit bushel{35.23907016688, L};
            constexpr precise_unit barrel{7056, i::inch.pow(3)};
            constexpr precise_unit sack{3.0, bushel};
            constexpr precise_unit strike{2.0, bushel};
        }  // namespace dry
        namespace grain {
            constexpr precise_unit bushel_corn{56.0, av::pound};
            constexpr precise_unit bushel_wheat{60.0, av::pound};
            constexpr precise_unit bushel_barley{48.0, av::pound};
            constexpr precise_unit bushel_oats{32.0, av::pound};
        }  // namespace grain

    }  // namespace us

    // Volume units
    constexpr precise_unit gal = us::gallon;

    /// FDA specific volume units in metric
    namespace metric {
        constexpr precise_unit tbsp{15.0, mL};
        constexpr precise_unit tsp{5.0, mL};
        constexpr precise_unit floz{30.0, mL};
        constexpr precise_unit cup{250.0, mL};
        constexpr precise_unit cup_uslegal{240.0, mL};
        constexpr precise_unit carat{0.2, g};
    }  // namespace metric

    /// Some Canada specific variants on the us units
    namespace canada {
        constexpr precise_unit tbsp{15.0, mL};
        constexpr precise_unit tsp{5.0, mL};
        constexpr precise_unit cup{250.0, mL};
        constexpr precise_unit cup_trad{227.3045, mL};
        constexpr precise_unit gallon{4.54609e-3, m.pow(3)};
        namespace grain {
            constexpr precise_unit bushel_oats{34.0, av::pound};
        }  // namespace grain
    }  // namespace canada

    /// Some Australia specific variants on the us units
    namespace australia {
        constexpr precise_unit tbsp{20.0, mL};
        constexpr precise_unit tsp{5.0, mL};
        constexpr precise_unit cup{250.0, mL};
    }  // namespace australia

    /// Imperial system units (British)
    namespace imp {
        constexpr precise_unit inch{2.539998, cm* eflag};
        constexpr precise_unit foot{12.0, inch};

        constexpr precise_unit thou{0.0254, mm};
        constexpr precise_unit barleycorn{1.0 / 3.0, inch};
        constexpr precise_unit rod{16.5, foot};
        constexpr precise_unit chain(4.0, rod);
        constexpr precise_unit link{0.01, chain};
        constexpr precise_unit pace{2.5, foot};
        constexpr precise_unit yard{3.0, foot};
        constexpr precise_unit furlong(201.168, m);
        constexpr precise_unit league(4828.032, m);
        constexpr precise_unit mile{5280.0, foot};
        constexpr precise_unit nautical_mile{6080, foot};
        constexpr precise_unit knot = nautical_mile / h;
        constexpr precise_unit acre{4840.0, yard.pow(2)};

        // area
        constexpr precise_unit perch{25.29285264, m* m};
        constexpr precise_unit rood{1011.7141056, m* m};
        // volume
        constexpr precise_unit gallon{4546.09, mL};
        constexpr precise_unit quart{0.25, gallon};
        constexpr precise_unit pint{0.5, quart};
        constexpr precise_unit gill{0.25, pint};
        constexpr precise_unit cup{0.5, pint};
        constexpr precise_unit floz{0.1, cup};
        constexpr precise_unit tbsp{0.5, floz};
        constexpr precise_unit tsp{1.0 / 3.0, tbsp};

        constexpr precise_unit barrel{36.0, gallon};
        constexpr precise_unit peck{2.0, gallon};
        constexpr precise_unit bushel{4.0, peck};
        constexpr precise_unit dram{1.0 / 8.0, floz};
        constexpr precise_unit minim{1.0 / 60.0, dram};
        // weight
        constexpr precise_unit drachm{1.7718451953125, g};
        constexpr precise_unit stone{6350.29318, g};
        constexpr precise_unit hundredweight{112.0, av::pound};
        constexpr precise_unit ton{2240.0, av::pound};
        constexpr precise_unit slug{14.59390294, kg};
    }  // namespace imp

    namespace apothecaries {
        constexpr precise_unit floz = imp::floz;
        constexpr precise_unit minim{59.1938802083333333333, milli* mL};
        constexpr precise_unit scruple{20.0, i::grain};
        constexpr precise_unit drachm{3.0, scruple};
        constexpr precise_unit ounce{8.0, drachm};
        constexpr precise_unit pound{12.0, ounce};
        constexpr precise_unit pint = imp::pint;
        constexpr precise_unit gallon = imp::gallon;
        constexpr precise_unit metric_ounce{28.0, g};
    }  // namespace apothecaries

    // Nautical units
    namespace nautical {
        constexpr precise_unit fathom(2.0, i::yard);
        constexpr precise_unit cable(120, fathom);
        constexpr precise_unit mile(1.852, km);
        constexpr precise_unit knot = mile / h;
        constexpr precise_unit league(3.0, mile);
    }  // namespace nautical

    /// Some historical Japanese units
    namespace japan {
        constexpr precise_unit shaku{10.0 / 33.0, m};
        constexpr precise_unit sun{0.1, shaku};
        constexpr precise_unit ken{1.0 + 9.0 / 11.0, m};
        constexpr precise_unit tsubo{100.0 / 30.25, m* m};
        constexpr precise_unit sho{2401.0 / 1331.0, L};
        constexpr precise_unit kan{15.0 / 4.0, kg};
        constexpr precise_unit go{2401.0 / 13310, L};
        constexpr precise_unit cup{200.0, mL};
    }  // namespace japan

    /// Some historical chinese units
    namespace chinese {
        constexpr precise_unit jin{0.5, kg};
        constexpr precise_unit liang{0.1, g};
        constexpr precise_unit qian{0.01, g};

        constexpr precise_unit li{500, m};
        constexpr precise_unit cun{10.0 / 300.0, m};
        constexpr precise_unit chi{10.0, cun};
        constexpr precise_unit zhang{10.0, chi};

    }  // namespace chinese

    /// Typographic units for typesetting or printing
    namespace typographic {
        namespace american {
            constexpr precise_unit line{1.0 / 12.0, i::inch};
            constexpr precise_unit point{1.0 / 6.0, line};
            constexpr precise_unit pica{12.0, point};
            constexpr precise_unit twip{1.0 / 20.0, point};

        }  // namespace american
        namespace printers {
            constexpr precise_unit point{0.013837, i::inch};
            constexpr precise_unit pica{12.0, point};
        }  // namespace printers

        namespace french {
            constexpr precise_unit point{15625.0 / 41559.0, mm};
            constexpr precise_unit ligne{6.0, point};
            constexpr precise_unit pouce{12.0, ligne};
            constexpr precise_unit didot = point;
            constexpr precise_unit cicero{12.0, didot};
            constexpr precise_unit pied{12.0, pouce};
            constexpr precise_unit toise{6.0, pied};

        }  // namespace french

        namespace metric {
            constexpr precise_unit point{375.0, micro* m};
            constexpr precise_unit quart{0.25, mm};

        }  // namespace metric

    }  // namespace typographic

    /// units related to distance
    namespace distance {
        constexpr precise_unit ly(9.4607304725808e15, m);
        constexpr precise_unit au(149597870700.0, m);
        constexpr precise_unit au_old(1.495979e11, m);
        constexpr precise_unit angstrom(1e-10, m);
        constexpr precise_unit parsec{3.085678e16, m};
        constexpr precise_unit smoot{67.0, precise::i::inch};
        constexpr precise_unit cubit{18.0, precise::i::inch};
        constexpr precise_unit longcubit{21.0, precise::i::inch};
        constexpr precise_unit arpent_us{58.47131, m};
        constexpr precise_unit arpent_fr{71.46466, m};
        constexpr precise_unit xu{0.1, precise::pico* precise::m};

    }  // namespace distance

    /// Units related to compass directions
    namespace direction {
        // using iflag as a complex coordinate
        constexpr precise_unit east = precise::one;
        constexpr precise_unit north = precise::iflag;
        constexpr precise_unit south{-1.0, precise::iflag};
        constexpr precise_unit west{-1.0, precise::one};
    }  // namespace direction

    //  Area units
    constexpr precise_unit acre = us::acre;

    /// Additional Area units
    namespace area {
        constexpr precise_unit are{100.0, m* m};
        constexpr precise_unit hectare(100.0, are);
        constexpr precise_unit barn{1e-28, m* m};
        constexpr precise_unit arpent{0.84628, acre};
    }  // namespace area

    /// additional mass units
    namespace mass {
        constexpr precise_unit quintal{100.0, kg};
        constexpr precise_unit ton_assay{29.0 + 1. / 6.0, g};
        constexpr precise_unit longton_assay{32.0 + 2.0 / 3.0, g};
        constexpr precise_unit Da(1.6605388628e-27, kg);
        constexpr precise_unit u = Da;
        constexpr precise_unit tonne(1000.0, kg);
    }  // namespace mass

    constexpr precise_unit t = mass::tonne;
    /// some extra volume units
    namespace volume {
        constexpr precise_unit stere = m.pow(3);
        constexpr precise_unit acre_foot = acre * us::foot;
        constexpr precise_unit drum{55.0, us::gallon};
    }  // namespace volume

    /// angle measure units
    namespace angle {
        constexpr precise_unit deg(constants::pi / 180.0, rad);
        constexpr precise_unit gon{0.9, deg};
        constexpr precise_unit grad(constants::pi / 200.0, rad);
        constexpr precise_unit arcmin(1.0 / 60.0, deg);
        constexpr precise_unit arcsec(1.0 / 60.0, arcmin);
        constexpr precise_unit
            brad(constants::tau / 256.0, rad);  // binary radian
    }  // namespace angle
    constexpr precise_unit deg = angle::deg;

    /// units related to temperature
    namespace temperature {
        constexpr precise_unit celsius{1.0, K* eflag};
        constexpr precise_unit degC = celsius;

        constexpr precise_unit fahrenheit{5.0 / 9.0, celsius};
        constexpr precise_unit degF = fahrenheit;

        constexpr precise_unit rankine{5.0 / 9.0, K};
        constexpr precise_unit degR = rankine;

        constexpr precise_unit reaumur{5.0 / 4.0, celsius};
    }  // namespace temperature

    constexpr precise_unit degC = temperature::celsius;
    constexpr precise_unit degF = temperature::fahrenheit;

    /// Units related to pressure
    namespace pressure {
        constexpr precise_unit psi{6894.757293168, Pa};
        constexpr precise_unit psig = psi * eflag;
        constexpr precise_unit inHg{3376.849669, Pa};  // at 60 degF
        constexpr precise_unit mmHg{133.322387415, Pa};
        constexpr precise_unit torr{101325.0 / 760.0,
                                    Pa* iflag};  // this is really close to mmHg
        constexpr precise_unit inH2O{248.843004, Pa};  // at 60 degF
        constexpr precise_unit mmH2O{1.0 / 25.4, inH2O};  // at 60 degF
        constexpr precise_unit atm(101325.0, Pa);
        constexpr precise_unit att = gm::at;  //!< technical atmosphere same as
                                              //!< gravitational metric system
    }  // namespace pressure

    // Power system units

    namespace electrical {
        constexpr precise_unit MW(1000000.0, W);
        constexpr precise_unit VAR = W * iflag;
        constexpr precise_unit MVAR(1000000.0, VAR);
        constexpr precise_unit kW(1000.0, W);
        constexpr precise_unit kVAR(1000.0, VAR);
        constexpr precise_unit mW(0.001, W);
        constexpr precise_unit puMW = MW * pu;
        constexpr precise_unit puV = pu * V;
        constexpr precise_unit puHz = pu * Hz;
        constexpr precise_unit MJ(1000000.0, J);
        constexpr precise_unit puOhm = ohm * pu;
        constexpr precise_unit puA = A * pu;
        constexpr precise_unit kV(1000.0, V);
        constexpr precise_unit mV(0.001, V);
        constexpr precise_unit mA(0.001, A);
    }  // namespace electrical

    constexpr precise_unit MW = electrical::MW;

    /// Power units
    namespace power {
        constexpr precise_unit hpE(746.0, W);  // electric horsepower
        constexpr precise_unit
            hpI(745.69987158227022, W);  // mechanical horsepower
        constexpr precise_unit hpS(9812.5, W);  // Boiler(steam) horsepower
        constexpr precise_unit hpM(735.49875, W);  // Mechanical horsepower

    }  // namespace power

    constexpr precise_unit hp = power::hpI;

    // Speed units
    constexpr precise_unit mph(mile / h);
    constexpr precise_unit mps(m / s);

    /// Energy units
    namespace energy {
        constexpr precise_unit kWh(3600000.0, J);
        constexpr precise_unit MWh(1000.0, kWh);
        constexpr precise_unit
            eV(1.602176634e-19, J);  // updated to 2019 exact definition

        constexpr precise_unit kcal(4184.0, J);
        constexpr precise_unit cal_4(4.204, J);
        constexpr precise_unit cal_15(4.18580, J);  // calorie at 15 degC
        constexpr precise_unit cal_20(4.18190, J);  // calorie at 20 degC
        constexpr precise_unit cal_mean(4.19002, J);  // mean calorie
        constexpr precise_unit
            cal_it(4.1868, J);  // international table calorie
        constexpr precise_unit cal_th(4.184, J);  // thermochemical calorie

        constexpr precise_unit btu_th{1054.350, J};  // thermochemical btu
        constexpr precise_unit btu_39{1059.67, J};
        constexpr precise_unit btu_59{1054.80, J};
        constexpr precise_unit btu_60{1054.68, J};
        constexpr precise_unit btu_mean{1055.87, J};
        constexpr precise_unit btu_it{1055.05585, J};  // international table
                                                       // btu
        constexpr precise_unit btu_iso{1055.06, J};  // rounded btu_it
        constexpr precise_unit quad(1055.05585262, J);
        constexpr precise_unit tonc(12000.0, btu_th / h);

        constexpr precise_unit therm_us(100000.0, btu_59);
        constexpr precise_unit therm_br(105505585.257, J);
        constexpr precise_unit therm_ec(100000, btu_iso);
        constexpr precise_unit EER(btu_th / W / h);  // Energy efficiency ratio
        constexpr precise_unit SG(lb / ft.pow(3) * pu);  // Specific gravity

        constexpr precise_unit ton_tnt{4.184, precise::giga* precise::J};
        constexpr precise_unit boe{5.8e6, btu_59};
        constexpr precise_unit foeb{6.05e6, btu_59};
        constexpr precise_unit hartree{4.35974465054e-18, J};
        constexpr precise_unit tonhour{3.5168528421, kWh};

        constexpr precise_unit scf_mol{1.1953, mol};
        constexpr precise_unit scf{1100.0, btu_it* eflag, commodities::nat_gas};
        constexpr precise_unit ncf{1163.0, btu_it* eflag, commodities::nat_gas};
        constexpr precise_unit scm{35.3146667, scf};
        constexpr precise_unit scm_mol{35.3146667, scf_mol};
        constexpr precise_unit ncm{1.055, scm};

    }  // namespace energy
    constexpr precise_unit btu = energy::btu_it;
    constexpr precise_unit cal = energy::cal_th;
    constexpr precise_unit kWh = energy::kWh;
    constexpr precise_unit MWh = energy::MWh;

    /// Some support for custom units
    namespace custom {
        constexpr int bShift(std::uint16_t val, std::uint32_t bit)
        {
            return ((static_cast<std::uint32_t>(val >> bit) & 0x1U) > 0U) ? 1 :
                                                                            0;
        }
        constexpr unsigned int bShiftu(std::uint16_t val, std::uint32_t bit)
        {
            return ((static_cast<std::uint32_t>(val >> bit) & 0x1U) > 0U) ? 1U :
                                                                            0U;
        }

        /** Construct a custom unit of a specific number
        @details the only valid operations are custom unit/s unit/m unit/m^2
        unit/m^3 unit/kg custom units cannot be inverted or multiplied by other
        units without undefined behavior something will happen but it may not be
        consistent,  there are 1024 possible custom units
        */
        constexpr detail::unit_data custom_unit(std::uint16_t customX)
        {
            return {7 - 4 * bShift(customX, 8U),  // 3 or 7
                    -2 + 3 * bShift(customX, 7U),  // -2 or 1
                    // 7 or 0  sometimes custom unit/time is used
                    7 * bShift(customX, 9U),
                    // -3 or -4  this is probably the most important for
                    // identifying custom units
                    -3 - bShift(customX, 6U),
                    3 * bShift(customX, 4U),  // 3 or 0
                    -2,  // this also is set so that 1/-2 = -2 for a 2 bit
                         // signed number
                    -2 + 2 * bShift(customX, 5U),
                    -2 * bShift(customX, 3U),
                    0,
                    0,
                    bShiftu(customX, 2U),
                    bShiftu(customX, 1U),
                    bShiftu(customX, 0U),
                    0};
        }
        /// Check if the unit is a custom unit or inverse custom unit
        inline bool is_custom_unit(const detail::unit_data& UT)
        {
            if (UT.mole() != -2) {
                // mole is always -2 regardless of inversion
                return false;
            }
            if (std::labs(UT.ampere()) < 2) {
                // ampere is either -3 or -4  or 3 or 4
                return false;
            }
            return true;
        }

        /// get the index of the custom unit
        inline int custom_unit_number(const detail::unit_data& UT)
        {
            int num = (UT.has_e_flag() ? 1 : 0) + (UT.has_i_flag() ? 2 : 0) +
                (UT.is_per_unit() ? 4 : 0);
            num += (std::labs(UT.meter()) < 4) ? 256 : 0;
            num += (std::labs(UT.second()) >= 6) ? 512 : 0;
            num += (std::labs(UT.kg()) <= 1) ? 128 : 0;
            num += (std::labs(UT.kelvin()) == 3) ? 16 : 0;
            num += (UT.ampere() == -4) ? 64 : 0;
            num += (std::labs(UT.candela()) >= 2) ? 0 : 32;
            num += (std::labs(UT.currency()) >= 2) ? 8 : 0;
            return num;
        }
        /// check if 1/custom unit
        inline bool is_custom_unit_inverted(const detail::unit_data& UT)
        {
            int key = UT.meter();
            if (key < 0) {
                return true;
            }
            if (key > 0) {
                return false;
            }
            key = UT.kg();
            switch (key) {
                case 1:
                case -2:
                case -3:
                    return false;
                case -1:
                case 2:
                case 3:
                    return true;
                default:
                    break;
            }
            key = UT.second();
            switch (key) {
                case 1:
                case -7:
                case -6:
                    return true;
                case -1:
                case 7:
                case 6:
                    return false;
                default:
                    break;
            }
            return (UT.ampere() == 3);
        }
        /** Construct a custom counting unit
        @details a counting unit is a custom unit that can be divided into
        meter/kg/s and inverted there are only 16 (0-15) available custom
        counting units due to the stringent requirement on extra operations
        */
        constexpr detail::unit_data custom_count_unit(std::uint16_t customX)
        {
            return {0,
                    0,
                    0,
                    3,  // detection codes
                    -3,  // detection codes
                    0,
                    -1 * bShift(customX, 3U),
                    0,
                    0,
                    0,
                    bShiftu(customX, 2U),
                    bShiftu(customX, 1U),
                    bShiftu(customX, 0U),
                    0};
        }

        /// Check if the unit is a custom count unit
        inline bool is_custom_count_unit(const detail::unit_data& UT)
        {
            if ((UT.kelvin() == -3) && (UT.ampere() == 3)) {
                return (UT.mole() != -2);
            }
            if ((UT.kelvin() == 3) && (UT.ampere() == -3)) {
                return (UT.mole() != -2);
            }
            return false;
        }
        /// Get the number code for the custom count unit
        inline std::uint16_t
            custom_count_unit_number(const detail::unit_data& UT)
        {
            unsigned int num = (UT.has_e_flag() ? 1U : 0U) +
                (UT.has_i_flag() ? 2U : 0U) + (UT.is_per_unit() ? 4U : 0U);
            num += (UT.candela() == 0) ? 0U : 8U;
            return static_cast<std::uint16_t>(num);
        }
        /// check if 1/custom unit
        inline bool is_custom_count_unit_inverted(const detail::unit_data& UT)
        {
            return ((UT.kelvin() == 3) && (UT.ampere() == -3));
        }

        /** Construct an equation unit
        @details an equation unit triggers the equation flag and an index 0-31
        */
        constexpr detail::unit_data equation_unit(std::uint16_t equation_number)
        {
            return {0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    0,
                    bShift(equation_number, 3U),
                    // 3 and 4 switched on purpose so radian is the high bit
                    bShift(equation_number, 4U),
                    bShiftu(equation_number, 2U),
                    bShiftu(equation_number, 1U),
                    bShiftu(equation_number, 0U),
                    1};
        }
        /// Generate the equation type used the unit
        inline constexpr int eq_type(const detail::unit_data& UT)
        {
            return ((UT.radian() != 0) ? 16 : 0) + ((UT.count() != 0) ? 8 : 0) +
                (UT.is_per_unit() ? 4 : 0) + (UT.has_i_flag() ? 2 : 0) +
                (UT.has_e_flag() ? 1 : 0);
        }
    }  // namespace custom

    /// Generate a custom unit from a code number
    constexpr precise_unit generate_custom_unit(std::uint16_t code)
    {
        return precise_unit(custom::custom_unit(code));
    }
    /// Generate a custom counting unit from a code number
    constexpr precise_unit generate_custom_count_unit(std::uint16_t code)
    {
        return precise_unit(custom::custom_count_unit(code));
    }

    /// logarithm based equation units
    namespace log {
        // nepers
        constexpr precise_unit neper = precise_unit(custom::equation_unit(1));
        // natural logarithm
        constexpr precise_unit logE = precise_unit(custom::equation_unit(9));
        constexpr precise_unit neperA = precise_unit(custom::equation_unit(9));
        // neper of power unit
        constexpr precise_unit neperP = precise_unit(custom::equation_unit(15));
        // base 10 logarithm
        constexpr precise_unit logbase10 =
            precise_unit(custom::equation_unit(0));
        // base 10 logarithm bel auto detect power
        constexpr precise_unit bel = precise_unit(custom::equation_unit(2));
        // base 10 logarithm of Power levels (assume power always)
        constexpr precise_unit belP = precise_unit(custom::equation_unit(10));
        // base 10 logarithm of Power levels (assume power always)
        constexpr precise_unit dBP = precise_unit(custom::equation_unit(11));
        // base 10 logarithm of Power levels (assume power always)
        constexpr precise_unit belA = precise_unit(custom::equation_unit(12));
        // base 10 logarithm of Power levels (assume power always)
        constexpr precise_unit dBA = precise_unit(custom::equation_unit(13));
        // base 2 logarithm
        constexpr precise_unit logbase2 =
            precise_unit(custom::equation_unit(8));
        // 10*base10 logarithm
        constexpr precise_unit dB = precise_unit(custom::equation_unit(3));
        // -base10 logarithm
        constexpr precise_unit neglog10 =
            precise_unit(custom::equation_unit(4));
        // -base100 logarithm
        constexpr precise_unit neglog100 =
            precise_unit(custom::equation_unit(5));
        // -base1000 logarithm
        constexpr precise_unit neglog1000 =
            precise_unit(custom::equation_unit(6));
        // -base50000 logarithm
        constexpr precise_unit neglog50000 =
            precise_unit(custom::equation_unit(7));

        constexpr precise_unit B_SPL{2 * 1e-5, precise::Pa* bel};
        constexpr precise_unit B_V = bel * V;
        constexpr precise_unit B_mV = bel * electrical::mV;
        constexpr precise_unit B_uV = bel * precise::micro * V;
        constexpr precise_unit B_10nV = bel * precise::ten * precise::nano * V;
        constexpr precise_unit B_W = bel * W;
        constexpr precise_unit B_kW = bel * electrical::kW;
        constexpr precise_unit dB_SPL{2 * 1e-5, precise::Pa* dB};
        constexpr precise_unit dB_V = dB * V;
        constexpr precise_unit dB_mV = dB * electrical::mV;
        constexpr precise_unit dB_uV = dB * precise::micro * V;
        constexpr precise_unit dB_10nV = dB * precise::ten * precise::nano * V;
        constexpr precise_unit dB_W = dB * W;
        constexpr precise_unit dB_kW = dB * electrical::kW;
        constexpr precise_unit dBZ =
            dB * ((micro * m).pow(3));  // for radar reflectivity
        constexpr precise_unit BZ =
            bel * ((micro * m).pow(3));  // for radar reflectivity
    }  // namespace log

    /// additional equation based units
    namespace equations {
        /// Detect if the unit is a unit of power
        inline bool is_power_unit(const detail::unit_data& UT)
        {
            return (
                (precise::W.base_units().has_same_base(UT)) ||
                (UT.count() == -2));
        }

        /// convert an equation unit to a single value
        inline double
            convert_equnit_to_value(double val, const detail::unit_data& UT)
        {
            if (!UT.is_equation()) {
                return val;
            }
            int logtype = custom::eq_type(UT);
            switch (logtype) {
                case 0:
                case 10:
                    return std::pow(10.0, val);
                case 1:
                    return std::exp(val / ((is_power_unit(UT)) ? 0.5 : 1.0));
                case 2:
                    return std::pow(
                        10.0, val / ((is_power_unit(UT)) ? 1.0 : 2.0));
                case 3:
                    return std::pow(
                        10.0, val / ((is_power_unit(UT)) ? 10.0 : 20.0));
                case 4:
                    return std::pow(10.0, -val);
                case 5:
                    return std::pow(100.0, -val);
                case 6:
                    return std::pow(1000.0, -val);
                case 7:
                    return std::pow(50000.0, -val);
                case 8:
                    return std::exp2(val);
                case 9:
                    return std::exp(val);
                case 11:
                    return std::pow(10.0, val / 10.0);
                case 12:
                    return std::pow(10.0, val / 2.0);
                case 13:
                    return std::pow(10.0, val / 20.0);
                case 14:
                    return std::pow(3.0, val);
                case 15:
                    return std::exp(val / 0.5);
                case 22:  // saffir simpson hurricane wind scale
                {
                    double out = -0.17613636364;
                    out = std::fma(out, val, 2.88510101010);
                    out = std::fma(out, val, -14.95265151515);
                    out = std::fma(out, val, 47.85191197691);
                    out = std::fma(out, val, 38.90151515152);
                    return out;
                }
                case 23: {  // Beaufort wind scale
                    double out = 0.00177396133;
                    out = std::fma(out, val, -0.05860071301);
                    out = std::fma(out, val, 0.93621452077);
                    out = std::fma(out, val, 0.24246097040);
                    out = std::fma(out, val, -0.12475759535);
                    return out;
                }
                case 24:  // Fujita scale
                    return 14.1 * std::pow(val + 2.0, 1.5);
                case 27:  // prism diopter
                    return std::atan(val / 100.0);
                case 29:  // moment magnitude scale
                    return std::pow(10.0, (val + 10.7) * 1.5);
                case 30:
                    return std::pow(10.0, (val + 3.2) * 1.5);
                default:
                    return val;
            }
        }

        /// convert a value to an equation value
        inline double
            convert_value_to_equnit(double val, const detail::unit_data& UT)
        {
            if (!UT.is_equation()) {
                return val;
            }
            int logtype = custom::eq_type(UT);
            if ((logtype < 16) && (val <= 0.0)) {
                return constants::invalid_conversion;
            }

            switch (logtype) {
                case 0:
                case 10:
                    return std::log10(val);
                case 1:
                    return ((is_power_unit(UT)) ? 0.5 : 1.0) * (std::log)(val);
                case 2:
                    return ((is_power_unit(UT)) ? 1.0 : 2.0) * std::log10(val);
                case 3:
                    return ((is_power_unit(UT)) ? 10.0 : 20.0) *
                        std::log10(val);
                case 4:
                    return -std::log10(val);
                case 5:
                    return -std::log10(val) / 2.0;
                case 6:
                    return -std::log10(val) / 3.0;
                case 7:
                    return -std::log10(val) / std::log10(50000);
                case 8:
                    return (std::log2)(val);
                case 9:
                    return (std::log)(val);
                case 11:
                    return 10.0 * std::log10(val);
                case 12:
                    return 2.0 * std::log10(val);
                case 13:
                    return 20.0 * std::log10(val);
                case 14:
                    return std::log10(val) / std::log10(3);
                case 15:
                    return 0.5 * (std::log)(val);
                case 22:  // saffir simpson hurricane scale from wind speed
                {  // using horners method on polynomial approximation of
                   // saffir-simpson wind speed scale
                    double out = 1.75748569529e-10;
                    out = std::fma(out, val, -9.09204303833e-08);
                    out = std::fma(out, val, 1.52274455780e-05);
                    out = std::fma(out, val, -7.73787973277e-04);
                    out = std::fma(out, val, 2.81978682167e-02);
                    out = std::fma(out, val, -6.67563481438e-01);
                    return out;
                }
                case 23: {  // Beaufort wind scale
                    double out = 2.18882896425e-08;
                    out = std::fma(out, val, -4.78236313769e-06);
                    out = std::fma(out, val, 3.91121840061e-04);
                    out = std::fma(out, val, -1.52427367162e-02);
                    out = std::fma(out, val, 4.24089585061e-01);
                    out = std::fma(out, val, 4.99241689370e-01);
                    return out;
                }
                case 24:  // fujita scale
                    return std::pow(val / 14.1, 2.0 / 3.0) - 2.0;
                case 27:  // prism diopter
                    return 100.0 * std::tan(val);
                case 29:  // moment magnitude scale
                    return 2.0 / 3.0 * std::log10(val) - 10.7;
                case 30:  // energy magnitude scale
                    return 2.0 / 3.0 * std::log10(val) - 3.2;
                default:
                    return val;
            }
        }
    }  // namespace equations

    /// Units used in the textile industry
    namespace textile {
        constexpr precise_unit tex = g / km;
        constexpr precise_unit denier{1.0 / 9.0, tex};
        constexpr precise_unit span{0.2286, m};
        constexpr precise_unit finger{0.1143, m};
        constexpr precise_unit nail{0.5, finger};
    }  // namespace textile

    /// Units used in clinical medicine
    namespace clinical {
        constexpr precise_unit pru = precise::pressure::mmHg * precise::s /
            precise::mL;  //!< peripheral vascular resistance unit
        constexpr precise_unit woodu =
            precise::pressure::mmHg * precise::min / precise::L;  //!< wood unit
        constexpr precise_unit diopter = m.inv();
        constexpr precise_unit prism_diopter(custom::equation_unit(27));
        constexpr precise_unit mesh = i::inch.inv();
        constexpr precise_unit charriere{1.0 / 3.0, mm};
        constexpr precise_unit drop{0.05, mL};
        constexpr precise_unit met =
            precise_unit{3.5, mL / min / kg};  //!< metabolic equivalent
        constexpr precise_unit hounsfield = generate_custom_unit(37);

    }  // namespace clinical

    /// Units used in chemical and biological laboratories
    namespace laboratory {
        constexpr precise_unit svedberg{1e-13, precise::s};
        constexpr precise_unit HPF = precise_unit(custom::custom_count_unit(5));
        constexpr precise_unit LPF{100.0, HPF};
        constexpr precise_unit enzyme_unit =
            precise::micro * precise::mol / precise::min;
        // International Unit
        constexpr precise_unit IU = precise_unit(custom::custom_count_unit(2));
        // Arbitrary Unit
        constexpr precise_unit arbU =
            precise_unit(custom::custom_count_unit(1));
        // index of reactivity
        constexpr precise_unit IR = precise_unit(custom::custom_count_unit(3));
        // limit of flocculation
        constexpr precise_unit Lf = precise_unit(custom::custom_count_unit(4));

        constexpr precise_unit PFU =
            precise_unit(1.0, precise::count, commodities::tissue);
        constexpr precise_unit pH =
            precise::mol / precise::L * precise::log::neglog10;
        constexpr precise_unit molarity = precise::mol / precise::L;
        constexpr precise_unit molality = precise::mol / precise::kg;
    }  // namespace laboratory
    // Weight units

    /// Units related to quantities of data
    namespace data {
        constexpr precise_unit bit = count;
        constexpr precise_unit nibble(4, count);
        constexpr precise_unit byte(8, count);

        // https://physics.nist.gov/cuu/Units/binary.html
        constexpr precise_unit kB(1000.0, byte);
        constexpr precise_unit MB(1000.0, kB);
        constexpr precise_unit GB(1000.0, MB);

        constexpr precise_unit KiB(1024.0, byte);
        constexpr precise_unit MiB(1024.0, KiB);
        constexpr precise_unit GiB(1024.0, MiB);

        constexpr precise_unit bit_s = log::logbase2;
        constexpr precise_unit shannon = bit_s;
        constexpr precise_unit hartley = precise::log::bel * precise::count;
        constexpr precise_unit ban = hartley;
        constexpr precise_unit dit = hartley;
        constexpr precise_unit deciban = precise::log::dB * precise::count;
        constexpr precise_unit nat = precise::log::neper * precise::count;

        constexpr precise_unit trit =
            precise_unit(precise::custom::equation_unit(14));
        // compute the number of digits
        constexpr precise_unit digits = precise_unit(custom::equation_unit(10));
    }  // namespace data

    constexpr precise_unit bit = data::bit;
    constexpr precise_unit B = data::byte;
    constexpr precise_unit kB = data::kB;
    constexpr precise_unit MB = data::MB;
    constexpr precise_unit GB = data::GB;

    /// Units related to computer computations
    namespace computation {
        constexpr precise_unit flop =
            precise_unit(1.0, precise::count, commodities::flop);
        constexpr precise_unit flops{flop / s};
        constexpr precise_unit mips = precise_unit(
            1.0,
            precise::count / precise::s,
            commodities::instruction);
    }  // namespace computation

    /// other special units
    namespace special {
        // Amplitude spectral density
        constexpr precise_unit ASD = precise_unit(
            detail::unit_data(1, 0, 3, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0));
        // Moment magnitude scale for earthquakes
        constexpr precise_unit moment_magnitude =
            precise_unit(custom::equation_unit(29)) * precise::cgs::dyn *
            precise::cm;
        constexpr precise_unit moment_energy =
            precise_unit(custom::equation_unit(30)) * precise::J;
        // Saffir Simpson hurricane wind scale
        constexpr precise_unit sshws =
            precise_unit(custom::equation_unit(22)) * precise::mph;
        // Beaufort wind scale
        constexpr precise_unit beaufort =
            precise_unit(custom::equation_unit(23)) * precise::mph;
        // Fujita tornado scale using original specification and approximation
        // https://en.wikipedia.org/wiki/Fujita_scale
        constexpr precise_unit fujita =
            precise_unit(custom::equation_unit(24)) * precise::mph;
        // mach number
        constexpr precise_unit mach = m / s * pu;
        // square root of Hertz
        constexpr precise_unit rootHertz = precise_unit(
            detail::unit_data(0, 0, -5, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0));
        // square root of meter
        constexpr precise_unit rootMeter = precise_unit(
            detail::unit_data(-5, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 0));

    }  // namespace special

    namespace other {
        // concentrations
        constexpr precise_unit ppm(1e-6, count);
        constexpr precise_unit ppb(1e-9, count);

        constexpr precise_unit candle{0.981, cd};
        // 2019 redefinition
        constexpr precise_unit faraday{96485.3321233100184, C};
        // others
        constexpr precise_unit rpm{constants::pi / 30.0, rad* Hz};
        constexpr precise_unit CFM(ft* ft* ft / min);

        constexpr precise_unit MegaBuck(1000000.0, currency);
        constexpr precise_unit GigaBuck(1000.0, MegaBuck);
    }  // namespace other

    constexpr precise_unit rpm = other::rpm;
}  // namespace precise

constexpr unit meter = unit_cast(precise::meter);
constexpr unit m = meter;
constexpr unit kilogram = unit_cast(precise::kg);
constexpr unit kg = kilogram;
constexpr unit second = unit_cast(precise::second);
constexpr unit s = second;

constexpr unit Ampere = unit_cast(precise::Ampere);
constexpr unit A = Ampere;

constexpr unit Kelvin = unit_cast(precise::Kelvin);
constexpr unit K = Kelvin;
constexpr unit mol = unit_cast(precise::mol);
constexpr unit cd = unit_cast(precise::cd);

constexpr unit currency = unit_cast(precise::currency);
constexpr unit count = unit_cast(precise::count);
constexpr unit pu = unit_cast(precise::pu);
constexpr unit iflag = unit_cast(precise::iflag);
constexpr unit eflag = unit_cast(precise::eflag);
constexpr unit rad = unit_cast(precise::rad);

constexpr unit defunit = unit_cast(precise::defunit);
constexpr unit
    invalid(detail::unit_data(nullptr), constants::invalid_conversion);

/// Check if a precise unit is a default unit
constexpr inline bool is_default(const precise_unit& utest)
{
    return (
        utest.multiplier() == 1.0 &&
        (utest.base_units() == defunit.base_units()));
}
/// Check if a unit is a default unit
constexpr inline bool is_default(const unit& utest)
{
    return (
        utest.multiplier() == 1.0 &&
        (utest.base_units() == defunit.base_units()));
}

/// Define a unitless number
constexpr unit one;
constexpr unit infinite = unit_cast(precise::infinite);
constexpr unit error = unit_cast(precise::error);
constexpr unit ratio = one;
constexpr unit percent = unit_cast(precise::percent);

/// Check if the unit has an error (NaN multiplier or error base units)
constexpr inline bool is_error(const precise_unit& utest)
{
    return (
        utest.multiplier() != utest.multiplier() ||
        utest.base_units() == precise::error.base_units());
}
/// Check if the unit has an error  (NaN multiplier or error base units)
constexpr inline bool is_error(const unit& utest)
{
    return (
        utest.multiplier() != utest.multiplier() ||
        utest.base_units() == error.base_units());
}
/// Check if the unit is a valid unit
constexpr inline bool is_valid(const precise_unit& utest)
{
    return !(
        (utest.multiplier() != utest.multiplier()) &&
        (utest.base_units() == precise::invalid.base_units()));
}
/// Check if the unit is a valid unit
constexpr inline bool is_valid(const unit& utest)
{
    return !(
        (utest.multiplier() != utest.multiplier()) &&
        (utest.base_units() == invalid.base_units()));
}

// SI prefixes as units
constexpr unit milli(1e-3, one);
constexpr unit micro(1e-6, one);
constexpr unit nano(1e-9, one);
constexpr unit pico(1e-12, one);
constexpr unit femto(1e-15, one);
constexpr unit atto(1e-18, one);
constexpr unit kilo(1e3, one);
constexpr unit mega(1e6, one);
constexpr unit giga(1e9, one);
constexpr unit tera(1e12, one);
constexpr unit peta(1e15, one);
constexpr unit exa(1e18, one);

// SI Derived units:
constexpr unit Hz = unit_cast(precise::Hz);

constexpr unit volt = unit_cast(precise::volt);
constexpr unit V = volt;
constexpr unit newton = unit_cast(precise::newton);
constexpr unit Pa = unit_cast(precise::Pa);
constexpr unit joule = unit_cast(precise::joule);
constexpr unit watt = unit_cast(precise::watt);
constexpr unit coulomb = unit_cast(precise::coulomb);
constexpr unit farad = unit_cast(precise::farad);
constexpr unit ohm = unit_cast(precise::ohm);
constexpr unit siemens = unit_cast(precise::siemens);
constexpr unit weber = unit_cast(precise::weber);
constexpr unit tesla = unit_cast(precise::tesla);
constexpr unit henry = unit_cast(precise::henry);
constexpr unit lumen = unit_cast(precise::lumen);
constexpr unit lux = unit_cast(precise::lux);
constexpr unit becquerel = unit_cast(precise::becquerel);
constexpr unit gray = unit_cast(precise::gray);
constexpr unit sievert = unit_cast(precise::sievert);
constexpr unit katal = unit_cast(precise::katal);

constexpr unit N = newton;
// this is something in some Windows libraries that needs to be worked around
#ifndef pascal
constexpr unit pascal = Pa;
#endif
constexpr unit J = joule;
constexpr unit W = watt;
constexpr unit C = coulomb;
constexpr unit F = farad;
constexpr unit S = siemens;
constexpr unit Wb = weber;
constexpr unit T = tesla;
constexpr unit H = henry;
constexpr unit lm = lumen;
constexpr unit lx = lux;
constexpr unit Bq = becquerel;
constexpr unit Gy = gray;
constexpr unit Sv = sievert;
constexpr unit kat = katal;
constexpr unit sr = rad * rad;

// Extra SI units

constexpr unit bar = unit_cast(precise::bar);
// Distance units
constexpr unit inch = unit_cast(precise::in);
constexpr unit in = inch;
constexpr unit foot = unit_cast(precise::ft);
constexpr unit ft = foot;

constexpr unit yard = unit_cast(precise::yd);
constexpr unit yd = yard;
constexpr unit mile = unit_cast(precise::mile);
constexpr unit cm = unit_cast(precise::cm);
constexpr unit km = unit_cast(precise::km);
constexpr unit mm = unit_cast(precise::mm);
constexpr unit nm = unit_cast(precise::nm);

//  Area units
constexpr unit acre = unit_cast(precise::acre);
// Time unit
constexpr unit min = unit_cast(precise::min);
constexpr unit ms = unit_cast(precise::ms);
constexpr unit ns = unit_cast(precise::ns);
constexpr unit hr = unit_cast(precise::hr);
constexpr unit h = unit_cast(precise::h);
constexpr unit yr = unit_cast(precise::yr);
// angle measure
constexpr unit deg = unit_cast(precise::deg);
// temperature
constexpr unit degC = unit_cast(precise::degC);
constexpr unit degF = unit_cast(precise::degF);

/// Check if a unit is a temperature
constexpr bool is_temperature(const precise_unit& utest)
{
    return (utest.has_same_base(K) && utest.base_units().has_e_flag());
}
/// Check if a unit is a temperature
constexpr bool is_temperature(const unit& utest)
{
    return (utest.has_same_base(K) && utest.base_units().has_e_flag());
}

namespace detail {

    /// Convert a temperature value from one unit base to another
    template<typename UX, typename UX2>
    double convertTemperature(double val, const UX& start, const UX2& result)
    {
        if (is_temperature(start)) {
            if (units::degF == unit_cast(start)) {
                val = (val - 32.0) * 5.0 / 9.0;
            } else if (start.multiplier() != 1.0) {
                val = val * start.multiplier();
            }
            val += 273.15;
            // convert to K
        } else {
            val = val * start.multiplier();
        }
        if (is_temperature(result)) {
            val -= 273.15;
            if (units::degF == unit_cast(result)) {
                val *= 9.0 / 5.0;
                val += 32.0;
            } else if (result.multiplier() != 1.0) {
                val = val / result.multiplier();
            }
            return val;
        }
        return val / result.multiplier();
    }

    /// Convert some flagged units from one type to another
    template<typename UX, typename UX2>
    double convertFlaggedUnits(
        double val,
        const UX& start,
        const UX2& result,
        double basis = constants::invalid_conversion)
    {
        if (is_temperature(start) || is_temperature(result)) {
            return convertTemperature(val, start, result);
        }
        if (start.has_same_base(precise::pressure::psi.base_units())) {
            if (start.has_e_flag() == result.has_e_flag()) {
                return val * start.multiplier() / result.multiplier();
            }
            if (start.has_e_flag()) {
                if (std::isnan(basis)) {
                    return (val * start.multiplier() +
                            precise::pressure::atm.multiplier()) /
                        result.multiplier();
                }
                return (val + basis) * start.multiplier() / result.multiplier();
            }
            if (std::isnan(basis)) {
                return (val * start.multiplier() -
                        precise::pressure::atm.multiplier()) /
                    result.multiplier();
            }
            return (val * start.multiplier() / result.multiplier()) - basis;
        }
        return constants::invalid_conversion;
    }

}  // namespace detail
// others
constexpr unit rpm = unit_cast(precise::rpm);

// Power system units
constexpr unit VAR = unit_cast(precise::electrical::VAR);
constexpr unit MW = unit_cast(precise::MW);
constexpr unit MVAR = unit_cast(precise::electrical::MVAR);
constexpr unit kW = unit_cast(precise::electrical::kW);
constexpr unit mW = unit_cast(precise::electrical::mW);
constexpr unit puMW = unit_cast(precise::electrical::puMW);
constexpr unit puV = unit_cast(precise::electrical::puV);
constexpr unit puHz = unit_cast(precise::electrical::puHz);
constexpr unit puOhm = unit_cast(precise::electrical::puOhm);
constexpr unit puA = unit_cast(precise::electrical::puA);
constexpr unit kV = unit_cast(precise::electrical::kV);
constexpr unit mV = unit_cast(precise::electrical::mV);
constexpr unit mA = unit_cast(precise::electrical::mA);
// Power units
constexpr unit hp = unit_cast(precise::hp);
constexpr unit mph = unit_cast(precise::mph);

/// conversion operations for per-unit fields
namespace puconversion {
    /// compute a base value for a particular value based on power system base
    /// values
    inline double generate_base(
        const detail::unit_data& unit,
        double basePower,
        double baseVoltage)
    {
        if (unit.has_same_base(W.base_units())) {
            return basePower;
        }
        if (unit.has_same_base(V.base_units())) {
            return baseVoltage;
        }
        if (unit.has_same_base(A.base_units())) {
            return basePower / baseVoltage;
        }
        if (unit.has_same_base(ohm.base_units())) {
            return baseVoltage * baseVoltage / basePower;
        }
        if (unit.has_same_base(S.base_units())) {
            return basePower / (baseVoltage * baseVoltage);
        }
        return constants::invalid_conversion;
    }
    /// some pu values have conventions for base values this function return
    /// those
    inline double assumedBase(const unit& start, const unit& result)
    {
        if (puHz == result || puHz == start) {  // assume 60 Hz
            return 60.0;
        }
        if (puMW == result || puMW == start) {  // assume 100MVA for power base
            return 100.0;
        }
        // mach number
        if (unit_cast(precise::special::mach) == result ||
            unit_cast(precise::special::mach) ==
                start) {  // assume NASA mach number approximation conversions
            return 341.25;
        }
        return constants::invalid_conversion;
    }

    /// Generate some known conversion between power system per unit values
    inline double knownConversions(
        double val,
        const detail::unit_data& start,
        const detail::unit_data& result)
    {
        if (start.has_same_base(puOhm.base_units())) {
            if (result.has_same_base(puMW.base_units()) ||
                result.has_same_base(puA.base_units())) {  // V^2/R assuming
                                                           // voltage=1.0 pu; or
                                                           // //I=V/R
                return 1.0 / val;
            }
        } else if (start.has_same_base(puA.base_units())) {
            if (result.has_same_base(puMW.base_units())) {  // P=IV assuming
                                                            // voltage=1.0 pu or
                                                            // //R=V/I
                return val;
            }
            if (result.has_same_base(
                    puOhm.base_units())) {  // P=IV assuming
                                            // voltage=1.0 pu or
                                            // //R=V/I
                return 1.0 / val;
            }
        } else if (start.has_same_base(
                       puMW.base_units())) {  // P=IV, or P=V^2/R
            if (result.has_same_base(
                    puA.base_units())) {  // IV assuming voltage=1.0 pu or
                return val;
            }
            if (result.has_same_base(puOhm.base_units())) {
                return 1.0 / val;
            }
        }
        return constants::invalid_conversion;
    }
}  // namespace puconversion

// Energy units
constexpr unit kcal = unit_cast(precise::energy::kcal);
constexpr unit btu = unit_cast(precise::btu);
constexpr unit kWh = unit_cast(precise::kWh);
constexpr unit MWh = unit_cast(precise::MWh);
// Volume units
constexpr unit L = unit_cast(precise::L);
// gallon not galileo
constexpr unit gal = unit_cast(precise::gal);
// Weight units

constexpr unit g = unit_cast(precise::g);
constexpr unit mg = unit_cast(precise::mg);
constexpr unit lb = unit_cast(precise::lb);
constexpr unit ton = unit_cast(precise::ton);
constexpr unit oz = unit_cast(precise::oz);
constexpr unit tonne = unit_cast(precise::mass::tonne);

// data sizes
constexpr unit bit = unit_cast(precise::bit);
constexpr unit B = unit_cast(precise::B);
constexpr unit kB = unit_cast(precise::kB);
constexpr unit MB = unit_cast(precise::MB);
constexpr unit GB = unit_cast(precise::GB);

/** check if a unit is some normal valid unit
@details not an error, not infinite, not one,not invalid, not defunit, the
multiplier is a normal number and >0*/
inline bool isnormal(const precise_unit& utest)
{
    return std::isnormal(utest.multiplier()) && (!is_error(utest)) &&
        utest != precise::one && utest != precise::defunit &&
        utest.multiplier() > 0;
}

/** check if a unit is some normal valid unit
@details not an error, not infinite,not invalid, not one, not defunit, the
multiplier is a normal number and >0*/
inline bool isnormal(const unit& utest)
{
    return std::isnormal(utest.multiplier_f()) && (!is_error(utest)) &&
        utest != one && utest != defunit && utest.multiplier() > 0;
}

namespace detail {
    /** Convert counting units into one another, radians, count, mole  these are
    all counting units but have different assumptions so while they are
    convertible they need to be handled differently
    */
    template<typename UX, typename UX2>
    inline double
        convertCountingUnits(double val, const UX& start, const UX2& result)
    {
        auto base_start = start.base_units();
        auto base_result = result.base_units();

        auto rad_start = base_start.radian();
        auto rad_result = base_result.radian();
        auto count_start = base_start.count();
        auto count_result = base_result.count();
        auto mol_start = base_start.mole();
        auto mol_result = base_result.mole();
        if (mol_start == mol_result && rad_start == rad_result &&
            (count_start == 0 || count_result == 0)) {
            val = val * start.multiplier() / result.multiplier();
            return val;
        }

        if (mol_start == mol_result &&
            ((rad_start == 0 &&
              (count_start == rad_result || count_start == 0)) ||
             (rad_result == 0 &&
              (count_result == rad_start || count_result == 0)))) {
            // define a conversion multiplier for radians<->count(rotations) of
            // various powers
            static constexpr std::array<double, 5> muxrad{
                {1.0 / (4.0 * constants::pi * constants::pi),
                 1.0 / (2.0 * constants::pi),
                 0.0,
                 2.0 * constants::pi,
                 4.0 * constants::pi * constants::pi}};
            int muxIndex =
                rad_result - rad_start + 2;  // +2 is to shift the index
            if (muxIndex < 0 || muxIndex > 4) {
                return constants::invalid_conversion;
            }
            val *= muxrad[muxIndex];
            // either 1 or the other is 0 in this equation other it would have
            // triggered before or not gotten here
            val = val * start.multiplier() / result.multiplier();
            return val;
        }
        if (rad_start == rad_result &&
            ((mol_start == 0 &&
              (count_start == mol_result || count_start == 0)) ||
             (mol_result == 0 &&
              (count_result == mol_start || count_result == 0)))) {
            // define multipliers for mol<->count conversions based on powers
            static constexpr std::array<double, 3> muxmol{
                {6.02214076e23, 0, 1.0 / 6.02214076e23}};

            const int muxIndex =
                mol_result - mol_start + 1;  // +1 is to shift the index
            if (muxIndex < 0 || muxIndex > 2) {
                return constants::invalid_conversion;
            }
            val *= muxmol[muxIndex];
            // either 1 or the other is 0 in this equation other it would have
            // triggered before or not gotten here
            val = val * start.multiplier() / result.multiplier();
            return val;
        }
        return constants::invalid_conversion;
    }
    // radians converted to mole is kind of dumb, theoretically possible but
    // probably shouldn't be supported

    template<typename UX, typename UX2>
    inline double
        extraValidConversions(double val, const UX& start, const UX2& result)
    {
        if (start.has_same_base(m.pow(3)) && result.has_same_base(J)) {
            // volume to scf or scm
            return val * start.multiplier() *
                precise::energy::scm.multiplier() / result.multiplier();
        }
        if (start.has_same_base(J) && result.has_same_base(m.pow(3))) {
            // volume to scf or scm
            return val * start.multiplier() /
                precise::energy::scm.multiplier() / result.multiplier();
        }
        return constants::invalid_conversion;
    }
}  // namespace detail
}  // namespace UNITS_NAMESPACE
