#ifndef VCSN_ALGOS_TIKZ_HH
# define VCSN_ALGOS_TIKZ_HH

# include <algorithm>
# include <iostream>
# include <unordered_map>
# include <unordered_set>
# include <vector>

# include <vcsn/dyn/fwd.hh>

namespace vcsn
{

  /*--------------------------.
  | tikz(automaton, stream).  |
  `--------------------------*/

  namespace detail
  {
    template <typename Aut>
    class tikzer
    {
    public:
      using automaton_t = Aut;
      using state_t = typename automaton_t::state_t;
      using transition_t = typename automaton_t::transition_t;
      using weightset_t = typename automaton_t::weightset_t;
      using weight_t = typename automaton_t::weight_t;

      tikzer(const automaton_t& aut, std::ostream& out)
        : aut_(aut)
        , ws_(*aut_.weightset())
        , os_(out)
      {}

      void format(const std::string& kind, const weight_t& w,
                  const std::string& opt = {})
      {
        os_ << "," << kind << opt;
        if (ws_.show_unit() || !ws_.is_unit(w))
          os_ << "," << kind << " text=$" << ws_.format(w) << "$";
      }

      void operator()()
      {
        os_ << "\\begin{tikzpicture}"
            << "[shorten >=1pt, node distance=2cm, pos=.4, >=stealth', initial text={}]" << std::endl;
        // Name all the states.
        std::unordered_map<state_t, unsigned> names;
        {
          size_t num = 0;
          for (auto s : aut_.states())
            {
              names[s] = num++;
              os_ << "  \\node[state";
              if (aut_.is_initial(s))
                format("initial", aut_.get_initial_weight(s));
              if (aut_.is_final(s))
                format("accepting", aut_.get_final_weight(s), " by arrow");
              os_ << "]"
                  << " (" << names[s] << ")";
              if (names[s])
                os_ << " [right of=" << names[s] - 1 << "]";
              os_ << " {$" << names[s] << "$};" << std::endl;
            }
        }

        for (auto src : aut_.states())
          {
            // Transitions from src, ordered by destination.
            // Keep a single destination as we use entries below.
            // FIXME: A heap might be nice.
            std::unordered_set<state_t> ds;
            std::vector<transition_t> ts;
            for (auto t: aut_.out(src))
              if (ds.find(aut_.dst_of(t)) == end(ds))
                {
                  ds.insert(aut_.dst_of(t));
                  ts.push_back(t);
                }
            std::sort(begin(ts), end(ts),
                      [this](transition_t a, transition_t b)
                      {
                        return aut_.dst_of(a) < aut_.dst_of(b);
                      });
            for (const auto& t: ts)
              {
                unsigned ns = names[src];
                unsigned nd = names[aut_.dst_of(t)];
                os_ << "  \\path[->] (" << ns << ")"
                    << " edge"
                    << (ns == nd ? "[loop above]" : "")
                    << " node[above]"
                    << " {$"
                    <<   aut_.entryset().format(aut_.entry_at(t), ", ")
                    << "$}"
                    << " (" << nd << ");" << std::endl;
              }
          }
        os_ << "\\end{tikzpicture}";
      }

    private:
      const automaton_t& aut_;
      const weightset_t& ws_;
      std::ostream& os_;
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
        return tikz(dynamic_cast<const Aut&>(*aut), out);
      }

      REGISTER_DECLARE(tikz,
                       (const automaton& aut, std::ostream& out) -> std::ostream&);
    }
  }
}

#endif // !VCSN_ALGOS_TIKZ_HH
