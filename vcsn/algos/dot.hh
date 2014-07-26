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

    /// \brief Format an automaton into Dot.
    ///
    /// \tparam Aut an automaton type.
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
      using super_type::ws_;

      using super_type::super_type;

      // Dot, by default, uses the X11 color naming scheme, whose "gray"
      // is really light (it looks almost blue in some cases).
      const char* gray = "color = DimGray";

    public:
      dotter(const automaton_t& aut, std::ostream& out,
             bool dot2tex = false)
        : super_type(aut, out)
        , dot2tex_(dot2tex)
      {}

      /// Format a TikZ attribute.
      /// \param sep   the separator to print before (if we print something).
      /// \param kind  the attribute name (e.g., "initial").
      /// \param w     the associated weight (e.g., initial weight).
      bool format(const std::string& sep,
                  const std::string& kind, const weight_t& w)
      {
        if (ws_.is_zero(w))
          return false;
        else
          {
            os_ << sep << kind;
            if (ws_.show_one() || !ws_.is_one(w))
              {
                os_ << ", " << kind << " text={";
                ws_.print(w, os_) << '}';
              }
            return true;
          }
      }

      /// Pretty-print state \a s for both dot and dot2tex.
      void
      print_state_(state_t s)
      {
        aut_->print_state(s, os_);
        bool has_attributes = false;
        if (dot2tex_)
          {
            has_attributes = true;
            os_ << " [";
            std::string style;
            std::string sep;
            std::string close;
            // I hate this piece of code.  There must be means to be
            // better looking...
            if (aut_->state_has_name(s))
              {
                os_ << "label = \"";
                aut_->print_state_name(s, os_, "latex");
                static bool debug = getenv("VCSN_DEBUG");
                if (debug)
                  os_ << " (" << s << ')';
                os_ << "\", style = \"named";
                sep = ", ";
                close = "\"";
              }
            else
              sep = "style = \"state, ";
            if (format(sep, "initial", aut_->get_initial_weight(s)))
              {
                sep = ", ";
                close = "\"";
              }
            if (format(sep, "accepting", aut_->get_final_weight(s)))
              close = "\"";
            os_ << close;
          }
        else
          {
            // Dot format.
            if (aut_->state_has_name(s))
              {
                has_attributes = true;
                os_ << " [label = \"";
                aut_->print_state_name(s, os_, "text");
                static bool debug = getenv("VCSN_DEBUG");
                if (debug)
                  os_ << " (" << s << ')';
                os_ << "\", shape = box";
              }
          }
        if (!has(useful_, s))
          {
            if (has_attributes)
              os_ << ", ";
            else
              {
                os_ << " [";
                has_attributes = true;
              }
            os_ << gray;
          }
        if (has_attributes)
          os_ << ']';
      }

      std::ostream& operator()()
      {
        os_ <<
          "digraph\n"
          "{\n"
          "  vcsn_context = \"" << aut_->context().vname() << "\"\n"
          "  rankdir = LR\n"
          "  edge ["
            << (dot2tex_
                ? "texmode = math, lblstyle = auto"
                : "arrowhead = vee, arrowsize = .6")
            << "]\n";

        if (dot2tex_)
          os_ <<
            "  d2toptions = \"--format tikz --tikzedgelabels --graphstyle=automaton --crop --nominsize --autosize\"\n"
            "  d2tdocpreamble = \""
            "    \\usepackage{amssymb}"
            "    \\usetikzlibrary{arrows, automata}"
            "    \\tikzstyle{automaton}=[shorten >=1pt, pos=.4, >=stealth', initial text=]"
            "    \\tikzstyle{named}=[rectangle, rounded corners]"
            "    \\tikzstyle{initial}=[initial by arrow]"
            "    \\tikzstyle{accepting}=[accepting by arrow]"
            "  \"\n";
        else
          {
            // Output the pre-initial and post-final states.
            if (!aut_->initial_transitions().empty()
                || !aut_->final_transitions().empty())
              {
                os_ <<
                  "  {\n"
                  "    node [shape = point, width = 0]\n";
                for (auto s : initials_())
                  {
                    os_ << "    I";
                    aut_->print_state(s, os_);
                    os_ << '\n';
                  }
                for (auto s : finals_())
                  {
                    os_ << "    F";
                    aut_->print_state(s, os_);
                    os_ << '\n';
                  }
                os_ << "  }\n";
              }
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
        //
        // Set the width to something nicer than the default and shape
        // to rounded.  Useless for circle, but useful for shape =
        // box, and simpler to set it once for all.
        if (!aut_->states().empty())
          {
            os_ << "  {\n"
                << "    node ["
                << (dot2tex_
                    ? "texmode = math, style = state"
                    : "shape = circle, style = rounded, width = 0.5")
                << "]\n";
            for (auto s : aut_->states())
              {
                os_ << "    ";
                print_state_(s);
                os_ << '\n';
              }
            os_ << "  }\n";
          }

        for (auto src : dot2tex_ ? aut_->states() : aut_->all_states())
          {
            // Sort by destination state.
            std::set<state_t> ds;
            if (dot2tex_)
              for (auto t: aut_->out(src))
                ds.insert(aut_->dst_of(t));
            else
              for (auto t: aut_->all_out(src))
                ds.insert(aut_->dst_of(t));
            for (auto dst: ds)
              {
                os_ << "  ";
                if (src == aut_->pre())
                  {
                    os_ << 'I';
                    aut_->print_state(dst, os_);
                    os_ << " -> ";
                    aut_->print_state(dst, os_);
                  }
                else if (dst == aut_->post())
                  {
                    aut_->print_state(src, os_);
                    os_ << " -> ";
                    os_ << 'F';
                    aut_->print_state(src, os_);
                  }
                else
                  {
                    aut_->print_state(src, os_);
                    os_ << " -> ";
                    aut_->print_state(dst, os_);
                  }

                std::string s = format_entry_(src, dst,
                                              dot2tex_ ? "latex" : "text");
                bool useless = !has(useful_, src) || !has(useful_, dst);
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
                os_ << '\n';
              }
          }
        return os_ << '}';
      }

      /// Whether to format for dot2tex.
      bool dot2tex_ = false;
      /// Useful states.
      std::set<state_t_of<Aut>> useful_ = useful_states(aut_);
    };
  }

  template <typename Aut>
  std::ostream&
  dot(const Aut& aut, std::ostream& out, bool dot2tex = false)
  {
    detail::dotter<Aut> dot(aut, out, dot2tex);
    return dot();
  }

  namespace dyn
  {
    namespace detail
    {
      /// Bridge.
      template <typename Aut, typename Ostream, typename Bool>
      std::ostream& dot(const automaton& aut, std::ostream& out,
                        bool dot2tex)
      {
        return dot(aut->as<Aut>(), out, dot2tex);
      }

      REGISTER_DECLARE(dot,
                       (const automaton& aut, std::ostream& out,
                        bool dot2tex) -> std::ostream&);
    }
  }
}

#endif // !VCSN_ALGOS_DOT_HH
