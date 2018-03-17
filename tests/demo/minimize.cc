#include <iostream>
#include <stdexcept>
#include <string>

#include <vcsn/misc/raise.hh>
#include <vcsn/misc/stream.hh>
#include <vcsn/dyn/algos.hh>
#include <vcsn/dyn/context.hh>
#include <vcsn/dyn/automaton.hh>

// Read an automaton, and minimize it twice.

namespace dyn = vcsn::dyn;

static dyn::automaton read_automaton(const std::string& f)
{
  auto is = vcsn::open_input_file(f);
  return dyn::read_automaton(*is);
}

static void biminimize(const dyn::automaton& aut)
{
  auto m1 = dyn::minimize(aut);
  auto m2 = dyn::minimize(m1);
  dyn::print(m2, std::cout, "daut") << '\n';
}

int
main(int argc, const char* argv[])
try
  {
    vcsn::require(argc == 2,
                  "invalid number of arguments: ", argc);
    biminimize(read_automaton(argv[1]));
  }
 catch (const std::exception& e)
   {
     std::cerr << argv[0] << ": " << e.what() << '\n';
     exit(EXIT_FAILURE);
   }
 catch (...)
   {
     std::cerr << argv[0] << ": unknown exception caught\n";
     exit(EXIT_FAILURE);
   }
