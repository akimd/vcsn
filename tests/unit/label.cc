#include <vcsn/config.hh>

#include <iomanip> // setw
#include <iostream>

#include <vcsn/alphabets/char.hh>
#include <vcsn/alphabets/setalpha.hh>
#include <vcsn/labelset/letterset.hh>
#include <vcsn/labelset/tupleset.hh>
#include <vcsn/labelset/wordset.hh>
#include <vcsn/misc/escape.hh>

// Include this one last, as it defines a macro `V`, which is used as
// a template parameter in boost/unordered/detail/allocate.hpp.
#include "tests/unit/test.hh"

/// The name of \a vs.
template <typename ValueSet>
std::string
set_name(const ValueSet& vs, vcsn::format fmt = vcsn::format::sname)
{
  std::ostringstream o;
  vs.print_set(o, fmt);
  return o.str();
}

static std::string
all_escaped()
{
  auto res = std::string{};
  for (int i = 1; i < 255; ++i)
    res += char(i);
  return vcsn::str_escape(res, "(-)'\\");
}

static unsigned
check_letterset()
{
  unsigned nerrs = 0;
  using labelset_t = vcsn::letterset<vcsn::set_alphabet<vcsn::char_letters>>;
  using genset_t = labelset_t::genset_t;
  genset_t gs{'a', 'b', 'c'};
  labelset_t ls{gs};

  ASSERT_EQ(ls.is_valid(conv(ls, "a")), true);
  ASSERT_EQ(ls.is_one(conv(ls, "a")), false);
  ASSERT_EQ(ls.is_special(conv(ls, "a")), false);

  ASSERT_EQ(ls.is_valid('x'), false);

  ASSERT_EQ(to_string(ls, conv(ls, "a")), "a");
  ASSERT_EQ(to_string(ls, ls.transpose(conv(ls, "a"))), "a");

  {
    // Skip the empty word (\x00), and the special letter (\xff).
    auto n = std::string{"letterset<char_letters(\x01-\xfe)>"};
    auto all = "letterset<char_letters(" + all_escaped() + ")>";
    std::istringstream is(n);
    ASSERT_EQ(all, set_name(labelset_t::make(is)));
  }

  return nerrs;
}



template <typename LabelSet>
static unsigned
check_wordset_make(const std::string& range)
{
  auto nerrs = 0U;

  // Skip the empty word (\x00), and the special letter (\xff).
  std::string n = "wordset<char_letters(" + range + ")>";
  auto is = std::istringstream{n};
  auto ls = LabelSet::make(is);

  // Check the labelset name: '\\' is the only escaped character.
  ASSERT_EQ("wordset<char_letters(" + all_escaped() + ")>",
            set_name(ls));

  // Make sure we reach all the characters.  All the characters are
  // escaped.
  {
    // All the (supported) characters.
    std::string all;
    for (int i = 1; i < 255; ++i)
      all += char(i);

    auto o = std::ostringstream{};
    for (int i = 1; i < 255; ++i)
      o << "\\x" << std::hex << std::setw(2) << std::setfill('0') << i;
    std::cerr << "Test: " << o.str() << '\n';
    ASSERT_EQ(vcsn::str_escape(all, "<>|[]- ',"),
              to_string(ls, conv(ls, o.str())));
  }

  return nerrs;
}

static unsigned
check_wordset()
{
  unsigned nerrs = 0;
  using labelset_t = vcsn::wordset<vcsn::set_alphabet<vcsn::char_letters>>;
  using genset_t = labelset_t::genset_t;
  const auto gs = genset_t{'a', 'b', 'c'};
  const auto ls = labelset_t{gs};

  ASSERT_EQ(ls.is_valid(conv(ls, "a")), true);
  ASSERT_EQ(ls.is_one(conv(ls, "a")), false);
  ASSERT_EQ(ls.is_special(conv(ls, "a")), false);

  ASSERT_EQ(ls.is_valid("x"), false);

  ASSERT_EQ(to_string(ls, conv(ls, "a")), "a");
  ASSERT_EQ(to_string(ls, ls.transpose(conv(ls, "a"))), "a");

  nerrs += check_wordset_make<labelset_t>("\x01-\xfe");
  nerrs += check_wordset_make<labelset_t>("\\x01-\\xfe");

  return nerrs;
}

static unsigned
check_tupleset()
{
  unsigned nerrs = 0;
  using wordset_t = vcsn::wordset<vcsn::set_alphabet<vcsn::char_letters>>;
  using genset_t = wordset_t::genset_t;
  const auto gs1 = genset_t{'a', 'b', 'c'};
  const auto ls1 = wordset_t{gs1};
  const auto gs2 = genset_t{'x', 'y', 'z'};
  const auto ls2 = wordset_t{gs2};

  using wwset_t = vcsn::tupleset<wordset_t, wordset_t>;
  using ww_t = wwset_t::value_t;
  const auto wwset = wwset_t{ls1, ls2};

  using letterset_t = vcsn::letterset<vcsn::set_alphabet<vcsn::char_letters>>;
  using wlset_t = vcsn::tupleset<wordset_t, letterset_t>;
  using wl_t = wlset_t::value_t;
  const auto wlset = wlset_t{ls1, letterset_t{gs2}};

  // sname.
  ASSERT_EQ(wwset_t::sname(),
            "lat<wordset<char_letters>, wordset<char_letters>>");

  // sname.
  ASSERT_EQ(wwset.sname(),
            "lat<wordset<char_letters>, wordset<char_letters>>");

  // print_set.
  ASSERT_EQ(set_name(wwset, vcsn::format::sname),
            "lat<wordset<char_letters(abc)>, wordset<char_letters(xyz)>>");
  ASSERT_EQ(set_name(wwset, vcsn::format::latex),
            "\\{a, b, c\\}^* \\times \\{x, y, z\\}^*");

  // make.
  // If you observe a runtime exception here, such as
  //
  // terminate called after throwing an instance of 'std::runtime_error'
  //  what():  unexpected: (: expected ,
  //
  // then your problem is that your compiler (e.g., G++ 4.8) is buggy.
  //
  // But you should no longer see such errors: tupleset has
  // workarounds.
  {
    std::string n
      = "lat<wordset<char_letters(ABC)>, wordset<char_letters(XYZ)>>";
    auto is = std::istringstream{n};
    ASSERT_EQ(set_name(wwset_t::make(is)), n);
  }
  {
    std::string n
      = "lat<wordset<char_letters(ABC)>, letterset<char_letters(XYZ)>>";
    auto is = std::istringstream{n};
    ASSERT_EQ(set_name(wlset_t::make(is)), n);
  }

  // equal.
  ASSERT_EQ(wwset.equal(ww_t{"ab", "x"}, ww_t{"ab", "x"}), true);
  ASSERT_EQ(wwset.equal(ww_t{"ab", "x"}, ww_t{"abc", "x"}), false);
  ASSERT_EQ(wwset.equal(ww_t{"ab", "x"}, ww_t{"", "x"}), false);
  ASSERT_EQ(wwset.equal(ww_t{"ab", "x"}, ww_t{"ab", "xx"}), false);
  ASSERT_EQ(wwset.equal(ww_t{"ab", "x"}, ww_t{"ab", "y"}), false);
  ASSERT_EQ(wwset.equal(ww_t{"ab", "x"}, ww_t{"ab", ""}), false);

  // less.
#define CHECK(L, R, Res)                                \
  do {                                                  \
    ASSERT_EQ(wwset.less(ww_t L, ww_t R), Res);    \
    ASSERT_EQ(wwset.less(ww_t R, ww_t L), !Res);   \
  } while (false)

  CHECK(("", ""),   ("a", ""),    true);
  CHECK(("", ""),   ("a", ""),    true);
  CHECK(("", ""),   ("",  "x"),   true);
  CHECK(("", ""),   ("a", "x"),   true);
  CHECK(("a", "x"), ("aa", "x"),  true);
  CHECK(("a", "x"), ("a", "xx"),  true);
  CHECK(("a", "x"), ("aa", "xx"), true);
#undef CHECK

  // special, is_special.
  ASSERT_VS_EQ(wwset, wwset.special(), ww_t(ls1.special(), ls2.special()));
  ASSERT_EQ(to_string(wwset, ww_t{ls1.special(),ls2.special()}), "");
  ASSERT_EQ(to_string(wwset, wwset.special()), "");
  ASSERT_EQ(wwset.is_special(wwset.special()), true);
  ASSERT_EQ(!wwset.is_special(ww_t{"abc", "xyz"}), true);

  // is_one.
  ASSERT_EQ(wwset.is_one(ww_t{ls1.one(), ls2.one()}), true);
  ASSERT_EQ(wwset.is_one(ww_t{ls1.one(), ls2.special()}), false);
  ASSERT_EQ(wwset.is_one(ww_t{ls1.one(), "x"}), false);

  // has_one.
  ASSERT_EQ(wwset_t::has_one(), true);
  ASSERT_EQ(wlset_t::has_one(), true);

  // format, transpose.
  ASSERT_EQ(to_string(wwset, ww_t{"abc", "xyz"}), "abc,xyz");
  ASSERT_EQ(to_string(wwset, wwset.transpose(ww_t{"abc","xyz"})), "cba,zyx");

  // conv.
  // Exposed to the same bugs as make, see above.
  ASSERT_VS_EQ(wwset, conv(wwset, "(abc,xyz)"), ww_t("abc", "xyz"));
  ASSERT_VS_EQ(wwset, conv(wwset, "(abc,\\e)"), ww_t("abc", ""));
  ASSERT_VS_EQ(wwset, conv(wwset, "(\\e,x)"),   ww_t("", "x"));
  ASSERT_VS_EQ(wwset, conv(wwset, "(\\e,\\e)"), ww_t("", ""));

  ASSERT_VS_EQ(wlset, conv(wlset, "(abc,x)"),   wl_t("abc", 'x'));

  // mul.
#define CHECK(L1, R1, L2, R2)                                   \
  ASSERT_VS_EQ(wwset,                                           \
               wwset.mul(ww_t(L1, R1), ww_t(L2, R2)),           \
               ww_t(L1 L2, R1 R2))
  CHECK("a",  "x",    "b",   "y");
  CHECK("aa", "xx",   "bb",  "yy");
  CHECK("",   "xx",   "bb",  "yy");
  CHECK("aa", "",     "bb",  "yy");
  CHECK("aa", "xx",   "",    "yy");
  CHECK("aa", "xx",   "bb",  "");

  CHECK("",   "",     "bb",  "yy");
  CHECK("",   "xx",   "",    "yy");
  CHECK("",   "xx",   "bb",  "");
  CHECK("aa", "",     "",    "yy");
  CHECK("aa", "",     "bb",  "");
  CHECK("aa", "xx",    "",   "");

  CHECK("",   "",     "",    "yy");
  CHECK("",   "",     "bb",  "");
  CHECK("",   "xx",   "",    "");

  CHECK("",   "",     "",    "");
#undef CHECK

  return nerrs;
}

/// Check the generators for a tuple of nullables.
static unsigned check_generators()
{
  unsigned nerrs = 0;
  using namespace vcsn;

  // Basic alphabet type.
  using alphabet_t = set_alphabet<char_letters>;

  // Single-tape labelset.
  using labelset1_t = letterset<alphabet_t>;

  // Create the labelsets.
  const auto ls1 = labelset1_t{{'a', 'b', 'c'}};
  const auto ls2 = labelset1_t{{'x', 'y'}};

  // Create the double-tape labelset.
  const auto ls = make_tupleset(ls1, ls2);

  std::cerr << "Working on: ";
  ls.print_set(std::cerr) << '\n';

  // Specify the syntax of printing labels.
  auto fmt = format{}.for_labels();

  const auto res = [&]{
    auto o = std::ostringstream{};
    const char* sep = "";
    for (auto&& g: ls.generators())
      {
        o << sep;
        ls.print(g, o, fmt);
        sep = ", ";
      }
    return o.str();
  }();

  ASSERT_EQ("\\e|x, \\e|y, a|\\e, a|x, a|y, b|\\e, b|x, b|y, c|\\e, c|x, c|y",
            res);

  return nerrs;
}

int main()
{
  size_t nerrs = 0;
  nerrs += check_letterset();
  nerrs += check_wordset();
  nerrs += check_tupleset();
  nerrs += check_generators();
  return !!nerrs;
}
