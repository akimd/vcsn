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
      using typename super_type::weightset_t;
      using typename super_type::weight_t;

      using super_type::os_;
      using super_type::aut_;
      using super_type::states_;
      using super_type::ws_;

      using super_type::super_type;

      /// Format a TikZ attribute.
      /// \param kind  the attribute name (e.g., "initial").
      /// \param w     the associated weight (e.g., initial weight).
      /// \param opt   the associated weight (e.g., initial weight).
      void format(const std::string& kind, const weight_t& w)
      {
        os_ << "," << kind;
        if (ws_.show_one() || !ws_.is_one(w))
          os_ << "," << kind << " text=$" << ws_.format(w) << "$";
      }

      void operator()()
      {
        os_ <<
          "% \\usetikzlibrary{arrows, automata, positioning}\n"
          "% \\tikzstyle{automaton}=[shorten >=1pt, node distance=2cm, pos=.4,\n"
          "%                        >=stealth', initial text=]\n"
          "% \\tikzstyle{accepting}=[accepting by arrow]\n";

        os_ << "\\begin{tikzpicture}[automaton]" << std::endl;

        for (auto s : aut_.states())
          {
            os_ << "  \\node[state";
            if (aut_.is_initial(s))
              format("initial", aut_.get_initial_weight(s));
            if (aut_.is_final(s))
              format("accepting", aut_.get_final_weight(s));
            os_ << "]"
                << " (" << states_[s] << ")";
            if (states_[s])
              os_ << " [right=of " << states_[s] - 1 << "]";
            os_ << " {$" << states_[s] << "$};" << std::endl;
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
                os_ << "  \\path[->] (" << ns << ")"
                    << " edge"
                    << (ns == nd ? "[loop above]" : "")
                    << " node[above]"
                    << " {$" << detail::format_entry(aut_, src, dst) << "$}"
                    << " (" << nd << ");" << std::endl;
              }
          }
        os_ << "\\end{tikzpicture}";
      }
    };
  }

  template <class Aut>
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
      template <typename Aut>
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
