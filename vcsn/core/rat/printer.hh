#ifndef VCSN_CORE_RAT_PRINTER_HH_
# define VCSN_CORE_RAT_PRINTER_HH_

# include <iostream>
# include <core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {

    template <class Weight>
    class printer : public visitor_traits<Weight>::const_visitor
    {
    public:
      typedef Weight weight_t;
    public:
      printer(std::ostream& out,
              const bool show_unit = false, const bool debug = false);
      virtual ~printer();
    public:
      virtual void visit(const prod<weight_t>& v);
      virtual void visit(const sum<weight_t>& v);
      virtual void visit(const star<weight_t>& v);
      virtual void visit(const one<weight_t>& v);
      virtual void visit(const zero<weight_t>& v);
      virtual void visit(const word<weight_t>& v);
    private:
      void print(const weight_t& w);
      /// Traverse n-ary node (+ and .).
      void print(const nary<weight_t>& n, const char op);
      void print_left_weight(const left_weighted<weight_t>& v);

      std::ostream& out_;
      const bool show_unit_;
      const bool debug_;
    };

  } // namespace rat
} // namespace vcsn

# include <core/rat/printer.hxx>

#endif // !VCSN_CORE_RAT_PRINTER_HH_
