#include <iostream>
#include <stdexcept>

#include <vcsn/algos/dot.hh>
#include <vcsn/algos/eps-removal.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/ctx/lal_char_b.hh>
#include <vcsn/ctx/lal_char_z.hh>
#include <vcsn/ctx/law_char_b.hh>
#include <vcsn/ctx/law_char_z.hh>
#include <vcsn/ctx/law_char_zmin.hh>
#include <vcsn/misc/direction.hh>

using namespace vcsn;

template<typename Aut>
void print(const Aut& a)
{
  std::cout << "States: " << a.num_states()
            << " Initials: " << a.num_initials()
            << " Finals: " << a.num_finals()
            << " Transitions: " << a.num_transitions()
            << std::endl;
}

template<typename Aut>
void printlaw(const Aut& a)
{
  print(a);
  unsigned c[2]={0,0};
  for(auto t : a.transitions())
    c[a.label_of(t) == a.labelset()->empty_word()]++;
  std::cout << " Eps-transitions: " << c[1]
            << " Non eps-trans: " << c[0]
            << std::endl;
}


int main()
{
  /// Test des lal (booléens)
  set_alphabet<char_letters> al{'a','b'};
  {
    ctx::lal_char_b ct{al};
    using automaton_t = mutable_automaton<ctx::lal_char_b>;
    using state_t = automaton_t::state_t;

    automaton_t res{ct};
    state_t s[3];
    for(int i=0;i<3;i++)
      s[i]=res.new_state();
    for(int i=0;i<3;i++)
      res.set_transition(s[i],s[(i+1)%3],'a');
    res.set_initial(s[0]);
    res.set_final(s[1]);
    std::cout << "***LAL***" << std::endl;
    print(res);
    std::cout << "Lal valid: "<< is_valid(res) << std::endl;
    std::cout << "Lal proper: "<< is_proper(res) << std::endl;
    std::cout << "eps_removal:" << std::endl;
    automaton_t pro = eps_removal(res);
    print(pro);
    std::cout << "backward_eps_removal:" << std::endl;
    automaton_t pro2 = eps_removal(res, direction_t::BACKWARD);
    print(pro2);
    std::cout << "eps_removal_here:"<< std::endl;
    eps_removal_here(res);
    print(res);
  }
  /// Test des law booléens
  {
    ctx::law_char_b ct{al};
    using automaton_t = mutable_automaton<ctx::law_char_b>;
    using state_t = automaton_t::state_t;

    automaton_t res{ct};
    state_t s[3];
    for(int i=0;i<3;i++)
      s[i]=res.new_state();
    for(int i=0;i<3;i++)
      res.set_transition(s[i],s[(i+1)%3],"a");
    for(int i=0;i<3;i++)
      res.set_transition(s[i],s[(i+2)%3],"");
    res.set_initial(s[0]);
    res.set_final(s[1]);
    std::cout << "***LAW***" << std::endl;
    printlaw(res);
    std::cout << "Law proper: "<< is_proper(res) << std::endl;
    std::cout << "Law valid: "<< is_valid(res) << std::endl;
    std::cout << "eps_removal:" << std::endl;
    automaton_t pro = eps_removal(res);
    printlaw(pro);
    std::cout << "backward_eps_removal:" << std::endl;
    automaton_t pro2 = eps_removal(res, direction_t::BACKWARD);
    print(pro2);
    std::cout << "Law proper: "<< is_proper(pro) << std::endl;
    std::cout << "Law valid: "<< is_valid(pro) << std::endl;
    std::cout << "eps_removal_here:"<< std::endl;
    eps_removal_here(res);
    printlaw(res);
  }
  /// Test des lal-char-z
  {
    ctx::lal_char_z ct{al};
    using automaton_t = mutable_automaton<ctx::lal_char_z>;
    using state_t = automaton_t::state_t;

    automaton_t res{ct};
    state_t s[3];
    for(int i=0;i<3;i++)
      s[i]=res.new_state();
    for(int i=0;i<3;i++)
      res.set_transition(s[i],s[(i+1)%3],'a',2);
    res.set_initial(s[0]);
    res.set_final(s[1]);
    std::cout << "***LAL Z***" << std::endl;
    print(res);
    std::cout << "Lal valid: "<< is_valid(res) << std::endl;
    std::cout << "Lal proper: "<< is_proper(res) << std::endl;
    std::cout << "eps_removal:" << std::endl;
    automaton_t pro = eps_removal(res);
    print(pro);
    std::cout << "eps_removal_here:"<< std::endl;
    eps_removal_here(res);
    print(res);
  }

  /// Test des law booléens Z
  {
    ctx::law_char_z ct{al};
    using automaton_t = mutable_automaton<ctx::law_char_z>;
    using state_t = automaton_t::state_t;

    automaton_t res{ct};
    state_t s[3];
    for(int i=0;i<3;i++)
      s[i]=res.new_state();
    for(int i=0;i<3;i++)
      res.set_transition(s[i],s[(i+1)%3],"a",2);
    for(int i=0;i<3;i++)
      res.set_transition(s[i],s[(i+2)%3],"",-1);
    res.set_initial(s[0]);
    res.set_final(s[1]);
    std::cout << "***LAW Z***" << std::endl;
    std::cout << "* Circuit of eps-transition" << std::endl;
    printlaw(res);
    std::cout << "Law proper: "<< is_proper(res) << std::endl;
    std::cout << "Law valid: "<< is_valid(res) << std::endl;
    try {
      automaton_t pro = eps_removal(res);
      printlaw(pro);
      std::cout << "Law proper: "<< is_proper(pro) << std::endl;
      std::cout << "Law valid: "<< is_valid(pro) << std::endl;
    }
    catch(std::domain_error& e) {
      std::cout << "Exception: " << e.what() << std::endl;
    }
    res.del_transition(s[1],s[0], "");
    std::cout << "* No circuit of eps-transition" << std::endl;
    printlaw(res);
    std::cout << "Law proper: "<< is_proper(res) << std::endl;
    std::cout << "Law valid: "<< is_valid(res) << std::endl;
    //dot(res,std::cerr);
    std::cout << "eps_removal:" << std::endl;
    automaton_t pro = eps_removal(res);
    printlaw(pro);
    std::cout << "Law proper: "<< is_proper(pro) << std::endl;
    std::cout << "Law valid: "<< is_valid(pro) << std::endl;
    std::cout << "backward_eps_removal:" << std::endl;
    automaton_t pro2 = eps_removal(res, direction_t::BACKWARD);
    print(pro2);
    std::cout << "Law proper: "<< is_proper(pro2) << std::endl;
    std::cout << "Law valid: "<< is_valid(pro2) << std::endl;
    //dot(pro2,std::cerr);
    std::cout << "eps_removal_here:"<< std::endl;
    eps_removal_here(res);
    printlaw(res);
  }
  /// Test des law booléens Zmin
  {
    ctx::law_char_zmin ct{al};
    using automaton_t = mutable_automaton<ctx::law_char_zmin>;
    using state_t = automaton_t::state_t;

    automaton_t res{ct};
    state_t s[3];
    for(int i=0;i<3;i++)
      s[i]=res.new_state();
    for(int i=0;i<3;i++)
      res.set_transition(s[i],s[(i+1)%3],"a",2);
    for(int i=0;i<3;i++)
      res.set_transition(s[i],s[(i+2)%3],"",-1);
    res.set_initial(s[0]);
    res.set_final(s[1]);
    std::cout << "***LAW Z-min+***" << std::endl;
    std::cout << "* Circuit of eps-transition" << std::endl;
    printlaw(res);
    std::cout << "Law proper: "<< is_proper(res) << std::endl;
    std::cout << "Law valid: "<< is_valid(res) << std::endl;
    try {
      automaton_t pro = eps_removal(res);
      printlaw(pro);
      std::cout << "Law proper: "<< is_proper(pro) << std::endl;
      std::cout << "Law valid: "<< is_valid(pro) << std::endl;
    }
    catch(std::domain_error& e) {
      std::cout << "Exception: " << e.what() << std::endl;
    }
    res.set_transition(s[1],s[0], "",3);
    std::cout << "* No negative circuit" << std::endl;
    printlaw(res);
    std::cout << "Law proper: "<< is_proper(res) << std::endl;
    std::cout << "Law valid: "<< is_valid(res) << std::endl;
    //dot(res,std::cerr);
    std::cout << "eps_removal:" << std::endl;
    automaton_t pro = eps_removal(res);
    printlaw(pro);
    std::cout << "Law proper: "<< is_proper(pro) << std::endl;
    std::cout << "Law valid: "<< is_valid(pro) << std::endl;
    std::cout << "backward_eps_removal:" << std::endl;
    automaton_t pro2 = eps_removal(res, direction_t::BACKWARD);
    print(pro2);
    std::cout << "Law proper: "<< is_proper(pro2) << std::endl;
    std::cout << "Law valid: "<< is_valid(pro2) << std::endl;
    dot(pro2,std::cout);
    std::cout << "eps_removal_here:" << std::endl;
    eps_removal_here(res);
    printlaw(res);
  }

}
