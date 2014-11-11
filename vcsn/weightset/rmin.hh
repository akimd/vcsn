#ifndef VCSN_WEIGHTSET_RMIN_HH
# define VCSN_WEIGHTSET_RMIN_HH

# include <vcsn/core/join.hh>
# include <vcsn/misc/raise.hh>
# include <vcsn/misc/stream.hh> // eat
# include <vcsn/weightset/fwd.hh>
# include <vcsn/weightset/min-plus.hh>
# include <vcsn/weightset/weightset.hh>

namespace vcsn
{
  namespace detail
  {
    class rmin_impl
      : public min_plus_impl<int>
    {
    public:
      using super_t = min_plus_impl<int>;
      using self_type = rmin;
      using value_t = typename super_t::value_t;

      static std::string sname()
      {
        return "rmin";
      }

      /// Build from the description in \a is.
      static rmin make(std::istream& is)
      {
        eat(is, sname());
        return {};
      }

      using super_t::conv;
      static value_t
      conv(self_type, value_t v)
      {
        return v;
      }

      std::ostream&
      print_set(std::ostream& o, const std::string& format = "text") const
      {
        if (format == "latex")
          o << "\\mathbb{R}_{\\text{min}}";
        else if (format == "text")
          o << sname();
        else
          raise("invalid format: ", format);
        return o;
      }
    };

    VCSN_JOIN_SIMPLE(b, rmin);
    VCSN_JOIN_SIMPLE(rmin, rmin);
  }
}

#endif // !VCSN_WEIGHTSET_RMIN_HH
