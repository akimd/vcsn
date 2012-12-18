#include <iostream>
#include <vcsn/algos/dyn.hh>

int
main (int argc, char* const argv[])
{
  using namespace vcsn::dyn;
  for (int i = 1; i < argc; ++i)
    {
      automaton a = read_automaton_file(argv[i]);
      automaton d = determinize(a);
      dotty(d, std::cout);
    }
}
