#ifndef VCSN_ALGOS_XDOT_HH
# define VCSN_ALGOS_XDOT_HH

# include <vcsn/algos/copy.hh>
# include <vcsn/core/xdot-automaton.hh>
# include <vcsn/dyn/automaton.hh> // dyn::make_automaton

namespace vcsn
{
  namespace detail
  {

    template <typename Aut>
    xdot_automaton<Aut>
    inject_xdot(const Aut& a)
    {
      std::cerr << "OK-A 100 vcsn::detail::inject_xdot\n";
      auto res =
        make_shared_ptr<xdot_automaton<Aut>>(a);
      vcsn::copy_into(a, res);

      // FIXME: Alfred: also initialize the associated data.

      for (auto s: res->all_states())
        std::cerr << "Has state " << s << " a name? "
                  << res->state_has_name(s)
                  << "\n";

      return res;
    }

  } // namespace detail

  /*--------------.
  | inject_xdot.  |
  `--------------*/

  template <typename Aut>
  inline
  auto
  inject_xdot(const Aut& a)
    -> xdot_automaton<Aut>
  {
    std::cerr << "OK-A 100 vcsn::inject_xdot\n";
    return detail::inject_xdot(a);
  }

  namespace dyn
  {
    namespace detail
    {

      /*------------------.
      | dyn::inject_xdot  |
      `------------------*/

      template <typename Aut>
      inline
      automaton
      inject_xdot(const automaton& aut)
      {
        std::cerr << "OK-A 100 vcsn::dyn::detail::inject_xdot\n";
        const auto& a = aut->as<Aut>();
        return make_automaton(::vcsn::inject_xdot(a));
      }

      REGISTER_DECLARE(inject_xdot,
                       (const automaton&) -> automaton);

    } // namespace detail
  } // namespace dyn
} // namespace vcsn

#endif // !VCSN_ALGOS_XDOT_HH
