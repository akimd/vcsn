#include <cassert>
#include <string>
#include <iostream>
#include <vcsn/algos/dyn.hh>
#include <boost/lexical_cast.hpp>

int
main (int argc, char* const argv[])
{
  assert(argc == 3);
  std::string cname = argv[1];
  size_t n = boost::lexical_cast<size_t>(argv[2]);
  using namespace vcsn::dyn;
  context* ctx = make_context(cname, "ab");
  automaton a = de_bruijn(*ctx, n);
  print(a, std::cout, FileType::dotty) << "\n";
}
