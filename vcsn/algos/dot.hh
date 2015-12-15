#pragma once

#include <algorithm>
#include <iostream>
#include <sstream>

#include <vcsn/dyn/fwd.hh>
#include <vcsn/dyn/automaton.hh>
#include <vcsn/algos/grail.hh>
#include <vcsn/algos/accessible.hh> // useful_states
#include <vcsn/misc/iostream.hh>
#include <vcsn/misc/set.hh>
#include <vcsn/misc/unordered_set.hh>

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
    class dotter: public printer<Aut>
    {
    private:
      using super_t = printer<Aut>;
      using typename super_t::automaton_t;
      using typename super_t::state_t;
      using typename super_t::polynomial_t;
      using typename super_t::transition_t;
      using typename super_t::weightset_t;
      using typename super_t::weight_t;

      using super_t::aut_;
      using super_t::finals_;
      using super_t::initials_;
      using super_t::os_;
      using super_t::ps_;
      using super_t::ws_;

      using super_t::super_t;

      // Dot, by default, uses the X11 color naming scheme, whose "gray"
      // is really light (it looks almost blue in some cases).
      const char* gray = "color = DimGray";

    public:
      dotter(const automaton_t& aut, std::ostream& out, format fmt)
        : super_t(aut, out)
        , format_(fmt)
      {
#if defined __GNUC__ && ! defined __clang__
        // GCC 4.9 and 5.0 warnings: see
        // <https://gcc.gnu.org/bugzilla/show_bug.cgi?id=65324>.
# pragma GCC diagnostic push
# pragma GCC diagnostic ignored "-Wzero-as-null-pointer-constant"
#endif
        bos_.push(detail::backslashify_output_filter());
#if defined __GNUC__ && ! defined __clang__
# pragma GCC diagnostic pop
#endif
        bos_.push(out);
      }

      /// Print the automaton on the stream.
      std::ostream& operator()()
      {
        print_prologue_();
        print_states_();
        print_transitions_();
        print_epilogue_();
        return os_;
      }

    private:
      /// Start the dot graph.
      void print_prologue_()
      {
        bos_ <<
          "digraph\n"
          "{\n"
          "  vcsn_context = \"";
        enable_();
        aut_->context().print_set(bos_, format::sname);
        disable_();
        bos_ << "\"\n"
          "  rankdir = LR\n"
          "  edge ["
             << (format_ == format::latex
                 ? "texmode = math, lblstyle = auto"
                 : "arrowhead = vee, arrowsize = .6")
             << "]\n";

        if (format_ == format::latex)
          bos_ <<
            "  d2toptions = \"--format tikz --tikzedgelabels"
            " --graphstyle=automaton --crop --nominsize --autosize\"\n"
            "  d2tdocpreamble = \""
            "    \\usepackage{amssymb}"
            "    \\usetikzlibrary{arrows.meta, automata, bending}"
            "    \\tikzstyle{automaton}=[shorten >=1pt, pos=.4,"
            " >={Stealth[bend,round]}, initial text=]"
            "    \\tikzstyle{named}=[rectangle, rounded corners]"
            "    \\tikzstyle{initial}=[initial by arrow]"
            "    \\tikzstyle{accepting}=[accepting by arrow]"
            "  \"\n";
      }

      /// Finish the dot graph.
      void print_epilogue_()
      {
        bos_ << '}';
      }

      /// Print a TikZ attribute.
      ///
      /// \param sep   the separator to print before (if we print something).
      /// \param kind  the attribute name (e.g., "initial").
      /// \param w     the associated weight (e.g., initial weight).
      bool print_(const std::string& sep,
                   const std::string& kind, const weight_t& w)
      {
        if (ws_.is_zero(w))
          return false;
        else
          {
            bos_ << sep << kind;
            if (ws_.show_one() || !ws_.is_one(w))
              {
                bos_ << ", " << kind << " text={";
                ws_.print(w, bos_, format_) << '}';
              }
            return true;
          }
      }

      /// Pretty-print state \a s.
      void
      print_state_(state_t s)
      {
        aut_->print_state(s, bos_);
        bool has_attributes = false;
        if (format_ == format::latex)
          {
            has_attributes = true;
            bos_ << " [";
            std::string style;
            std::string sep;
            std::string close;
            // I hate this piece of code.  There must be means to be
            // better looking...
            if (aut_->state_has_name(s))
              {
                bos_ << "label = \"";
                enable_();
                aut_->print_state_name(s, bos_, format_);
                disable_();
                static bool debug = getenv("VCSN_DEBUG");
                if (debug)
                  bos_ << " (" << s << ')';
                bos_ << "\", style = \"named";
                sep = ", ";
                close = "\"";
              }
            else
              sep = "style = \"state, ";
            if (print_(sep, "initial", aut_->get_initial_weight(s)))
              {
                sep = ", ";
                close = "\"";
              }
            if (print_(sep, "accepting", aut_->get_final_weight(s)))
              close = "\"";
            bos_ << close;
          }
        else
          {
            // Dot format.
            if (aut_->state_has_name(s))
              {
                has_attributes = true;
                bos_ << " [label = \"";
                enable_();
                aut_->print_state_name(s, bos_, format_);
                disable_();
                static bool debug = getenv("VCSN_DEBUG");
                if (debug)
                  bos_ << " (" << s << ')';
                bos_ << "\", shape = box";
              }
            if (!aut_->state_is_strict(s))
              {
                if (has_attributes)
                  bos_ << ", ";
                else
                  {
                    bos_ << " [";
                    has_attributes = true;
                  }
                bos_ << "style = dashed";
              }
          }
        if (!has(useful_, s))
          {
            if (has_attributes)
              bos_ << ", ";
            else
              {
                bos_ << " [";
                has_attributes = true;
              }
            bos_ << gray;
          }
        if (has_attributes)
          bos_ << ']';
      }

      /// Print the states.
      void print_states_()
      {
        if (format_ != format::latex)
          {
            // Output the pre-initial and post-final states.
            if (!initial_transitions(aut_).empty()
                || !final_transitions(aut_).empty())
              {
                bos_ <<
                  "  {\n"
                  "    node [shape = point, width = 0]\n";
                for (auto s : initials_())
                  {
                    bos_ << "    I";
                    aut_->print_state(s, bos_);
                    bos_ << '\n';
                  }
                for (auto s : finals_())
                  {
                    bos_ << "    F";
                    aut_->print_state(s, bos_);
                    bos_ << '\n';
                  }
                bos_ << "  }\n";
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
            bos_ << "  {\n"
                 << "    node ["
                 << (format_ == format::latex
                     ? "texmode = math, style = state"
                     : "shape = circle, style = rounded, width = 0.5")
                 << "]\n";
            for (auto s : aut_->states())
              {
                bos_ << "    ";
                print_state_(s);
                bos_ << '\n';
              }
            bos_ << "  }\n";
          }
      }

      /// Print the transitions between state \a src and state \a dst.
      void print_transitions_(const state_t src, const state_t dst,
                              const polynomial_t& entry)
      {
        bos_ << "  ";
        if (src == aut_->pre())
          {
            bos_ << 'I';
            aut_->print_state(dst, bos_);
          }
        else
          aut_->print_state(src, bos_);
        bos_ << " -> ";
        if (dst == aut_->post())
          {
            bos_ << 'F';
            aut_->print_state(src, bos_);
          }
        else
          aut_->print_state(dst, bos_);

        auto e = to_string(ps_, entry, format_, ", ");
        bool useless = !has(useful_, src) || !has(useful_, dst);
        if (!e.empty() || useless)
          {
            bos_ << " [";
            const char* sep = "";
            if (!e.empty())
              {
                bos_ << "label = \"";
                enable_();
                bos_ << e;
                disable_();
                bos_ << "\"";
                sep = ", ";
              }
            if (useless)
              bos_ << sep << gray;
            bos_ << ']';
          }
        bos_ << '\n';
      }

      /// Print all the transitions, sorted by src state, then dst state.
      void print_transitions_()
      {
        // For each src state, the destinations, sorted.
        std::map<state_t, polynomial_t> dsts;
        for (auto src : aut_->all_states())
          if (aut_->state_is_strict(src)
              && (format_ != format::latex || src != aut_->pre()))
            {
              dsts.clear();
              for (auto t: aut_->all_out(src))
                if (format_ != format::latex
                    || aut_->dst_of(t) != aut_->post())
                  // Bypass weight_of(set), because we know that the weight is
                  // nonzero, and that there is only one weight per letter.
                  ps_.new_weight(dsts[aut_->dst_of(t)],
                                 aut_->label_of(t), aut_->weight_of(t));
              for (const auto& p: dsts)
                print_transitions_(src, p.first, p.second);
            }
      }

      /// Enable the escaping of backslashes.
      void enable_()
      {
        boost::iostreams::flush(bos_);
        bos_.component<detail::backslashify_output_filter>(0)->enable();
      }

      /// Disable the escaping of backslashes.
      void disable_()
      {
        boost::iostreams::flush(bos_);
        bos_.component<detail::backslashify_output_filter>(0)->disable();
      }

      /// The output stream, with a backslashify filter.
      detail::io::filtering_ostream bos_;
      /// Format for labels and weights.
      format format_ = {};
      /// Useful states, without evaluating the lazy states.
      std::unordered_set<state_t_of<Aut>> useful_ = useful_states(aut_, false);
    };
  }

  /// Print an automaton in Graphviz's Dot format.
  ///
  /// \param aut     the automaton to print.
  /// \param out     the output stream.
  /// \param fmt     how to format the automaton.
  template <typename Aut>
  std::ostream&
  dot(const Aut& aut, std::ostream& out, format fmt = {})
  {
    detail::dotter<Aut> dot{aut, out, fmt};
    return dot();
  }
}
