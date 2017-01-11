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

    /// \brief Format an automaton into FAdo.
    ///
    /// \tparam Aut an automaton type.
    template <Automaton Aut>
    class fado_impl: public printer<Aut>
    {
      static_assert(context_t_of<Aut>::is_lal || context_t_of<Aut>::is_lat,
                    "fado: requires tuple or letter labels");
      static_assert(std::is_same<weightset_t_of<Aut>, b>::value,
                    "fado: requires Boolean weights");

      using super_t = printer<Aut>;

      using super_t::aut_;
      using super_t::finals_;
      using super_t::initials_;
      using super_t::is_transducer_;
      using super_t::list_states_;
      using super_t::ls_;
      using super_t::os_;
      using super_t::print_transitions_;

      using super_t::super_t;
      using typename super_t::transition_t;

    public:
      /// Actually output \a aut_ on \a os_.
      // http://www.dcc.fc.up.pt/~rvr/FAdoDoc/_modules/fio.html#saveToFile
      void operator()()
      {
        bool is_deter = is_deterministic_(aut_) && is_proper_(aut_);
        os_ << (is_transducer_ ? "@Transducer" : is_deter ? "@DFA" : "@NFA");
        list_states_(finals_());
        if (!is_deter)
          {
            os_ << " *";
            list_states_(initials_());
          }
        print_transitions_();
      }

    private:
      template <typename LS>
      void print_label_(const LS& ls, const typename LS::value_t& l) const
      {
        if (ls.is_one(l))
          os_ << "@epsilon";
        else
          ls.print(l, os_, format::raw);
      }

      /// Acceptor.
      template <typename Label>
      void print_label_(const Label& l, std::false_type) const
      {
        print_label_(ls_, l);
      }

      /// Two-tape automaton.
      template <typename Label>
      void print_label_(const Label& l, std::true_type) const
      {
        print_label_(ls_.template set<0>(), std::get<0>(l));
        os_ << ' ';
        print_label_(ls_.template set<1>(), std::get<1>(l));
      }

      void print_transition_(const transition_t t) const override
      {
        aut_->print_state(aut_->src_of(t), os_);
        os_ << ' ';
        print_label_(aut_->label_of(t), is_transducer_);
        os_ << ' ';
        aut_->print_state(aut_->dst_of(t), os_);
      }

      template <Automaton A>
      bool is_deterministic_(const A& a)
      {
        return vcsn::is_deterministic(a);
      }

      template <Automaton A>
      bool is_proper_(const A& a)
      {
        return vcsn::is_proper(a);
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
      static_assert(context_t_of<Aut>::is_lal,
                    "grail: requires letter labels");
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
