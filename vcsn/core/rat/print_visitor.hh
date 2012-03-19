#ifndef VCSN_CORE_RAT_PRINT_VISITOR_HH_
# define VCSN_CORE_RAT_PRINT_VISITOR_HH_

# include <iostream>
# include <core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {

    template <class WeightSet>
    class PrintVisitor : public visitor_traits<WeightSet>::ConstVisitor
    {
    public:
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
    public:
      PrintVisitor(std::ostream& out, const bool show_unit = false, const bool debug = false);
      virtual ~PrintVisitor();
    public:
      virtual void visit(const node<WeightSet>& v);
      virtual void visit(const concat<WeightSet>& v);
      virtual void visit(const plus<WeightSet>& v);
      virtual void visit(const kleene<WeightSet>& v);
      virtual void visit(const one<WeightSet>& v);
      virtual void visit(const zero<WeightSet>& v);
      virtual void visit(const word<WeightSet>& v);
    private:
      std::ostream &out_;
      const bool show_unit_;
      const bool debug_;
    };

  } // rat
} // vcsn

# include <core/rat/print_visitor.hxx>

#endif // !VCSN_CORE_RAT_PRINT_VISITOR_HH_
