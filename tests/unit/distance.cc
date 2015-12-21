#include <iomanip>

#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/ctx/context.hh>
#include <vcsn/ctx/lal_char_z.hh>
#include <vcsn/algos/distance.hh>

using namespace vcsn;

template <Automaton Aut>
static void print_fw(std::vector<std::vector<weight_t_of<Aut>>> fw)
{
  using ws_t = weightset_t_of<Aut>;
  for (auto ve : fw)
    {
      for (auto d : ve)
        {
          std::cout << " " << std::setw(5);
          ws_t::print(d, std::cout);
        }
      std::cout << std::endl;
    }
}

static unsigned check_basic()
{
  using automaton_t = mutable_automaton<ctx::lal_char_z>;
  using state_t = state_t_of<automaton_t>;

  std::cout << "** CHECK_BASIC **" << std::endl;

  set_alphabet<char_letters> al{'a', 'b'};
  ctx::lal_char_z ctx{al};
  automaton_t aut = make_shared_ptr<automaton_t>(ctx);

  state_t s[3];
  for(int i = 0; i < 3; i++)
    s[i] = aut->new_state();
  aut->set_transition(s[0], s[1], 'a', 1);
  aut->set_transition(s[1], s[2], 'b', 2);

  auto fw = all_distances(aut);
  print_fw<automaton_t>(fw);
  return 0;
}

static unsigned check_zmin()
{
  using lal_char_zmin = context<vcsn::ctx::lal_char, vcsn::zmin>;
  using automaton_t = mutable_automaton<lal_char_zmin>;
  using state_t = state_t_of<automaton_t>;

  std::cout << "** CHECK_ZMIN **" << std::endl;

  set_alphabet<char_letters> al{'a','b'};
  lal_char_zmin ctx{al};
  automaton_t aut = make_shared_ptr<automaton_t>(ctx);

  state_t s[6];
  for(int i = 0; i < 6; i++)
    s[i] = aut->new_state();
  aut->set_transition(s[4], s[0], 'a', 3);
  aut->set_transition(s[0], s[4], 'a', 1);
  aut->set_transition(s[2], s[4], 'a', 2);
  aut->set_transition(s[1], s[3], 'a', 4);
  aut->set_transition(s[3], s[2], 'a', 1);
  aut->set_transition(s[5], s[0], 'a', 3);
  aut->set_transition(s[0], s[1], 'b', 1);
  aut->set_transition(s[3], s[4], 'b', 6);
  aut->set_transition(s[2], s[1], 'b', 3);
  aut->set_transition(s[1], s[2], 'b', 4);
  aut->set_transition(s[4], s[5], 'b', 2);
  aut->set_transition(s[5], s[1], 'b', 9);

  auto fw = all_distances(aut);
  print_fw<automaton_t>(fw);
  return 0;
}

static unsigned check_r_loop()
{
  using lal_char_r = context<vcsn::ctx::lal_char, vcsn::r>;
  using automaton_t = mutable_automaton<lal_char_r>;
  using state_t = state_t_of<automaton_t>;

  std::cout << "** CHECK_R_LOOP **" << std::endl;

  lal_char_r c{set_alphabet<char_letters>{'a'}};
  automaton_t aut = make_shared_ptr<automaton_t>(c);
  state_t s = aut->new_state();
  aut->set_transition(s, s, 'a', 0.5);

  auto fw = all_distances(aut);
  print_fw<automaton_t>(fw);
  return 0;
}

int main()
{
  unsigned nb_errors = 0;
  nb_errors += check_basic();
  nb_errors += check_zmin();
  nb_errors += check_r_loop();
  return !!nb_errors;
}
