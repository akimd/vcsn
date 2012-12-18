#include <iostream>
#include <vcsn/algos/dyn.hh>

int
main (int argc, char* const argv[])
{
  assert(argc == 2);
  using namespace vcsn::dyn;
  automaton a = read_automaton_file(argv[1]);
  automaton d = lift(a);
  print(d, std::cout);
}
