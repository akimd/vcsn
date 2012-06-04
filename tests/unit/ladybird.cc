#include <cstdlib>
#include <cstring>
#include <iostream>

#include <vcsn/algos/dotty.hh>
#include <vcsn/factory/ladybird.hh>
#include <vcsn/weights/b.hh>
#include <vcsn/weights/z.hh>
#include <vcsn/weights/z_min.hh>

void
syntax(const char* argv0)
{
  const char* prog = (prog = strrchr(argv0, '/')) ? prog + 1 : argv0;
  std::cerr << "Usage: " << std::endl
	    << '\t' << prog << " n" << std::endl;
  exit(1);
}

int
main(int argc, char const** argv)
{
  if (argc != 2 && argc != 3)
    syntax(argv[0]);

  int n = atoi(argv[1]);
  if (n <= 0)
    syntax(argv[0]);

  auto lb = vcsn::ladybird<vcsn:: W>(n);
  vcsn::dotty(lb, std::cout);
}
