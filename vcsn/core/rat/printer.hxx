#ifndef VCSN_CORE_RAT_PRINTER_HXX
# define VCSN_CORE_RAT_PRINTER_HXX

# include <core/rat/general_print.hh>

namespace vcsn
{
  namespace rat
  {

    template <class WeightSet>
    printer<WeightSet>::printer(std::ostream& out,
                                          const bool show_unit,
                                          const bool debug)
      : out_(out)
      , show_unit_(show_unit)
      , debug_(debug)
    {}

    template <class WeightSet>
    printer<WeightSet>::~printer()
    {
      out_ << std::flush;
    }

    template <class WeightSet>
    void
    printer<WeightSet>::visit(const node<WeightSet>& v)
    {
      assert(false);
    }

    template <class WeightSet>
    void
    printer<WeightSet>::visit(const concat<WeightSet>& v)
    {
      print_iterable(v, '.', out_, *this, show_unit_, debug_);
    }

    template <class WeightSet>
    void
    printer<WeightSet>::visit(const plus<WeightSet>& v)
    {
      print_iterable(v, '.', out_, *this, show_unit_, debug_);
    }

    template <class WeightSet>
    void
    printer<WeightSet>::visit(const kleene<WeightSet>& v)
    {
      print_post_exp(v, '*', out_, *this, show_unit_, debug_);
    }

    template <class WeightSet>
    void
    printer<WeightSet>::visit(const one<WeightSet>& v)
    {
      print_weight(v.left_weight(), v.get_weight_set(), out_, show_unit_);
      out_ << "\\e";
    }

    template <class WeightSet>
    void
    printer<WeightSet>::visit(const zero<WeightSet>& v)
    {
      print_weight(v.left_weight(), v.get_weight_set(), out_, show_unit_);
      out_ << "\\z";
    }

    template <class WeightSet>
    void
    printer<WeightSet>::visit(const word<WeightSet>& v)
    {
      print_weight(v.left_weight(), v.get_weight_set(), out_, show_unit_);
      out_ <<* v.get_word();
    }

  } // !rat
} // !vcsn

#endif // !VCSN_CORE_RAT_PRINTER_HXX
