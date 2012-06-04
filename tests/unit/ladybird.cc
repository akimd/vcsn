#include <cstdlib>
#include <cstring>
#include <iostream>

#include <vcsn/algos/dotty.hh>
#include <vcsn/ctx/char.hh>
#include <vcsn/factory/ladybird.hh>

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

  vcsn::ctx::char_<vcsn::W> ctx;
  auto lb = vcsn::make_ladybird(n, ctx);
  vcsn::dotty(lb, std::cout);
}
