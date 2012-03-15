#ifndef VCSN_CORE_RAT_EXP_PRINT_VISITOR_HXX
# define VCSN_CORE_RAT_EXP_PRINT_VISITOR_HXX

namespace vcsn
{
  namespace rat_exp
  {

    template <class WeightSet>
    PrintVisitor<WeightSet>::PrintVisitor(std::ostream &out) :
      out_(out)
    { }

    template <class WeightSet>
    PrintVisitor<WeightSet>::~PrintVisitor()
    {
      out_ << std::flush;
    }

    template <class WeightSet>
    void
    PrintVisitor<WeightSet>::visit(const RatExpNode<WeightSet> &v)
    {
      assert(false);
    }

    template <class WeightSet>
    void
    PrintVisitor<WeightSet>::visit(const RatExpConcat<WeightSet> &v)
    {
      print_weight(v.left_weight());
      for (unsigned i = v.size(); i != 1; --i)
        out_ << '(';

      typename RatExpConcat<WeightSet>::const_iterator end = v.end();
      typename RatExpConcat<WeightSet>::const_iterator it  = v.begin();
      (* it)->accept(*this);
      ++it;
      for (; end != it; ++it)
      {
        out_ << '.';
        (* it)->accept(*this);
        out_ << ')';
      }
      print_weight(v.right_weight());
    }

    template <class WeightSet>
    void
    PrintVisitor<WeightSet>::visit(const RatExpPlus<WeightSet> &v)
    {
      print_weight(v.left_weight());
      for (unsigned i = v.size(); i != 1; --i)
        out_ << '(';

      typename RatExpConcat<WeightSet>::const_iterator end = v.end();
      typename RatExpConcat<WeightSet>::const_iterator it  = v.begin();
      (* it)->accept(*this);
      ++it;
      for (; end != it; ++it)
      {
        out_ << '+';
        (* it)->accept(*this);
        out_ << ')';
      }
      print_weight(v.right_weight());
    }

    template <class WeightSet>
    void
    PrintVisitor<WeightSet>::visit(const RatExpKleene<WeightSet> &v)
    {
      print_weight(v.left_weight());
      RatExpNode<WeightSet> *sub_exp = v.get_sub();
      if (sub_exp != nullptr)
      {
        out_ << '(';
        sub_exp->accept(*this);
        out_ << ")*";
      }
      print_weight(v.right_weight());
    }

    template <class WeightSet>
    void
    PrintVisitor<WeightSet>::visit(const RatExpOne<WeightSet> &v)
    {
      out_ << "\\e";
    }

    template <class WeightSet>
    void
    PrintVisitor<WeightSet>::visit(const RatExpZero<WeightSet> &v)
    {
      out_ << "\\z";
    }

    template <class WeightSet>
    void
    PrintVisitor<WeightSet>::visit(const RatExpWord<WeightSet> &v)
    {
      out_ << *v.get_word();
    }

    template<class WeightSet>
    void
    PrintVisitor<WeightSet>::print_weight(const weight_t &w)
    {
      if (!WeightSet::is_unit(w))
      {
        out_ << '{'
             << w
             << '}';
      }
    }

  } // rat_exp
} // vcsn

#endif // !VCSN_CORE_RAT_EXP_PRINT_VISITOR_HXX
