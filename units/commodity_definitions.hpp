/*
Copyright (c) 2019-2022,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#pragma once

#include <cstdlib>
#include <cstdint>

namespace UNITS_NAMESPACE {

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

        constexpr std::uint32_t generatePackagingCodeUser(std::uint32_t code)
        {
            return 0x9C000000U + (((code & 0x7FU)+0x80U) << 21U);
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
                tank = generatePackagingCode(28),
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
                tankcar = generatePackagingCode(106),

                // 110 - 120 organizational packaging
                group = generatePackagingCode(110),
                piece = generatePackagingCode(111),
                assembly = generatePackagingCode(112),


                lift = generatePackagingCodeUser(3),
                ration = generatePackagingCodeUser(4),
                stick = generatePackagingCodeUser(5),

                container = generatePackagingCodeUser(11),
                bin = generatePackagingCodeUser(12),
                bulk_bag = generatePackagingCodeUser(13),
                bag = generatePackagingCodeUser(14),
                ball = generatePackagingCodeUser(16),
                bulk_pack = generatePackagingCodeUser(17),
                capsule = generatePackagingCodeUser(18),
                assortment = generatePackagingCodeUser(19),
                vial = generatePackagingCodeUser(20),
                bunk = generatePackagingCodeUser(21),
                billet = generatePackagingCodeUser(22),
                bundle = generatePackagingCodeUser(23),
                board = generatePackagingCodeUser(24),
                segment = generatePackagingCodeUser(25),
                spool = generatePackagingCodeUser(26),
                strip = generatePackagingCodeUser(27),
                skid = generatePackagingCodeUser(28),
                skein = generatePackagingCodeUser(29),
                shipment = generatePackagingCodeUser(30),
                syringe = generatePackagingCodeUser(31),

                small_tin = generatePackagingCodeUser(33),
                large_tin = generatePackagingCodeUser(88),

                treatment = generatePackagingCodeUser(35),
                tablet = generatePackagingCodeUser(36),

                wheel = generatePackagingCodeUser(38),
                wrap = generatePackagingCodeUser(39),
                hanging_container = generatePackagingCodeUser(40),
                cast = generatePackagingCodeUser(42),
                lift_val = generatePackagingCodeUser(43),
                carset = generatePackagingCodeUser(44),

                card = generatePackagingCodeUser(46),
                cone = generatePackagingCodeUser(48),
                cylinder = generatePackagingCodeUser(50),
                combo = generatePackagingCodeUser(51),
                lot = generatePackagingCodeUser(52),
                book = generatePackagingCodeUser(53),
                block = generatePackagingCodeUser(54),
                round = generatePackagingCodeUser(55),
                cassette = generatePackagingCodeUser(56),
                beam = generatePackagingCodeUser(57),
                band = generatePackagingCodeUser(58),
                sleeve = generatePackagingCodeUser(59),
                disk = generatePackagingCodeUser(60),
                deal = generatePackagingCodeUser(61),
                dispenser = generatePackagingCodeUser(62),
                pack = generatePackagingCodeUser(63),
                pail = generatePackagingCodeUser(64),
                reel = generatePackagingCodeUser(65),
                room = generatePackagingCodeUser(66),
                session = generatePackagingCodeUser(67),
                belt = generatePackagingCodeUser(68),
                dose = generatePackagingCodeUser(69),
                strand = generatePackagingCodeUser(70),
                zone = generatePackagingCodeUser(71),
                envelope = generatePackagingCodeUser(72),
                blank = generatePackagingCodeUser(73),
                head = generatePackagingCodeUser(74),
                jar = generatePackagingCodeUser(75),
                joint = generatePackagingCode(76),
                keg = generatePackagingCodeUser(77),
                barge = generatePackagingCodeUser(78),

                pallet = generatePackagingCodeUser(80),
                plate = generatePackagingCodeUser(81),
                panel = generatePackagingCodeUser(82),
                meal = generatePackagingCodeUser(83),
                ring = generatePackagingCodeUser(84),
                sack = generatePackagingCodeUser(85),
                set = generatePackagingCodeUser(86),
                thread = generatePackagingCodeUser(87),
                particle = generatePackagingCodeUser(89),
                
                sling = generatePackagingCodeUser(90),
                line = generatePackagingCodeUser(91),
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
            labor = 546811,

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

            // food
            capsaicin = 623452,
        };

    }  // namespace commodities

}