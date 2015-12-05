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
    class nmin_impl
      : public min_plus_impl<unsigned int>
    {
    public:
      using super_t = min_plus_impl<unsigned int>;
      using self_t = nmin;
      using value_t = typename super_t::value_t;

      static symbol sname()
      {
        static auto res = symbol{"nmin"};
        return res;
      }

      /// Build from the description in \a is.
      static nmin make(std::istream& is)
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

      static value_t
      conv(std::istream& is, bool = true)
      {
        if (is.peek() == '-')
          fail_reading(is, sname(), ": negative values are invalid");
        return super_t::conv(is);
      }

      std::ostream&
      print_set(std::ostream& o, format fmt = {}) const
      {
        switch (fmt.kind())
          {
          case format::latex:
            o << "\\mathbb{N}_{\\text{min}}";
            break;
          case format::sname:
            o << sname();
            break;
          case format::text:
            o << "ℕmin";
            break;
          case format::raw:
            assert(0);
            break;
          }
        return o;
      }
    };

    VCSN_JOIN_SIMPLE(b, nmin);
    VCSN_JOIN_SIMPLE(nmin, nmin);
  }

  template <>
  struct is_tropical<nmin> : std::true_type
  {};
}
