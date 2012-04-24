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

      printer(std::ostream& out,
              const weightset_t& ws,
              const bool debug = false);
      virtual ~printer();

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

      std::ostream& out_;
      const weightset_t& ws_;
      const bool debug_;
    };

  } // namespace rat
} // namespace vcsn

# include <vcsn/core/rat/printer.hxx>

#endif // !VCSN_CORE_RAT_PRINTER_HH
