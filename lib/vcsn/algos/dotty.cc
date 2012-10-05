#include <vcsn/core/mutable_automaton.hh>
#include <vcsn/algos/dotty.hh>
#include <map>

namespace vcsn
{
  using dotty_stream_map_t = std::map<std::string, dotty_stream_t*>;
  dotty_stream_map_t& dotty_stream_map()
  {
    static dotty_stream_map_t instance;
    return instance;
  }

  bool dotty_register(const std::string& ctx, const dotty_stream_t& fn)
  {
    dotty_stream_map()[ctx] = fn;
    return true;
  }

  void
  dotty(const abstract_mutable_automaton& aut, std::ostream& out)
  {
    std::string ctx = aut.abstract_context().name();
    auto i = dotty_stream_map().find(ctx);
    if (i == dotty_stream_map().end())
      throw std::runtime_error("dotty: no implementation available for " + ctx);
    else
      (i->second)(aut, out);
  }

  // FIXME: Code duplication.
  using dotty_string_map_t = std::map<std::string, dotty_string_t*>;
  dotty_string_map_t& dotty_string_map()
  {
    static dotty_string_map_t instance;
    return instance;
  }

  bool dotty_register(const std::string& ctx, const dotty_string_t& fn)
  {
    dotty_string_map()[ctx] = fn;
    return true;
  }

  std::string
  dotty(const abstract_mutable_automaton& aut)
  {
    std::string ctx = aut.abstract_context().name();
    auto i = dotty_string_map().find(ctx);
    if (i == dotty_string_map().end())
      throw std::runtime_error("dotty: no implementation available for " + ctx);
    else
      (i->second)(aut);
  }
}
