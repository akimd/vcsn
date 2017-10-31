#include <iostream>
#include <stdexcept>

#include <vcsn/algos/dot.hh>
#include <vcsn/algos/info.hh>
#include <vcsn/algos/proper.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/ctx/lal_char_b.hh>
#include <vcsn/ctx/lal_char_z.hh>
#include <vcsn/ctx/law_char_b.hh>
#include <vcsn/ctx/law_char_z.hh>
#include <vcsn/misc/direction.hh>
#include <vcsn/weightset/zmin.hh>

using namespace vcsn;

template <Automaton Aut>
void
print_info(const Aut& a)
{
  vcsn::info(a, std::cout, 2, false) << '\n';
}

/// Test des lal (booléens)
static void
check_lal_char_b()
{
  auto al = set_alphabet<char_letters>{'a', 'b'};
  auto ctx = ctx::lal_char_b{al};
  using automaton_t = mutable_automaton<ctx::lal_char_b>;
  using state_t = state_t_of<automaton_t>;

  auto res = vcsn::make_shared_ptr<automaton_t>(ctx);
  state_t s[3];
  for (int i = 0; i < 3; ++i)
    s[i] = res->new_state();
  for (int i = 0; i < 3; ++i)
    res->set_transition(s[i], s[(i + 1) % 3], 'a');
  res->set_initial(s[0]);
  res->set_final(s[1]);
  std::cout << "---------- LAL\n";
  print_info(res);
  std::cout << "proper:\n";
  auto pro = proper(res);
  print_info(pro);
  std::cout << "forward_proper:\n";
  auto pro2 = proper(res, direction::forward);
  print_info(pro2);
  std::cout << "proper_here:\n";
  proper_here(res);
  print_info(res);
}

/// Test des law booléens
static void
check_law_char_b()
{
  auto al = set_alphabet<char_letters>{'a', 'b'};
  auto ctx = ctx::law_char_b{al};
  using automaton_t = mutable_automaton<ctx::law_char_b>;
  using state_t = state_t_of<automaton_t>;

  auto res = vcsn::make_shared_ptr<automaton_t>(ctx);
  state_t s[3];
  for (int i = 0; i < 3; ++i)
    s[i] = res->new_state();
  for (int i = 0; i < 3; ++i)
    res->set_transition(s[i], s[(i + 1) % 3], "a");
  for (int i = 0; i < 3; ++i)
    res->set_transition(s[i], s[(i + 2) % 3], "");
  res->set_initial(s[0]);
  res->set_final(s[1]);

  std::cout << "---------- LAW\n";
  print_info(res);
  std::cout << "proper:\n";
  auto pro = proper(res);
  print_info(pro);
  std::cout << "forward_proper:\n";
  auto pro2 = proper(res, direction::forward);
  print_info(pro2);
  std::cout << "proper_here:\n";
  proper_here(res);
  print_info(res);
}

// Test des lal-char-z
static void
check_lal_char_z()
{
  auto al = set_alphabet<char_letters>{'a', 'b'};
  auto ctx = ctx::lal_char_z{al};
  using automaton_t = mutable_automaton<ctx::lal_char_z>;
  using state_t = state_t_of<automaton_t>;

  auto res = vcsn::make_shared_ptr<automaton_t>(ctx);
  state_t s[3];
  for (int i = 0; i < 3; ++i)
    s[i] = res->new_state();
  for (int i = 0; i < 3; ++i)
    res->set_transition(s[i], s[(i + 1) % 3], 'a', 2);
  res->set_initial(s[0]);
  res->set_final(s[1]);

  std::cout << "---------- LAL Z\n";
  print_info(res);
  std::cout << "proper:\n";
  auto pro = proper(res);
  print_info(pro);
  std::cout << "proper_here:\n";
  proper_here(res);
  print_info(res);
}

/// Test des law Z
static void
check_law_char_z()
{
  auto al = set_alphabet<char_letters>{'a', 'b'};
  auto ctx = ctx::law_char_z{al};
  using automaton_t = mutable_automaton<ctx::law_char_z>;
  using state_t = state_t_of<automaton_t>;

  auto res = vcsn::make_shared_ptr<automaton_t>(ctx);
  state_t s[3];
  for (int i = 0; i < 3; ++i)
    s[i] = res->new_state();
  for (int i = 0; i < 3; ++i)
    res->set_transition(s[i], s[(i + 1) % 3], "a", 2);
  for (int i = 0; i < 3; ++i)
    res->set_transition(s[i], s[(i + 2) % 3], "", -1);
  res->set_initial(s[0]);
  res->set_final(s[1]);

  std::cout << "---------- LAW Z\n";
  std::cout << "* Circuit of eps-transition\n";
  print_info(res);
  try
  {
    auto pro = proper(res);
    print_info(pro);
  }
  catch (std::runtime_error& e)
  {
    std::cout << "Exception: " << e.what() << '\n';
  }
  res->del_transition(s[1], s[0], "");
  std::cout << "* No circuit of eps-transition\n";
  print_info(res);
  // dot(res,std::cerr);
  std::cout << "proper:\n";
  auto pro = proper(res);
  print_info(pro);
  std::cout << "forward_proper:\n";
  auto pro2 = proper(res, direction::forward);
  print_info(pro2);
  // dot(pro2,std::cerr);
  std::cout << "proper_here:\n";
  proper_here(res);
  print_info(res);
}

/// Check law Zmin
static void
check_law_char_zmin()
{
  auto al = set_alphabet<char_letters>{'a', 'b'};
  using law_char_zmin = vcsn::context<vcsn::ctx::law_char, vcsn::zmin>;
  auto ctx = law_char_zmin{al};
  using automaton_t = mutable_automaton<law_char_zmin>;
  using state_t = state_t_of<automaton_t>;

  auto res = vcsn::make_shared_ptr<automaton_t>(ctx);
  state_t s[3];
  for (int i = 0; i < 3; ++i)
    s[i] = res->new_state();
  for (int i = 0; i < 3; ++i)
    res->set_transition(s[i], s[(i + 1) % 3], "a", 2);
  for (int i = 0; i < 3; ++i)
    res->set_transition(s[i], s[(i + 2) % 3], "", -1);
  res->set_initial(s[0]);
  res->set_final(s[1]);
  std::cout << "---------- LAW Z-min+\n";
  std::cout << "* Circuit of eps-transition\n";
  print_info(res);
  try
  {
    auto pro = proper(res);
    print_info(pro);
  }
  catch (std::runtime_error& e)
  {
    std::cout << "Exception: " << e.what() << '\n';
  }
  res->set_transition(s[1], s[0], "", 3);
  std::cout << "* No negative circuit\n";
  print_info(res);
  // dot(res,std::cerr);
  std::cout << "proper:\n";
  auto pro = proper(res);
  print_info(pro);
  std::cout << "forward_proper:\n";
  auto pro2 = proper(res, direction::forward);
  print_info(pro2);
  dot(pro2, std::cout) << '\n';
  std::cout << "proper_here:\n";
  proper_here(res);
  print_info(res);
}

int
main()
{
  check_lal_char_b();
  std::cout << '\n';
  check_law_char_b();
  std::cout << '\n';
  check_lal_char_z();
  std::cout << '\n';
  check_law_char_z();
  std::cout << '\n';
  check_law_char_zmin();
}
