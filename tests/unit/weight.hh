#include "tests/unit/test.hh"

/// Check axiomatic laws of a weightset (semiring).
template <typename WeightSet>
bool check_common(const WeightSet& ws)
{
  size_t nerrs = 0;
  auto z = ws.zero();
  auto o = ws.one();

  // is_zero, is_one.
  ASSERT_EQ(ws.is_zero(z), true);
  ASSERT_EQ(ws.is_zero(o), false);
  ASSERT_EQ(ws.is_one(z), false);
  ASSERT_EQ(ws.is_one(o), true);

  // equal.
  ASSERT_EQ(ws.equal(z, z), true);
  ASSERT_EQ(ws.equal(z, o), false);
  ASSERT_EQ(ws.equal(o, z), false);
  ASSERT_EQ(ws.equal(o, o), true);

  // add, zero, one.
  ASSERT_VS_EQ(ws, ws.add(z, z), z);
  ASSERT_VS_EQ(ws, ws.add(z, o), o);
  ASSERT_VS_EQ(ws, ws.add(o, z), o);

  // mul, zero, one.
  ASSERT_VS_EQ(ws, ws.mul(z, z), z);
  ASSERT_VS_EQ(ws, ws.mul(z, o), z);
  ASSERT_VS_EQ(ws, ws.mul(o, z), z);
  ASSERT_VS_EQ(ws, ws.mul(o, o), o);

  // conv, format.
  ASSERT_VS_EQ(ws, conv(ws, to_string(ws, z)), z);
  ASSERT_VS_EQ(ws, conv(ws, to_string(ws, o)), o);

  return nerrs;
}
