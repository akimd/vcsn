#ifndef VCSN_CORE_RAT_PRINTER_HXX
# define VCSN_CORE_RAT_PRINTER_HXX

namespace vcsn
{
  namespace rat
  {

    template <typename RatExpSet>
    inline
    printer<RatExpSet>::printer(std::ostream& out,
                                const ratexpset_t& rs,
                                const bool debug)
      : out_(out)
      , ctx_(rs.context())
      , debug_(debug)
    {}


# define DEFINE                                 \
    template <typename RatExpSet>               \
    inline                                      \
    auto                                        \
    printer<RatExpSet>

# define VISIT(Type)                          \
    DEFINE::visit(const Type ## _t& v)        \
      -> void

    VISIT(intersection)
    {
      print(v, '&');
    }

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
      // Force parens around the child if it needs a left weight.  This is
      // not needed for right weights: compare e<w>* with (<w>e)*.
      const node_t& child = *v.sub();
      bool child_needs_left_weight = shows_left_weight_(child);
      print(v.left_weight());
      print_child(child, v, child_needs_left_weight);
      out_ << '*';
      print(v.right_weight());
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
      ctx_.labelset()->print(out_, v.value());
    }

    DEFINE::print_child(const node_t& child, const node_t& parent,
                        bool force_parens)
      -> void
    {
      bool parent_has_precedence = precedence(child) < precedence(parent);
      bool needs_parens = parent_has_precedence || force_parens;
      if (needs_parens)
        out_ << '(';
      child.accept(*this);
      if (needs_parens)
        out_ << ')';
    }

    DEFINE::print(const nary_t& n, const char op)
      -> void
    {
      bool need_weight_parens = shows_weight_(n);

      print(n.left_weight());
      if (need_weight_parens)
        out_ << '(';
      bool first = true;
      for (auto i: n)
        {
          if (! first)
            out_ << op;
          print_child(*i, n);
          first = false;
        }

      if (need_weight_parens)
        out_ << ')';
      print(n.right_weight());
    }

    DEFINE::print(const weight_t& w)
      -> void
    {
      if (shows_(w))
        {
          const char lbracket = '<';
          const char rbracket = '>';
          out_ << lbracket;
          ctx_.weightset()->print(out_, w);
          out_ << rbracket;
        }
    }

# undef VISIT
# undef DEFINE

  } // namespace rat
} // namespace vcsn

#endif // !VCSN_CORE_RAT_PRINTER_HXX
