#pragma once

#include <iostream>

#include <vcsn/algos/project.hh> // bad layering: should not be in algos.
#include <vcsn/core/rat/identities.hh>
#include <vcsn/core/rat/visitor.hh>
#include <vcsn/ctx/traits.hh>
#include <vcsn/labelset/labelset.hh> // has_generators_mem_fn
#include <vcsn/misc/algorithm.hh> // initial_range
#include <vcsn/misc/attributes.hh>
#include <vcsn/misc/builtins.hh>
#include <vcsn/misc/cast.hh>
#include <vcsn/misc/format.hh>

namespace vcsn
{
  namespace rat
  {
    /// The possible node precedence levels, increasing.
    ///
    /// When printing a word (i.e., a label with several letters),
    /// beware that it may require parens.  Think of star(atom(ab)):
    /// if we print it as 'ab*', it actually means 'a(b*)'.  So give
    /// words a lower precedence than star.  This is the role of
    /// 'word' below.
    ///
    /// Was part of printer<ExpSet>, but it resulted in different
    /// incompatible types between, for instance,
    /// printer<lat<lal_char, lal_char>> and printer<lal_char>, which
    /// is a problem when the former calls print_child of the latter,
    /// passing an argument of type precedence_t.
    enum class precedence_t
    {
      sum,
      tuple,
      products,
      shuffle = products,
      conjunction = products,
      infiltration = products,
      ldiv,
      prod,
      word = prod, // Multi-letter atoms.
      lweight,
      rweight,
      unary, // All the unary (postfix) operators.
      star = unary,
      complement = unary,
      transposition = unary,
      exponent = unary,
      zero,
      one,
      atom,
    };

    /// Exponents in UTF-8.
    constexpr static const char* const superscripts[] =
      {
        "⁰", "¹", "²", "³", "⁴", "⁵", "⁶", "⁷", "⁸", "⁹"
      };

    /// Pretty-printer for rational expressions.
    template <typename ExpSet>
    class printer
      : public ExpSet::const_visitor
    {
    public:
      using expressionset_t = ExpSet;
      using super_t = typename expressionset_t::const_visitor;
      using self_t = printer;

      using context_t = context_t_of<expressionset_t>;
      using identities_t = typename expressionset_t::identities_t;
      using labelset_t = labelset_t_of<context_t>;
      using label_t = label_t_of<context_t>;
      using weight_t = weight_t_of<context_t>;

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

      /// Name of this algorithm, for error messages.
      constexpr static const char* me() { return "print"; }

      /// A printer.
      printer(const expressionset_t& rs, std::ostream& out);

      /// Set output format.
      void format(format fmt);

      /// Entry point: print \a v.
      std::ostream&
      operator()(const value_t& v)
      {
        return print_(*v);
      }

      /// Print a child node, given its parent's precedence.
      ///
      /// Public function, to support tuples.
      void print_child(const node_t& child, precedence_t parent);

    private:
      /// Print \a v.
      std::ostream& print_(const node_t& v);

      /// Print a weight.
      void print_(const weight_t& w)
      {
        rs_.weightset()->print(w, out_, fmt_.for_weights());
      }

      VCSN_RAT_VISIT(atom, v);
      VCSN_RAT_VISIT(complement, v)    { print_(v, complement_); }
      VCSN_RAT_VISIT(conjunction, v)   { print_(v, conjunction_); }
      VCSN_RAT_VISIT(infiltration, v)  { print_(v, infiltration_); }
      VCSN_RAT_VISIT(ldiv, v)          { print_(v, ldiv_); }
      VCSN_RAT_VISIT(lweight, v);
      VCSN_RAT_VISIT(one, v);
      VCSN_RAT_VISIT(prod, v);
      VCSN_RAT_VISIT(rweight, v);
      VCSN_RAT_VISIT(shuffle, v)       { print_(v, shuffle_); }
      VCSN_RAT_VISIT(star, v)          { print_(v, star_); }
      VCSN_RAT_VISIT(sum, v)           { print_sum_(v); }
      VCSN_RAT_VISIT(transposition, v) { print_(v, transposition_); }
      VCSN_RAT_VISIT(zero, v);

      using tuple_t = typename super_t::tuple_t;

      template <bool = context_t::is_lat,
                typename Dummy = void>
      struct visit_tuple
      {
        /// Print one tape.
        template <size_t I>
        void print_(const tuple_t& v)
        {
          if (I)
            visitor_.out_ << visitor_.tuple_middle;
          auto rs = detail::project<I>(visitor_.rs_);
          auto print = make_printer(rs, visitor_.out_);
          print.format(visitor_.fmt_);
          print.print_child(*std::get<I>(v.sub()), precedence_t::tuple);
        }

        /// Print all the tapes.
        template <size_t... I>
        void print_(const tuple_t& v, detail::index_sequence<I...>)
        {
          using swallow = int[];
          (void) swallow
          {
            (print_<I>(v),
             0)...
          };
        }

        /// Entry point.
        void operator()(const tuple_t& v)
        {
          visitor_.out_ << visitor_.tuple_left;
          print_(v, labelset_t::indices);
          visitor_.out_ << visitor_.tuple_right;
        }
        const self_t& visitor_;
      };

      template <typename Dummy>
      struct visit_tuple<false, Dummy>
      {
        void operator()(const tuple_t&)
        {
          BUILTIN_UNREACHABLE();
        }
        const self_t& visitor_;
      };

      void visit(const tuple_t& v, std::true_type) override
      {
        visit_tuple<>{*this}(v);
      }


      /// Whether \a v is an atom whose label is a letter.
      ///
      /// Used to know if we can print a sum of expressions as a
      /// letter range.
      bool is_letter_(const node_t& v) const
      {
        auto atom = dynamic_cast<const atom_t*>(&v);
        return atom && rs_.labelset()->is_letter(atom->value());
      }

      /// Whether \a v is an atom whose label is not a letter.
      ///
      /// Used to decide when to issues parens via precedence
      /// ("letter" and "word" have different precedence).  Actually,
      /// this routine checks whether this node is a self-delimited
      /// atom.  Letters are of course self-delimited, so we don't add
      /// parens to "a" in "a*".  Tuples printed as labels are not, so
      /// we need to add parens to "a|x" in "(a|x)*".
      ///
      /// Note that 1-tape tuple are self-delimited (well, if what
      /// they contain is self-delimited).  But then visually there is
      /// no difference between a lal and a lat<lal>.  I think there
      /// should be one, so let's keep it this way.  Likewise for
      /// 0-tape tuples.
      bool is_word_(const node_t& v) const
      {
        auto atom = dynamic_cast<const atom_t*>(&v);
        return (atom
                && (context_t::is_lat
                    || ! rs_.labelset()->is_letter(atom->value())));
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

      /// The precedence of \a v (to decide when to print parens).
      precedence_t precedence_(const node_t& v) const;

      /// Print a child node, given its parent.
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
             const auto& ls = *rs_.labelset();
             // Require strictly increasing order.
             return ls.less(l, r) || ls.equal(l, r);
           });
      }

      /// Print a sum, when the labelset has a genset() function.
      template <typename LS = labelset_t>
      auto print_sum_(const sum_t& v)
        -> std::enable_if_t<detail::has_generators_mem_fn<LS>{}, void>
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
                vcsn::detail::print_label_class(*rs_.labelset(), letters,
                                                out_, fmt_);
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
        -> std::enable_if_t<!detail::has_generators_mem_fn<LS>{}, void>
      {
        print_(v, sum_);
      }

      /// Output stream.
      std::ostream& out_;
      /// Output format.
      class format fmt_;
      /// The expressionset.
      const expressionset_t& rs_;
      /// Whether to be overly verbose.
      const bool debug_ = !!getenv("VCSN_PARENS");

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
      /// Left and right braces for exponents.
      const char* lexponent_ = nullptr;
      const char* rexponent_ = nullptr;
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
      const char* infiltration_ = nullptr;
      const char* shuffle_ = nullptr;
      const char* product_ = nullptr;
      const char* sum_ = nullptr;

      /// Left tuple delimiter.
      const char* tuple_left = nullptr;
      /// Tuple tape separator.
      const char* tuple_middle = nullptr;
      /// Right tuple delimiter.
      const char* tuple_right = nullptr;

      /// The constants.
      const char* zero_ = nullptr;
      const char* one_ = nullptr;
      unsigned int exponent_threshold_ = 0;
    };

    template <typename ExpSet>
    printer<ExpSet>
    make_printer(const ExpSet& rs, std::ostream& out)
    {
      return {rs, out};
    }
  } // namespace rat
} // namespace vcsn

#include <vcsn/core/rat/printer.hxx>
