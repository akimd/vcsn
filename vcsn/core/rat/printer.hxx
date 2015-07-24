#include <vcsn/misc/escape.hh>
#include <vcsn/misc/indent.hh>
#include <vcsn/misc/memory.hh> // address
#include <vcsn/misc/raise.hh>

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
          CASE(zero);
          CASE(one);
          CASE(atom);
          CASE(sum);
          CASE(prod);
          CASE(ldiv);
          CASE(conjunction);
          CASE(shuffle);
          CASE(star);
          CASE(transposition);
          CASE(lweight);
          CASE(rweight);
          CASE(complement);
          CASE(tuple);
#undef CASE
        }
      return o;
    }

    template <typename ExpSet>
    inline
    printer<ExpSet>::printer(const expressionset_t& rs,
                             std::ostream& out)
      : out_(out)
      , rs_(rs)
    {}


#define DEFINE                                  \
    template <typename ExpSet>                  \
    inline                                      \
    auto                                        \
    printer<ExpSet>

    DEFINE::operator()(const node_t& v)
      -> std::ostream&
    {
      static bool print = !! getenv("VCSN_PRINT");
      if (print)
        out_ << '<' << v.type() << "@0x" << address(v) << '>' << vcsn::incendl;
      if (debug_ && format_ == "latex")
        out_ << (rs_.identities().is_distributive()
                 ? "{\\color{red}{" : "{\\color{blue}{");
      v.accept(*this);
      if (debug_ && format_ == "latex")
        out_ << "}}";
      if (print)
        out_ << vcsn::decendl << "</" << v.type() << '>';
      return out_;
    }

    DEFINE::format(const std::string& format)
      -> void
    {
      format_ = format;
      if (format_ == "latex")
        {
          lgroup_        = "{";
          rgroup_        = "}";
          langle_        = " \\left\\langle ";
          rangle_        = " \\right\\rangle ";
          lparen_        = "\\left(";
          rparen_        = "\\right)";
          star_          = "^{*}";
          complement_    = "^{c}";
          transposition_ = "^{T}";
          conjunction_   = " \\& ";
          shuffle_       = " \\between ";
          product_       = " \\, ";
          sum_           = (rs_.identities().is_distributive() ? " \\oplus "
                            : " + ");
          zero_          = "\\emptyset";
          one_           = "\\varepsilon";
          lmul_          = "\\,";
          rmul_          = "\\,";
          ldiv_          = " \\backslash ";
        }
      else if (format_ == "text")
        {
          lgroup_        = "";
          rgroup_        = "";
          langle_        = "<";
          rangle_        = ">";
          lparen_        = "(";
          rparen_        = ")";
          star_          = "*";
          complement_    = "{c}";
          transposition_ = "{T}";
          conjunction_   = "&";
          shuffle_       = ":";
          product_       = "";
          sum_           = "+";
          zero_          = "\\z";
          one_           = "\\e";
          lmul_          = "";
          rmul_          = "";
          ldiv_          = "{\\}";
        }
      else
        raise("invalid output format for expression: ", str_escape(format));
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
            CASE(atom);
            CASE(complement);
            CASE(conjunction);
            CASE(ldiv);
            CASE(lweight);
            CASE(one);
            CASE(prod);
            CASE(rweight);
            CASE(shuffle);
            CASE(star);
            CASE(sum);
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
      rs_.weightset()->print(v.weight(), out_, format_);
      out_ << rangle_ << lmul_;
      print_child_(*v.sub(), v);
    }

    VISIT(rweight)
    {
      print_child_(*v.sub(), v);
      out_ << rmul_ << langle_;
      rs_.weightset()->print(v.weight(), out_, format_);
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
      rs_.labelset()->print(v.value(), out_, format_);
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
      operator()(child);
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
    inline
    auto
    printer<ExpSet>::print_(const unary_t<Type>& v, const char* op)
      -> void
    {
      print_child_(*v.sub(), v);
      out_ << op;
    }

    template <typename ExpSet>
    template <type_t Type>
    inline
    auto
    printer<ExpSet>::print_(const variadic_t<Type>& n, const char* op)
      -> void
    {
      bool first = true;
      for (auto i: n)
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
