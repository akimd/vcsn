#ifndef VCSN_CORE_RAT_PRINTER_HH
# define VCSN_CORE_RAT_PRINTER_HH

# include <iostream>
# include <vcsn/core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {

    template <class WeightSet>
    class printer
      : public const_visitor<typename WeightSet::value_t>
    {
    public:
      using weightset_t = WeightSet;
      using weight_t = typename weightset_t::value_t;
      using super_type = const_visitor<weight_t>;
      using node_t = node<weight_t>;

      printer(std::ostream& out,
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
      virtual void visit(const prod<weight_t>& v);
      virtual void visit(const sum<weight_t>& v);
      virtual void visit(const star<weight_t>& v);
      virtual void visit(const one<weight_t>& v);
      virtual void visit(const zero<weight_t>& v);
      virtual void visit(const atom<weight_t>& v);

      void print(const weight_t& w);
      /// Traverse n-ary node (+ and .).
      void print(const nary<weight_t>& n, const char op);

      /// Whether the visited node, if sum, prod, or star, requires
      /// outer parens.  The top level node does not these parens,
      /// unless debug mode, or is an inner node with weights.
      bool parens_(const inner<weight_t>& n)
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
