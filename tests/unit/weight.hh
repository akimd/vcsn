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

  // is_equal.
  ASSERT_EQ(ws.is_equal(z, z), true);
  ASSERT_EQ(ws.is_equal(z, o), false);
  ASSERT_EQ(ws.is_equal(o, z), false);
  ASSERT_EQ(ws.is_equal(o, o), true);

  // add, zero, one.
  ASSERT_EQ(ws.is_equal(ws.add(z, z), z), true);
  ASSERT_EQ(ws.is_equal(ws.add(z, o), o), true);
  ASSERT_EQ(ws.is_equal(ws.add(o, z), o), true);

  // mul, zero, one.
  ASSERT_EQ(ws.is_equal(ws.mul(z, z), z), true);
  ASSERT_EQ(ws.is_equal(ws.mul(z, o), z), true);
  ASSERT_EQ(ws.is_equal(ws.mul(o, z), z), true);
  ASSERT_EQ(ws.is_equal(ws.mul(o, o), o), true);

  // conv, format.
  ASSERT_EQ(ws.is_equal(ws.conv(ws.format(z)), z), true);
  ASSERT_EQ(ws.is_equal(ws.conv(ws.format(o)), o), true);

  return nerrs;
}
