#pragma once

// current interface
namespace gridUnits {
namespace new_impl {

using units_t = griddyn::units::unit;

static units_t defUnit = units_t::dimensionless("defUnit");

static units_t deg = units_t::degree();
static units_t rad = units_t::radian();
static units_t hour = units_t::scale(units_t::second(), 3600);
static units_t kW = units_t::scale(units_t::watt(), 1000);
static units_t Watt = units_t::watt();
static units_t Ohm = units_t::ohm();
static units_t Amp = units_t::ampere();
static units_t C = units_t::celsius();
static units_t km = units_t::scale(units_t::meter(), 1000);
static units_t kV = units_t::scale(units_t::volt(), 1000);
static units_t MW = units_t::scale(units_t::watt(), 1000000);
static units_t sec = units_t::second();
static units_t Hz = units_t::hertz();

static units_t puMW = units_t::per_unit(MW);
static units_t puV = units_t::per_unit(units_t::volt());
static units_t MVAR = MW;
static units_t puHz = units_t::error("puHz");
static units_t puA = units_t::error("puA");
static units_t rps = units_t::error("rps");
static units_t MWps = units_t::error("MWps");
static units_t puOhm = units_t::error("puOhm");
static units_t puMWps = units_t::error("puMWps");

inline std::string to_string (units_t unitType)
{
  return unitType.name();
}

inline units_t getUnits (const std::string &unitString, units_t defValue = units_t::dimensionless("(default argument)"))
{
  try
  {
    return units_t::parse(unitString);
  }
  catch (...)
  {
    return defValue;
  }
}

double unitConversion (double val,
                       units_t in,
                       units_t out, double a = 100, double b = 100);

inline double unitConversionTime (double val, units_t in, units_t out)
{
  return out.apply_transform(in.unapply_transform(val));
}

inline double unitConversionPower (double val,
                            const units_t in,
                            const units_t out,
                            double basePower = 100,
                            double localBaseVoltage = 100)
{
  (void) basePower;
  (void) localBaseVoltage;
  return out.apply_transform(in.unapply_transform(val));
}

inline double unitConversionDistance (double val, units_t in, units_t out)
{
  return out.apply_transform(in.unapply_transform(val));
}

inline double unitConversionAngle (double val, units_t in, units_t out)
{
  return out.apply_transform(in.unapply_transform(val));
}

inline double unitConversionFreq (double val, units_t in, units_t out, double baseFreq = 60)
{
  (void) baseFreq;
  return out.apply_transform(in.unapply_transform(val));
}

inline double unitConversionCost (double val, units_t in, units_t out, double basePower = 100)
{
  (void) basePower;
  return out.apply_transform(in.unapply_transform(val));
}

inline double unitConversionTemperature (double val, units_t in, units_t out)
{
  return out.apply_transform(in.unapply_transform(val));
}

} // namespace new_impl
} // namespace gridUnits
