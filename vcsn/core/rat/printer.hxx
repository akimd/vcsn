#ifndef VCSN_CORE_RAT_PRINTER_HXX
# define VCSN_CORE_RAT_PRINTER_HXX

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
    printer<WeightSet>::visit(const prod<WeightSet>& v)
    {
      print(v, '.');
    }

    template <class WeightSet>
    void
    printer<WeightSet>::visit(const sum<WeightSet>& v)
    {
      print(v, '+');
    }

    template <class WeightSet>
    void
    printer<WeightSet>::visit(const star<WeightSet>& v)
    {
      const auto& sub = v.get_sub();
      assert(sub);
      print(v.left_weight(), v.get_weight_set());
      if (debug_)
        out_ << '*';
      out_ << '(';
      sub->accept(*this);
      out_ << ")*";
      print(v.right_weight(), v.get_weight_set());
    }

    template <class WeightSet>
    void
    printer<WeightSet>::visit(const one<WeightSet>& v)
    {
      print_left_weight(v);
      out_ << "\\e";
    }

    template <class WeightSet>
    void
    printer<WeightSet>::visit(const zero<WeightSet>& v)
    {
      print_left_weight(v);
      out_ << "\\z";
    }

    template <class WeightSet>
    void
    printer<WeightSet>::visit(const word<WeightSet>& v)
    {
      print_left_weight(v);
      out_ <<* v.get_word();
    }

    template <class WeightSet>
    void
    printer<WeightSet>::print(const typename WeightSet::value_t& w,
                              const WeightSet& ws)
    {
      if (show_unit_ || !ws.is_unit(w))
        out_ << '{' << w << '}';
    }

    template <class WeightSet>
    void
    printer<WeightSet>::print(const nary<weightset_t>& n, const char op)
    {
      assert(n.size());

      print(n.left_weight(), n.get_weight_set());

      if (debug_)
        out_ << op;
      out_ << '(';
      bool first = true;
      for (auto i: n)
        {
          if (!first)
            out_ << op;
          first = false;
          i->accept(*this);
        }
      out_ << ')';

      print(n.right_weight(), n.get_weight_set());
    }

    template <class WeightSet>
    void
    printer<WeightSet>::print_left_weight(const left_weighted<WeightSet>& v)
    {
      print(v.left_weight(), v.get_weight_set());
    }

  } // namespace rat
} // namespace vcsn

#endif // !VCSN_CORE_RAT_PRINTER_HXX
