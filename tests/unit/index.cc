#include <vcsn/misc/tuple.hh>

#include <iostream>
#include <map>
#include <string>

template <typename Int, Int... Ints>
std::ostream& operator<<(std::ostream& o,
                         const std::integer_sequence<Int, Ints...>&)
{
  return (o << ... << Ints);
}

int main()
{
  std::cout << vcsn::detail::make_index_range<10, 13>{} << '\n';
}
