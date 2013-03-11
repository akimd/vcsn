#ifndef VCSN_ALGOS_MAKE_CONTEXT_HH
# define VCSN_ALGOS_MAKE_CONTEXT_HH

# include <map>
# include <regex>
# include <set>

# include <vcsn/dyn/fwd.hh>
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

     So we must define a means to instantiate a weightset with or
     without an alphabet, depending on its need.

     We rely on partial template specialization to do so, which
     requires that we use a struct (named weightsetter).

     For some reason, the prototype of make_context is really needed
     to please the compiler, which needs it to compile weightsetter.
     */

  template <typename Ctx>
  Ctx
  make_context(const std::string& name);

  namespace details
  {
    template <typename WeightSet>
    struct weightsetter
    {
      static
      WeightSet
      make(const std::string&)
      {
        return {};
      }
    };

    template <typename Ctx>
    struct weightsetter<ratexpset<Ctx>>
    {
      static
      ratexpset<Ctx>
      make(const std::string& name)
      {
        // name is, for instance, "ratexpset<lal_char(abcd)_z>".
        assert(name.substr(0, 10) == "ratexpset<");
        assert(name.substr(name.size() - 1, 1) == ">");
        std::string ctx = name.substr(10, name.size() - 11);
        return {make_context<Ctx>(ctx)};
      }
    };

    template <typename Ctx>
    typename std::enable_if<Ctx::is_lau, Ctx>::type
    make_context(const std::string& name)
    {
      // name: lal_ratexpset<law_char(xyz)_b>
      //       ^^^ ^^^^^^^^^^^^^^^^^^^^^^^^^^
      //       kind         weightset
      //
      // There is no "char(...)_".
      std::string kind = name.substr(0, 3);

      if (kind != "lau")
        throw std::runtime_error("make_context: Ctx::is_lau but read "
                                 + kind + ": " + name);

      if (name[3] != '_')
        throw std::runtime_error("make_context: expected a '_' after " + kind
                                 + ": " + name);

      std::string weightset = name.substr(4);
      auto ws = weightsetter<typename Ctx::weightset_t>::make(weightset);
      Ctx res(typename Ctx::labelset_t{}, ws);
      assert(res.vname(true) == name);
      return res;
    }

    template <typename Ctx>
    typename std::enable_if<Ctx::is_lal || Ctx::is_lan || Ctx::is_law,
                            Ctx>::type
    make_context(const std::string& name)
    {
      // name: lal_char(abc)_ratexpset<law_char(xyz)_b>.
      //       ^^^ ^^^^ ^^^  ^^^^^^^^^^^^^^^^^^^^^^^^^
      //        |   |    |        weightset
      //        |   |    +-- gens
      //        |   +-- labelset
      //        +-- kind
      //
      // FIXME: regex is not usable in G++ 4.8 yet.
      std::string kind = name.substr(0, 3);

      if ((Ctx::is_lan && kind != "lan")
          || (Ctx::is_lal && kind != "lal")
          || (Ctx::is_law && kind != "law"))
        {
          std::string ctxk =
            Ctx::is_lal ? "lal"
            : Ctx::is_lan ? "lan"
            : Ctx::is_law ? "law"
            : "invalid Context Kind";
          throw std::runtime_error("make_context: Ctx::is_" + ctxk
                                   + " but read " + kind + ": " + name);
        }

      if (name[3] != '_')
        throw std::runtime_error("make_context: expected a '_' after " + kind
                                 + ": " + name);

      auto lparen = name.find('(');
      auto rparen = name.find(')');
      if (lparen == std::string::npos)
        throw std::runtime_error("make_context: missing '(': "
                                 + name);
      std::string labelset = name.substr(4, lparen - 4);
      std::string genset = name.substr(lparen + 1, rparen - lparen - 1);
      std::string weightset = name.substr(rparen + 2);

      typename Ctx::labelset_t::letters_t ls(begin(genset), end(genset));
      auto gs = typename Ctx::labelset_t(ls);
      auto ws = weightsetter<typename Ctx::weightset_t>::make(weightset);
      Ctx res(gs, ws);
      assert(res.vname(true) == name);
      return res;
    }

  }


  template <typename Ctx>
  Ctx
  make_context(const std::string& name)
  {
    return details::make_context<Ctx>(name);
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
      context
      make_context(const std::string& name)
      {
        return std::make_shared<Ctx>(vcsn::make_context<Ctx>(name));
      }

      using make_context_t = auto (const std::string& name) -> context;

      bool
      make_context_register(const std::string& ctx,
                            const make_context_t& fn);

      /*-----------------.
      | make_ratexpset.  |
      `-----------------*/

      template <typename Ctx>
      ratexpset
      make_ratexpset(const context& ctx)
      {
        return std::make_shared<concrete_abstract_ratexpset<Ctx>>
          (dynamic_cast<const Ctx&>(*ctx));
      }

      using make_ratexpset_t = auto (const context& ctx) -> ratexpset;

      bool
      make_ratexpset_register(const std::string& ctx,
                              const make_ratexpset_t& fn);

    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_MAKE_CONTEXT_HH
