// fuzz_target.cc

#include "units/units.hpp"
#include <string>
#include <exception>

static bool cflag=units::disableCustomCommodities();

extern "C" int LLVMFuzzerTestOneInput(const uint8_t *Data, size_t Size)
{
    std::string test1(reinterpret_cast<const char *>(Data), Size);
    auto unit = units::unit_from_string(test1);
    if (!unit.is_error())
    {
        auto str=units::to_string(unit);
        auto u2=units::unit_from_string(str);
        if (u2.is_error())
        {
            throw(6);
        }
        //if (units::unit_cast(u2)!=units::unit_cast(unit))
       // {
        //    throw(5);
        //}
    }
    //its::clearCustomCommodities();
    return 0;  // Non-zero return values are reserved for future use.
}
