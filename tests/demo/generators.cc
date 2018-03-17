#include <iostream> //std::cout

#include <vcsn/alphabets/char.hh> // type of letter
#include <vcsn/alphabets/setalpha.hh> // alphabet
#include <vcsn/labelset/letterset.hh> // letterset
#include <vcsn/labelset/tupleset.hh> // tupleset

/// Iterate over the generators of a tuple of labelsets.
int main()
{
  using namespace vcsn;

  // Basic alphabet type.
  using alphabet_t = set_alphabet<char_letters>;

  // Single-tape labelset.
  using labelset1_t = letterset<alphabet_t>;

  // Create the labelsets.
  const auto ls1 = labelset1_t{{'a', 'b', 'c'}};
  auto ls2 = labelset1_t{{'x', 'y'}};

  // Labelset (double-tape).
  using labelset_t = tupleset<labelset1_t, labelset1_t>;

  // Create the double-tape labelset.
  const auto ls = labelset_t{ls1, ls1};

  // Specify the syntax of printing labels.
  const auto fmt = vcsn::format{}.for_labels();

  const char* sep = "{ ";
  for (auto&& g: ls.generators())
    {
      std::cout << sep;
      ls.print(g, std::cout, fmt);
      sep = ", ";
    }
  std::cout << " }\n";
}
