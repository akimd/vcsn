#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/dotty.hh>
#include <map>

namespace vcsn
{
  using dotty_map_t = std::map<std::string, dotty_t*>;
  dotty_map_t& map()
  {
    static dotty_map_t instance;
    return instance;
  }

  bool dotty_register(const std::string& ctx, const dotty_t& fn)
  {
    map()[ctx] = fn;
    return true;
  }

  void
  dotty(const abstract_mutable_automaton& aut, std::ostream& out)
  {
    std::string ctx = aut.abstract_context().name();
    auto i = map().find(ctx);
    if (i == map().end())
      throw std::runtime_error("dotty: no implementation available for " + ctx);
    else
      (i->second)(aut, out);
  }
}
