#ifndef VCSN_WEIGHTSET_ZMIN_HH
# define VCSN_WEIGHTSET_ZMIN_HH

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
    class zmin_impl
      : public min_plus_impl<int>
    {
    public:
      using super_t = min_plus_impl<int>;
      using self_type = zmin;
      using value_t = typename super_t::value_t;

      static std::string sname()
      {
        return "zmin";
      }

      /// Build from the description in \a is.
      static zmin make(std::istream& is)
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
          o << "\\mathbb{Z}_{\\text{min}}";
        else if (format == "text")
          o << sname();
        else
          raise("invalid format: ", format);
        return o;
      }
    };

    VCSN_JOIN_SIMPLE(b, zmin);
    VCSN_JOIN_SIMPLE(zmin, zmin);
  }
}

#endif // !VCSN_WEIGHTSET_ZMIN_HH
