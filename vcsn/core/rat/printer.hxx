#include <vcsn/misc/escape.hh>
#include <vcsn/misc/indent.hh>
#include <vcsn/misc/memory.hh> // address
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/type_traits.hh>

namespace vcsn
{
  namespace rat
  {
    inline
    std::ostream&
    operator<<(std::ostream& o, type_t t)
    {
      switch (t)
        {
#define CASE(T) case type_t::T: o << #T; break
          CASE(add);
          CASE(atom);
          CASE(complement);
          CASE(compose);
          CASE(conjunction);
          CASE(infiltration);
          CASE(ldiv);
          CASE(lweight);
          CASE(one);
          CASE(prod);
          CASE(rweight);
          CASE(shuffle);
          CASE(star);
          CASE(transposition);
          CASE(tuple);
          CASE(zero);
#undef CASE
        }
      return o;
    }

    template <typename ExpSet>
    printer<ExpSet>::printer(const expressionset_t& rs,
                             std::ostream& out)
      : out_{out}
      , rs_{rs}
    {}


#define DEFINE                                  \
    template <typename ExpSet>                  \
    auto                                        \
    printer<ExpSet>

    DEFINE::print_(const node_t& v)
      -> std::ostream&
    {
      static bool print = !! getenv("VCSN_PRINT");
      if (print)
        out_ << '<' << v.type() << "@0x" << address(v) << '>' << vcsn::incendl;
      if (debug_ && fmt_ == format::latex)
        out_ << (rs_.identities().is_distributive()
                 ? "{\\color{red}{" : "{\\color{blue}{");
      v.accept(*this);
      if (debug_ && fmt_ == format::latex)
        out_ << "}}";
      if (print)
        out_ << vcsn::decendl << "</" << v.type() << '>';
      return out_;
    }

    DEFINE::format(class format fmt)
      -> void
    {
      fmt_ = fmt;
      if (fmt_ == format::latex)
        {
          lgroup_        = "{";
          rgroup_        = "}";
          langle_        = " \\left\\langle ";
          rangle_        = " \\right\\rangle ";
          lparen_        = "\\left(";
          rparen_        = "\\right)";
          lexponent_     = "^{";
          rexponent_     = "}";
          star_          = "^{*}";
          compose_       = "@";
          complement_    = "^{c}";
          transposition_ = "^{T}";
          conjunction_   = " \\& ";
          infiltration_  = " \\uparrow ";
          shuffle_       = " \\between ";
          product_       = " \\, ";
          add_           = (rs_.identities().is_distributive() ? " \\oplus "
                            : " + ");
          zero_          = "\\emptyset";
          one_           = "\\varepsilon";
          lmul_          = "\\,";
          rmul_          = "\\,";
          ldiv_          = " \\backslash ";
          tuple_left     = " \\left. ";
          tuple_middle   = " \\middle| ";
          tuple_right    = " \\right. ";
          exponent_threshold_= 2;
        }
      else if (fmt_ == format::text)
        {
          lgroup_        = "";
          rgroup_        = "";
          langle_        = "<";
          rangle_        = ">";
          lparen_        = "(";
          rparen_        = ")";
          lexponent_     = "{";
          rexponent_     = "}";
          star_          = "*";
          complement_    = "{c}";
          compose_       = "@";
          transposition_ = "{T}";
          conjunction_   = "&";
          infiltration_  = "&:";
          shuffle_       = ":";
          product_       = "";
          add_           = "+";
          zero_          = "\\z";
          one_           = "\\e";
          lmul_          = "";
          rmul_          = "";
          ldiv_          = "{\\}";
          tuple_left     = "";
          tuple_middle   = "|";
          tuple_right    = "";
          exponent_threshold_= 4;
        }
      else if (fmt_ == format::utf8)
        {
          lgroup_        = "";
          rgroup_        = "";
          langle_        = "⟨";
          rangle_        = "⟩";
          lparen_        = "(";
          rparen_        = ")";
          star_          = "*";
          compose_       = "@";
          complement_    = "ᶜ";
          transposition_ = "ᵗ";
          conjunction_   = "&";
          infiltration_  = "&:";
          shuffle_       = ":";
          product_       = "";
          add_           = "+";
          zero_          = "∅";
          one_           = "ε";
          lmul_          = "";
          rmul_          = "";
          ldiv_          = "{\\}";
          tuple_left     = "";
          tuple_middle   = "|";
          tuple_right    = "";
          exponent_threshold_= 2;
        }
      else
        raise("expression: invalid format: ", fmt_);
    }

    DEFINE::precedence_(const node_t& v) const
      -> precedence_t
    {
      if (is_word_(v))
        return precedence_t::word;
      else
        switch (v.type())
          {
#define CASE(Type)                              \
            case exp::type_t::Type:             \
              return precedence_t::Type
            CASE(add);
            CASE(atom);
            CASE(complement);
            CASE(compose);
            CASE(conjunction);
            CASE(infiltration);
            CASE(ldiv);
            CASE(lweight);
            CASE(one);
            CASE(prod);
            CASE(rweight);
            CASE(shuffle);
            CASE(star);
            CASE(transposition);
            CASE(tuple);
            CASE(zero);
#undef CASE
          }
      abort(); // Unreachable.
    }

#define VISIT(Type)                             \
    DEFINE::visit(const Type ## _t& v)          \
      -> void

    VISIT(lweight)
    {
      out_ << langle_;
      print_(v.weight());
      out_ << rangle_ << lmul_;
      print_child_(*v.sub(), v);
    }

    VISIT(rweight)
    {
      print_child_(*v.sub(), v);
      out_ << rmul_ << langle_;
      print_(v.weight());
      out_ << rangle_;
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
      rs_.labelset()->print(v.value(), out_,
                            fmt_.for_labels().meta("!&%|@',[-]<> (){}\n*+:.?"));
    }

    /// Specific handling to using exponents as syntactic sugar.
    VISIT(prod)
    {
      for (auto it = begin(v); it != end(v); /* nothing */)
        {
          if (it != begin(v))
            out_ << product_;

          const auto& i = *it;
          // Find the size of the sequence of nodes equal to i.
          auto next =
            std::find_if(it, end(v),
                         [&](const auto& e) { return !rs_.equal(e, i); });
          auto count = std::distance(it, next);
          if (1 < count
              && (!expressionset_t::context_t::is_lal
                  || ! is_letter_(*i)
                  || exponent_threshold_ < count))
            {
              // We can display an exponent.
              print_child(*i, precedence_t::exponent);
              if (fmt_ == format::utf8)
                {
                  // Recursive lambda to display UTF-8 characters
                  // in the correct order.
                  std::function<void(int)> print = [this, &print](int n)
                    {
                      if (n)
                        {
                          print(n / 10);
                          out_ << superscripts[n % 10];
                        }
                    };
                  print(count);
                }
              else
                out_ << lexponent_ << count << rexponent_;
              it = next;
            }
          else
            {
              // Print a single node.
              print_child_(*i, v);
              ++it;
            }
        }
    }

    DEFINE::print_child(const node_t& child, precedence_t parent)
      -> void
    {
      static bool force = !! getenv("VCSN_PARENS");
      bool parent_has_precedence = precedence_(child) <= parent;
      bool needs_parens =
        (force
         || (parent_has_precedence
             && ! (parent == precedence_t::unary && child.is_unary())
             && ! is_braced_(child)));
      if (needs_parens)
        out_ << lparen_;
      else if (parent == precedence_t::unary)
        out_ << lgroup_;
      print_(child);
      if (needs_parens)
        out_ << rparen_;
      else if (parent == precedence_t::unary)
        out_ << rgroup_;
    }

    DEFINE::print_child_(const node_t& child, const node_t& parent)
      -> void
    {
      print_child(child, precedence_(parent));
    }

    template <typename ExpSet>
    template <type_t Type>
    auto
    printer<ExpSet>::print_(const unary_t<Type>& v, const char* op)
      -> void
    {
      print_child_(*v.sub(), v);
      out_ << op;
    }

    template <typename ExpSet>
    template <type_t Type>
    auto
    printer<ExpSet>::print_(const variadic_t<Type>& n, const char* op)
      -> void
    {
      bool first = true;
      for (const auto& i: n)
        {
          if (! first)
            out_ << op;
          print_child_(*i, n);
          first = false;
        }
    }

#undef VISIT
#undef DEFINE

  } // namespace rat
} // namespace vcsn
