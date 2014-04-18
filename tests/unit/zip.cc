#include <vcsn/misc/tuple.hh>
#include <vcsn/misc/zip.hh>

#include <iostream>
#include <list>
#include <vector>
#include <string>

template <typename... Args>
std::ostream& operator<<(std::ostream& o, std::tuple<Args...>& args)
{
  return vcsn::detail::print(o, args);
}

int main()
{
  std::vector<int> ints
    = { 1, 20, 30, 40 };
  std::list<double> floats
    = { 1.1, 2.22, 3.3, 4.4 };
  std::vector<std::string> strings
    = { "one", "deux", "three", "four" };

  for (auto i: vcsn::zip(ints, floats, strings))
    std::cout << '('
              << std::get<0>(i)
              << ", "
              << std::get<1>(i)
              << ", "
              << std::get<2>(i)
              << ')'
              << std::endl;
}
