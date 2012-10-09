#ifndef VCSN_ALGOS_MAKE_CONTEXT_HH
# define VCSN_ALGOS_MAKE_CONTEXT_HH

# include <map>
# include <set>

# include <vcsn/core/fwd.hh>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/core/rat/abstract_kratexpset.hh>

namespace vcsn
{

  /*---------------.
  | make_context.  |
  `---------------*/

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


  /*------------------.
  | make_kratexpset.  |
  `------------------*/

  template <typename Ctx>
  abstract_kratexpset*
  abstract_make_kratexpset(const ctx::abstract_context& ctx)
  {
    return new concrete_abstract_kratexpset<Ctx>(dynamic_cast<const Ctx&>(ctx));
  }

  using make_kratexpset_t =
    auto (const ctx::abstract_context& ctx) -> abstract_kratexpset*;

  bool
  make_kratexpset_register(const std::string& ctx,
                           const make_kratexpset_t& fn);

  /// Abstract.
  abstract_kratexpset*
  make_kratexpset(const ctx::abstract_context& ctx);

} // vcsn::

#endif // !VCSN_ALGOS_EDIT_AUTOMATON_HH
