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
    class zmin_impl
      : public min_plus_impl<int>
    {
    public:
      using super_t = min_plus_impl<int>;
      using self_t = zmin;
      using value_t = typename super_t::value_t;

      static symbol sname()
      {
        static symbol res("zmin");
        return res;
      }

      /// Build from the description in \a is.
      static zmin make(std::istream& is)
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
