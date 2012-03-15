#ifndef VCSN_CORE_RAT_EXP_PRINT_DEBUG_VISITOR_HXX
# define VCSN_CORE_RAT_EXP_PRINT_DEBUG_VISITOR_HXX

# include <cassert>

# include <core/rat_exp/print_debug_visitor.hh>
# include <core/rat_exp/node.hh>
# include <core/rat_exp/general_print.hh>

namespace vcsn
{
  namespace rat_exp
  {

    template <class WeightSet>
    PrintDebugVisitor<WeightSet>::PrintDebugVisitor(std::ostream &out,
                                                    const bool show_unit)
      : out_(out)
      , show_unit_(show_unit)
    { }

    template <class WeightSet>
    PrintDebugVisitor<WeightSet>::~PrintDebugVisitor()
    {
      out_ << std::flush;
    }

    template <class WeightSet>
    void
    PrintDebugVisitor<WeightSet>::visit(const RatExpNode<WeightSet>&)
    {
      assert(false);
    }

    template <class WeightSet>
    void
    PrintDebugVisitor<WeightSet>::visit(const RatExpConcat<WeightSet>& v)
    {
      print_iterable<true>(v, '.', out_, *this, show_unit_);
    }

    template <class WeightSet>
    void
    PrintDebugVisitor<WeightSet>::visit(const RatExpPlus<WeightSet>& v)
    {
      print_iterable<true>(v, '+', out_, *this, show_unit_);
    }

    template <class WeightSet>
    void
    PrintDebugVisitor<WeightSet>::visit(const RatExpKleene<WeightSet>& v)
    {
      print_post_exp<true>(v, '*', out_, *this, show_unit_);
    }

    template <class WeightSet>
    void
    PrintDebugVisitor<WeightSet>::visit(const RatExpOne<WeightSet>&)
    {
      out_ << "\\e";
    }

    template <class WeightSet>
    void
    PrintDebugVisitor<WeightSet>::visit(const RatExpZero<WeightSet>&)
    {
      out_ << "\\z";
    }

    template <class WeightSet>
    void
    PrintDebugVisitor<WeightSet>::visit(const RatExpWord<WeightSet>& v)
    {
      print_weight(v.left_weight(), v.get_weight_set(), out_, show_unit_);
      out_ << "w(" << *v.get_word() << ')';
    }

  } // rat_exp
} // vcsn

#endif // !VCSN_CORE_RAT_EXP_PRINT_DEBUG_VISITOR_HXX
