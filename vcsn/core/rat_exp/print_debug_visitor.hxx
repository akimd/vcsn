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
    PrintDebugVisitor<WeightSet>::PrintDebugVisitor(std::ostream &out) :
      out_(out)
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
      print_iterable<RatExpConcat<WeightSet>, true>(v, '.', out_, *this);
    }

    template <class WeightSet>
    void
    PrintDebugVisitor<WeightSet>::visit(const RatExpPlus<WeightSet>& v)
    {
      print_iterable<RatExpPlus<WeightSet>, true>(v, '+', out_, *this);
    }

    template <class WeightSet>
    void
    PrintDebugVisitor<WeightSet>::visit(const RatExpKleene<WeightSet>& v)
    {
      print_weight(v.left_weight(), v.get_weight_set(), out_);
      const RatExpNode<WeightSet> * sub_exp = v.getSubNode();
      if (sub_exp != nullptr)
      {
        out_ << "*(";
        sub_exp->accept(*this);
        out_ << ")*";
      }
      print_weight(v.right_weight(), v.get_weight_set(), out_);
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
      print_weight(v.left_weight(), v.get_weight_set(), out_);
      out_ << "w(" << *v.get_word() << ')';
    }

  } // rat_exp
} // vcsn

#endif // !VCSN_CORE_RAT_EXP_PRINT_DEBUG_VISITOR_HXX
