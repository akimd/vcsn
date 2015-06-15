#pragma once

#include <iostream>

#include <vcsn/ctx/traits.hh>
#include <vcsn/core/rat/identities.hh>
#include <vcsn/core/rat/visitor.hh>
#include <vcsn/labelset/labelset.hh> // has_genset_mem_fn
#include <vcsn/misc/algorithm.hh> // initial_range
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/cast.hh>

namespace vcsn
{
  namespace rat
  {

    template <typename ExpSet>
    class printer
      : public ExpSet::const_visitor
    {
    public:
      using expressionset_t = ExpSet;
      using context_t = context_t_of<expressionset_t>;
      using identities_t = typename expressionset_t::identities_t;
      using labelset_t = labelset_t_of<context_t>;
      using label_t = label_t_of<context_t>;
      using weight_t = weight_t_of<context_t>;

      using super_t = typename expressionset_t::const_visitor;
      /// Actual node, without indirection.
      using node_t = typename super_t::node_t;
      /// A shared_ptr to node_t.
      using value_t = typename node_t::value_t;
      using inner_t = typename super_t::inner_t;
      template <type_t Type>
      using unary_t = typename super_t::template unary_t<Type>;
      template <type_t Type>
      using variadic_t = typename super_t::template variadic_t<Type>;
      using leaf_t = typename super_t::leaf_t;

      printer(const expressionset_t& rs,
              std::ostream& out,
              const bool debug = !!getenv("VCSN_PARENS"));

      /// Set output format.
      void format(const std::string& format);

      /// Entry point: print \a v.
      std::ostream& operator()(const node_t& v);

      /// Entry point: print \a v.
      std::ostream&
      operator()(const std::shared_ptr<const node_t>& v)
      {
        return operator()(*v);
      }

    private:

# define DEFINE(Type)                                        \
      using Type ## _t = typename super_t::Type ## _t;       \
      virtual void visit(const Type ## _t& v)

      DEFINE(atom);
      DEFINE(complement)    { print_(v, complement_); }
      DEFINE(conjunction)   { print_(v, conjunction_); }
      DEFINE(ldiv)          { print_(v, ldiv_); }
      DEFINE(lweight);
      DEFINE(one);
      DEFINE(prod)          { print_(v, product_); }
      DEFINE(rweight);
      DEFINE(shuffle)       { print_(v, shuffle_); }
      DEFINE(star)          { print_(v, star_); }
      DEFINE(sum)           { print_sum_(v); }
      DEFINE(transposition) { print_(v, transposition_); }
      DEFINE(zero);

# undef DEFINE

      /// Whether \a v is an atom whose label is a letter.
      bool is_letter_(const node_t& v) const
      {
        auto atom = dynamic_cast<const atom_t*>(&v);
        return atom && ctx_.labelset()->is_letter(atom->value());
      }

      /// Whether \a v is an atom whose label is not a letter.
      bool is_word_(const node_t& v) const
      {
        auto atom = dynamic_cast<const atom_t*>(&v);
        return atom && ! context_t::is_lat
                    && ! ctx_.labelset()->is_letter(atom->value());
      }

      /// Whether is naturally braced.
      ///
      /// This is the case of sums of letters printed as range: we
      /// want to print `[a-z]*`, not `([a-z])*`.
      bool is_braced_(const node_t& v) const
      {
        if (auto s = dynamic_cast<const sum_t*>(&v))
          {
            auto range = letter_range(s->begin(), s->end());
            return (end(range) == s->end()
                    && 3 < boost::distance(range));
          }
        else
          return false;
      }

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
      precedence_t precedence_(const node_t& v) const;

      /// Print the given child node, also knowing its parent.
      void print_child_(const node_t& child, const node_t& parent);

      /// Print a unary node.
      template <rat::exp::type_t Type>
      void print_(const unary_t<Type>& n, const char* op);

      /// Print an n-ary node.
      template <rat::exp::type_t Type>
      void print_(const variadic_t<Type>& n, const char* op);

      /// Whether the left weight shows.
      ATTRIBUTE_PURE
      bool shows_left_weight_(const node_t& n)
      {
        return n.type() == rat::type_t::lweight;
      }

      /// Return the longest range of expressions that are letters, in
      /// strictly increasing order.
      template <typename Iterator>
      auto letter_range(Iterator i, Iterator end) const
        -> boost::iterator_range<Iterator>
      {
        return detail::initial_sorted_range
          (i, end,
           [this](const value_t& c) { return is_letter_(*c); },
           [this](const value_t& lhs, const value_t& rhs)
           {
             auto l = std::dynamic_pointer_cast<const atom_t>(lhs)->value();
             auto r = std::dynamic_pointer_cast<const atom_t>(rhs)->value();
             const auto& ws = *ctx_.labelset();
             // Require strictly increasing order.
             return ws.less(l, r) || ws.equal(l, r);
           });
      }

      /// Print a sum, when the labelset has a genset() function.
      template <typename LS = labelset_t>
      auto print_sum_(const sum_t& v)
        -> enable_if_t<detail::has_genset_mem_fn<LS>{}, void>
      {
        bool first = true;
        // Use classes for sums of letters.
        for (auto i = std::begin(v), end = std::end(v);
             i != end;
             /* nothing. */)
          {
            if (! first)
              out_ << sum_;
            first = false;
            // If in front of a row of letters, in strictly increasing
            // order, issue a class.
            auto r = letter_range(i, end);
            if (3 < distance(r))
              {
                // Gather the letters.
                auto letters = std::vector<label_t>{};
                for (/* nothing. */; i != r.end(); ++i)
                  letters
                    .emplace_back(down_pointer_cast<const atom_t>(*i)->value());
                vcsn::detail::print_label_class(*ctx_.labelset(), letters,
                                                out_, format_);
              }
            else
              {
                // Otherwise, just print the child.
                print_child_(**i, v);
                ++i;
              }
          }
      }

      /// Print a sum, when the labelset does not have a genset() function.
      template <typename LS = labelset_t>
      auto print_sum_(const sum_t& v)
        -> enable_if_t<!detail::has_genset_mem_fn<LS>{}, void>
      {
        print_(v, sum_);
      }

      /// Output stream.
      std::ostream& out_;
      /// Output format.
      std::string format_;
      /// Context to decode labels and weights.
      const context_t& ctx_;
      const identities_t identities_;
      /// Whether to be overly verbose.
      const bool debug_;

      /// Left and right boundaries (typically braces for LaTeX).
      ///
      /// Used to group operand of unary operators, e.g. "a" -> "a",
      /// "a*" -> "{a}^{*}" and "a**" -> "{{a}^{*}}^{*}".
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
      /// The expression operators.
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

#include <vcsn/core/rat/printer.hxx>
