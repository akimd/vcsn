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
      using nary_t = typename super_type::template nary_t<Type>;
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
        out_ << ldelim_;
        v.accept(*this);
        out_ << rdelim_;
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

      DEFINE(intersection);
      DEFINE(prod);
      DEFINE(sum);
      DEFINE(shuffle);
      DEFINE(star);
      DEFINE(one);
      DEFINE(zero);
      DEFINE(atom);

# undef DEFINE

      /// The possible node precedence levels, increasing.
      enum class precedence_t
        {
          sum,
          shuffle,
          intersection,
          prod,
          word, // There's no corresponding type in this case.
          star,
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

      /// Print \a w if needed.
      void print_left_weight(const node_t& w);

      /// Print \a w if needed.
      void print_right_weight(const inner_t& w);

      /// Print an n-ary node.
      template <rat::exp::type_t Type>
      void print(const nary_t<Type>& n, const char* op);

      /// Whether w is displayed.
      ATTRIBUTE_PURE
      bool shows_(const weight_t& w)
      {
        return !ctx_.weightset()->is_one(w);
      }

      /// Whether the left weight shows.
      ATTRIBUTE_PURE
      bool shows_left_weight_(const node_t& n)
      {
        return shows_(n.left_weight());
      }

      /// Whether the right weight shows.
      ATTRIBUTE_PURE
      bool shows_right_weight_(const node_t& n)
      {
        return (n.is_inner()
                && shows_(static_cast<const inner_t&>(n).right_weight()));
      }

      /// Whether one of the weights shows.
      ATTRIBUTE_PURE
      bool shows_weight_(const node_t& n)
      {
        return shows_left_weight_(n) || shows_right_weight_(n);
      }

      /// Output stream.
      std::ostream& out_;
      const context_t& ctx_;
      /// Whether to be overly verbose.
      const bool debug_;

      /// Left and right output delimiters for the whole ratexp.
      const char* ldelim_ = "";
      const char* rdelim_ = "";
      /// Left and right brackets for weights.
      const char* lbracket_ = "<";
      const char* rbracket_ = ">";
      /// Left and right parentheses.
      const char* lparen_ = "(";
      const char* rparen_ = ")";
      /// External product.
      const char* lmul_ = "";
      const char* rmul_ = "";
      /// The ratexp operators.
      const char* star_ = "*";
      const char* intersection_ = "&";
      const char* shuffle_ = ":";
      const char* product_ = ".";
      const char* sum_ = "+";
      /// The constants.
      const char* zero_ = "\\z";
      const char* one_ = "\\e";
    };

  } // namespace rat
} // namespace vcsn

# include <vcsn/core/rat/printer.hxx>

#endif // !VCSN_CORE_RAT_PRINTER_HH
