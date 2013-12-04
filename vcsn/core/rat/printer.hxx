#ifndef VCSN_CORE_RAT_PRINTER_HXX
# define VCSN_CORE_RAT_PRINTER_HXX

# include <vcsn/misc/escape.hh>

namespace vcsn
{
  namespace rat
  {

    template <typename RatExpSet>
    inline
    printer<RatExpSet>::printer(std::ostream& out,
                                const ratexpset_t& rs,
                                const bool debug)
      : out_(out)
      , ctx_(rs.context())
      , debug_(debug)
    {}


# define DEFINE                                 \
    template <typename RatExpSet>               \
    inline                                      \
    auto                                        \
    printer<RatExpSet>

    DEFINE::format(const std::string& format)
      -> void
    {
      if (format == "latex")
        {
          ldelim_       = "$";
          rdelim_       = "$";
          lbracket_     = "";
          rbracket_     = "";
          lparen_       = "\\left(";
          rparen_       = "\\right)";
          star_         = "^*";
          intersection_ = " \\cap ";
          shuffle_      = " \\between ";
          product_      = " \\cdot ";
          sum_          = " + ";
          zero_         = "\\emptyset";
          one_          = "\\varepsilon";
          lmul_         = "\\,";
          rmul_         = "\\,";
        }
      else if (format == "text")
        {
          ldelim_       = "";
          rdelim_       = "";
          lbracket_     = "<";
          rbracket_     = ">";
          lparen_       = "(";
          rparen_       = ")";
          star_         = "*";
          intersection_ = "&";
          shuffle_      = "{:}";
          product_      = ".";
          sum_          = "+";
          zero_         = "\\z";
          one_          = "\\e";
          lmul_         = "";
          rmul_         = "";
        }
      else
        throw std::domain_error("invalid output format for ratexp: "
                                + str_escape(format));
    }

    DEFINE::precedence(const node_t& v) const
      -> precedence_t
    {
      const atom_t* atom = dynamic_cast<const atom_t*>(&v);
      if (atom && ! ctx_.labelset()->is_letter(atom->value()))
        return precedence_t::word;
      else
        switch (v.type())
          {
# define CASE(Type)                             \
            case exp::type_t::Type:             \
              return precedence_t::Type;
            CASE(intersection);
            CASE(sum);
            CASE(shuffle);
            CASE(prod);
            CASE(star);
            CASE(zero);
            CASE(one);
            CASE(atom);
# undef CASE
          }
      abort(); // Unreachable.
    }

# define VISIT(Type)                          \
    DEFINE::visit(const Type ## _t& v)        \
      -> void

    VISIT(intersection)
    {
      print(v, intersection_);
    }

    VISIT(shuffle)
    {
      print(v, shuffle_);
    }

    VISIT(sum)
    {
      print(v, sum_);
    }

    VISIT(prod)
    {
      print(v, product_);
    }

    VISIT(star)
    {
      // Force parens around the child if it needs a left weight.  This is
      // not needed for right weights: compare e<w>* with (<w>e)*.
      const node_t& child = *v.sub();
      bool child_needs_left_weight = shows_left_weight_(child);
      print_left_weight(v);
      print_child(child, v, child_needs_left_weight);
      out_ << star_;
      print_right_weight(v);
    }

    VISIT(zero)
    {
      print_left_weight(v);
      out_ << zero_;
    }

    VISIT(one)
    {
      print_left_weight(v);
      out_ << one_;
    }

    VISIT(atom)
    {
      print_left_weight(v);
      ctx_.labelset()->print(out_, v.value());
    }

    DEFINE::print_child(const node_t& child, const node_t& parent,
                        bool force_parens)
      -> void
    {
      bool parent_has_precedence = precedence(child) < precedence(parent);
      bool needs_parens = parent_has_precedence || force_parens;
      if (needs_parens)
        out_ << lparen_;
      child.accept(*this);
      if (needs_parens)
        out_ << rparen_;
    }

    template <typename RatExpSet>
    template <type_t Type>
    inline
    auto
    printer<RatExpSet>::print(const nary_t<Type>& n, const char* op)
      -> void
    {
      bool need_weight_parens = shows_weight_(n);

      print_left_weight(n);
      if (need_weight_parens)
        out_ << lparen_;
      bool first = true;
      for (auto i: n)
        {
          if (! first)
            out_ << op;
          print_child(*i, n);
          first = false;
        }

      if (need_weight_parens)
        out_ << rparen_;
      print_right_weight(n);
    }

    DEFINE::print_left_weight(const node_t& e)
      -> void
    {
      if (shows_(e.left_weight()))
        {
          out_ << lbracket_;
          ctx_.weightset()->print(out_, e.left_weight());
          out_ << rbracket_ << lmul_;
        }
    }

    DEFINE::print_right_weight(const inner_t& e)
      -> void
    {
      if (shows_(e.right_weight()))
        {
          out_ << rmul_ << lbracket_;
          ctx_.weightset()->print(out_, e.right_weight());
          out_ << rbracket_;
        }
    }

# undef VISIT
# undef DEFINE

  } // namespace rat
} // namespace vcsn

#endif // !VCSN_CORE_RAT_PRINTER_HXX
