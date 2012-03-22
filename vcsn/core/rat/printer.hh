#ifndef VCSN_CORE_RAT_PRINTER_HH_
# define VCSN_CORE_RAT_PRINTER_HH_

# include <iostream>
# include <core/rat/visitor.hh>

namespace vcsn
{
  namespace rat
  {

    template <class WeightSet>
    class printer
      : public visitor_traits<typename WeightSet::value_t>::const_visitor
    {
    public:
      typedef WeightSet weightset_t;
      typedef typename weightset_t::value_t weight_t;
      typedef typename visitor_traits<weight_t>::const_visitor super_type;

    public:
      printer(std::ostream& out,
              const weightset_t& ws,
              const bool show_unit = false, const bool debug = false);
      virtual ~printer();
    public:
      virtual void visit(const prod<weight_t>& v);
      virtual void visit(const sum<weight_t>& v);
      virtual void visit(const star<weight_t>& v);
      virtual void visit(const one<weight_t>& v);
      virtual void visit(const zero<weight_t>& v);
      virtual void visit(const atom<weight_t>& v);
    private:
      void print(const weight_t& w);
      /// Traverse n-ary node (+ and .).
      void print(const nary<weight_t>& n, const char op);
      void print_left_weight(const leaf<weight_t>& v);

      std::ostream& out_;
      const weightset_t& ws_;
      const bool show_unit_;
      const bool debug_;
    };

  } // namespace rat
} // namespace vcsn

# include <core/rat/printer.hxx>

#endif // !VCSN_CORE_RAT_PRINTER_HH_
