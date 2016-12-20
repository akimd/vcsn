#include <vcsn/misc/yaml.hh>

namespace vcsn
{
  namespace dyn LIBVCSN_API
  {
    inline
    bool are_equal(const automaton& lhs, const automaton& rhs)
    {
      return compare(lhs, rhs) == 0;
    }

    inline
    bool are_equal(const expression& lhs, const expression& rhs)
    {
      return compare(lhs, rhs) == 0;
    }

    inline
    bool less_than(const automaton& lhs, const automaton& rhs)
    {
      return compare(lhs, rhs) < 0;
    }

    inline
    bool less_than(const expression& lhs, const expression& rhs)
    {
      return compare(lhs, rhs) < 0;
    }

    inline
    std::string configuration(const std::string& key)
    {
      return vcsn::configuration(key);
    }
  }
}
