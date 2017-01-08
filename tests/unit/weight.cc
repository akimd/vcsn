#include <iostream>
#include <limits>
#include <vcsn/weightset/b.hh>
#include <vcsn/weightset/f2.hh>
#include <vcsn/weightset/log.hh>
#include <vcsn/weightset/r.hh>
#include <vcsn/weightset/q.hh>
#include <vcsn/weightset/nmin.hh>
#include <vcsn/weightset/rmin.hh>
#include <vcsn/weightset/zmin.hh>

#include "tests/unit/weight.hh"

// FIXME: Check that invalid conv throws.
// FIXME: Use Google Tests, or Boost Tests.

// Common behavior for b and f2.
template <typename WeightSet>
bool check_bool(const WeightSet& ws, bool one_plus_one)
{
  size_t nerrs = 0;

  nerrs += check_common(ws);

  // format.
  ASSERT_EQ(to_string(ws, ws.zero()), "0");
  ASSERT_EQ(to_string(ws, ws.one()), "1");

  // conv.
  ASSERT_EQ(conv(ws, "0"), 0);
  ASSERT_EQ(conv(ws, "1"), 1);

  // add: "or" or "xor".
  ASSERT_VS_EQ(ws, ws.add(1, 1), one_plus_one);

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
  ASSERT_EQ(to_string(ws, conv(ws, In)), Out)

  CHECK("-1/1",  "-1");
  CHECK("-3/2",  "-3/2");
  CHECK("0/1",   "0");
  CHECK("0",     "0");
  CHECK("42/1",  "42");
  CHECK("-42/2", "-21");
#undef CHECK

  // add.
#define CHECK(Lhs, Rhs, Out)                      \
  ASSERT_EQ(to_string(ws, ws.add(Lhs, Rhs)), Out)

  CHECK(ws.zero(), conv(ws, "8/2"), "4");
  CHECK(ws.one(), conv(ws, "8/2"), "5");
  CHECK(conv(ws, "1/3"), conv(ws, "1/6"), "1/2");
  CHECK(conv(ws, "3/2"), conv(ws, "2/3"), "13/6");
  CHECK(conv(ws, "8/2"), conv(ws, "2/2"), "5");
  CHECK(conv(ws, "168/9"), conv(ws, "14/13"), "770/39");
#undef CHECK

  // mul.
#define CHECK(Lhs, Rhs, Out)                      \
  ASSERT_EQ(to_string(ws, ws.mul(Lhs, Rhs)), Out)

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
  ASSERT_EQ(to_string(ws, ws.star(conv(ws, In))), Out)

  CHECK("1/2", "2");
  CHECK("-1/2", "2/3");
#undef CHECK

  // equal.
#define CHECK(Lhs, Rhs, Out)                            \
  ASSERT_EQ(ws.equal(conv(ws, Lhs), conv(ws, Rhs)), Out)
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
  ASSERT_EQ(to_string(ws, ws.mul(conv(ws, Lhs), conv(ws, Rhs))), Out)

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
  ASSERT_EQ(to_string(ws, ws.zero()), "0");
  ASSERT_EQ(to_string(ws, ws.one()), "1");

  // conv.
  ASSERT_EQ(to_string(ws, conv(ws, "-1")), "-1");
  ASSERT_EQ(to_string(ws, conv(ws, "-3.2")), "-3.2");
  ASSERT_EQ(to_string(ws, conv(ws, "0.1")), "0.1");

  // add.
  ASSERT_VS_EQ(ws, ws.add(3.2, 2.3), 5.5);
  ASSERT_VS_EQ(ws, ws.add(ws.zero(), 8.2), 8.2);
  ASSERT_VS_EQ(ws, ws.add(ws.one(), 8.2), 9.2);

  // mul.
  ASSERT_VS_EQ(ws, ws.mul(ws.zero(), 8.2), 0);
  ASSERT_VS_EQ(ws, ws.mul(ws.one(), 8.3), 8.3);
  ASSERT_VS_EQ(ws, ws.mul(.5, 4), 2);

  // equal
#define CHECK(Lhs, Rhs, Out)                            \
  ASSERT_EQ(ws.equal(Lhs, Rhs), Out)
  CHECK(.5, .5, true);
  CHECK(0.1, 1.0, false);
#undef CHECK

  return nerrs;
}

/// Check Nmin, Zmin or Rmin.
template <typename WeightSet>
static size_t check_tropical_min()
{
  size_t nerrs = 0;
  WeightSet ws;
  constexpr auto is_signed = std::is_signed<typename WeightSet::value_t>{};

  // format.
  ASSERT_EQ(to_string(ws, ws.zero()), "oo");
  ASSERT_EQ(to_string(ws, ws.one()), "0");
  ASSERT_EQ(to_string(ws, 42), "42");

  // conv.
  ASSERT_EQ(to_string(ws, conv(ws, "oo")), "oo");
  ASSERT_EQ(to_string(ws, conv(ws, "0")), "0");
  ASSERT_EQ(to_string(ws, conv(ws, "42")), "42");

  // add: min.
  ASSERT_VS_EQ(ws, ws.add(23, 42), 23);
  ASSERT_VS_EQ(ws, ws.add(42, 23), 23);
  ASSERT_VS_EQ(ws, ws.add(ws.zero(), 12), 12);
  if (is_signed)
    ASSERT_VS_EQ(ws, ws.add(-12, ws.zero()), -12);

  // mul: add.
  ASSERT_VS_EQ(ws, ws.mul(23, 42), 23+42);
  ASSERT_VS_EQ(ws, ws.mul(42, 23), 42+23);
  ASSERT_VS_EQ(ws, ws.mul(ws.zero(), 12), ws.zero());
  if (is_signed)
    ASSERT_VS_EQ(ws, ws.mul(-12, ws.zero()), ws.zero());
  ASSERT_VS_EQ(ws, ws.mul(ws.one(), 12), 12);
  if (is_signed)
    ASSERT_VS_EQ(ws, ws.mul(-12, ws.one()), -12);

  // div: sub.
  if (is_signed)
    ASSERT_VS_EQ(ws, ws.rdivide(23, 42), 23-42);
  ASSERT_VS_EQ(ws, ws.rdivide(42, 23), 42-23);
  ASSERT_VS_EQ(ws, ws.rdivide(ws.zero(), 12), ws.zero());
  if (is_signed)
    ASSERT_VS_EQ(ws, ws.rdivide(ws.one(), 12), -12);
  if (is_signed)
    ASSERT_VS_EQ(ws, ws.rdivide(-12, ws.one()), -12);

  // This is not associative: ws.mul(A, ws.rdivide(B, C)) gives a
  // different result in the last case: 2+(3-4)=4294967295.  That
  // matters in the implementation of eppstein's k-shortest paths.
#define CHECK(A, B, C)                                          \
  ASSERT_VS_EQ(ws, ws.rdivide(ws.mul(A, B), C), (A + B) - C)

  CHECK(2, 0, 0);
  CHECK(2, 4, 0);
  CHECK(2, 0, 4);
  CHECK(2, 3, 4);
#undef CHECK

  // equal
#define CHECK(Lhs, Rhs, Out)                            \
  ASSERT_EQ(ws.equal(Lhs, Rhs), Out)
  CHECK(1, 1, true);
  CHECK(0, 1, false);
#undef CHECK

  return nerrs;
}



static size_t check_log()
{
  size_t nerrs = 0;
  vcsn::log ws;

  // format.
  ASSERT_EQ(to_string(ws, ws.zero()), "oo");
  ASSERT_EQ(to_string(ws, ws.one()), "0");
  ASSERT_EQ(to_string(ws, 42), "42");

  // conv.
  ASSERT_EQ(to_string(ws, conv(ws, "oo")), "oo");
  ASSERT_EQ(to_string(ws, conv(ws, "0")), "0");
  ASSERT_EQ(to_string(ws, conv(ws, "42")), "42");

  // add: +log
  ASSERT_VS_EQ(ws, ws.add(23, 42), -log(exp(-23) + exp(-42)));
  ASSERT_VS_EQ(ws, ws.add(42, 23), -log(exp(-23) + exp(-42)));
  ASSERT_VS_EQ(ws, ws.add(ws.zero(), 12), 12);
  ASSERT_VS_EQ(ws, ws.add(-12, ws.zero()), -12);

  // sub.
  ASSERT_VS_EQ(ws, ws.sub(ws.add(23, 42), 42), 23);
  // FIXME: NAN. ASSERT_VS_EQ(ws, ws.sub(ws.zero(), ws.sub(ws.zero(), 12)), 12);
  ASSERT_VS_EQ(ws, ws.sub(12, ws.zero()), 12);

  // mul: add.
  ASSERT_VS_EQ(ws, ws.mul(23, 42), 23+42);
  ASSERT_VS_EQ(ws, ws.mul(42, 23), 42+23);
  ASSERT_VS_EQ(ws, ws.mul(ws.zero(), 12), ws.zero());
  ASSERT_VS_EQ(ws, ws.mul(-12, ws.zero()), ws.zero());
  ASSERT_VS_EQ(ws, ws.mul(ws.one(), 12), 12);
  ASSERT_VS_EQ(ws, ws.mul(-12, ws.one()), -12);

  // div: sub.
  ASSERT_VS_EQ(ws, ws.rdivide(23, 42), 23-42);
  ASSERT_VS_EQ(ws, ws.rdivide(42, 23), 42-23);
  ASSERT_VS_EQ(ws, ws.rdivide(ws.zero(), 12), ws.zero());
  ASSERT_VS_EQ(ws, ws.rdivide(ws.one(), 12), -12);
  ASSERT_VS_EQ(ws, ws.rdivide(-12, ws.one()), -12);


  // equal
#define CHECK(Lhs, Rhs, Out)                            \
  ASSERT_EQ(ws.equal(Lhs, Rhs), Out)
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
  nerrs += check_log();
  nerrs += check_tropical_min<vcsn::nmin>();
  nerrs += check_tropical_min<vcsn::zmin>();
  nerrs += check_tropical_min<vcsn::rmin>();
  nerrs += check_r();
  nerrs += check_q();
  return !!nerrs;
}
