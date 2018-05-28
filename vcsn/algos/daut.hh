#pragma once

#include <iostream>

#include <vcsn/algos/detail/printer.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/dyn/fwd.hh>

namespace vcsn
{

  namespace detail
  {
    /*-------------------------.
    | daut(automaton, stream). |
    `-------------------------*/

    /// \brief Format an automaton into Daut.
    ///
    /// \tparam Aut an automaton type.
    template <Automaton Aut>
    class daut_impl: public printer<Aut>
    {
    private:
      using super_t = printer<Aut>;
      using typename super_t::automaton_t;
      using typename super_t::state_t;
      using typename super_t::polynomial_t;
      using typename super_t::transition_t;

      using super_t::aut_;
      using super_t::os_;
      using super_t::ps_;


    public:
      daut_impl(const automaton_t& aut, std::ostream& out, format fmt = {})
        : super_t(aut, out)
        , format_(fmt)
      {}

      /// Print the automaton on the stream.
      std::ostream& operator()()
      {
        print_context_();
        print_transitions_();
        return os_;
      }

    private:
      void print_context_()
      {
        os_ << "context = ";
        aut_->context().print_set(os_, format(format::utf8).show_open(false));
      }

      /// Print the transitions between state \a src and state \a dst.
      void print_transitions_(const state_t src, const state_t dst,
                              const polynomial_t& entry)
      {
        os_ << '\n';
        if (src == aut_->pre())
          os_ << '$';
        else
          aut_->print_state(src, os_);
        os_ << " -> ";
        if (dst == aut_->post())
          os_ << '$';
        else
          aut_->print_state(dst, os_);

        auto e = to_string(ps_, entry, format_, ", ");
        if (!e.empty())
          os_ << ' ' << e;
      }

      /// Print all the transitions, sorted by src state, then dst state.
      void print_transitions_()
      {
        // For each src state, the destinations, sorted.
        auto dsts = std::map<state_t, polynomial_t>{};
        for (auto src : aut_->all_states())
          if (!aut_->is_lazy(src))
            {
              dsts.clear();
              for (auto t: all_out(aut_, src))
                // Bypass weight_of(set), because we know that the weight is
                // nonzero, and that there is only one weight per letter.
                ps_.new_weight(dsts[aut_->dst_of(t)],
                               aut_->label_of(t), aut_->weight_of(t));
              for (const auto& p: dsts)
                print_transitions_(src, p.first, p.second);
            }
      }

      /// Format for labels and weights.
      format format_ = {};
    };
  }

  /// Print an automaton in Daut format.
  ///
  /// \param aut     the automaton to print.
  /// \param out     the output stream.
  /// \param fmt     how to format the automaton.
  template <Automaton Aut>
  std::ostream&
  daut(const Aut& aut, std::ostream& out = std::cout, format fmt = {})
  {
    auto daut = detail::daut_impl<Aut>{aut, out, fmt};
    return daut();
  }
}
