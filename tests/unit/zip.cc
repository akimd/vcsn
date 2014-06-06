#include <iostream>
#include <list>
#include <vector>
#include <string>

#include <vcsn/misc/tuple.hh>
#include <vcsn/misc/zip.hh>

namespace std
{
  template <typename... Args>
  ostream& operator<<(ostream& o, const tuple<Args...>& args)
  {
    return vcsn::detail::print(args, o);
  }
}

int main()
{
  std::vector<int> ints
    = { 1, 20, 30, 40 };
  std::list<double> floats
    = { 1.1, 2.22, 3.3, 4.4 };
  std::vector<std::string> strings
    = { "one", "deux", "three", "four" };

  using vcsn::zip;
  for (auto i: zip(ints, floats, strings))
    std::cout << i << std::endl;

  for (auto i: zip(zip(ints), zip(floats, strings)))
    std::cout << i << std::endl;

  // Make sure iterator and const_iterators are compatible.
  auto c = zip(ints, zip(floats, strings));
  auto i = c.begin();
  auto i_const = c.cbegin();
  i_const = i;
}
