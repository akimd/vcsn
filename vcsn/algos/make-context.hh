#ifndef VCSN_ALGOS_MAKE_CONTEXT_HH
# define VCSN_ALGOS_MAKE_CONTEXT_HH

# include <istream>
# include <map>
# include <set>
# include <sstream>

# include <vcsn/ctx/fwd.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/dyn/ratexpset.hh>
# include <vcsn/misc/stream.hh> // eat

namespace vcsn
{

  namespace detail
  {

    // Fwd decls.
    template <typename Ctx>
    Ctx make_context(std::istream& is);

    /*-----------------.
    | make_weightset.  |
    `-----------------*/

    /* Some contexts, such as "lal_char_br", use RatExps as weight set.
       But RatExps need a context, and a labelset.  Other weight sets,
       such as b or zmin, do not need a labelset to be buildable.

       So we must define a means to instantiate a weightset with or
       without an alphabet, depending on its need.

       We rely on partial template specialization to do so, which
       requires that we use a struct (named weightsetter).
     */

    template <typename WeightSet>
    struct weightsetter
    {
      static
      WeightSet
      make(std::istream& is)
      {
        std::string ws;
        char c;
        while (is >> c)
          {
            if (c == '>')
              {
                is.unget();
                break;
              }
            ws.append(1, c);
          }
        return {};
      }
    };

    template <typename Ctx>
    struct weightsetter<ratexpset<Ctx>>
    {
      static
      ratexpset<Ctx>
      make(std::istream& is)
      {
        // name is, for instance, "ratexpset<lal_char(abcd)_z>".
        std::string rs;
        char c;
        while (is >> c)
          {
            if (c == '<')
              {
                is.unget();
                break;
              }
            rs.append(1, c);
          }
        if (rs != "ratexpset")
          throw std::runtime_error("make_weightset: unexpected "
                                   + rs
                                   + " expected ratexpset");
        eat(is, '<');
        auto ctx = make_context<Ctx>(is);
        eat(is, '>');
        return {ctx};
      }
    };

    template <typename WeightSet>
    WeightSet
    make_weightset(std::istream& is)
    {
      return weightsetter<WeightSet>::make(is);
    }

    /*---------------.
    | make_context.  |
    `---------------*/

    template <typename Ctx>
    Ctx
    make_context(std::istream& is)
    {
      auto ls = Ctx::labelset_t::make(is);
      eat(is, '_');
      auto ws = make_weightset<typename Ctx::weightset_t>(is);
      return {ls, ws};
    }
  }


  template <typename Ctx>
  Ctx
  make_context(const std::string& name)
  {
    std::istringstream is{name};
    auto res = detail::make_context<Ctx>(is);
    std::string remainder;
    is >> remainder;
    if (res.vname(true) != name)
      throw std::runtime_error("make_context: built "
                               + res.vname(true)
                               + " instead of " + name);
    return res;
  }

  /*----------.
  | dynamic.  |
  `----------*/

  namespace dyn
  {
    namespace detail
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

      REGISTER_DECLARE(make_context,
                       (const std::string& name) -> context);

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

      REGISTER_DECLARE(make_ratexpset,
                       (const context& ctx) -> ratexpset);

    }
  }

} // vcsn::

#endif // !VCSN_ALGOS_MAKE_CONTEXT_HH
