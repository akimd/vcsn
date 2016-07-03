#pragma once

#include <vcsn/algos/detail/printer.hh>
#include <vcsn/algos/is-deterministic.hh>
#include <vcsn/dyn/fwd.hh>
#include <vcsn/weightset/fwd.hh> // b

namespace vcsn
{
  /*--------.
  | fado.   |
  `--------*/

  namespace detail
  {

    /// \brief Format an automaton into Fado.
    ///
    /// \tparam Aut an automaton type.
    template <Automaton Aut>
    class fado_impl: public printer<Aut>
    {
      static_assert(context_t_of<Aut>::is_lal || context_t_of<Aut>::is_lan,
                    "fado: requires letter or nullable labels");
      static_assert(std::is_same<weightset_t_of<Aut>, b>::value,
                    "fado: requires Boolean weights");

      using super_t = printer<Aut>;

      using super_t::aut_;
      using super_t::finals_;
      using super_t::initials_;
      using super_t::list_states_;
      using super_t::os_;
      using super_t::print_transitions_;

      using super_t::super_t;

    public:
      /// Actually output \a aut_ on \a os_.
      // http://www.dcc.fc.up.pt/~rvr/FAdoDoc/_modules/fa.html#saveToFile
      void operator()()
      {
        bool is_deter = is_deterministic_(aut_);
        os_ << (is_deter ? "@DFA" : "@NFA");
        list_states_(finals_());
        if (!is_deter)
          {
            os_ << " *";
            list_states_(initials_());
          }
        print_transitions_();
      }

    private:
      template <Automaton A>
      std::enable_if_t<labelset_t_of<A>::is_free(), bool>
      is_deterministic_(const A& a)
      {
        return vcsn::is_deterministic(a);
      }

      template <Automaton A>
      std::enable_if_t<!labelset_t_of<A>::is_free(), bool>
      is_deterministic_(const A&)
      {
        return false;
      }
    };
  }

  /// \brief Format automaton to FAdo format.
  ///
  /// \tparam Aut an automaton type.
  template <Automaton Aut>
  std::ostream&
  fado(const Aut& aut, std::ostream& out = std::cout)
  {
    auto fado = detail::fado_impl<Aut>{aut, out};
    fado();
    return out;
  }



  /*----------.
  | grailer.  |
  `----------*/

  namespace detail
  {
    /// \brief Print an automaton in Fado format.
    ///
    /// \tparam Aut an automaton type.
    ///
    /// See https://cs.uwaterloo.ca/research/tr/1993/01/93-01.pdf.
    template <Automaton Aut>
    class grail_impl: public printer<Aut>
    {
      static_assert(context_t_of<Aut>::is_lal || context_t_of<Aut>::is_lan,
                    "grail: requires letter or nullable labels");
      static_assert(std::is_same<weightset_t_of<Aut>, b>::value,
                    "grail: requires Boolean weights");

      using super_t = printer<Aut>;

      using typename super_t::automaton_t;
      using typename super_t::state_t;
      using typename super_t::transition_t;

      using super_t::aut_;
      using super_t::finals_;
      using super_t::initials_;
      using super_t::os_;
      using super_t::print_transitions_;

      using super_t::super_t;

    public:
      /// Actually output \a aut_ on \a os_.
      void operator()()
      {
        // Don't end with a \n.
        const char* sep = "";
        for (auto s: initials_())
          {
            os_ << sep
                << "(START) |- ";
            aut_->print_state(s, os_);
            sep = "\n";
          }
        print_transitions_();
        for (auto s: finals_())
          {
            os_ << '\n';
            aut_->print_state(s, os_) <<  " -| (FINAL)";
          }
      }
    };
  }

  /// \brief Print automaton in Grail format.
  ///
  /// \tparam Aut an automaton type.
  template <Automaton Aut>
  std::ostream&
  grail(const Aut& aut, std::ostream& out)
  {
    auto grail = detail::grail_impl<Aut>{aut, out};
    grail();
    return out;
  }
}
