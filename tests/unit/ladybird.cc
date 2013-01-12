#include <cstdlib>
#include <cstring>
#include <iostream>

#include <vcsn/algos/dot.hh>
// CTX(WeightSet) => lal_char_WeightSet.
#define CTX(WS) CTX_(WS)
// Two layers to force the evaluation of W.
#define CTX_(WS) lal_char_ ## WS
#define HEADER(WS) <vcsn/ctx/CTX(W).hh>
#include HEADER(W)
#include <vcsn/factory/ladybird.hh>

void
syntax(const char* argv0)
{
  std::cerr << "Usage: " << std::endl
            << '\t' << argv0 << " n" << std::endl;
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

  vcsn::ctx::CTX(W) ctx{{'a', 'b', 'c'}};
  auto lb = vcsn::ladybird(n, ctx);
  std::cout << vcsn::dot(lb) << '\n';
}
