#include <vcsn/misc/tuple.hh>
#include <vcsn/misc/zip-maps.hh>

#include <iostream>
#include <map>
#include <string>

template <typename... Args>
std::ostream& operator<<(std::ostream& o, std::tuple<Args...>& args)
{
  return vcsn::detail::print(args, o);
}

int main()
{
  auto ints = std::map<char, int>
    { {'1', 1},
      {'2', 20},
      {'3', 30},
      {'4', 40} };
  auto floats = std::map<char, double>
    { // {'1', 1.1},
      {'2', 2.22},
      {'3', 3.3}
      //, {4, 4.4}
    };
  auto strings = std::map<char, std::string>
    { {'1', "one"},
      {'2', "deux"},
      {'3', "three"},
      {'4', "four"} };

  for (const auto& i: vcsn::zip_maps<vcsn::as_tuple>(ints, floats, strings))
    std::cout << std::get<0>(i).first << ':'
              << '('
              << std::get<0>(i).second
              << ", "
              << std::get<1>(i).second
              << ", "
              << std::get<2>(i).second
              << ")\n";
}
