#include <iostream>

#include <vcsn/algos/dotty.hh>
#include <vcsn/algos/eval.hh>
#include <vcsn/algos/product.hh>
#include <vcsn/alphabets/char.hh>
#include <vcsn/alphabets/setalpha.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/weights/z.hh>

#define EVAL(automaton, str, strResult)                                 \
  do {                                                                  \
    int w = vcsn::eval(automaton, str);                                 \
    if (w != strResult)                                                 \
      {                                                                 \
        std::cout << "FAIL: eval(\""                                    \
                  << str << "\") == " << w << " != "                    \
                  << strResult << std::endl;                            \
        res = 1;                                                        \
      }                                                                 \
    else                                                                \
      std::cout << "PASS: " << str << std::endl;                        \
  } while (0)

int main()
{
  using alpha_t = vcsn::set_alphabet<vcsn::char_letters>;
  struct context_t
  {
    using genset_t = alpha_t;
    using weightset_t = vcsn::z;
  };
  using automaton_t =
    vcsn::mutable_automaton<context_t, vcsn::labels_are_letters>;

  alpha_t alpha{'a', 'b', 'c'};

  vcsn::z z;
  automaton_t aut(alpha, z);
  int res = 0;
  // Test empty automaton
  EVAL(aut, "aa", 0);
  EVAL(aut, "bbb", 0);

  auto s1 = aut.new_state();
  auto s2 = aut.new_state();
  aut.set_initial(s1);
  aut.set_final(s2);
  aut.set_transition(s1, s1 , 'a');
  aut.set_transition(s1, s1 , 'b');

  aut.set_transition(s1, s2 , 'a', 2);

  aut.set_transition(s2, s2 , 'a');
  aut.set_transition(s2, s2 , 'b');

  EVAL(aut, "aabab", 6);
  EVAL(aut, "abab", 4);
  EVAL(aut, "aaaa", 8);
  EVAL(aut, "b", 0);
  EVAL(aut, "a", 2);

  auto s3 = aut.new_state();

  aut.set_transition(s1, s3, 'c');
  aut.set_transition(s3, s1, 'a');
  aut.set_transition(s3, s2, 'a');

  EVAL(aut, "caa", 3);

  auto prod = product(aut, aut);
  EVAL(prod, "aabab", 36);

  return res;
}
#undef EVAL
