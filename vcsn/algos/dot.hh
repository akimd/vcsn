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
    class dotter: public outputter<Aut>
    {
    private:
      using super_t = outputter<Aut>;
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
      dotter(const automaton_t& aut, std::ostream& out,
             bool dot2tex = false)
        : super_t(aut, out)
        , dot2tex_(dot2tex)
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
        aut_->context().print_set(bos_, format::text);
        bos_ << "\"\n"
          "  rankdir = LR\n"
          "  edge ["
            << (dot2tex_
                ? "texmode = math, lblstyle = auto"
                : "arrowhead = vee, arrowsize = .6")
            << "]\n";

        if (dot2tex_)
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

      /// Format a TikZ attribute.
      /// \param sep   the separator to print before (if we print something).
      /// \param kind  the attribute name (e.g., "initial").
      /// \param w     the associated weight (e.g., initial weight).
      bool format_(const std::string& sep,
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
                ws_.print(w, bos_) << '}';
              }
            return true;
          }
      }

      /// Pretty-print state \a s for both dot and dot2tex.
      void
      print_state_(state_t s)
      {
        aut_->print_state(s, bos_);
        bool has_attributes = false;
        if (dot2tex_)
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
                aut_->print_state_name(s, bos_, format::latex);
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
            if (format_(sep, "initial", aut_->get_initial_weight(s)))
              {
                sep = ", ";
                close = "\"";
              }
            if (format_(sep, "accepting", aut_->get_final_weight(s)))
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
                aut_->print_state_name(s, bos_, format::text);
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
        if (!dot2tex_)
          {
            // Output the pre-initial and post-final states.
            if (!aut_->initial_transitions().empty()
                || !aut_->final_transitions().empty())
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
                << (dot2tex_
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

        auto e = to_string(ps_, entry,
                           dot2tex_ ? format::latex : format::text,
                           ", ");
        bool useless = !has(useful_, src) || !has(useful_, dst);
        if (!e.empty() || useless)
          {
            bos_ << " [";
            const char* sep = "";
            if (!e.empty())
              {
                enable_();
                bos_ << "label = \"" << e << "\"";
                disable_();
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
              && (!dot2tex_ || src != aut_->pre()))
            {
              dsts.clear();
              for (auto t: aut_->all_out(src))
                if (!dot2tex_ || aut_->dst_of(t) != aut_->post())
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
      /// Whether to format for dot2tex.
      bool dot2tex_ = false;
      /// Useful states, without evaluating the lazy states.
      std::unordered_set<state_t_of<Aut>> useful_ = useful_states(aut_, false);
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
    }
  }
}
