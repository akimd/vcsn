#pragma once

#include <algorithm>
#include <iostream>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <vcsn/algos/grail.hh> // outputter

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
    template <typename Aut>
    class tikzer: public outputter<Aut>
    {
    public:
      using super_t = outputter<Aut>;
      using typename super_t::automaton_t;
      using typename super_t::state_t;
      using typename super_t::transition_t;
      using typename super_t::weight_t;

      using super_t::aut_;
      using super_t::print_entry_;
      using super_t::os_;
      using super_t::ws_;

      using super_t::super_t;

      /// Format an initial/final weight.
      /// \param kind  the weight name (e.g., "initial").
      /// \param w     the weight
      void print_finitial_(const std::string& kind, const weight_t& w)
      {
        if (!ws_.is_zero(w))
          {
            os_ << ',' << kind;
            if (ws_.show_one() || !ws_.is_one(w))
              {
                os_ << ',' << kind << " text=$\\left\\langle ";
                ws_.print(w, os_, "latex") << "\\right\\rangle$";
              }
          }
      }

      void operator()()
      {
        os_ <<
          "\\documentclass{standalone}\n"
          "  \\usepackage{tikz}\n"
          "  \\usetikzlibrary{arrows.meta, automata, bend,"
          " positioning, shapes.misc}\n"
          "  \\tikzstyle{automaton}=[shorten >=1pt,"
          " >={Stealth[bend,round]}, initial text=]\n"
          "  \\tikzstyle{accepting}=[accepting by arrow]\n"
          "\n"
          "\\begin{document}\n"
          "\\begin{tikzpicture}[automaton, auto]\n"
          ;

        state_t prev = aut_->null_state();
        for (auto s : aut_->states())
          {
            os_ << "  \\node[state";
            print_finitial_("initial", aut_->get_initial_weight(s));
            print_finitial_("accepting", aut_->get_final_weight(s));
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
            aut_->print_state_name(s, os_, "latex");
            os_ << "$};\n";
            prev = s;
        }

        for (auto src : aut_->states())
          {
            std::set<state_t> ds;
            for (auto t: aut_->out(src))
              ds.insert(aut_->dst_of(t));
            for (auto dst: ds)
              {
                os_ << "  \\path[->] (";
                aut_->print_state(src, os_);
                os_ << ") edge"
                    << (src == dst ? "[loop above]" : "")
                    << " node"
                    << " {$";
                print_entry_(src, dst, os_, "latex");
                os_ << "$} (";
                aut_->print_state(dst, os_);
                os_ << ");\n";
              }
          }
        os_ <<
          "\\end{tikzpicture}\n"
          "\\end{document}";
      }
    };
  }

  /// \brief Print automaton to TikZ format.
  ///
  /// \tparam AutPtr an automaton type.
  template <typename AutPtr>
  std::ostream&
  tikz(const AutPtr& aut, std::ostream& out)
  {
    detail::tikzer<AutPtr> t{aut, out};
    t();
    return out;
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Ostream>
      std::ostream& tikz(const automaton& aut, std::ostream& out)
      {
        return tikz(aut->as<Aut>(), out);
      }
    }
  }
}
