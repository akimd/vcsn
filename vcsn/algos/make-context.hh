#ifndef VCSN_ALGOS_MAKE_CONTEXT_HH
# define VCSN_ALGOS_MAKE_CONTEXT_HH

# include <istream>
# include <map>
# include <set>
# include <sstream>

# include <vcsn/ctx/fwd.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/dyn/ratexpset.hh>
# include <vcsn/misc/escape.hh>

namespace vcsn
{

  namespace detail
  {

    // Fwd decls.
    template <typename Ctx>
    typename std::enable_if<Ctx::is_lao, Ctx>::type
    make_context(std::istream& is);

    template <typename Ctx>
    typename std::enable_if<Ctx::is_lal || Ctx::is_lan || Ctx::is_law,
                            Ctx>::type
    make_context(std::istream& is);

    // Tools.
    namespace
    {
      inline
      void eat(std::istream& is, char c)
      {
        if (is.peek() != c)
          throw std::runtime_error("make_context: unexpected: "
                                   + str_escape(is.peek())
                                   + ": expected " + str_escape(c));
        is.ignore();
      }
    }

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
    typename std::enable_if<Ctx::is_lao, Ctx>::type
    make_context(std::istream& is)
    {
      // name: lao_ratexpset<law_char(xyz)_b>
      //       ^^^ ^^^^^^^^^^^^^^^^^^^^^^^^^^
      //       kind         weightset
      //
      // There is no "char(...)_".
      char kind[4];
      is.get(kind, sizeof kind);
      if (Ctx::kind_t::sname() != kind)
        throw std::runtime_error("make_context: Ctx::is_" + Ctx::kind_t::sname()
                                 + " but read " + kind);
      eat(is, '_');
      auto ws = make_weightset<typename Ctx::weightset_t>(is);
      Ctx res(typename Ctx::labelset_t{}, ws);
      return res;
    }

    template <typename Ctx>
    typename std::enable_if<Ctx::is_lal || Ctx::is_lan || Ctx::is_law,
                            Ctx>::type
    make_context(std::istream& is)
    {
      // name: lal_char(abc)_ratexpset<law_char(xyz)_b>.
      //       ^^^ ^^^^ ^^^  ^^^^^^^^^^^^^^^^^^^^^^^^^
      //        |   |    |        weightset
      //        |   |    +-- gens
      //        |   +-- letter_type
      //        +-- kind
      //
      // If more complexe "parsing" is needed, consider regex.
      // See vcsn/misc/regex.hh.
      char kind[4];
      is.get(kind, sizeof kind);
      if (Ctx::kind_t::sname() != kind)
        throw std::runtime_error("make_context: Ctx::is_" + Ctx::kind_t::sname()
                                 + " but read " + kind);
      eat(is, '_');
      std::string letter_type;
      {
        char c;
        while (is >> c)
          {
            if (c == '(')
              {
                is.unget();
                break;
              }
            letter_type.append(1, c);
          }
      }
      // The list of generators (letters).
      std::string gens;
      {
        eat(is, '(');
        char l;
        while (is >> l)
          {
            if (l == ')')
              break;
            gens.append(1, l);
          }
      }
      typename Ctx::labelset_t::letters_t ls(begin(gens), end(gens));
      auto gs = typename Ctx::labelset_t(ls);
      eat(is, '_');
      auto ws = make_weightset<typename Ctx::weightset_t>(is);
      Ctx res(gs, ws);
      return res;
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
