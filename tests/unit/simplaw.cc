#include <vcsn/weights/z.hh>
#include <vcsn/alphabets/char.hh>
#include <vcsn/alphabets/setalpha.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/dotty.hh>
#include <iostream>

int main()
{
  struct context_t
  {
    using genset_t = vcsn::set_alphabet<vcsn::char_letters>;
    genset_t gs_ = {'a', 'b', 'c', 'd'};
    using weightset_t = vcsn::z;
    weightset_t ws_;
  };
  context_t ctx;
  using automaton_t =
    vcsn::mutable_automaton<context_t, vcsn::labels_are_words>;
  automaton_t aut(ctx);

  auto s1 = aut.new_state();
  auto s2 = aut.new_state();
  auto s3 = aut.new_state();

  aut.set_initial(s1);
  aut.set_final(s2, 10);
  aut.set_transition(s1, s2, "ca", 42);
  aut.set_transition(s2, s3, "a", 1);
  aut.set_transition(s2, s1, "ba", 1);
  aut.set_transition(s3, s2, "", 2);

  vcsn::dotty(aut, std::cout);
}
