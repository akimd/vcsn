#ifndef VCSN_ALGOS_DOT_HH
# define VCSN_ALGOS_DOT_HH

# include <algorithm>
# include <iostream>

# include <vcsn/dyn/fwd.hh>
# include <vcsn/dyn/automaton.hh>
# include <vcsn/algos/grail.hh>
# include <vcsn/algos/accessible.hh> // useful_states
# include <vcsn/misc/escape.hh>
# include <vcsn/misc/set.hh>

namespace vcsn
{

  namespace detail
  {
    /*-------------------------.
    | dot(automaton, stream).  |
    `-------------------------*/

    template <typename Aut>
    class dotter: public outputter<Aut>
    {
    private:
      using super_type = outputter<Aut>;
      using typename super_type::automaton_t;
      using typename super_type::state_t;
      using typename super_type::transition_t;
      using typename super_type::weightset_t;
      using typename super_type::weight_t;

      using super_type::aut_;
      using super_type::finals_;
      using super_type::format_entry_;
      using super_type::initials_;
      using super_type::os_;
      using super_type::states_;
      using super_type::ws_;

      using super_type::super_type;

      // Dot, by default, uses the X11 color naming scheme, whose "gray"
      // is really light (it looks almost blue in some cases).
      const char* gray = "color = DimGray";

    public:
      std::ostream& operator()()
      {
        auto useful = useful_states(aut_);

        os_ <<
          "digraph\n"
          "{\n"
          "  vcsn_context = \"" << aut_.context().vname() << "\"\n"
          "  rankdir = LR\n";

        // Output the pre-initial and post-final states.
        if (!aut_.initial_transitions().empty()
            || !aut_.final_transitions().empty())
          {
            os_ <<
              "  {\n"
              "    node [shape = point, width = 0]\n";
            for (auto s : initials_())
              os_ << "    I" << states_[s] << '\n';
            for (auto s : finals_())
              os_ << "    F" << states_[s] << '\n';
            os_ << "  }\n";
          }

        // Output all the states to make "print | read" idempotent.
        //
        // Put the useless ones in gray.  This does not work:
        //
        // { 0 1 2 }
        // { node [color = gray] 2 }
        //
        // because 2 was already "declared", and dot does not associate
        // "color = gray" to it.
        if (!aut_.states().empty())
          {
            os_ << "  {\n"
                << "    node [shape = circle]\n";
            for (auto s : aut_.states())
              {
                os_ << "    " << states_[s];
                static bool debug = getenv("VCSN_DEBUG");
                if (debug)
                  os_ << " [label = \"" << states_[s] << " (" << s << ")\"]";
                if (!has(useful, s))
                  os_ << " [" << gray << ']';
                os_ << '\n';
              }
            os_ << "  }\n";
          }

        for (auto src : aut_.all_states())
          {
            // Sort by destination state.
            std::set<state_t> ds;
            for (auto t: aut_.all_out(src))
              ds.insert(aut_.dst_of(t));
            for (auto dst: ds)
              {
                if (src == aut_.pre())
                  {
                    unsigned n = states_[dst];
                    os_ << "  I" << n << " -> " << n;
                  }
                else if (dst == aut_.post())
                  {
                    unsigned n = states_[src];
                    os_ << "  " << n << " -> F" << n;
                  }
                else
                  {
                    unsigned ns = states_[src];
                    unsigned nd = states_[dst];
                    os_ << "  " << ns << " -> " << nd;
                  }
                std::string s = format_entry_(src, dst);
                bool useless = !has(useful, src) || !has(useful, dst);
                if (!s.empty() || useless)
                  {
                    os_ << " [";
                    const char* sep = "";
                    if (!s.empty())
                      {
                        os_ << "label = \"" << str_escape(s) << "\"";
                        sep = ", ";
                      }
                    if (useless)
                      os_ << sep << gray;
                    os_ << ']';
                  }
                os_ << "\n";
              }
          }
        return os_ << '}';
      }
    };
  }

  template <typename Aut>
  std::ostream&
  dot(const Aut& aut, std::ostream& out)
  {
    detail::dotter<Aut> dot(aut, out);
    return dot();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Ostream>
      std::ostream& dot(const automaton& aut, std::ostream& out)
      {
        return dot(aut->as<Aut>(), out);
      }

      REGISTER_DECLARE(dot,
                        (const automaton& aut, std::ostream& out) -> std::ostream&);
    }
  }
}

#endif // !VCSN_ALGOS_DOT_HH
