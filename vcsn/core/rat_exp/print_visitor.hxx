#ifndef VCSN_CORE_RAT_EXP_PRINT_VISITOR_HXX
# define VCSN_CORE_RAT_EXP_PRINT_VISITOR_HXX

# include <core/rat_exp/general_print.hh>

namespace vcsn
{
  namespace rat_exp
  {

    template <class WeightSet>
    PrintVisitor<WeightSet>::PrintVisitor(std::ostream& out) :
      out_(out)
    { }

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
      print_iterable(v, '.', out_, *this);
    }

    template <class WeightSet>
    void
    PrintVisitor<WeightSet>::visit(const RatExpPlus<WeightSet>& v)
    {
      print_iterable(v, '.', out_, *this);
    }

    template <class WeightSet>
    void
    PrintVisitor<WeightSet>::visit(const RatExpKleene<WeightSet>& v)
    {
      print_post_exp(v, '*', out_, *this);
    }

    template <class WeightSet>
    void
    PrintVisitor<WeightSet>::visit(const RatExpOne<WeightSet>&)
    {
      out_ << "\\e";
    }

    template <class WeightSet>
    void
    PrintVisitor<WeightSet>::visit(const RatExpZero<WeightSet>&)
    {
      out_ << "\\z";
    }

    template <class WeightSet>
    void
    PrintVisitor<WeightSet>::visit(const RatExpWord<WeightSet>& v)
    {
      out_ << *v.get_word();
    }

  } // rat_exp
} // vcsn

#endif // !VCSN_CORE_RAT_EXP_PRINT_VISITOR_HXX
