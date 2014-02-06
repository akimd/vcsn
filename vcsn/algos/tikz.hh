#ifndef VCSN_ALGOS_TIKZ_HH
# define VCSN_ALGOS_TIKZ_HH

# include <algorithm>
# include <iostream>
# include <unordered_map>
# include <unordered_set>
# include <vector>

// FIXME: factor dot and tikz.
# include <vcsn/algos/grail.hh> // outputter
# include <vcsn/algos/dot.hh> // format_entry

# include <vcsn/dyn/fwd.hh>

namespace vcsn
{

  /*--------------------------.
  | tikz(automaton, stream).  |
  `--------------------------*/

  namespace detail
  {
    template <typename Aut>
    class tikzer: public outputter<Aut>
    {
    public:
      using super_type = outputter<Aut>;
      using typename super_type::automaton_t;
      using typename super_type::state_t;
      using typename super_type::transition_t;
      using typename super_type::weight_t;

      using super_type::aut_;
      using super_type::format_entry_;
      using super_type::os_;
      using super_type::states_;
      using super_type::ws_;

      using super_type::super_type;

      /// Format a TikZ attribute.
      /// \param kind  the attribute name (e.g., "initial").
      /// \param w     the associated weight (e.g., initial weight).
      void format(const std::string& kind, const weight_t& w)
      {
        if (!ws_.is_zero(w))
          {
            os_ << ',' << kind;
            if (ws_.show_one() || !ws_.is_one(w))
              {
                os_ << ',' << kind << " text=$";
                ws_.print(os_, w) << '$';
              }
          }
      }

      void operator()()
      {
        os_ <<
          "\\documentclass{standalone}\n"
          "  \\usepackage{tikz}\n"
          "  \\usetikzlibrary{arrows, automata, positioning}\n"
          "  \\tikzstyle{automaton}=[shorten >=1pt, node distance=2cm, pos=.4,\n"
          "                         >=stealth', initial text=]\n"
          "  \\tikzstyle{accepting}=[accepting by arrow]\n"
          "\n"
          "\\begin{document}\n"
          "\\begin{tikzpicture}[automaton]\n"
          ;

        for (auto s : aut_.states())
          {
            os_ << "  \\node[state";
            format("initial", aut_.get_initial_weight(s));
            format("accepting", aut_.get_final_weight(s));
            os_ << ']'
                << " (" << states_[s] << ')';
            if (states_[s])
              os_ << " [right=of " << states_[s] - 1 << ']';
            os_ << " {$" << states_[s] << "$};\n";
        }

        for (auto src : aut_.states())
          {
            unsigned ns = states_[src];
            std::set<state_t> ds;
            for (auto t: aut_.out(src))
              ds.insert(aut_.dst_of(t));
            for (auto dst: ds)
              {
                unsigned nd = states_[dst];
                os_ << "  \\path[->] (" << ns << ')'
                    << " edge"
                    << (ns == nd ? "[loop above]" : "")
                    << " node[above]"
                    << " {$" << format_entry_(src, dst) << "$}"
                    << " (" << nd << ");\n";
              }
          }
        os_ <<
          "\\end{tikzpicture}\n"
          "\\end{document}";
      }
    };
  }

  template <typename Aut>
  std::ostream&
  tikz(const Aut& aut, std::ostream& out)
  {
    detail::tikzer<Aut> t{aut, out};
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

      REGISTER_DECLARE(tikz,
                        (const automaton& aut, std::ostream& out) -> std::ostream&);
    }
  }
}

#endif // !VCSN_ALGOS_TIKZ_HH
