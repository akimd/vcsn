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

  ASSERT_EQ(ts.vname(), "lat<law_char(abc), law_char(xyz)>");

  ASSERT_EQ(ts.format(l), "(abc, xyz)");
  ASSERT_EQ(ts.format(ts.transpose(l)), "(cba, zyx)");

  return nerrs;
}

int main()
{
  size_t nerrs = 0;
  nerrs += check_letterset();
  nerrs += check_tupleset();
  return !!nerrs;
}
