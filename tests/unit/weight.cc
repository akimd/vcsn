#include <tests/unit/test.hh>
#include <iostream>
#include <vcsn/weights/b.hh>
#include <vcsn/weights/f2.hh>
#include <vcsn/weights/r.hh>
#include <vcsn/weights/q.hh>
#include <vcsn/weights/zmin.hh>

// FIXME: Check that invalid conv throws.
// FIXME: Use Google Tests, or Boost Tests.

// Common behavior for b and f2.
template <typename WeightSet>
bool check_bool(const WeightSet& ws, bool one_plus_one)
{
  size_t nerrs = 0;

  // format.
  ASSERT_EQ(ws.format(ws.zero()), "0");
  ASSERT_EQ(ws.format(ws.one()), "1");

  // conv.
  ASSERT_EQ(ws.format(ws.conv("0")), "0");
  ASSERT_EQ(ws.format(ws.conv("1")), "1");

  // add: "or" or "xor".
  ASSERT_EQ(ws.add(0, 0), 0);
  ASSERT_EQ(ws.add(0, 1), 1);
  ASSERT_EQ(ws.add(1, 0), 1);
  ASSERT_EQ(ws.add(1, 1), one_plus_one);

  // mul: and.
  ASSERT_EQ(ws.mul(0, 0), 0);
  ASSERT_EQ(ws.mul(0, 1), 0);
  ASSERT_EQ(ws.mul(1, 0), 0);
  ASSERT_EQ(ws.mul(1, 1), 1);

  return nerrs;
}

static size_t check_b()
{
  vcsn::b ws;
  // add: or.
  return check_bool(ws, 1);
}

static size_t check_f2()
{
  vcsn::f2 ws;
  // add: xor.
  return check_bool(ws, 0);
}

static size_t check_q()
{
  size_t nerrs = 0;
  vcsn::q ws;

  // format.
  ASSERT_EQ(ws.format(ws.zero()), "0");
  ASSERT_EQ(ws.format(ws.one()), "1");

  // conv.
#define CHECK(In, Out)                          \
  ASSERT_EQ(ws.format(ws.conv(In)), Out)

  CHECK("-1/1", "-1");
  CHECK("-3/2", "-3/2");
  CHECK("0/1", "0");
  CHECK("42/1", "42");
  CHECK("-42/2", "-21");
#undef CHECK

  // add.
#define CHECK(Lhs, Rhs, Out)                      \
  ASSERT_EQ(ws.format(ws.add(Lhs, Rhs)), Out)

  CHECK(ws.zero(), ws.conv("8/2"), "4");
  CHECK(ws.one(), ws.conv("8/2"), "5");
  CHECK(ws.conv("1/3"), ws.conv("1/6"), "1/2");
  CHECK(ws.conv("3/2"), ws.conv("2/3"), "13/6");
  CHECK(ws.conv("8/2"), ws.conv("2/2"), "5");
  CHECK(ws.conv("168/9"), ws.conv("14/13"), "770/39");
#undef CHECK

  // mul.
#define CHECK(Lhs, Rhs, Out)                      \
  ASSERT_EQ(ws.format(ws.mul(Lhs, Rhs)), Out)

  CHECK(ws.zero(), ws.conv("8/3"), "0");
  CHECK(ws.one(), ws.conv("8/3"), "8/3");
  CHECK(ws.one(), ws.conv("8/4"), "2");

  CHECK(ws.conv("-3/2"), ws.conv("2/3"), "-1");
  CHECK(ws.conv("8/2"), ws.conv("2/2"), "4");
  CHECK(ws.conv("800000/2"), ws.conv("0/2"), "0");
  CHECK(ws.conv("800000/2"), ws.conv("1/2"), "200000");
#undef CHECK

  // star.
#define CHECK(In, Out)                          \
  ASSERT_EQ(ws.format(ws.star(ws.conv(In))), Out)

  CHECK("1/2", "2");
  CHECK("-1/2", "2/3");
#undef CHECK

  return nerrs;
}

static size_t check_r()
{
  size_t nerrs = 0;
  vcsn::r ws;

  // format.
  ASSERT_EQ(ws.format(ws.zero()), "0");
  ASSERT_EQ(ws.format(ws.one()), "1");

  // conv.
  ASSERT_EQ(ws.format(ws.conv("-1")), "-1");
  ASSERT_EQ(ws.format(ws.conv("-3.2")), "-3.2");
  ASSERT_EQ(ws.format(ws.conv("0.1")), "0.1");

  // add.
  ASSERT_EQ(ws.add(3.2, 2.3), 5.5);
  ASSERT_EQ(ws.add(ws.zero(), 8.2), 8.2);
  ASSERT_EQ(ws.add(ws.one(), 8.2), 9.2);

  // mul: add.
  ASSERT_EQ(ws.mul(ws.zero(), 8.2), 0);
  ASSERT_EQ(ws.mul(ws.one(), 8.3), 8.3);

  return nerrs;
}

static size_t check_zmin()
{
  size_t nerrs = 0;
  vcsn::zmin ws;

  // format.
  ASSERT_EQ(ws.format(ws.zero()), "oo");
  ASSERT_EQ(ws.format(ws.one()), "0");
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
  ASSERT_EQ(ws.mul(ws.one(), 12), 12);
  ASSERT_EQ(ws.mul(-12, ws.one()), -12);

  return nerrs;
}

int main()
{
  size_t nerrs = 0;
  nerrs += check_b();
  nerrs += check_f2();
  nerrs += check_zmin();
  nerrs += check_r();
  nerrs += check_q();
  return !!nerrs;
}
