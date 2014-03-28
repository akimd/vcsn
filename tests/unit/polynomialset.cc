#undef NDEBUG
#include <cassert>
#include <vcsn/ctx/law_char_z.hh>
#include <vcsn/weights/zmin.hh>
#include <vcsn/weights/polynomialset.hh>
#include <tests/unit/test.hh>
#include <tests/unit/weight.hh>

template <class T>
static
bool
check_conv_fail(T& ps, const std::string& str)
{
  try
    {
      auto p = conv(ps, str);
      std::cerr << "ERROR: " << str << " -> " << format(ps, p) << std::endl;
    }
  catch (const std::exception&)
    {
      return true;
    }
  return false;
}

// Called with law_char_z.
template <typename PolynomialSet>
static
bool
check_assoc(const PolynomialSet& ps)
{
  size_t nerrs = 0;
  using ps_t = PolynomialSet;
  typename ps_t::value_t u = ps.one();
  ps.set_weight(u, "ab", 12);
  ASSERT_EQ(format(ps, u), "\\e + <12>ab");

  u = ps.add(u, u);
  ASSERT_EQ(format(ps, u), "<2>\\e + <24>ab");

  assert(!ps.is_zero(u));
  assert(ps.is_zero(ps.zero()));

  typename ps_t::value_t v;
  ps.set_weight(v, "ab", 24);
  ps.set_weight(v, "", 2);
  assert(u == v);

  u = ps.mul(u, u);
  ASSERT_EQ(format(ps, u), "<4>\\e + <96>ab + <576>abab");

  ps.add_weight(v, "ab", 96 - 24);
  ps.set_weight(v, "", 4);
  ps.set_weight(v, "abab", 576);
  assert(u == v);

  return nerrs;
}


// Called with law_char_z.
template <typename PolynomialSet>
static
bool
check_conv(const PolynomialSet& ps)
{
  size_t nerrs = 0;

#define CHECK(In, Out)                          \
  do {                                          \
    if (getenv("VERBOSE"))                      \
      std::cerr << "check_conv: In: " << In;    \
    ASSERT_EQ(format(ps, conv(ps, In)), Out);     \
  } while (false)
#define CHECK_FAIL(In)                          \
  ASSERT_EQ(check_conv_fail(ps, In), true);

  CHECK("\\e", "\\e");
  CHECK("\\z", "\\z");
  CHECK_FAIL("");
  CHECK("a", "a");
  CHECK("a+b", "a + b");
  CHECK("a+a+a", "<3>a");
  CHECK("a+b+a", "<2>a + b");
  // Check military-order.
  CHECK("aa+b+aa", "b + <2>aa");

  CHECK_FAIL("a++a");
  CHECK_FAIL("+a");
  CHECK_FAIL("a+");
  CHECK("<2>a+<3>b+<5>c+<10>a+<10>c+<10>b+<10>d",
        "<12>a + <13>b + <15>c + <10>d");
  CHECK_FAIL("<2>a++<3>a");
  CHECK_FAIL("+<1>a");
  CHECK_FAIL("<2>a+");
  CHECK("<2>+a", "<2>\\e + a");
  CHECK("  <2>  ", "<2>\\e");
  CHECK("a+<1>a+<-2>a", "\\z");
  // Check long numbers before smaller ones to exercise some issues
  // when reusing an ostringstream: we might keep previous characters.
  CHECK("<1000>a + <1>a + <0>a",
        "<1001>a");
#undef CHECK_FAIL
#undef CHECK
  return nerrs;
}


template <class PolynomialSet>
static
bool
check_star_fail(const PolynomialSet& ps, const std::string& str)
{
  try
    {
      ps.star(conv(ps, str));
    }
  catch (const std::runtime_error&)
    {
      return true;
    }
  return false;
}


// Called with law_char_zmin.
template <typename PolynomialSet>
static
bool
check_star(const PolynomialSet& ps)
{
  size_t nerrs = 0;

#define CHECK(In, Out)                                  \
  do {                                                  \
    if (getenv("VERBOSE"))                              \
      std::cerr << "check_star: In: " << In;            \
    ASSERT_EQ(format(ps, ps.star(conv(ps, In))), Out);    \
  } while (false)

  CHECK("<123>\\e",         "<0>\\e");
  CHECK("<oo>\\e",          "<0>\\e");
  CHECK("<123>\\e+<oo>\\e", "<0>\\e");
  CHECK("\\z",              "<0>\\e");
#undef CHECK
  ASSERT_EQ(check_star_fail(ps, "<-1>\\e"), true);
  ASSERT_EQ(check_star_fail(ps, "<123>a"), true);
  ASSERT_EQ(check_star_fail(ps, "<123>\\e+<12>a"), true);
  ASSERT_EQ(format(ps, ps.star(conv(ps, "<12>\\e+<oo>a"))), "<0>\\e");
  ASSERT_EQ(check_star_fail(ps, "<123>\\e+<oo>a+<3>a"), true);
  return nerrs;
}

int main()
{
  size_t errs = 0;

  {
    using context_t = vcsn::ctx::law_char_z;
    context_t ctx {{'a', 'b', 'c', 'd'}};
    using ps_t = vcsn::polynomialset<context_t>;
    ps_t ps{ctx};

    errs += check_common(ps);
    errs += check_assoc(ps);
    errs += check_conv(ps);
  }
  {
    using context_t = vcsn::context<vcsn::ctx::law_char, vcsn::zmin>;
    context_t ctx {{'a', 'b'}};
    using ps_t = vcsn::polynomialset<context_t>;
    ps_t ps{ctx};

    errs += check_common(ps);
    errs += check_star(ps);
  }
  return !!errs;
}
