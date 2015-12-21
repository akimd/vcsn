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
void print(const Aut& a)
{
  vcsn::info(a, std::cout) << std::endl;
}

/// Test des lal (booléens)
static void check_lal_b()
{
  set_alphabet<char_letters> al{'a','b'};
  ctx::lal_char_b ctx{al};
  using automaton_t = mutable_automaton<ctx::lal_char_b>;
  using state_t = state_t_of<automaton_t>;

  automaton_t res = vcsn::make_shared_ptr<automaton_t>(ctx);
  state_t s[3];
  for(int i=0;i<3;i++)
    s[i]=res->new_state();
  for(int i=0;i<3;i++)
    res->set_transition(s[i],s[(i+1)%3],'a');
  res->set_initial(s[0]);
  res->set_final(s[1]);
  std::cout << "***LAL***" << std::endl;
  print(res);
  std::cout << "proper:" << std::endl;
  automaton_t pro = proper(res);
  print(pro);
  std::cout << "forward_proper:" << std::endl;
  automaton_t pro2 = proper(res, direction::forward);
  print(pro2);
  std::cout << "proper_here:" << std::endl;
  proper_here(res);
  print(res);
}

/// Test des law booléens
static void check_law_char_b()
{
  set_alphabet<char_letters> al{'a','b'};
  ctx::law_char_b ctx{al};
  using automaton_t = mutable_automaton<ctx::law_char_b>;
  using state_t = state_t_of<automaton_t>;

  automaton_t res = vcsn::make_shared_ptr<automaton_t>(ctx);
  state_t s[3];
  for(int i=0;i<3;i++)
    s[i]=res->new_state();
  for(int i=0;i<3;i++)
    res->set_transition(s[i],s[(i+1)%3],"a");
  for(int i=0;i<3;i++)
    res->set_transition(s[i],s[(i+2)%3],"");
  res->set_initial(s[0]);
  res->set_final(s[1]);

  std::cout << "***LAW***" << std::endl;
  print(res);
  std::cout << "proper:" << std::endl;
  automaton_t pro = proper(res);
  print(pro);
  std::cout << "forward_proper:" << std::endl;
  automaton_t pro2 = proper(res, direction::forward);
  print(pro2);
  std::cout << "proper_here:" << std::endl;
  proper_here(res);
  print(res);
}

// Test des lal-char-z
static void check_lal_char_z()
{
  set_alphabet<char_letters> al{'a','b'};
  ctx::lal_char_z ctx{al};
  using automaton_t = mutable_automaton<ctx::lal_char_z>;
  using state_t = state_t_of<automaton_t>;

  automaton_t res = vcsn::make_shared_ptr<automaton_t>(ctx);
  state_t s[3];
  for(int i=0;i<3;i++)
    s[i]=res->new_state();
  for(int i=0;i<3;i++)
    res->set_transition(s[i],s[(i+1)%3],'a',2);
  res->set_initial(s[0]);
  res->set_final(s[1]);
  std::cout << "***LAL Z***" << std::endl;
  print(res);
  std::cout << "proper:" << std::endl;
  automaton_t pro = proper(res);
  print(pro);
  std::cout << "proper_here:" << std::endl;
  proper_here(res);
  print(res);
}

/// Test des law Z
static void check_law_char_z()
{
  set_alphabet<char_letters> al{'a','b'};
  ctx::law_char_z ctx{al};
  using automaton_t = mutable_automaton<ctx::law_char_z>;
  using state_t = state_t_of<automaton_t>;

  automaton_t res = vcsn::make_shared_ptr<automaton_t>(ctx);
  state_t s[3];
  for(int i=0;i<3;i++)
    s[i]=res->new_state();
  for(int i=0;i<3;i++)
    res->set_transition(s[i],s[(i+1)%3],"a",2);
  for(int i=0;i<3;i++)
    res->set_transition(s[i],s[(i+2)%3],"",-1);
  res->set_initial(s[0]);
  res->set_final(s[1]);

  std::cout << "***LAW Z***" << std::endl;
  std::cout << "* Circuit of eps-transition" << std::endl;
  print(res);
  try {
    automaton_t pro = proper(res);
    print(pro);
  }
  catch(std::runtime_error& e) {
    std::cout << "Exception: " << e.what() << std::endl;
  }
  res->del_transition(s[1],s[0], "");
  std::cout << "* No circuit of eps-transition" << std::endl;
  print(res);
  //dot(res,std::cerr);
  std::cout << "proper:" << std::endl;
  automaton_t pro = proper(res);
  print(pro);
  std::cout << "forward_proper:" << std::endl;
  automaton_t pro2 = proper(res, direction::forward);
  print(pro2);
  //dot(pro2,std::cerr);
  std::cout << "proper_here:" << std::endl;
  proper_here(res);
  print(res);
}

/// Test des law Zmin
static void check_law_char_zmin()
{
  set_alphabet<char_letters> al{'a','b'};
  using law_char_zmin = vcsn::context<vcsn::ctx::law_char, vcsn::zmin>;
  law_char_zmin ctx{al};
  using automaton_t = mutable_automaton<law_char_zmin>;
  using state_t = state_t_of<automaton_t>;

  automaton_t res = vcsn::make_shared_ptr<automaton_t>(ctx);
  state_t s[3];
  for(int i=0;i<3;i++)
    s[i]=res->new_state();
  for(int i=0;i<3;i++)
    res->set_transition(s[i],s[(i+1)%3],"a",2);
  for(int i=0;i<3;i++)
    res->set_transition(s[i],s[(i+2)%3],"",-1);
  res->set_initial(s[0]);
  res->set_final(s[1]);
  std::cout << "***LAW Z-min+***" << std::endl;
  std::cout << "* Circuit of eps-transition" << std::endl;
  print(res);
  try {
    automaton_t pro = proper(res);
    print(pro);
  }
  catch(std::runtime_error& e) {
    std::cout << "Exception: " << e.what() << std::endl;
  }
  res->set_transition(s[1],s[0], "",3);
  std::cout << "* No negative circuit" << std::endl;
  print(res);
  //dot(res,std::cerr);
  std::cout << "proper:" << std::endl;
  automaton_t pro = proper(res);
  print(pro);
  std::cout << "forward_proper:" << std::endl;
  automaton_t pro2 = proper(res, direction::forward);
  print(pro2);
  dot(pro2, std::cout) << std::endl;
  std::cout << "proper_here:" << std::endl;
  proper_here(res);
  print(res);
}


int main()
{
  check_lal_b();
  check_law_char_b();
  check_lal_char_z();
  check_law_char_z();
  check_law_char_zmin();
}
