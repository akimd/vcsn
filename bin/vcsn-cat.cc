#include <iostream>
#include <cassert>

#include <vcsn/algos/dyn.hh>
#include <vcsn/dyn/ratexp.hh>

#include <lib/vcsn/rat/driver.hh>

void
work_aut(const std::string& file)
{
  // Input.
  using namespace vcsn::dyn;
  vcsn::dyn::automaton aut = read_automaton_file(file);

  // Process.

  // Output.
  print(aut, std::cout) << std::endl;
}

void
work_exp(const std::string& ctx_name, const std::string& labelset,
         const std::string& file)
{
  // Input.
  using namespace vcsn::dyn;
  vcsn::dyn::context* ctx = make_context(ctx_name, labelset);
  vcsn::dyn::ratexp exp = read_ratexp_file(file, *ctx);

  // Process.

  // Output.
  print(exp, std::cout) << std::endl;
}

int main(int argc, char *const argv[])
try
  {
    assert(2 < argc);
    std::string opt = argv[1];
    argc -= 2;
    argv += 2;
    if (opt == "-a")
      {
        assert(1 == argc);
        work_aut(argv[0]);
      }
    else if (opt == "-e")
      {
        assert(3 == argc);
        work_exp(argv[0], argv[1], argv[2]);
      }
    else
      {
        std::cerr << argv[0] << ": invalid option: " << opt << std::endl;
        exit(EXIT_FAILURE);
      }
  }
 catch (const std::exception& e)
   {
     std::cerr << e.what() << std::endl;
     exit(EXIT_FAILURE);
   }
