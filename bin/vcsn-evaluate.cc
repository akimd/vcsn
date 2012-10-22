#include <cassert>
#include <iostream>
#include <vcsn/algos/dyn.hh>

int
main (int argc, char* const argv[])
{
  assert(argc == 3);
  using namespace vcsn::dyn;
  automaton a = parse_file(argv[1]);
  std::string s = argv[2];
  std::cout << eval(a, s) << std::endl;
}
