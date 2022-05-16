/// basic commodity definitions

#include <cstdlib>
#include <cstdint>

namespace commodities {
/// generate a harmonized code for use with the units library based on the
/// international harmonized code for international trade

constexpr std::uint32_t generateHarmonizedCode(
    std::int32_t chapter,
    std::int32_t section,
    std::int32_t subsection)
{
    return ((chapter % 100) << 14) + ((section % 100) << 7) +
        ((subsection % 100));
}

constexpr std::uint32_t generateHarmonizedCode(
    std::int32_t chapter,
    std::int32_t section,
    std::int32_t subsection,
    std::int32_t code1,
    bool digit = false)
{
    return ((chapter % 100) << 14) + ((section % 100) << 7) +
        ((subsection % 100)) + ((code1 % 100) << 21) +
        ((digit) ? 0x1000000U : 0U);
}
constexpr std::uint32_t
    generateStringCode(char c1, char c2, char c3, char c4, char c5)
{
    return 0x60000000U + (((c1 - '_') & 0X1FU) << 20U) +
        (((c2 - '_') & 0X1FU) << 15U) + (((c3 - '_') & 0X1FU) << 10U) +
        (((c4 - '_') & 0X1FU) << 5U) + ((c5 - '_') & 0X1FU);
}

constexpr std::uint32_t generateKnownCode(std::uint32_t code)
{
    return 0x9C000000U + (code & 0x03FFFFFF);
}

constexpr std::uint32_t generatePackagingCode(std::uint32_t code)
{
    return 0x9C000000U + ((code & 0xFFU) << 21U);
}

namespace packaging {
    /** Enumeration of potential packaging/form factor modifiers
    aligned with recommendation 21 of international trade to the extent possible
    */
    enum packaging : std::uint32_t {
        // bulk 0-9
        bulk = generatePackagingCode(0),
        powder = generatePackagingCode(1),
        grain = generatePackagingCode(2),
        nodule = generatePackagingCode(3),
        liquid = generatePackagingCode(4),
        gas_liquid = generatePackagingCode(5),
        gas = generatePackagingCode(6),

        // loose,unpacked 10-19
        loose = generatePackagingCode(10),
        tube = generatePackagingCode(11),
        rod = generatePackagingCode(12),
        roll = generatePackagingCode(13),
        coil = generatePackagingCode(14),
        sheet = generatePackagingCode(15),
        bar = generatePackagingCode(16),
        ingot = generatePackagingCode(17),

        // rigid box type
        match_box = generatePackagingCode(21),
        can = generatePackagingCode(22),
        carton = generatePackagingCode(23),
        crate = generatePackagingCode(24),
        chest = generatePackagingCode(25),
        cage = generatePackagingCode(26),
        basket = generatePackagingCode(27),
        bottlecrate = generatePackagingCode(29),

        // rigid drum 30-39
        ampoule = generatePackagingCode(31),
        bottle = generatePackagingCode(32),
        jerrycan = generatePackagingCode(33),
        drum = generatePackagingCode(34),
        vat = generatePackagingCode(35),
        // rigid bulb 40-49
        jug = generatePackagingCode(41),
        bulbous_bottle = generatePackagingCode(42),
        carboy = generatePackagingCode(43),
        barrel = generatePackagingCode(44),
        large_barrel = generatePackagingCode(45),
        // rigid other 50-59
        bucket = generatePackagingCode(51),
        coffin = generatePackagingCode(54),
        // flexible bagtype 60-69
        sachet = generatePackagingCode(61),
        bag_small = generatePackagingCode(62),
        bag_medium = generatePackagingCode(63),
        bag_large = generatePackagingCode(64),
        bale = generatePackagingCode(65),
        net = generatePackagingCode(66),
        filmpack = generatePackagingCode(67),
        // 70-89 reserved
        // other special packages
        bobbin = generatePackagingCode(91),

        // the remaining are not standard containers
        // 100 - 109 vehicles
        vehicle = generatePackagingCode(100),
        car = generatePackagingCode(101),
        locomotive = generatePackagingCode(102),
        caboose = generatePackagingCode(103),
        train = generatePackagingCode(104),
        truck = generatePackagingCode(105),

        // 110 - 120 organizational packaging
        group = generatePackagingCode(110),

        lift = generatePackagingCode(3),
        ration = generatePackagingCode(4),
        stick = generatePackagingCode(5),

        container = generatePackagingCode(11),
        bin = generatePackagingCode(12),
        bulk_bag = generatePackagingCode(13),
        bag = generatePackagingCode(14),
        ball = generatePackagingCode(16),
        bulk_pack = generatePackagingCode(17),
        capsule = generatePackagingCode(18),
        assortment = generatePackagingCode(19),
        vial = generatePackagingCode(20),
        bunk = generatePackagingCode(21),
        billet = generatePackagingCode(22),
        bundle = generatePackagingCode(23),
        board = generatePackagingCode(24),
        segment = generatePackagingCode(25),
        spool = generatePackagingCode(26),
        strip = generatePackagingCode(27),
        skid = generatePackagingCode(28),
        skien = generatePackagingCode(29),
        shipment = generatePackagingCode(30),
        syringe = generatePackagingCode(31),

        small_tin = generatePackagingCode(33),
        large_tin = generatePackagingCode(88),

        treatment = generatePackagingCode(35),
        tablet = generatePackagingCode(36),

        wheel = generatePackagingCode(38),
        wrap = generatePackagingCode(39),
        hanging_container = generatePackagingCode(40),
        cast = generatePackagingCode(42),
        lift_val = generatePackagingCode(43),
        carset = generatePackagingCode(44),

        card = generatePackagingCode(46),
        cone = generatePackagingCode(48),
        cylinder = generatePackagingCode(50),
        combo = generatePackagingCode(51),
        lot = generatePackagingCode(52),
        book = generatePackagingCode(53),
        block = generatePackagingCode(54),
        round = generatePackagingCode(55),
        cassette = generatePackagingCode(56),
        beam = generatePackagingCode(57),
        band = generatePackagingCode(58),
        sleeve = generatePackagingCode(59),
        disk = generatePackagingCode(60),
        deal = generatePackagingCode(61),
        dispenser = generatePackagingCode(62),
        pack = generatePackagingCode(63),
        pail = generatePackagingCode(64),
        reel = generatePackagingCode(65),
        room = generatePackagingCode(66),
        session = generatePackagingCode(67),
        belt = generatePackagingCode(68),
        dose = generatePackagingCode(69),
        strand = generatePackagingCode(70),
        zone = generatePackagingCode(71),
        envelope = generatePackagingCode(72),
        blank = generatePackagingCode(73),
        head = generatePackagingCode(74),
        jar = generatePackagingCode(75),
        joint = generatePackagingCode(76),
        keg = generatePackagingCode(77),
        barge = generatePackagingCode(78),

        pallet = generatePackagingCode(80),
        plate = generatePackagingCode(81),
        panel = generatePackagingCode(82),
        meal = generatePackagingCode(83),
        ring = generatePackagingCode(84),
        sack = generatePackagingCode(85),
        set = generatePackagingCode(86),
        thread = generatePackagingCode(87),
        particle = generatePackagingCode(88),
        sling = generatePackagingCode(90),
        line = generatePackagingCode(90),
    };
}  // namespace packaging

// https://en.wikipedia.org/wiki/List_of_traded_commodities
enum commodity : std::uint32_t {
    water = generateHarmonizedCode(22, 1, 90),
    // metals
    gold = generateHarmonizedCode(71, 8, 0),
    copper = generateHarmonizedCode(74, 0, 0),
    silver = generateHarmonizedCode(71, 6, 0),
    platinum = generateHarmonizedCode(71, 10, 11),
    palladium = generateHarmonizedCode(71, 10, 21),
    zinc = generateHarmonizedCode(79, 0, 0),
    tin = generateHarmonizedCode(80, 0, 0),
    lead = generateHarmonizedCode(78, 0, 0),
    iron = generateHarmonizedCode(72, 0, 0),
    aluminum = generateHarmonizedCode(76, 0, 0),
    alluminum_alloy = generateHarmonizedCode(76, 01, 20),
    nickel = generateHarmonizedCode(75, 0, 0),
    cobalt = generateHarmonizedCode(81, 05, 0),
    molybdenum = generateHarmonizedCode(81, 02, 0),
    carbon = 17,

    // energy
    oil = generateHarmonizedCode(27, 9, 0),
    coal = generateHarmonizedCode(27, 11, 0),
    heat_oil = generateHarmonizedCode(27, 10, 19),
    nat_gas = generateHarmonizedCode(27, 11, 12),
    brent_crude = generateHarmonizedCode(27, 9, 0, 10, true),
    ethanol = generateHarmonizedCode(22, 07, 10),
    propane = generateHarmonizedCode(27, 11, 12),
    gasoline = generateHarmonizedCode(27, 10, 12, 15, true),
    // grains
    wheat = generateHarmonizedCode(10, 01, 99),
    corn = generateHarmonizedCode(10, 05, 90),
    soybeans = generateHarmonizedCode(12, 10, 90),
    soybean_meal = generateHarmonizedCode(12, 8, 10),
    soybean_oil = generateHarmonizedCode(15, 07, 00),
    oats = generateHarmonizedCode(10, 04, 90),
    rice = generateHarmonizedCode(10, 06, 00),
    durum_wheat = generateHarmonizedCode(10, 01, 10),
    canola = generateHarmonizedCode(15, 14, 00),
    rough_rice = generateHarmonizedCode(10, 06, 10),
    rapeseed = generateHarmonizedCode(12, 05, 0),
    adzuki = generateHarmonizedCode(07, 13, 32),
    barley = generateHarmonizedCode(10, 03, 90),
    // meats
    live_cattle = generateHarmonizedCode(01, 02, 29),
    feeder_cattle = generateHarmonizedCode(01, 02, 29, 40, true),
    lean_hogs = generateHarmonizedCode(01, 03, 92),
    milk = generateHarmonizedCode(04, 01, 0),
    paper = generateHarmonizedCode(48, 02, 0),
    plastic = generateHarmonizedCode(39, 0, 0),
    // soft
    cotton = generateHarmonizedCode(52, 1, 0),
    orange_juice = generateHarmonizedCode(20, 9, 11),
    sugar = generateHarmonizedCode(17, 01, 0),
    sugar_11 = generateHarmonizedCode(17, 01, 13),
    coffee = generateHarmonizedCode(9, 1, 0),
    cocoa = generateHarmonizedCode(18, 1, 0),
    palm_oil = generateHarmonizedCode(15, 11, 0),
    rubber = generateHarmonizedCode(40, 0, 0),
    wool = generateHarmonizedCode(51, 01, 0),
    lumber = generateHarmonizedCode(44, 00, 0),

    // other common unit blocks
    people = generateKnownCode(15),
    passenger = 115126,
    vehicle = generateHarmonizedCode(87, 3, 0),
    failure = generateStringCode('f', 'a', 'i', 'l', '_'),
    freight = 56226,

    // clinical
    tissue = generateKnownCode(52632250),
    cell = generateStringCode('c', 'e', 'l', 'l', '_'),
    embryo = generateKnownCode(52632253),
    Hahnemann = generateKnownCode(2352622),
    Korsakov = generateKnownCode(26262656),
    protein = generateKnownCode(325255),
    creatinine = generateKnownCode(2566225),

    // computer
    pixel = generateStringCode('p', 'i', 'x', 'e', 'l'),
    voxel = generateStringCode('v', 'o', 'x', 'e', 'l'),
    errors = generateStringCode('e', 'r', 'r', 'o', 'r'),
    flop = generateStringCode('f', 'l', 'o', 'p', '_'),
    instruction = generateKnownCode(8086),

    // emmissions

};

}  // namespace commodities