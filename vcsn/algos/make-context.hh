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

    /*---------------.
    | make_context.  |
    `---------------*/

    template <typename Ctx>
    Ctx
    make_context(std::istream& is)
    {
      auto ls = Ctx::labelset_t::make(is);
      eat(is, '_');
      auto ws = Ctx::weightset_t::make(is);
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
