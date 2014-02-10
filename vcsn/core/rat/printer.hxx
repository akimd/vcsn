#ifndef VCSN_CORE_RAT_PRINTER_HXX
# define VCSN_CORE_RAT_PRINTER_HXX

# include <vcsn/misc/escape.hh>
# include <vcsn/misc/raise.hh>

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
      format_ = format;
      if (format_ == "latex")
        {
          lgroup_       = "{";
          rgroup_       = "}";
          lbracket_     = " \\langle ";
          rbracket_     = " \\rangle ";
          lparen_       = "\\left(";
          rparen_       = "\\right)";
          star_         = "^{*}";
          complement_   = "^{c}";
          intersection_ = " \\cap ";
          shuffle_      = " \\between ";
          product_      = ctx_.is_law ? " \\cdot " : " \\, ";
          sum_          = " + ";
          zero_         = "\\emptyset";
          one_          = "\\varepsilon";
          lmul_         = "\\,";
          rmul_         = "\\,";
        }
      else if (format_ == "text")
        {
          lgroup_       = "";
          rgroup_       = "";
          lbracket_     = "<";
          rbracket_     = ">";
          lparen_       = "(";
          rparen_       = ")";
          star_         = "*";
          complement_   = "{c}";
          intersection_ = "&";
          shuffle_      = ":";
          product_      = ctx_.is_law ? "." : "";
          sum_          = "+";
          zero_         = "\\z";
          one_          = "\\e";
          lmul_         = "";
          rmul_         = "";
        }
      else
        raise("invalid output format for ratexp: ", str_escape(format));
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
            CASE(atom);
            CASE(complement);
            CASE(intersection);
            CASE(lweight);
            CASE(one);
            CASE(prod);
            CASE(rweight);
            CASE(shuffle);
            CASE(star);
            CASE(sum);
            CASE(zero);
# undef CASE
          }
      abort(); // Unreachable.
    }

# define VISIT(Type)                          \
    DEFINE::visit(const Type ## _t& v)        \
      -> void

    VISIT(lweight)
    {
      out_ << lbracket_;
      ctx_.weightset()->print(out_, v.weight(), format_);
      out_ << rbracket_ << lmul_;
      print_child(*v.sub(), v);
    }

    VISIT(rweight)
    {
      print_child(*v.sub(), v);
      out_ << rmul_ << lbracket_;
      ctx_.weightset()->print(out_, v.weight(), format_);
      out_ << rbracket_;
    }

    VISIT(zero)
    {
      (void) v;
      out_ << zero_;
    }

    VISIT(one)
    {
      (void) v;
      out_ << one_;
    }

    VISIT(atom)
    {
      ctx_.labelset()->print(out_, v.value(), format_);
    }

    DEFINE::print_child(const node_t& child, const node_t& parent,
                        bool force_parens)
      -> void
    {
      static bool force = !! getenv("VCSN_PARENS");
      bool parent_has_precedence = precedence(child) <= precedence(parent);
      bool needs_parens =
        (force
         || force_parens
         || (parent_has_precedence && ! (parent.is_unary() && child.is_unary())))
        && ! (child.type() == type_t::atom && context_t::is_lal);
      if (needs_parens)
        out_ << lparen_;
      else if (parent.is_unary())
        out_ << lgroup_;
      child.accept(*this);
      if (needs_parens)
        out_ << rparen_;
      else if (parent.is_unary())
        out_ << rgroup_;
    }

    template <typename RatExpSet>
    template <type_t Type>
    inline
    auto
    printer<RatExpSet>::print(const unary_t<Type>& v, const char* op)
      -> void
    {
      // Force parens around the child if it is a left weight.  This
      // is not needed for right weights: compare e<w>* with (<w>e)*.
      const node_t& child = *v.sub();
      print_child(child, v, child.type() == rat::type_t::lweight);
      out_ << op;
    }

    template <typename RatExpSet>
    template <type_t Type>
    inline
    auto
    printer<RatExpSet>::print(const nary_t<Type>& n, const char* op)
      -> void
    {
      bool first = true;
      for (auto i: n)
        {
          if (! first)
            out_ << op;
          print_child(*i, n);
          first = false;
        }
    }

# undef VISIT
# undef DEFINE

  } // namespace rat
} // namespace vcsn

#endif // !VCSN_CORE_RAT_PRINTER_HXX
