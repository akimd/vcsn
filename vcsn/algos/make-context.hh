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

  /* Some contexts, such as "char_br_lal", use RatExps as weight set.
     But RatExps need a context, and a genset.  Other weight sets,
     such as b or zmin, do not need a genset to be buildable.

     So we must define a mean instantiate a weightset with or without
     an alphabet, depending on its need.

     We rely on partial template specialization to do so, which
     requires that we use a struct (named weightsetter).

     For some reason, the prototype of make_context is really needed
     to please the compiler, which needs it to compile weightsetter.
     */

  template <typename Ctx>
  Ctx*
  make_context(const typename Ctx::genset_t::letters_t& ls);

  template <typename WeightSet>
  struct weightsetter
  {
    static
    WeightSet
    // FIXME: Breaking the abstraction over set<char>.
    make(const std::set<char>&)
    {
      return {};
    }
  };

  template <typename Ctx>
  struct weightsetter<kratexpset<Ctx>>
  {
    static
    kratexpset<Ctx>
    make(const typename Ctx::genset_t::letters_t& ls)
    {
      return {*make_context<Ctx>(ls)};
    }
  };

  template <typename Ctx>
  Ctx*
  make_context(const typename Ctx::genset_t::letters_t& ls)
  {
    auto gs = typename Ctx::genset_t(ls);
    auto ws = weightsetter<typename Ctx::weightset_t>::make(ls);
    // FIXME: memory management.
    return new Ctx(gs, ws);
  }

  template <typename Ctx>
  ctx::abstract_context*
  abstract_make_context(const std::string& letters)
  {
    std::set<char> ls;
    for (auto l: letters)
      ls.insert(l);
    return make_context<Ctx>(ls);
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
