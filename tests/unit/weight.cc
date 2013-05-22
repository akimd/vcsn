#include <tests/unit/test.hh>
#include <iostream>
#include <vcsn/weights/b.hh>
#include <vcsn/weights/f2.hh>
#include <vcsn/weights/zmin.hh>

// FIXME: Check that invalid conv throws.
// FIXME: Use Google Tests, or Boost Tests.

// Common behavior for b and f2.
template <typename WeightSet>
bool check_bool(const WeightSet& ws)
{
  bool res = true;

  // format.
  ASSERT_EQ(ws.format(ws.zero()), "0");
  ASSERT_EQ(ws.format(ws.unit()), "1");

  // conv.
  ASSERT_EQ(ws.format(ws.conv("0")), "0");
  ASSERT_EQ(ws.format(ws.conv("1")), "1");

  // add: xor.
  ASSERT_EQ(ws.add(0, 0), 0);
  ASSERT_EQ(ws.add(0, 1), 1);
  ASSERT_EQ(ws.add(1, 0), 1);
  ASSERT_EQ(ws.add(1, 1), 0);

  // mul: and.
  ASSERT_EQ(ws.add(0, 0), 0);
  ASSERT_EQ(ws.add(0, 1), 0);
  ASSERT_EQ(ws.add(1, 0), 0);
  ASSERT_EQ(ws.add(1, 1), 1);

  return res;
}

static bool check_b()
{
  bool res = true;
  vcsn::f2 ws;

  res &= check_bool(ws);

  // add: or.
  ASSERT_EQ(ws.add(0, 0), 0);
  ASSERT_EQ(ws.add(0, 1), 1);
  ASSERT_EQ(ws.add(1, 0), 1);
  ASSERT_EQ(ws.add(1, 1), 1);

  return res;
}

static bool check_f2()
{
  bool res = true;
  vcsn::f2 ws;

  res &= check_bool(ws);

  // add: xor.
  ASSERT_EQ(ws.add(0, 0), 0);
  ASSERT_EQ(ws.add(0, 1), 1);
  ASSERT_EQ(ws.add(1, 0), 1);
  ASSERT_EQ(ws.add(1, 1), 0);

  return res;
}

static bool check_zmin()
{
  bool res = true;
  vcsn::zmin ws;

  // format.
  ASSERT_EQ(ws.format(ws.zero()), "oo");
  ASSERT_EQ(ws.format(ws.unit()), "0");
  ASSERT_EQ(ws.format(42), "42");

  // conv.
  ASSERT_EQ(ws.format(ws.conv("oo")), "oo");
  ASSERT_EQ(ws.format(ws.conv("0")), "0");
  ASSERT_EQ(ws.format(ws.conv("42")), "42");

  // add: min.
  ASSERT_EQ(ws.add(23, 42), 23);
  ASSERT_EQ(ws.add(42, 23), 23);
  ASSERT_EQ(ws.add(ws.zero(), 12), 12);
  ASSERT_EQ(ws.add(-12, ws.zero()), -12);

  // mul: add.
  ASSERT_EQ(ws.mul(23, 42), 23+42);
  ASSERT_EQ(ws.mul(42, 23), 42+23);
  ASSERT_EQ(ws.mul(ws.zero(), 12), ws.zero());
  ASSERT_EQ(ws.mul(-12, ws.zero()), ws.zero());
  ASSERT_EQ(ws.mul(ws.unit(), 12), 12);
  ASSERT_EQ(ws.mul(-12, ws.unit()), -12);

  return res;
}

int main()
{
  size_t nerrs = 0;
  nerrs += !check_zmin();
  return !!nerrs;
}
