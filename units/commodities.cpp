/*
Copyright (c) 2019-2021,
Lawrence Livermore National Security, LLC;
See the top-level NOTICE for additional details. All rights reserved.
SPDX-License-Identifier: BSD-3-Clause
*/
#include "units.hpp"

#include <algorithm>
#include <array>
#include <atomic>
#include <cctype>
#include <cstring>
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>
#include <tuple>
#include <unordered_map>

/*
// https://en.wikipedia.org/wiki/List_of_traded_commodities
enum commodity : std::uint32_t
{
    water", 1,
    // metals
    gold", 2,
    copper", 4,
    silver", 6,
    platinum", 7,
    palladium", 8,
    zinc", 9,
    tin", 10,
    lead", 11,
    aluminum", 12,
    alluminum_alloy", 13,
    nickel", 14,
    cobolt", 15,
    molybdenum", 16,

    // energy
    oil", 101,
    heat_oil", 102,
    nat_gas", 103,
    brent_crude", 104,
    ethanol", 105,
    propane", 107,
    // grains
    wheat", 404,
    corn", 405,
    soybeans", 406,
    soybean_meal", 407,
    soybean_oil", 408,
    oats", 409,
    rice", 410,
    red_wheat", 411,
    spring_wheat", 412,
    canola", 413,
    rough_rice", 414,
    rapeseed", 415,
    adzuci", 418,
    barley", 420,
    // meats
    live_cattle", 601,
    feeder_cattle", 602,
    lean_hogs", 603,
    milk", 604,

    // soft
    cotton", 945,
    orange_juice", 947,
    sugar", 948,
    sugar_11", 949,
    sugar_14", 950,
    coffee", 952,
    cocoa", 961,
    palm_oil", 971,
    rubber", 999,
    wool", 946,
    lumber", 5007,

    // other common unit blocks
    people", 115125,
    particles", 117463,
    cars", 43567,

    // clinical
    tissue", 4622626,
    cell", 45236884,
    embryo", 52632253,
    Hahnemann", 2352622,
    Korsakov", 262626562,
};

*/

namespace UNITS_NAMESPACE {
namespace commodities {
    using commodityMap = std::unordered_map<std::uint32_t, const char*>;
    static const commodityMap commodity_names{
        {water, "water"},
        // metals
        {gold, "gold"},
        {copper, "copper"},
        {silver, "silver"},
        {platinum, "platinum"},
        {palladium, "palladium"},
        {zinc, "zinc"},
        {tin, "tin"},
        {lead, "lead"},
        {aluminum, "aluminum"},
        {alluminum_alloy, "alluminum_alloy"},
        {nickel, "nickel"},
        {cobolt, "cobolt"},
        {molybdenum, "molybdenum"},

        // energy
        {oil, "oil"},
        {heat_oil, "heat_oil"},
        {nat_gas, "nat_gas"},
        {brent_crude, "brent_crude"},
        {ethanol, "ethanol"},
        {propane, "propane"},
        // grains
        {wheat, "wheat"},
        {corn, "corn"},
        {soybeans, "soybeans"},
        {soybean_meal, "soybean_meal"},
        {soybean_oil, "soybean_oil"},
        {oats, "oats"},
        {rice, "rice"},
        {red_wheat, "red_wheat"},
        {spring_wheat, "spring_wheat"},
        {canola, "canola"},
        {rough_rice, "rough_rice"},
        {rapeseed, "rapeseed"},
        {adzuci, "adzuci"},
        {barley, "barley"},
        // meats
        {live_cattle, "live_cattle"},
        {feeder_cattle, "feeder_cattle"},
        {lean_hogs, "lean_hogs"},
        {milk, "milk"},

        // soft
        {cotton, "cotton"},
        {orange_juice, "orange_juice"},
        {sugar, "sugar"},
        {sugar_11, "sugar_11"},
        {sugar_14, "sugar_14"},
        {coffee, "coffee"},
        {cocoa, "cocoa"},
        {palm_oil, "palm_oil"},
        {rubber, "rubber"},
        {wool, "wool"},
        {lumber, "lumber"},

        // other common unit blocks
        {people, "people"},
        {particles, "particles"},
        {cars, "cars"},

        // clinical
        {tissue, "tissue"},
        {cell, "cell"},
        {embryo, "embryo"},
        {Hahnemann, "Hahnemann"},
        {Korsakov, "Korsakov"},
        {creatinine, "creatinine"},
        {protein, "protein"},

        {pixel, "pixel"},
        {voxel, "voxel"},
        {1073741824, "cxcomm[1073741824]"},  // this is a _____ string commodity
                                             // that might somehow get generated
    };

    using commodityNameMap = std::unordered_map<std::string, std::uint32_t>;
    static const commodityNameMap commodity_codes{
        {"_", 0},  // null commodity code, would cause some screwy things with
                   // the strings
        {"__", 0},  // null commodity code, would cause some screwy things with
                    // the strings
        {"___", 0},  // null commodity code, would cause some screwy things with
                     // the strings
        {"____", 0},  // null commodity code, would cause some screwy things
                      // with the strings
        {"_____", 0},  // null commodity code, would cause some screwy things
                       // with the strings
        {"water", water},
        // metals
        {"gold", gold},
        {"copper", copper},
        {"silver", silver},
        {"platinum", platinum},
        {"palladium", palladium},
        {"zinc", zinc},
        {"tin", tin},
        {"lead", lead},
        {"aluminum", aluminum},
        {"alluminum_alloy", alluminum_alloy},
        {"nickel", nickel},
        {"cobolt", cobolt},
        {"molybdenum", molybdenum},

        // energy
        {"oil", oil},
        {"heat_oil", heat_oil},
        {"nat_gas", nat_gas},
        {"brent_crude", brent_crude},
        {"ethanol", ethanol},
        {"propane", propane},
        // grains
        {"wheat", wheat},
        {"corn", corn},
        {"soybeans", soybeans},
        {"soybean_meal", soybean_meal},
        {"soybean_oil", soybean_oil},
        {"oats", oats},
        {"rice", rice},
        {"red_wheat", red_wheat},
        {"spring_wheat", spring_wheat},
        {"canola", canola},
        {"rough_rice", rough_rice},
        {"rapeseed", rapeseed},
        {"adzuci", adzuci},
        {"barley", barley},
        // meats
        {"live_cattle", live_cattle},
        {"feeder_cattle", feeder_cattle},
        {"lean_hogs", lean_hogs},
        {"milk", milk},

        // soft
        {"cotton", cotton},
        {"orange_juice", orange_juice},
        {"sugar", sugar},
        {"sugar_11", sugar_11},
        {"sugar_14", sugar_14},
        {"coffee", coffee},
        {"cocoa", cocoa},
        {"palm_oil", palm_oil},
        {"rubber", rubber},
        {"wool", wool},
        {"lumber", lumber},

        // other common unit blocks
        {"people", people},
        {"particles", particles},
        {"cars", cars},

        // clinical
        {"tissue", tissue},
        {"cell", cell},
        {"cells", cell},
        {"embryo", embryo},
        {"hahnemann", Hahnemann},
        {"korsakov", Korsakov},
        {"protein", protein},
        {"creatinine", creatinine},
        {"prot", protein},
        {"creat", creatinine},
        // computer
        {"voxel", voxel},
        {"pixel", pixel},
        {"vox", voxel},
        {"pix", pixel},
        {"dot", pixel},
        {"error", errors},
        {"errors", errors},
    };
}  // namespace commodities
static constexpr std::uint32_t Ac{54059}; /* a prime */
static constexpr std::uint32_t Bc{76963}; /* another prime */
// static constexpr std::uint32_t Cc{ 86969 }; /* yet another prime */
static constexpr std::uint32_t firstH{37}; /* also prime */

uint32_t stringHash(const std::string& str)
{
    std::uint32_t hash{firstH};
    for (auto c : str) {
        hash = (hash * Ac) ^ (static_cast<std::uint32_t>(c) * Bc);
    }
    return hash;  // or return h % C;
}

static std::atomic<bool> allowCustomCommodities{true};

void disableCustomCommodities()
{
    allowCustomCommodities.store(false);
}
void enableCustomCommodities()
{
    allowCustomCommodities.store(true);
}
static commodities::commodityNameMap customCommodityCodes;
static std::unordered_map<std::uint32_t, std::string> customCommodityNames;
/// remove some escaped characters from a string mainly the escape character and
/// (){}[]
static void removeEscapeSequences(std::string& str)
{
    auto eloc = str.find_first_of('\\');
    while (eloc < str.size() - 1) {
        auto nc = str[eloc + 1];
        switch (nc) {
            case '\\':
            case '{':
            case '}':
            case '(':
            case ')':
            case '[':
            case ']':
                str.erase(eloc, 1);
                break;
            default:
                break;
        }
        eloc = str.find_first_of('\\', eloc + 1);
    }
}
// get the code to use for a particular commodity
uint32_t getCommodity(std::string comm)
{
    removeEscapeSequences(comm);
    std::transform(comm.begin(), comm.end(), comm.begin(), ::tolower);
    if (allowCustomCommodities.load(std::memory_order_acquire)) {
        if (!customCommodityCodes.empty()) {
            auto fnd2 = customCommodityCodes.find(comm);
            if (fnd2 != customCommodityCodes.end()) {
                return fnd2->second;
            }
        }
    }

    auto fnd = commodities::commodity_codes.find(comm);
    if (fnd != commodities::commodity_codes.end()) {
        return fnd->second;
    }

    if (comm.compare(0, 7, "cxcomm[") == 0) {
        return static_cast<int32_t>(atoi(comm.c_str() + 7));
    }
    if ((comm.size() < 6) && std::all_of(comm.begin(), comm.end(), [](char x) {
            return (x == ' ' || (x >= '_' && x <= '}'));
        })) {
        std::uint32_t hkey{0x40000000};
        unsigned int shift{0};
        for (auto c : comm) {
            if (c == ' ') {
                c = '_';
            }
            c -= '_';
            hkey += static_cast<std::uint32_t>(c) << shift;
            shift += 5;
        }
        return hkey;
    }
    // generate a hash code for a custom string
    auto hcode = stringHash(comm);
    hcode &= 0x1FFFFFFFU;
    hcode |= 0x60000000U;
    addCustomCommodity(comm, hcode);

    return hcode;
}

// get the code to use for a particular commodity
std::string getCommodityName(std::uint32_t commodity)
{
    if (allowCustomCommodities.load(std::memory_order_acquire)) {
        if (!customCommodityNames.empty()) {
            auto fnd2 = customCommodityNames.find(commodity);
            if (fnd2 != customCommodityNames.end()) {
                return fnd2->second;
            }
        }
    }
    auto fnd = commodities::commodity_names.find(commodity);
    if (fnd != commodities::commodity_names.end()) {
        return fnd->second;
    }

    if ((commodity & 0x60000000U) == 0x40000000U) {
        std::string ret;
        ret.push_back((commodity & 0X1FU) + '_');
        ret.push_back(((commodity >> 5U) & 0X1FU) + '_');
        ret.push_back(((commodity >> 10U) & 0X1FU) + '_');
        ret.push_back(((commodity >> 15U) & 0X1FU) + '_');
        ret.push_back(((commodity >> 20U) & 0X1FU) + '_');
        while (!ret.empty() && ret.back() == '_') {
            ret.pop_back();
        }
        return ret;
    }
    return std::string("CXCOMM[") + std::to_string(commodity) + "]";
}

// add a custom commodity for later retrieval
void addCustomCommodity(std::string comm, std::uint32_t code)
{
    if (allowCustomCommodities.load()) {
        std::transform(comm.begin(), comm.end(), comm.begin(), ::tolower);
        customCommodityNames.emplace(code, comm);
        customCommodityCodes.emplace(comm, code);
    }
}

void clearCustomCommodities()
{
    customCommodityNames.clear();
    customCommodityCodes.clear();
}
}  // namespace UNITS_NAMESPACE
