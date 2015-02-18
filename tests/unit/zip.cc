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
    = { 1.1, 2.22, 3.3, 4.4, 5.5, 6.6 };
  std::vector<std::string> strings
    = { "one", "deux", "three", "four", "five" };

  using vcsn::zip;
  using vcsn::zip_with_padding;
  for (auto i: zip(ints))
    std::cout << i << std::endl;

  for (auto i: zip(ints, floats, strings))
    std::cout << i << std::endl;

  auto pad = std::make_tuple (0, 0.0, "zero", 'a');
  std::string msg = "test";
  for (auto i : vcsn::zip_with_padding(pad, ints, floats, strings, msg))
    std::cout << i << std::endl;


  for (auto i: zip(zip(ints), zip(floats, strings)))
    std::cout << i << std::endl;

  // Make sure iterator and const_iterators are compatible.
  auto c = zip(ints, zip(floats, strings));
  auto i = c.begin();
  auto i_const = c.cbegin();
  i_const = i;
}
