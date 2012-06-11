#ifndef VCSN_CORE_RAT_PRINTER_HXX
# define VCSN_CORE_RAT_PRINTER_HXX

namespace vcsn
{
  namespace rat
  {

    template <typename Context>
    inline
    printer<Context>::printer(std::ostream& out,
                              const context_t& ctx,
                              const bool debug)
      : out_(out)
      , ctx_(ctx)
      , debug_(debug)
    {}


# define DEFINE                                 \
    template <typename Context>                 \
    inline                                      \
    auto                                        \
    printer<Context>

# define VISIT(Type)                          \
    DEFINE::visit(const Type ## _t& v)        \
      -> void

    VISIT(sum)
    {
      print(v, '+');
    }

    VISIT(prod)
    {
      print(v, '.');
    }

    VISIT(star)
    {
      const auto& sub = v.sub();
      assert(sub);

      bool parens = parens_(v);
      if (debug_)
        out_ << '*';
      if (parens)
        out_ << '(';
      print(v.left_weight());

      {
        // If our argument shows weight, then add inner parens, so
        // that ({2}a)* => ({2}a)*, not => {2}a* (which is actually
        // read as {2}(a*)).
        bool innerp = shows_weight_(*sub);
        if (innerp)
          out_ << '(';
        sub->accept(*this);
        if (innerp)
          out_ << ')';
      }
      out_ << "*";
      print(v.right_weight());
      if (parens)
        out_ << ')';
    }

    VISIT(zero)
    {
      print(v.left_weight());
      out_ << "\\z";
    }

    VISIT(one)
    {
      print(v.left_weight());
      out_ << "\\e";
    }

    VISIT(atom)
    {
      print(v.left_weight());
      bool p = parens_(v);
      if (p)
        out_ << '(';
      ctx_.genset()->output(out_, v.value());
      if (p)
        out_ << ')';
    }

    DEFINE::print(const weight_t& w)
      -> void
    {
      if (shows_(w))
        {
          out_ << '{';
          ctx_.weightset()->print(out_, w);
          out_ << '}';
        }
    }

    DEFINE::print(const nary_t& n, const char op)
      -> void
    {
      assert(n.size());

      print(n.left_weight());

      bool parens = parens_(n);
      if (debug_)
        out_ << op;
      if (parens)
        out_ << '(';
      bool first = true;
      for (auto i: n)
        {
          if (!first)
            out_ << op;
          first = false;
          i->accept(*this);
        }
      if (parens)
        out_ << ')';

      print(n.right_weight());
    }

# undef VISIT
# undef DEFINE

  } // namespace rat
} // namespace vcsn

#endif // !VCSN_CORE_RAT_PRINTER_HXX
