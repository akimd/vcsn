#include <tests/unit/test.hh>
#include <iostream>
#include <vcsn/labelset/letterset.hh>
#include <vcsn/alphabets/setalpha.hh>
#include <vcsn/alphabets/char.hh>

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

int main()
{
  size_t nerrs = 0;
  nerrs += check_letterset();
  return !!nerrs;
}
