#ifndef VCSN_ALGOS_MAKE_CONTEXT_HH
# define VCSN_ALGOS_MAKE_CONTEXT_HH

# include <map>
# include <set>

# include <vcsn/core/fwd.hh>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/core/rat/abstract_ratexpset.hh>

namespace vcsn
{

  /*---------------.
  | make_context.  |
  `---------------*/

  /* Some contexts, such as "lal_char_br", use RatExps as weight set.
     But RatExps need a context, and a labelset.  Other weight sets,
     such as b or zmin, do not need a labelset to be buildable.

     So we must define a mean instantiate a weightset with or without
     an alphabet, depending on its need.

     We rely on partial template specialization to do so, which
     requires that we use a struct (named weightsetter).

     For some reason, the prototype of make_context is really needed
     to please the compiler, which needs it to compile weightsetter.
     */

  template <typename Ctx>
  Ctx*
  make_context(const typename Ctx::labelset_t::letters_t& ls);

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
  struct weightsetter<ratexpset<Ctx>>
  {
    static
    ratexpset<Ctx>
    make(const typename Ctx::labelset_t::letters_t& ls)
    {
      return {*make_context<Ctx>(ls)};
    }
  };

  template <typename Ctx>
  Ctx*
  make_context(const typename Ctx::labelset_t::letters_t& ls)
  {
    auto gs = typename Ctx::labelset_t(ls);
    auto ws = weightsetter<typename Ctx::weightset_t>::make(ls);
    // FIXME: memory management.
    return new Ctx(gs, ws);
  }


  /*----------.
  | dynamic.  |
  `----------*/

  namespace dyn
  {
    namespace details
    {
      /*---------------.
      | make_context.  |
      `---------------*/

      template <typename Ctx>
      dyn::context*
      make_context(const std::string& letters)
      {
        std::set<char> ls;
        for (auto l: letters)
          ls.insert(l);
        return vcsn::make_context<Ctx>(ls);
      }

      using make_context_t = auto (const std::string& gens) -> context*;

      bool
      make_context_register(const std::string& ctx,
                            const make_context_t& fn);

      /*-----------------.
      | make_ratexpset.  |
      `-----------------*/

      template <typename Ctx>
      abstract_ratexpset*
      make_ratexpset(const dyn::context& ctx)
      {
        return new concrete_abstract_ratexpset<Ctx>
          (dynamic_cast<const Ctx&>(ctx));
      }

      using make_ratexpset_t = auto (const context& ctx) -> abstract_ratexpset*;

      bool
      make_ratexpset_register(const std::string& ctx,
                              const make_ratexpset_t& fn);

    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_EDIT_AUTOMATON_HH
