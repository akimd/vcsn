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
    printer<WeightSet>::visit(const concat<WeightSet>& v)
    {
      print_iterable(v, '.', out_, *this, show_unit_, debug_);
    }

    template <class WeightSet>
    void
    printer<WeightSet>::visit(const plus<WeightSet>& v)
    {
      print_iterable(v, '+', out_, *this, show_unit_, debug_);
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

    template <class WeightSet>
    void
    print_weight(const typename WeightSet::value_t& w,
                 const WeightSet& ws, std::ostream& out,
                 const double show_unit)
    {
      if (show_unit || !ws.is_unit(w))
        out << '{' << w << '}';
    }

    template <class Iterate>
    void
    print_iterable(const Iterate& o,
                   const char op,
                   std::ostream& out,
                   typename visitor_traits<typename Iterate::weightset_t>::ConstVisitor& v,
                   const bool show_unit,
                   const bool debug)
    {
      if (!o.empty())
      {
        print_weight(o.left_weight(), o.get_weight_set(), out, show_unit);

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

        print_weight(o.right_weight(), o.get_weight_set(), out, show_unit);
      }
    }

    template <class PostExp>
    void
    print_post_exp(const PostExp& e,
                   const char op,
                   std::ostream& out,
                   typename visitor_traits<typename PostExp::weightset_t>::ConstVisitor& v,
                   const bool show_unit,
                   const bool debug)
    {
      auto sub_exp = e.get_sub();
      if (sub_exp != nullptr)
      {
        print_weight(e.left_weight(), e.get_weight_set(), out, show_unit);
        if (debug)
          out << op;
        out << '(';
        sub_exp->accept(v);
        out << ')' << op;
        print_weight(e.right_weight(), e.get_weight_set(), out, show_unit);
      }
    }

  } // !rat
} // !vcsn

#endif // !VCSN_CORE_RAT_PRINTER_HXX
