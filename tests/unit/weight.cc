#include <iostream>
#include <limits>
#include <vcsn/weights/b.hh>
#include <vcsn/weights/f2.hh>
#include <vcsn/weights/r.hh>
#include <vcsn/weights/q.hh>
#include <vcsn/weights/zmin.hh>

#include <tests/unit/test.hh>
#include <tests/unit/weight.hh>

// FIXME: Check that invalid conv throws.
// FIXME: Use Google Tests, or Boost Tests.

// Common behavior for b and f2.
template <typename WeightSet>
bool check_bool(const WeightSet& ws, bool one_plus_one)
{
  size_t nerrs = 0;

  nerrs += check_common(ws);

  // format.
  ASSERT_EQ(ws.format(ws.zero()), "0");
  ASSERT_EQ(ws.format(ws.one()), "1");

  // conv.
  ASSERT_EQ(conv(ws, "0"), 0);
  ASSERT_EQ(conv(ws, "1"), 1);

  // add: "or" or "xor".
  ASSERT_EQ(ws.add(1, 1), one_plus_one);

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

  nerrs += check_common(ws);

  // conv.
#define CHECK(In, Out)                          \
  ASSERT_EQ(ws.format(conv(ws, In)), Out)

  CHECK("-1/1",  "-1");
  CHECK("-3/2",  "-3/2");
  CHECK("0/1",   "0");
  CHECK("0",     "0");
  CHECK("42/1",  "42");
  CHECK("-42/2", "-21");
#undef CHECK

  // add.
#define CHECK(Lhs, Rhs, Out)                      \
  ASSERT_EQ(ws.format(ws.add(Lhs, Rhs)), Out)

  CHECK(ws.zero(), conv(ws, "8/2"), "4");
  CHECK(ws.one(), conv(ws, "8/2"), "5");
  CHECK(conv(ws, "1/3"), conv(ws, "1/6"), "1/2");
  CHECK(conv(ws, "3/2"), conv(ws, "2/3"), "13/6");
  CHECK(conv(ws, "8/2"), conv(ws, "2/2"), "5");
  CHECK(conv(ws, "168/9"), conv(ws, "14/13"), "770/39");
#undef CHECK

  // mul.
#define CHECK(Lhs, Rhs, Out)                      \
  ASSERT_EQ(ws.format(ws.mul(Lhs, Rhs)), Out)

  CHECK(ws.zero(), conv(ws, "8/3"), "0");
  CHECK(ws.one(), conv(ws, "8/3"), "8/3");
  CHECK(ws.one(), conv(ws, "8/4"), "2");

  CHECK(conv(ws, "-3/2"), conv(ws, "2/3"), "-1");
  CHECK(conv(ws, "8/2"), conv(ws, "2/2"), "4");
  CHECK(conv(ws, "800000/2"), conv(ws, "0/2"), "0");
  CHECK(conv(ws, "800000/2"), conv(ws, "1/2"), "200000");
#undef CHECK

  // star.
#define CHECK(In, Out)                          \
  ASSERT_EQ(ws.format(ws.star(conv(ws, In))), Out)

  CHECK("1/2", "2");
  CHECK("-1/2", "2/3");
#undef CHECK

  // equals.
#define CHECK(Lhs, Rhs, Out)                            \
  ASSERT_EQ(ws.equals(conv(ws, Lhs), conv(ws, Rhs)), Out)
  CHECK("8/16",  "1/2",  true);
  CHECK("0/16",  "0",    true);
  CHECK("16/8",  "2",    true);
  CHECK("2",     "3",    false);
  CHECK("1/2",   "1/3",  false);
  CHECK("-1/2",  "-1/2", true);
  CHECK("1/-2",  "-1/2", true);
  CHECK("-1/-2", "1/2",  true);
  CHECK("3/6",   "1/2",  true);
  CHECK("-3/6",  "-1/2", true);
  CHECK("3/-6",  "-1/2", true);
  CHECK("-3/-6", "1/2",  true);
#undef CHECK

  // limits.
#define CHECK(Lhs, Rhs, Out)                      \
  ASSERT_EQ(ws.format(ws.mul(conv(ws, Lhs), conv(ws, Rhs))), Out)

  std::string max_signed =
    std::to_string(std::numeric_limits<int>::max());
  std::string max_unsigned =
    std::to_string(std::numeric_limits<unsigned int>::max());

  CHECK("1/" + max_signed,
        max_signed + "/1",
        "1");
  CHECK("-1/" + max_unsigned,
        "-1",
        "1/" + max_unsigned);
  CHECK("1/-" + max_unsigned,
            "1",
        "-1/" + max_unsigned);
  CHECK("1/" + max_unsigned,
        max_signed,
        max_signed + "/" + max_unsigned);
#undef CHECK

#define CHECK(Str, Fails)                       \
  try                                           \
    {                                           \
      fails = false;                            \
      conv(ws, Str);                            \
    }                                           \
  catch (std::exception q)                      \
    {                                           \
      fails = true;                             \
    }                                           \
  ASSERT_EQ(fails, Fails)

  bool fails;
  CHECK("1/0",   true);
  CHECK("1/-0",  true);
  CHECK("abc",   true);
  CHECK("1/abc", true);
  CHECK("-1/2",  false);
  CHECK("1/-2",  false);
  CHECK("-1/-2", false);
#undef CHECK

  return nerrs;
}

static size_t check_r()
{
  size_t nerrs = 0;
  vcsn::r ws;

  nerrs += check_common(ws);

  // format.
  ASSERT_EQ(ws.format(ws.zero()), "0");
  ASSERT_EQ(ws.format(ws.one()), "1");

  // conv.
  ASSERT_EQ(ws.format(conv(ws, "-1")), "-1");
  ASSERT_EQ(ws.format(conv(ws, "-3.2")), "-3.2");
  ASSERT_EQ(ws.format(conv(ws, "0.1")), "0.1");

  // add.
  ASSERT_EQ(ws.add(3.2, 2.3), 5.5);
  ASSERT_EQ(ws.add(ws.zero(), 8.2), 8.2);
  ASSERT_EQ(ws.add(ws.one(), 8.2), 9.2);

  // mul.
  ASSERT_EQ(ws.mul(ws.zero(), 8.2), 0);
  ASSERT_EQ(ws.mul(ws.one(), 8.3), 8.3);
  ASSERT_EQ(ws.mul(.5, 4), 2);

  // equals
#define CHECK(Lhs, Rhs, Out)                            \
  ASSERT_EQ(ws.equals(Lhs, Rhs), Out)
  CHECK(.5, .5, true);
  CHECK(0.1, 1.0, false);
#undef CHECK

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
  ASSERT_EQ(ws.format(conv(ws, "oo")), "oo");
  ASSERT_EQ(ws.format(conv(ws, "0")), "0");
  ASSERT_EQ(ws.format(conv(ws, "42")), "42");

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

  // equals
#define CHECK(Lhs, Rhs, Out)                            \
  ASSERT_EQ(ws.equals(Lhs, Rhs), Out)
  CHECK(1, 1, true);
  CHECK(0, 1, false);
#undef CHECK

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
