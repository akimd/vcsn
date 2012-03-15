#ifndef VCSN_CORE_RAT_EXPE_GENERAL_PRINT_HH
# define VCSN_CORE_RAT_EXPE_GENERAL_PRINT_HH

# include <iostream>

# include <core/rat_exp/visitor.hh>

namespace vcsn
{
  namespace rat_exp
  {

    template <class WeightSet>
    void
    print_weight(const typename WeightSet::value_t& w, const WeightSet& ws, std::ostream& out)
    {
      if (!ws.is_unit(w))
      {
        out << '{'
            << w
            << '}';
      }
    }

    template <bool debug = false, class Iterate>
    void
    print_iterable(const Iterate& o,
                   const char op,
                   std::ostream& out,
                   typename visitor_traits<typename Iterate::weightset_t>::ConstVisitor& v)
    {
      unsigned size = o.size();
      if (0 != size)
      {
        print_weight(o.left_weight(), o.get_weight_set(), out);

        if (debug)
          out << op;
        for (unsigned i = o.size(); i != 0; --i)
          out << '(';

        auto end = o.end();
        auto it = o.begin();
        (* it)->accept(v);
        ++it;
        for(; end != it; ++it)
        {
          out << op;
          (* it)->accept(v);
          out << ')';
        }
        out << ')';

        print_weight(o.right_weight(), o.get_weight_set(), out);
      }
    }

    template <bool debug = false, class PostExp>
    void
    print_post_exp(const PostExp& e,
                   const char op,
                   std::ostream& out,
                   typename visitor_traits<typename PostExp::weightset_t>::ConstVisitor& v)
    {
      auto sub_exp = e.get_sub();
      if (sub_exp != nullptr)
      {
        print_weight(e.left_weight(), e.get_weight_set(), out);
        if (debug)
          out << op;
        out << '(';
        sub_exp->accept(v);
        out << ')' << op;
        print_weight(e.right_weight(), e.get_weight_set(), out);
      }
    }

  } // rat_exp
} // vcsn


#endif // VCSN_CORE_RAT_EXPE_GENERAL_PRINT_HH
