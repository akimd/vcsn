#include <algorithm> // min
#include <chrono>
#include <iostream>
#include <limits>

#include <boost/format.hpp>
#include <boost/range/irange.hpp>

#include <vcsn/ctx/lal_char_b.hh>

int main()
{
  auto ctx = vcsn::ctx::lal_char_b();
  // vcsn:: automaton.
  auto aut = vcsn::make_mutable_automaton(ctx);
  // dyn:: automaton.
  auto a = vcsn::dyn::automaton(aut);

  using clock = std::chrono::steady_clock;

  // Best of three.
  using duration_t = std::chrono::duration<double, std::milli>;
  duration_t d(std::numeric_limits<double>::max());
  for (auto run: boost::irange(0, 3))
  {
    (void) run;
    clock::time_point start = clock::now();
    for (auto i: boost::irange(0, 5 * 1000 * 1000))
      {
        (void) i;
        vcsn::dyn::is_proper(a);
      }
    duration_t t = clock::now() - start;
    d = std::min(d, t);
  };

  std::cout << d.count() << "ms\n";
}
