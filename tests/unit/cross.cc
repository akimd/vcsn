#include <iostream>
#include <list>
#include <vector>
#include <string>

#include <vcsn/misc/tuple.hh>
#include <vcsn/misc/cross.hh>

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
  std::ostream& os = std::cout;

  auto ints = std::vector<int>           { 1,     20,      30,      40 };
  auto floats = std::list<double>        { 1.1,   2.22,    3.3,     4.4 };
  auto strings = std::vector<std::string>{ "one", "deux", "three", "four" };

  using vcsn::cross;
  os << "cross(ints, floats, strings)\n";
  for (auto i: cross(ints, floats, strings))
    os << i << '\n';
  os << "\n\n";

  os << "cross(ints)\n";
  for (auto i: cross(ints))
    os << i << '\n';
  os << "\n\n";

  os << "cross(cross(ints), cross(floats, strings))\n";
  for (auto i: cross(cross(ints), cross(floats, strings)))
    os << i << '\n';
  os << "\n\n";

  {
    auto empty = std::vector<int>{};
    os << "cross(ints, empty)\n";
    for (auto i: cross(ints, empty))
      os << i << '\n';
    os << "\n\n";

    os << "cross(empty, ints)\n";
    for (auto i: cross(empty, ints))
      os << i << '\n';
    os << "\n\n";

    os << "cross(int, empty, ints)\n";
    for (auto i: cross(ints, empty, ints))
      os << i << '\n';
  }

  // Make sure iterator and const_iterators are compatible.
  auto c = cross(ints, cross(floats, strings));
  auto i = c.begin();
  auto i_const = c.cbegin();
  i_const = i;
}
