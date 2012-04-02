#ifndef VCSN_CORE_RAT_PRINTER_HXX
# define VCSN_CORE_RAT_PRINTER_HXX

namespace vcsn
{
  namespace rat
  {

    template <class WeightSet>
    printer<WeightSet>::printer(std::ostream& out,
                                const weightset_t& ws,
                                const bool debug)
      : out_(out)
      , ws_(ws)
      , debug_(debug)
    {}

    template <class WeightSet>
    printer<WeightSet>::~printer()
    {
      out_ << std::flush;
    }

    template <class WeightSet>
    void
    printer<WeightSet>::visit(const prod<weight_t>& v)
    {
      print(v, '.');
    }

    template <class WeightSet>
    void
    printer<WeightSet>::visit(const sum<weight_t>& v)
    {
      print(v, '+');
    }

    template <class WeightSet>
    void
    printer<WeightSet>::visit(const star<weight_t>& v)
    {
      const auto& sub = v.get_sub();
      assert(sub);
      print(v.left_weight());
      if (debug_)
        out_ << '*';
      out_ << '(';
      sub->accept(*this);
      out_ << "*)";
      print(v.right_weight());
    }

    template <class WeightSet>
    void
    printer<WeightSet>::visit(const one<weight_t>& v)
    {
      print(v.left_weight());
      out_ << "\\e";
    }

    template <class WeightSet>
    void
    printer<WeightSet>::visit(const zero<weight_t>& v)
    {
      print(v.left_weight());
      out_ << "\\z";
    }

    template <class WeightSet>
    void
    printer<WeightSet>::visit(const atom<weight_t>& v)
    {
      print(v.left_weight());
      out_ << v.get_atom();
    }

    template <class WeightSet>
    void
    printer<WeightSet>::print(const weight_t& w)
    {
      if (ws_.show_unit() || !ws_.is_unit(w))
        {
          out_ << '{';
          ws_.print(out_, w);
          out_ << '}';
        }
    }

    template <class WeightSet>
    void
    printer<WeightSet>::print(const nary<weight_t>& n, const char op)
    {
      assert(n.size());

      print(n.left_weight());

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

      print(n.right_weight());
    }


  } // namespace rat
} // namespace vcsn

#endif // !VCSN_CORE_RAT_PRINTER_HXX
