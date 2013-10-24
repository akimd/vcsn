#ifndef VCSN_CORE_RAT_PRINTER_HH
# define VCSN_CORE_RAT_PRINTER_HH

# include <iostream>
# include <vcsn/core/rat/visitor.hh>
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
      using nary_t = typename super_type::nary_t;
      using prod_t = typename super_type::prod_t;
      using sum_t = typename super_type::sum_t;
      using leaf_t = typename super_type::leaf_t;
      using star_t = typename super_type::star_t;
      using zero_t = typename super_type::zero_t;
      using one_t = typename super_type::one_t;
      using atom_t = typename super_type::atom_t;

      printer(std::ostream& out,
              const ratexpset_t& rs,
              const bool debug = !!getenv("VCSN_DEBUG"));

      /// Entry point: print \a v.
      std::ostream&
      operator()(const node_t& v)
      {
        v.accept(*this);
        out_ << std::flush;
        return out_;
      }

      /// Entry point: print \a v.
      std::ostream&
      operator()(const std::shared_ptr<const node_t>& v)
      {
        return operator()(*v);
      }

    private:
      virtual void visit(const prod_t& v);
      virtual void visit(const sum_t& v);
      virtual void visit(const star_t& v);
      virtual void visit(const one_t& v);
      virtual void visit(const zero_t& v);
      virtual void visit(const atom_t& v);

      /// The possible node precedence levels.
      enum class precedence_t
        {
          sum,
          prod,
          word, // There's no corresponding type in this case.
          star,
          zero,
          one,
          atom,
        };

      /// We use node precedence to decide when to print parens.
      precedence_t precedence(const node_t& v)
      {
        const atom_t* atom = dynamic_cast<const atom_t*>(&v);
        if (atom && ! ctx_.labelset()->is_letter(atom->value()))
          return precedence_t::word;
        else
          switch (v.type())
            {
# define CASE(Type) \
  case exp::type_t::Type: \
    return precedence_t::Type;
            CASE(sum);
            CASE(prod);
            CASE(star);
            CASE(zero);
            CASE(one);
            CASE(atom);
# undef CASE
            }
        abort(); // Unreachable.
      }

      /// Print the given child node, also knowing its parent.  If force_parens
      /// is true then print parens around it, even if they are not needed to
      /// disambiguate.
      void print_child(const node_t& child, const node_t& parent,
                       bool force_parens = false);

      void print(const weight_t& w);
      /// Traverse n-ary node (+ and .).
      void print(const nary_t& n, const char op);

      /// Whether w is displayed.
      bool shows_(const weight_t& w)
      {
        return !ctx_.weightset()->is_one(w);
      }

      /// Whether the left weight shows.
      bool shows_left_weight_(const node_t& n)
      {
        return shows_(n.left_weight());
      }

      /// Whether the right weight shows.
      bool shows_right_weight_(const node_t& n)
      {
        return (n.is_inner()
                && shows_(static_cast<const inner_t&>(n).right_weight()));
      }

      /// Whether one of the weights shows.
      bool shows_weight_(const node_t& n)
      {
        return shows_left_weight_(n) || shows_right_weight_(n);
      }

      /// Output stream.
      std::ostream& out_;
      const context_t& ctx_;
      /// Whether to be overly verbose.
      const bool debug_;
    };

  } // namespace rat
} // namespace vcsn

# include <vcsn/core/rat/printer.hxx>

#endif // !VCSN_CORE_RAT_PRINTER_HH
