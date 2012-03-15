#ifndef VCSN_CORE_RAT_EXP_PRINT_DEBUG_VISITOR_HXX
# define VCSN_CORE_RAT_EXP_PRINT_DEBUG_VISITOR_HXX

# include <cassert>

# include <core/rat_exp/print_debug_visitor.hh>
# include <core/rat_exp/node.hh>

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
      print_weight(v.left_weight(), v.get_weight_set());
      out_ << '.';
      for (unsigned i = v.size(); i != 0; --i)
        out_ << '(';

      auto end = v.end();
      auto it  = v.begin();
      (* it)->accept(*this);
      ++it;
      for (; end != it; ++it)
      {
        out_ << '.';
        (* it)->accept(*this);
        out_ << ')';
      }
      out_ << ')';
      print_weight(v.right_weight(), v.get_weight_set());
    }

    template <class WeightSet>
    void
    PrintDebugVisitor<WeightSet>::visit(const RatExpPlus<WeightSet>& v)
    {
      print_weight(v.left_weight(), v.get_weight_set());
      out_ << '+';
      for (unsigned i = v.size(); i != 0; --i)
        out_ << '(';

      auto end = v.end();
      auto it  = v.begin();
      (* it)->accept(*this);
      ++it;
      for (; end != it; ++it)
      {
        out_ << '+';
        (* it)->accept(*this);
        out_ << ')';
      }
      out_ << ')';
      print_weight(v.right_weight(), v.get_weight_set());
    }

    template <class WeightSet>
    void
    PrintDebugVisitor<WeightSet>::visit(const RatExpKleene<WeightSet>& v)
    {
      print_weight(v.left_weight(), v.get_weight_set());
      const RatExpNode<WeightSet> * sub_exp = v.getSubNode();
      if (sub_exp != nullptr)
      {
        out_ << "*(";
        sub_exp->accept(*this);
        out_ << ")*";
      }
      print_weight(v.right_weight(), v.get_weight_set());
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
      print_weight(v.left_weight(), v.get_weight_set());
      out_ << "w(" << *v.get_word() << ')';
    }

    template<class WeightSet>
    void
    PrintDebugVisitor<WeightSet>::print_weight(const weight_t& w, const weightset_t& ws)
    {
      if (!ws.is_unit(w))
      {
        out_ << '{'
             << w
             << '}';
      }
    }

  } // rat_exp
} // vcsn

#endif // !VCSN_CORE_RAT_EXP_PRINT_DEBUG_VISITOR_HXX
