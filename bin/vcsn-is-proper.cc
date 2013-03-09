#include <iostream>

#include <vcsn/dyn/algos.hh>
#include "parse-args.hh"

static bool
work_aut(const options& opts)
{
  using namespace vcsn::dyn;
  // Input.
  auto aut = read_automaton(opts);

  // Process.
  bool res = is_proper(aut);

  // Output.
  std::cout << (res ? "true" : "false") << std::endl;
  return res ? 0 : 2;
}

static bool
work_exp(const options&)
{
  throw std::runtime_error("not implemented for rational expressions");
}

int main(int argc, char* const argv[])
{
  options opts;
  try
    {
      opts = parse_args(argc, argv);
      return opts.is_automaton ? work_aut(opts) : work_exp(opts);
    }
  catch (const std::exception& e)
    {
      std::cerr << opts.program << ": " << e.what() << std::endl;
      exit(EXIT_FAILURE);
    }
  catch (...)
    {
      std::cerr << opts.program << ": unknown exception caught" << std::endl;
      exit(EXIT_FAILURE);
    }
}
