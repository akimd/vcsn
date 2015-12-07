// This test relies on assert.
#undef NDEBUG
#include <iostream>
#include <vcsn/algos/dot.hh>
#include <vcsn/core/mutable-automaton.hh>
#include <vcsn/ctx/lao_z.hh>

int main()
{
  auto ctx = vcsn::ctx::lao_z{};
  auto aut = vcsn::make_mutable_automaton(ctx);

  auto s1 = aut->new_state();
  auto s2 = aut->new_state();
  auto s3 = aut->new_state();
  aut->set_initial(s1);
  aut->set_final(s2, 10);
  aut->set_transition(s1, s2, {}, 51);
  aut->set_transition(s2, s3, {}, 3);
  aut->set_transition(s2, s1, {}, 1);
  int v = aut->add_weight(aut->set_transition(s1, s1, {}, 2), 40);
  assert(v == 42);
  aut->set_transition(s1, s3, {}, 1);
  vcsn::dot(aut, std::cout) << '\n';
  assert(aut->num_states() == 3);
  assert(aut->num_transitions() == 5);

#define CHECK(Message, Transitions)             \
  do {                                          \
    std::cout << Message << '\n';               \
    for (auto t: Transitions)                   \
      {                                         \
        assert(aut->has_transition(t));         \
        std::cout << "  ";                      \
        aut->print(t, std::cout) << '\n';       \
      }                                         \
  } while (false)

  CHECK("Leaving s1", aut->out(s1, {}));
  CHECK("Entering s1", aut->in(s1, {}));
  CHECK("Between s1 and s1", aut->outin(s1, s1));
#undef CHECK

  aut->add_transition(s1, s1, {}, -42);
  vcsn::dot(aut, std::cout) << '\n';
  assert(aut->outin(s1, s1).empty());
  assert(aut->num_states() == 3);
  assert(aut->num_transitions() == 4);

  aut->del_state(s1);
  vcsn::dot(aut, std::cout) << '\n';
  assert(!aut->has_state(s1));
  assert(aut->has_state(s2));
  assert(aut->has_state(s3));

  assert(aut->num_states() == 2);
  assert(aut->num_transitions() == 1);

  aut->set_transition(s2, s3, {}, 0);

  vcsn::dot(aut, std::cout) << '\n';
  assert(aut->num_states() == 2);
  assert(aut->num_transitions() == 0);
}
