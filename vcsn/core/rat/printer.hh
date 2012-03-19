#ifndef VCSN_CORE_RAT_PRINTER_HH_
# define VCSN_CORE_RAT_PRINTER_HH_

# include <iostream>
# include <core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {

    template <class WeightSet>
    class printer : public visitor_traits<WeightSet>::const_visitor
    {
    public:
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
    public:
      printer(std::ostream& out,
              const bool show_unit = false, const bool debug = false);
      virtual ~printer();
    public:
      virtual void visit(const concat<weightset_t>& v);
      virtual void visit(const plus<weightset_t>& v);
      virtual void visit(const kleene<weightset_t>& v);
      virtual void visit(const one<weightset_t>& v);
      virtual void visit(const zero<weightset_t>& v);
      virtual void visit(const word<weightset_t>& v);
    private:
      void print_weight(const weight_t& w, const weightset_t& ws);
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
