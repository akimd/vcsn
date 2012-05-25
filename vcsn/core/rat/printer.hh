#ifndef VCSN_CORE_RAT_PRINTER_HH
# define VCSN_CORE_RAT_PRINTER_HH

# include <iostream>
# include <vcsn/core/rat/visitor.hh>
# include <vcsn/misc/cast.hh>

namespace vcsn
{
  namespace rat
  {

    template <typename GenSet, typename WeightSet, typename Kind>
    class printer
      : public const_visitor<typename atom_trait<Kind, GenSet>::type,
                             typename WeightSet::value_t>
    {
    public:
      using genset_t = GenSet;
      using weightset_t = WeightSet;
      using kind_t = Kind;
      using atom_value_t = typename atom_trait<kind_t, genset_t>::type;
      using weight_t = typename weightset_t::value_t;
      using super_type = const_visitor<atom_value_t, weight_t>;
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
              const genset_t& gs,
              const weightset_t& ws,
              const bool debug = false);

      /// Entry point: print \a v.
      void
      operator()(const node_t& v)
      {
        top_ = true;
        v.accept(*this);
        out_ << std::flush;
      }

      /// Entry point: print \a v.
      void
      operator()(std::shared_ptr<const node_t> v)
      {
        operator()(*v);
      }

    private:
      virtual void visit(const prod_t& v);
      virtual void visit(const sum_t& v);
      virtual void visit(const star_t& v);
      virtual void visit(const one_t& v);
      virtual void visit(const zero_t& v);
      virtual void visit(const atom_t& v);

      void print(const weight_t& w);
      /// Traverse n-ary node (+ and .).
      void print(const nary_t& n, const char op);

      /// Whether the visited node, if sum, prod, or star, requires
      /// outer parens.  The top level node does not need parens,
      /// unless debug mode, or is a sum/prod/star node with weights.
      bool parens_(const inner_t& n)
      {
        bool res =
          !top_
          || ws_.show_unit()
          || !ws_.is_unit(n.left_weight())
          || !ws_.is_unit(n.right_weight());
        top_ = false;
        return res;
      }

      /// Output stream.
      std::ostream& out_;
      const genset_t& gs_;
      const weightset_t& ws_;
      /// Whether to be overly verbose.
      const bool debug_;
      /// Whether the visited node is the top-level node.  Used by
      /// parens_.
      bool top_ = true;
    };

  } // namespace rat
} // namespace vcsn

# include <vcsn/core/rat/printer.hxx>

#endif // !VCSN_CORE_RAT_PRINTER_HH
