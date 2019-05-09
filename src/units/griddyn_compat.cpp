#include "griddyn_compat.hpp"

namespace gridUnits {
namespace new_impl {

double unitConversion (double val,
                       units_t in,
                       units_t out, double basePower, double localBaseVoltage)
{
  if (in == defUnit || out == defUnit || !in.needs_transform(out)) return val; // don't want to scale things to base units if we won't have something to convert back to

  std::string inout = std::string("in: ") + in.name() + " -- out: " + out.name();

  // TODO: double-check these
  if (in == puV) { in = units_t::volt(); val = val * localBaseVoltage; }
  if (out == puV) { out = units_t::volt(); val = val / localBaseVoltage; }
  if (in == puMW) { in = MW; val = val * basePower; }
  if (out == puMW) { out = MW; val = val / basePower; }

  // things we don't support yet:
  if (is_per_unit(in)) throw std::runtime_error(std::string("input is per-unit: ") + inout);
  if (is_per_unit(out)) throw std::runtime_error(std::string("output is per-unit: ") + inout);
  if (in == units_t::error("(nil)") || out == units_t::error("(nil)"))
  {
    throw std::runtime_error(std::string("in or out is dimensionless: ") + inout);
  }

  if (in != out && (in != griddyn::units::unit::dimensionless() && out != griddyn::units::unit::dimensionless()))
  {
    throw std::runtime_error(std::string("in is a different type from out: ") + inout);
  }

  return out.unapply_transform(in.apply_transform(val));
}

} // namespace new_impl
} // namespace gridUnits
