#include <vcsn/misc/escape.hh>
#include <vcsn/misc/indent.hh>
#include <vcsn/misc/memory.hh> // address
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/type_traits.hh>

namespace vcsn::rat
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
        CASE(infiltrate);
        CASE(ldivide);
        CASE(lweight);
        CASE(name);
        CASE(one);
        CASE(mul);
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
  template <typename ExpSet>                    \
  auto                                          \
  printer<ExpSet>

  DEFINE::print_(const node_t& v)
  -> std::ostream&
  {
    if (tagged_)
      out_ << '<' << v.type() << "@0x" << address(v) << '>' << vcsn::incendl;
    if (parens_ && fmt_ == format::latex)
      out_ << (rs_.identities().is_distributive()
               ? "{\\color{red}{" : "{\\color{blue}{");
    v.accept(*this);
    if (parens_ && fmt_ == format::latex)
      out_ << "}}";
    if (tagged_)
      out_ << vcsn::decendl << "</" << v.type() << '>';
    return out_;
  }

  DEFINE::format(class format fmt)
  -> void
  {
    fmt_ = fmt;
    const auto text = fmt_ == format::text;
    switch (fmt_.kind())
      {
      case format::latex:
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
        infiltrate_    = " \\uparrow ";
        shuffle_       = " \\between ";
        product_       = " \\, ";
        add_           = (rs_.identities().is_distributive() ? " \\oplus "
                          : " + ");
        zero_          = "\\emptyset";
        one_           = "\\varepsilon";
        lweight_       = "\\,";
        rweight_       = "\\,";
        ldivide_       = " \\backslash ";
        tuple_left     = " \\left. ";
        tuple_middle   = " \\middle| ";
        tuple_right    = " \\right. ";
        exponent_threshold_= 2;
        break;
      case format::ere:
      case format::text:
      case format::redgrep:
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
        infiltrate_    = "&:";
        shuffle_       = ":";
        product_       = "";
        add_           = text ? "+" : "|";
        zero_          = "\\z";
        one_           = text ? "\\e" : "()";
        lweight_       = "";
        rweight_       = "";
        ldivide_       = "{\\}";
        tuple_left     = "";
        tuple_middle   = "|";
        tuple_right    = "";
        exponent_threshold_= 4;
        break;
      case format::utf8:
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
        infiltrate_    = "&:";
        shuffle_       = ":";
        product_       = "";
        add_           = "+";
        zero_          = "∅";
        one_           = "ε";
        lweight_       = "";
        rweight_       = "";
        ldivide_       = "{\\}";
        tuple_left     = "";
        tuple_middle   = "|";
        tuple_right    = "";
        exponent_threshold_= 2;
        break;
      default:
        raise("expression: invalid format: ", fmt_);
        break;
      }
  }

#define VISIT(Type)                             \
  DEFINE::visit(const Type ## _t& v)            \
  -> void

  VISIT(lweight)
  {
    out_ << langle_;
    print_(v.weight());
    out_ << rangle_ << lweight_;
    print_child_(*v.sub(), v);
  }

  VISIT(rweight)
  {
    print_child_(*v.sub(), v);
    out_ << rweight_ << langle_;
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
    const auto meta
      = fmt_ == format::ere     ? "|[-](){}\\\n*+.?" "$^" ""
      : fmt_ == format::redgrep ? "|[-](){}\\\n*+.?" "$^" "!&"
      :                           "|[-](){}\\\n*+.?" "!&:% @',<>";
    rs_.labelset()->print(v.value(), out_,
                          fmt_.for_labels().meta(meta));
  }

  /// Specific handling to using exponents as syntactic sugar.
  VISIT(mul)
  {
    for (auto it = begin(v); it != end(v); /* nothing */)
      {
        if (it != begin(v))
          out_ << product_;

        const auto& i = *it;
        // Find the size of the sequence of nodes equal to i.
        const auto next =
          std::find_if(it, end(v),
                       [&](const auto& e) { return !rs_.equal(e, i); });
        const auto count = std::distance(it, next);
        if (1 < count
            && (!is_letter_(*i) || exponent_threshold_ < count))
          {
            // We can display an exponent.
            print_child(*i, precedence_t::exponent);
            if (fmt_ == format::utf8)
              print_exponent_utf8(count, out_);
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

  VISIT(name)
  {
    if (fmt_ == format::latex)
      out_ << "\\mathsf{" << v.name_get() << '}';
    else
      out_ << v.name_get();
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
  printer<ExpSet>::print_(const variadic_t<Type>& n, const char* op,
                          bool skip_first)
    -> void
  {
    bool first = true;
    for (const auto& i: n)
      {
        if (skip_first)
        {
          skip_first = false;
          continue;
        }
        if (! first)
          out_ << op;
        print_child_(*i, n);
        first = false;
      }
  }

#undef VISIT
#undef DEFINE

}
