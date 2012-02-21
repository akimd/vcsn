#include <vcsn/weights/b.hh>
#include <vcsn/weights/z.hh>
#include <vcsn/factory/ladybird.hh>
#include <vcsn/algos/dotty.hh>
#include <iostream>
#include <cstdlib>
#include <cstring>

void
syntax(char* argv0)
{
  const char* program = strrchr(argv0, '/');
  program = program ? program + 1 : argv0;
  std::cerr << "Usage: " << std::endl
	    << '\t' << program << " n" << std::endl;
  exit(1);
}


int
main(int argc, char** argv)
{
  if ((argc != 2 && argc != 3))
    syntax(argv[0]);

  int n = atoi(argv[1]);
  if (n <= 0)
    syntax(argv[0]);

  auto lb = vcsn::ladybird<vcsn:: W>(n);
  vcsn::dotty(lb, std::cout);
}
