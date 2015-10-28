#pragma once

#include <vcsn/core/join.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/stream.hh> // eat
#include <vcsn/weightset/fwd.hh>
#include <vcsn/weightset/min-plus.hh>
#include <vcsn/weightset/weightset.hh>

namespace vcsn
{
  namespace detail
  {
    class rmin_impl
      : public min_plus_impl<double>
    {
    public:
      using super_t = min_plus_impl<double>;
      using self_t = rmin;
      using value_t = typename super_t::value_t;

      static symbol sname()
      {
        static symbol res("rmin");
        return res;
      }

      /// Build from the description in \a is.
      static rmin make(std::istream& is)
      {
        eat(is, sname());
        return {};
      }

      using super_t::conv;
      static value_t
      conv(self_t, value_t v)
      {
        return v;
      }

      std::ostream&
      print_set(std::ostream& o, format fmt = {}) const
      {
        if (fmt == format::latex)
          o << "\\mathbb{R}_{\\text{min}}";
        else if (fmt == format::text)
          o << sname();
        else
          raise("invalid format: ", fmt);
        return o;
      }
    };

    VCSN_JOIN_SIMPLE(b, rmin);
    VCSN_JOIN_SIMPLE(rmin, rmin);
  }

  template <>
  struct is_tropical<rmin> : std::true_type
  {};
}
