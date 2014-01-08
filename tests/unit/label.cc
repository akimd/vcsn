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
  using labelset_t =
    vcsn::ctx::letterset<vcsn::set_alphabet<vcsn::char_letters>>;
  using genset_t = labelset_t::genset_t;
  genset_t gs{'a', 'b', 'c'};
  labelset_t ls{gs};

  ASSERT_EQ(ls.is_valid(conv(ls, "a")), true);
  ASSERT_EQ(ls.is_one(conv(ls, "a")), false);
  ASSERT_EQ(ls.is_special(conv(ls, "a")), false);

  ASSERT_EQ(ls.is_valid('x'), false);

  ASSERT_EQ(ls.format(conv(ls, "a")), "a");
  ASSERT_EQ(ls.format(ls.transpose(conv(ls, "a"))), "a");

  return nerrs;
}

static unsigned
check_tupleset()
{
  unsigned nerrs = 0;
  using labelset_t =
    vcsn::ctx::wordset<vcsn::set_alphabet<vcsn::char_letters>>;
  using genset_t = labelset_t::genset_t;
  genset_t gs1{'a', 'b', 'c'};
  labelset_t ls1{gs1};
  genset_t gs2{'x', 'y', 'z'};
  labelset_t ls2{gs2};

  using tupleset_t = vcsn::ctx::tupleset<labelset_t, labelset_t>;
  tupleset_t ts{ls1, ls2};
  using label_t = tupleset_t::value_t;

  label_t l{"abc", "xyz"};

  // sname.
  ASSERT_EQ(tupleset_t::sname(), "lat<law_char, law_char>");

  // vname.
  ASSERT_EQ(ts.vname(false), "lat<law_char, law_char>");
  ASSERT_EQ(ts.vname(), "lat<law_char(abc), law_char(xyz)>");

  // equals.
  ASSERT_EQ(ts.equals(label_t{"ab", "x"}, label_t{"ab", "x"}), true);
  ASSERT_EQ(ts.equals(label_t{"ab", "x"}, label_t{"abc", "x"}), false);
  ASSERT_EQ(ts.equals(label_t{"ab", "x"}, label_t{"", "x"}), false);
  ASSERT_EQ(ts.equals(label_t{"ab", "x"}, label_t{"ab", "xx"}), false);
  ASSERT_EQ(ts.equals(label_t{"ab", "x"}, label_t{"ab", "y"}), false);
  ASSERT_EQ(ts.equals(label_t{"ab", "x"}, label_t{"ab", ""}), false);

  // special, is_special.
  ASSERT_EQ(ts.equals(ts.special(), label_t{ls1.special(),ls2.special()}), true);
  ASSERT_EQ(ts.format(label_t{ls1.special(),ls2.special()}), "");
  ASSERT_EQ(ts.format(ts.special()), "");
  ASSERT_EQ(ts.is_special(ts.special()), true);
  ASSERT_EQ(!ts.is_special(l), true);

  // is_one.
  ASSERT_EQ(ts.is_one(label_t{ls1.one(), ls2.one()}), true);
  ASSERT_EQ(ts.is_one(label_t{ls1.one(), ls2.special()}), false);
  ASSERT_EQ(ts.is_one(label_t{ls1.one(), "x"}), false);

  // format, transpose.
  ASSERT_EQ(ts.format(l), "(abc, xyz)");
  ASSERT_EQ(ts.format(ts.transpose(l)), "(cba, zyx)");

  return nerrs;
}

int main()
{
  size_t nerrs = 0;
  nerrs += check_letterset();
# if HAVE_CORRECT_LIST_INITIALIZER_ORDER
  nerrs += check_tupleset();
# endif
  return !!nerrs;
}
