#include <vcsn/misc/tuple.hh>
#include <vcsn/misc/zip-maps.hh>

#include <iostream>
#include <map>
#include <string>

template <typename... Args>
std::ostream& operator<<(std::ostream& o, std::tuple<Args...>& args)
{
  return vcsn::detail::print(o, args);
}

int main()
{

  std::multimap<char, int> ints
    = { {'1', 1},
        {'2', 20},
        {'2', 200},
        {'2', 2000},
        {'3', 30},
        {'4', 40} };
  std::multimap<char, double> floats
    = { // {'1', 1.1},
        {'2', 2.2},
        {'2', 2.22},
        {'3', 3.3}
        //, {4, 4.4}
  };
  std::multimap<char, std::string> strings
    = { {'1', "one"},
        {'2', "two"},
        {'2', "deux"},
        {'3', "three"},
        {'4', "four"} };

  for (auto i: vcsn::zip_maps(ints, floats, strings))
    std::cout << i.first << ':' << i.second << std::endl;
}
