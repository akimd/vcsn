#ifndef VCSN_CORE_RAT_PRINTER_HH_
# define VCSN_CORE_RAT_PRINTER_HH_

# include <iostream>
# include <core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {

    template <class WeightSet>
    class printer : public visitor_traits<WeightSet>::ConstVisitor
    {
    public:
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
    public:
      printer(std::ostream& out,
              const bool show_unit = false, const bool debug = false);
      virtual ~printer();
    public:
      virtual void visit(const concat<WeightSet>& v);
      virtual void visit(const plus<WeightSet>& v);
      virtual void visit(const kleene<WeightSet>& v);
      virtual void visit(const one<WeightSet>& v);
      virtual void visit(const zero<WeightSet>& v);
      virtual void visit(const word<WeightSet>& v);
    private:
      void print_weight(const typename WeightSet::value_t& w,
                        const WeightSet& ws);
      /// Traverse n-ary node (+ and .).
      template <typename NAry>
      void print_iterable(const NAry& n, const char op);

      std::ostream& out_;
      const bool show_unit_;
      const bool debug_;
    };

  } // rat
} // vcsn

# include <core/rat/printer.hxx>

#endif // !VCSN_CORE_RAT_PRINTER_HH_
