#ifndef VCSN_CORE_RAT_EXP_PRINT_VISITOR_HXX
# define VCSN_CORE_RAT_EXP_PRINT_VISITOR_HXX

# include <core/rat/general_print.hh>

namespace vcsn
{
  namespace rat
  {

    template <class WeightSet>
    PrintVisitor<WeightSet>::PrintVisitor(std::ostream& out,
                                          const bool show_unit,
                                          const bool debug)
      : out_(out)
      , show_unit_(show_unit)
      , debug_(debug)
    {}

    template <class WeightSet>
    PrintVisitor<WeightSet>::~PrintVisitor()
    {
      out_ << std::flush;
    }

    template <class WeightSet>
    void
    PrintVisitor<WeightSet>::visit(const RatExpNode<WeightSet>& v)
    {
      assert(false);
    }

    template <class WeightSet>
    void
    PrintVisitor<WeightSet>::visit(const RatExpConcat<WeightSet>& v)
    {
      print_iterable(v, '.', out_, *this, show_unit_, debug_);
    }

    template <class WeightSet>
    void
    PrintVisitor<WeightSet>::visit(const RatExpPlus<WeightSet>& v)
    {
      print_iterable(v, '.', out_, *this, show_unit_, debug_);
    }

    template <class WeightSet>
    void
    PrintVisitor<WeightSet>::visit(const RatExpKleene<WeightSet>& v)
    {
      print_post_exp(v, '*', out_, *this, show_unit_, debug_);
    }

    template <class WeightSet>
    void
    PrintVisitor<WeightSet>::visit(const RatExpOne<WeightSet>& v)
    {
      print_weight(v.left_weight(), v.get_weight_set(), out_, show_unit_);
      out_ << "\\e";
    }

    template <class WeightSet>
    void
    PrintVisitor<WeightSet>::visit(const RatExpZero<WeightSet>& v)
    {
      print_weight(v.left_weight(), v.get_weight_set(), out_, show_unit_);
      out_ << "\\z";
    }

    template <class WeightSet>
    void
    PrintVisitor<WeightSet>::visit(const RatExpWord<WeightSet>& v)
    {
      print_weight(v.left_weight(), v.get_weight_set(), out_, show_unit_);
      out_ <<* v.get_word();
    }

  } // !rat
} // !vcsn

#endif // !VCSN_CORE_RAT_EXP_PRINT_VISITOR_HXX
