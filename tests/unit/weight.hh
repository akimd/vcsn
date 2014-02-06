/// Check axiomatic laws of a weightset (semiring).
template <typename WeightSet>
inline
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

  // equals.
  ASSERT_EQ(ws.equals(z, z), true);
  ASSERT_EQ(ws.equals(z, o), false);
  ASSERT_EQ(ws.equals(o, z), false);
  ASSERT_EQ(ws.equals(o, o), true);

  // add, zero, one.
  ASSERT_EQ(ws.equals(ws.add(z, z), z), true);
  ASSERT_EQ(ws.equals(ws.add(z, o), o), true);
  ASSERT_EQ(ws.equals(ws.add(o, z), o), true);

  // mul, zero, one.
  ASSERT_EQ(ws.equals(ws.mul(z, z), z), true);
  ASSERT_EQ(ws.equals(ws.mul(z, o), z), true);
  ASSERT_EQ(ws.equals(ws.mul(o, z), z), true);
  ASSERT_EQ(ws.equals(ws.mul(o, o), o), true);

  // conv, format.
  ASSERT_EQ(ws.equals(conv(ws, format(ws, z)), z), true);
  ASSERT_EQ(ws.equals(conv(ws, format(ws, o)), o), true);

  return nerrs;
}
