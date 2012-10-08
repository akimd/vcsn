#ifndef VCSN_ALGOS_MAKE_CONTEXT_HH
# define VCSN_ALGOS_MAKE_CONTEXT_HH

# include <map>
# include <vcsn/core/fwd.hh>
# include <vcsn/ctx/fwd.hh>

namespace vcsn
{

  template <typename Ctx>
  ctx::abstract_context*
  abstract_make_context(const std::string& letters)
  {
    std::set<char> ls;
    for (auto l: letters)
      ls.insert(l);
    auto* res = new Ctx{ls};
    return res;
  }

  using make_context_t =
    auto (const std::string& gens) -> ctx::abstract_context*;
  
  bool
  make_context_register(const std::string& ctx,
                        const make_context_t& fn);
  
  /// Abstract.
  ctx::abstract_context*
  make_context(const std::string& name, const std::string& gens);

} // vcsn::

#endif // !VCSN_ALGOS_EDIT_AUTOMATON_HH
