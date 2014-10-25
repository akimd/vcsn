#include <vcsn/config.hh>

#include <iostream>

#include <tests/unit/test.hh>

#include <vcsn/alphabets/char.hh>
#include <vcsn/alphabets/setalpha.hh>
#include <vcsn/labelset/letterset.hh>
#include <vcsn/labelset/tupleset.hh>
#include <vcsn/labelset/wordset.hh>

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

  ASSERT_EQ(format(ls, conv(ls, "a")), "a");
  ASSERT_EQ(format(ls, ls.transpose(conv(ls, "a"))), "a");

  return nerrs;
}

static unsigned
check_tupleset()
{
  unsigned nerrs = 0;
  using wordset_t = vcsn::wordset<vcsn::set_alphabet<vcsn::char_letters>>;
  using genset_t = wordset_t::genset_t;
  genset_t gs1{'a', 'b', 'c'};
  wordset_t ls1{gs1};
  genset_t gs2{'x', 'y', 'z'};
  wordset_t ls2{gs2};

  using wwset_t = vcsn::tupleset<wordset_t, wordset_t>;
  using ww_t = wwset_t::value_t;
  wwset_t wwset{ls1, ls2};

  using letterset_t = vcsn::letterset<vcsn::set_alphabet<vcsn::char_letters>>;
  using wlset_t = vcsn::tupleset<wordset_t, letterset_t>;
  using wl_t = wlset_t::value_t;
  wlset_t wlset{ls1, letterset_t{gs2}};

  // sname.
  ASSERT_EQ(wwset_t::sname(), "lat<law_char, law_char>");

  // vname.
  ASSERT_EQ(wwset.vname(false), "lat<law_char, law_char>");
  ASSERT_EQ(wwset.vname(), "lat<law_char(abc), law_char(xyz)>");

  // print_set.
  {
    std::ostringstream o;
    wwset.print_set(o, "text");
    ASSERT_EQ(o.str(), "lat<law_char(abc), law_char(xyz)>");
  }
  {
    std::ostringstream o;
    wwset.print_set(o, "latex");
    ASSERT_EQ(o.str(), "\\{a, b, c\\}^* \\times \\{x, y, z\\}^*");
  }

  // make.
  // If you observe a runtime exception here, such as
  //
  // terminate called after throwing an instance of 'std::runtime_error'
  //  what():  unexpected: (: expected ,
  //
  // then your problem is that your compiler (e.g., G++ 4.8) is buggy.
  // But really, you should no longer see such errors: tupleset has workarounds.
  {
    std::string n = "lat<law_char(ABC), law_char(XYZ)>";
    std::istringstream is(n);
    ASSERT_EQ(wwset_t::make(is).vname(), n);
  }
  {
    std::string n = "lat<law_char(ABC), lal_char(XYZ)>";
    std::istringstream is(n);
    ASSERT_EQ(wlset_t::make(is).vname(), n);
  }

  // equals.
  ASSERT_EQ(wwset.equals(ww_t{"ab", "x"}, ww_t{"ab", "x"}), true);
  ASSERT_EQ(wwset.equals(ww_t{"ab", "x"}, ww_t{"abc", "x"}), false);
  ASSERT_EQ(wwset.equals(ww_t{"ab", "x"}, ww_t{"", "x"}), false);
  ASSERT_EQ(wwset.equals(ww_t{"ab", "x"}, ww_t{"ab", "xx"}), false);
  ASSERT_EQ(wwset.equals(ww_t{"ab", "x"}, ww_t{"ab", "y"}), false);
  ASSERT_EQ(wwset.equals(ww_t{"ab", "x"}, ww_t{"ab", ""}), false);

  // less_than.
#define CHECK(L, R, Res)                                \
  do {                                                  \
    ASSERT_EQ(wwset.less_than(ww_t L, ww_t R), Res);    \
    ASSERT_EQ(wwset.less_than(ww_t R, ww_t L), !Res);   \
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
  ASSERT_EQ(wwset.equals(wwset.special(), ww_t{ls1.special(),ls2.special()}),
            true);
  ASSERT_EQ(format(wwset, ww_t{ls1.special(),ls2.special()}), "");
  ASSERT_EQ(format(wwset, wwset.special()), "");
  ASSERT_EQ(wwset.is_special(wwset.special()), true);
  ASSERT_EQ(!wwset.is_special(ww_t{"abc", "xyz"}), true);

  // is_one.
  ASSERT_EQ(wwset.is_one(ww_t{ls1.one(), ls2.one()}), true);
  ASSERT_EQ(wwset.is_one(ww_t{ls1.one(), ls2.special()}), false);
  ASSERT_EQ(wwset.is_one(ww_t{ls1.one(), "x"}), false);

  // has_one.
  ASSERT_EQ(wwset_t::has_one(), true);
  ASSERT_EQ(wlset_t::has_one(), false);

  // format, transpose.
  ASSERT_EQ(format(wwset, ww_t{"abc", "xyz"}), "(abc,xyz)");
  ASSERT_EQ(format(wwset, wwset.transpose(ww_t{"abc","xyz"})), "(cba,zyx)");

  // conv.
  // Exposed to the same bugs as make, see above.
  ASSERT_EQ(wwset.equals(conv(wwset, "(abc,xyz)"), ww_t{"abc", "xyz"}), true);
  ASSERT_EQ(wwset.equals(conv(wwset, "(abc,\\e)"), ww_t{"abc", ""}), true);
  ASSERT_EQ(wwset.equals(conv(wwset, "(\\e,x)"),   ww_t{"", "x"}), true);
  ASSERT_EQ(wwset.equals(conv(wwset, "(\\e,\\e)"), ww_t{"", ""}), true);

  ASSERT_EQ(wlset.equals(conv(wlset, "(abc,x)"),   wl_t{"abc", 'x'}), true);

  // concat.
#define CHECK(L1, R1, L2, R2)                                           \
  ASSERT_EQ(wwset.equals(wwset.concat(ww_t{L1, R1}, ww_t{L2, R2}),      \
                         ww_t{L1 L2, R1 R2}), true)
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

int main()
{
  size_t nerrs = 0;
  nerrs += check_letterset();
  nerrs += check_tupleset();
  return !!nerrs;
}
