#include <iostream>
#include <sstream>

#include <vcsn/weights/b.hh>
#include <vcsn/alphabets/char.hh>
#include <vcsn/alphabets/setalpha.hh>
#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/determinize.hh>
#include <vcsn/algos/dotty.hh>
#include <vcsn/factory/ladybird.hh>

struct context_t
{
  using genset_t = vcsn::set_alphabet<vcsn::char_letters>;
  genset_t gs_ = genset_t{'a', 'b', 'c'};
  using weightset_t = vcsn::b;
  weightset_t ws_ = weightset_t{};
};

context_t context{};

using automaton_t =
  vcsn::mutable_automaton<context_t, vcsn::labels_are_letters>;

// (a+b)*a(a+b)^n.
automaton_t
factory(const context_t& ctx, unsigned n)
{
  assert(n > 1);

  automaton_t res(ctx);

  auto init = res.new_state();
  res.set_initial(init);
  res.set_transition(init, init, 'a');
  res.set_transition(init, init, 'b');

  auto prev = res.new_state();
  res.set_transition(init, prev, 'a');

  --n;

  while (--n)
    {
      auto next = res.new_state();
      res.set_transition(prev, next, 'a');
      res.set_transition(prev, next, 'b');
      prev = next;
    }
  res.set_final(prev);
  return res;
}

bool idempotence(std::string str, automaton_t& aut, bool display_aut)
{
  auto determ = vcsn::determinize(aut);
  auto id = vcsn::determinize(determ);
  std::stringstream determ_string;
  std::stringstream id_string;
  vcsn::dotty(determ, determ_string);
  vcsn::dotty(id, id_string);
  if (!determ_string.str().compare(id_string.str()))
    {
      std::cout <<"PASS: Check Idempotence for " << str << std::endl;
      if (display_aut)
        std::cout << determ_string.str();
      return false;
    }
  else
    {
      std::cout << "FAIL: determinize(aut) != determinize(determinize(aut)) for "
                << str << std::endl;
      return true;
    }
}

bool check_simple(size_t n, bool display_aut)
{
  automaton_t aut = factory(context, n);
  std::stringstream ss;
  ss << "simple automaton " << n;
  return idempotence(ss.str(), aut, display_aut);
}

int main()
{
  int exit = 0;
  exit |= check_simple(5, true);
  exit |= check_simple(10, false);

  auto ladybird = vcsn::ladybird<context_t>(4, context);
  auto determ_ladybird = vcsn::determinize(ladybird);

  exit |= idempotence("ladybird 4", ladybird, true);
  return exit;
}
