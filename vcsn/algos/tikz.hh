#pragma once

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <vcsn/algos/grail.hh> // printer

#include <vcsn/dyn/fwd.hh>

namespace vcsn
{

  /*--------------------------.
  | tikz(automaton, stream).  |
  `--------------------------*/

  namespace detail
  {
    /// \brief Format automaton to TikZ format.
    ///
    /// \tparam Aut an automaton type, not a pointer type.
    template <Automaton Aut>
    class tikzer: public printer<Aut>
    {
    public:
      using super_t = printer<Aut>;
      using typename super_t::automaton_t;
      using typename super_t::state_t;
      using typename super_t::polynomial_t;
      using typename super_t::transition_t;
      using typename super_t::weight_t;

      using super_t::aut_;
      using super_t::os_;
      using super_t::ps_;
      using super_t::ws_;

      using super_t::super_t;

      /// Format an initial/final weight.
      /// \param kind  the weight name (e.g., "initial").
      /// \param w     the weight
      void print_finitial_(const std::string& kind, const weight_t& w) const
      {
        if (!ws_.is_zero(w))
          {
            os_ << ',' << kind;
            if (ws_.show_one() || !ws_.is_one(w))
              {
                os_ << ',' << kind << " text=$\\left\\langle ";
                ws_.print(w, os_, format::latex) << "\\right\\rangle$";
              }
          }
      }

      void operator()()
      {
        os_ <<
          "\\documentclass{standalone}\n"
          "  \\usepackage{tikz}\n"
          "  \\usetikzlibrary{arrows.meta, automata, bending,"
          " positioning, shapes.misc}\n"
          "  \\tikzstyle{automaton}=[shorten >=1pt,"
          " >={Stealth[bend,round]}, initial text=]\n"
          "  \\tikzstyle{accepting}=[accepting by arrow]\n"
          "\n"
          "\\begin{document}\n"
          "\\begin{tikzpicture}[automaton, auto]\n"
          ;

        print_states_();
        print_transitions_();

        os_ <<
          "\\end{tikzpicture}\n"
          "\\end{document}";
      }

    private:
      /// Pretty-print states.
      void print_states_() const
      {
        state_t prev = aut_->null_state();
        for (auto s : aut_->states())
          {
            os_ << "  \\node[state";
            if (aut_->is_lazy(s))
              os_ << ",dashed";
            else
              {
                print_finitial_("initial", aut_->get_initial_weight(s));
                print_finitial_("accepting", aut_->get_final_weight(s));
              }
            if (aut_->state_has_name(s))
              os_ << ",rounded rectangle";
            os_ << "] (";
            aut_->print_state(s, os_);
            os_ << ')';
            if (prev != aut_->null_state())
              {
                os_ << " [right=of ";
                aut_->print_state(prev, os_);
                os_ << ']';
              }
            os_ << " {$";
            aut_->print_state_name(s, os_, format::latex);
            os_ << "$};\n";
            prev = s;
        }
      }

      /// Print the transitions between state \a src and state \a dst.
      void print_transitions_(const state_t src, const state_t dst,
                              const polynomial_t& entry) const
      {
        os_ << "  \\path[->] (";
        aut_->print_state(src, os_);
        os_ << ") edge"
            << (src == dst ? "[loop above]" : "")
            << " node"
            << " {$";
        ps_.print(entry, os_, format::latex, ", ");
        os_ << "$} (";
        aut_->print_state(dst, os_);
        os_ << ");\n";
      }

      /// Print all the transitions, sorted by src state, then dst state.
      void print_transitions_()
      {
        // For each src state, the destinations, sorted.
        std::map<state_t, polynomial_t> dsts;
        for (auto src : aut_->states())
          if (!aut_->is_lazy(src))
            {
              dsts.clear();
              for (auto t: out(aut_, src))
                // Bypass weight_of(set), because we know that the weight is
                // nonzero, and that there is only one weight per letter.
                ps_.new_weight(dsts[aut_->dst_of(t)],
                               aut_->label_of(t), aut_->weight_of(t));
              for (const auto& p: dsts)
                print_transitions_(src, p.first, p.second);
            }
      }
    };
  }

  /// \brief Print automaton to TikZ format.
  ///
  /// \tparam AutPtr an automaton type.
  template <Automaton AutPtr>
  std::ostream&
  tikz(const AutPtr& aut, std::ostream& out)
  {
    detail::tikzer<AutPtr> t{aut, out};
    t();
    return out;
  }
}
