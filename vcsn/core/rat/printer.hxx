#ifndef VCSN_CORE_RAT_PRINTER_HXX
# define VCSN_CORE_RAT_PRINTER_HXX

namespace vcsn
{
  namespace rat
  {

    template <class Weight>
    printer<Weight>::printer(std::ostream& out,
                                const bool show_unit,
                                const bool debug)
      : out_(out)
      , show_unit_(show_unit)
      , debug_(debug)
    {}

    template <class Weight>
    printer<Weight>::~printer()
    {
      out_ << std::flush;
    }

    template <class Weight>
    void
    printer<Weight>::visit(const prod<weight_t>& v)
    {
      print(v, '.');
    }

    template <class Weight>
    void
    printer<Weight>::visit(const sum<weight_t>& v)
    {
      print(v, '+');
    }

    template <class Weight>
    void
    printer<Weight>::visit(const star<weight_t>& v)
    {
      const auto& sub = v.get_sub();
      assert(sub);
      print(v.left_weight());
      if (debug_)
        out_ << '*';
      out_ << '(';
      sub->accept(*this);
      out_ << ")*";
      print(v.right_weight());
    }

    template <class Weight>
    void
    printer<Weight>::visit(const one<weight_t>& v)
    {
      print_left_weight(v);
      out_ << "\\e";
    }

    template <class Weight>
    void
    printer<Weight>::visit(const zero<weight_t>& v)
    {
      print_left_weight(v);
      out_ << "\\z";
    }

    template <class Weight>
    void
    printer<Weight>::visit(const word<weight_t>& v)
    {
      print_left_weight(v);
      out_ <<* v.get_word();
    }

    template <class Weight>
    void
    printer<Weight>::print(const weight_t& w)
    {
      // FIXME: if (show_unit_ || !ws.is_unit(w))
        out_ << '{' << w << '}';
    }

    template <class Weight>
    void
    printer<Weight>::print(const nary<weight_t>& n, const char op)
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

    template <class Weight>
    void
    printer<Weight>::print_left_weight(const left_weighted<weight_t>& v)
    {
      print(v.left_weight());
    }

  } // namespace rat
} // namespace vcsn

#endif // !VCSN_CORE_RAT_PRINTER_HXX
