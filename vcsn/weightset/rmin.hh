#pragma once

#include <vcsn/core/join.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/stream.hh> // eat
#include <vcsn/weightset/fwd.hh>
#include <vcsn/weightset/min-plus.hh>
#include <vcsn/weightset/nmin.hh>
#include <vcsn/weightset/weightset.hh>
#include <vcsn/weightset/zmin.hh>

namespace vcsn
{
  namespace detail
  {
    class rmin_impl
      : public min_plus_impl<rmin_impl, double>
    {
    public:
      using super_t = min_plus_impl<rmin_impl, double>;
      using self_t = rmin;
      using value_t = typename super_t::value_t;

      static symbol sname()
      {
        static auto res = symbol{"rmin"};
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

      static value_t
      conv(nmin, const nmin::value_t v)
      {
        return v;
      }

      static value_t
      conv(zmin, const zmin::value_t v)
      {
        return v;
      }

      static constexpr bool has_lightening_weights() { return true; }

      std::ostream&
      print_set(std::ostream& o, format fmt = {}) const
      {
        switch (fmt.kind())
          {
          case format::latex:
            o << "\\mathbb{R}_{\\text{min}}";
            break;
          case format::sname:
            o << sname();
            break;
          case format::text:
            o << "Rmin";
            break;
          case format::utf8:
            o << "ℝmin";
            break;
          case format::ere:
        case format::raw:
        case format::redgrep:
            assert(0);
            break;
          }
        return o;
      }
    };

    // Random generation.
    template <typename RandomGenerator>
    class random_weight<rmin, RandomGenerator>
      : public random_weight_base<rmin, RandomGenerator>
    {
    public:
      using super_t = random_weight_base<rmin, RandomGenerator>;
      using value_t = typename super_t::weight_t;

      using super_t::super_t;

    private:
      value_t pick_value_() const
      {
        auto dis =
          std::uniform_real_distribution<rmin::value_t>(super_t::min_, super_t::max_);
        return dis(super_t::gen_);
      }
    };

    VCSN_JOIN_SIMPLE(b,    rmin);
    VCSN_JOIN_SIMPLE(nmin, rmin);
    VCSN_JOIN_SIMPLE(zmin, rmin);
    VCSN_JOIN_SIMPLE(rmin, rmin);
  }

  template <>
  struct is_tropical<rmin> : std::true_type
  {};
}
