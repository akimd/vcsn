#ifndef VCSN_CORE_RAT_PRINTER_HH
# define VCSN_CORE_RAT_PRINTER_HH

# include <iostream>
# include <vcsn/core/rat/visitor.hh>
# include <vcsn/misc/attributes.hh>
# include <vcsn/misc/cast.hh>

namespace vcsn
{
  namespace rat
  {

    template <typename RatExpSet>
    class printer
      : public RatExpSet::const_visitor
    {
    public:
      using ratexpset_t = RatExpSet;
      using context_t = typename ratexpset_t::context_t;
      using weight_t = typename context_t::weightset_t::value_t;
      using super_type = typename ratexpset_t::const_visitor;
      using node_t = typename super_type::node_t;
      using inner_t = typename super_type::inner_t;
      template <type_t Type>
      using unary_t = typename super_type::template unary_t<Type>;
      template <type_t Type>
      using variadic_t = typename super_type::template variadic_t<Type>;
      using leaf_t = typename super_type::leaf_t;

      printer(std::ostream& out,
              const ratexpset_t& rs,
              const bool debug = !!getenv("VCSN_PARENS"));

      /// Set output format.
      void format(const std::string& format);

      /// Entry point: print \a v.
      std::ostream&
      operator()(const node_t& v)
      {
        v.accept(*this);
        return out_;
      }

      /// Entry point: print \a v.
      std::ostream&
      operator()(const std::shared_ptr<const node_t>& v)
      {
        return operator()(*v);
      }

    private:

# define DEFINE(Type)                                           \
      using Type ## _t = typename super_type::Type ## _t;       \
      virtual void visit(const Type ## _t& v)

      DEFINE(atom);
      DEFINE(complement)    { print(v, complement_); }
      DEFINE(conjunction)   { print(v, conjunction_); }
      DEFINE(ldiv)          { print(v, ldiv_); }
      DEFINE(lweight);
      DEFINE(one);
      DEFINE(prod)          { print(v, product_); }
      DEFINE(rweight);
      DEFINE(shuffle)       { print(v, shuffle_); }
      DEFINE(star)          { print(v, star_); }
      DEFINE(sum)           { print(v, sum_); }
      DEFINE(transposition) { print(v, transposition_); }
      DEFINE(zero);

# undef DEFINE

      /// The possible node precedence levels, increasing.
      ///
      /// When printing a word (i.e., a label with several letters),
      /// beware that it may require parens.  Think of
      /// star(atom(ab)): if we print it as 'ab*', it actually means
      /// 'a(b*)'.  Then give words a precedence lower than that of
      /// star.  This is the role of 'word' below.
      enum class precedence_t
      {
        sum,
        shuffle,
        conjunction,
        ldiv,
        prod,
        word = prod, // Multi-letter atoms.
        lweight,
        rweight,
        star,
        complement,
        transposition,
        zero,
        one,
        atom,
      };

      /// The precedence of \a v (to decide when to print parens).
      precedence_t precedence(const node_t& v) const;

      /// Print the given child node, also knowing its parent.  If force_parens
      /// is true then print parens around it, even if they are not needed to
      /// disambiguate.
      void print_child(const node_t& child, const node_t& parent,
                       bool force_parens = false);

      /// Print a unary node.
      template <rat::exp::type_t Type>
      void print(const unary_t<Type>& n, const char* op);

      /// Print an n-ary node.
      template <rat::exp::type_t Type>
      void print(const variadic_t<Type>& n, const char* op);

      /// Whether the left weight shows.
      ATTRIBUTE_PURE
      bool shows_left_weight_(const node_t& n)
      {
        return n.type() == rat::type_t::lweight;
      }

      /// Output stream.
      std::ostream& out_;
      /// Output format.
      std::string format_;
      /// Context to decode labels and weights.
      const context_t& ctx_;
      /// Whether to be overly verbose.
      const bool debug_;

      /// Left and right boundaries (typically braces for LaTeX).
      const char* lgroup_ = nullptr;
      const char* rgroup_ = nullptr;
      /// Left and right angle brackets for weights.
      const char* langle_ = nullptr;
      const char* rangle_ = nullptr;
      /// Left and right parentheses.
      const char* lparen_ = nullptr;
      const char* rparen_ = nullptr;
      /// External product.
      const char* lmul_ = nullptr;
      const char* rmul_ = nullptr;
      /// Quotient.
      const char* ldiv_ = nullptr;
      /// The ratexp operators.
      const char* star_ = nullptr;
      const char* complement_ = nullptr;
      const char* transposition_ = nullptr;
      const char* conjunction_ = nullptr;
      const char* shuffle_ = nullptr;
      const char* product_ = nullptr;
      const char* sum_ = nullptr;
      /// The constants.
      const char* zero_ = nullptr;
      const char* one_ = nullptr;
    };

  } // namespace rat
} // namespace vcsn

# include <vcsn/core/rat/printer.hxx>

#endif // !VCSN_CORE_RAT_PRINTER_HH
