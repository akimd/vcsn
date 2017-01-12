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
      : public min_plus_impl<nmin_impl, unsigned int>
    {
    public:
      using super_t = min_plus_impl<nmin_impl, unsigned int>;
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

      value_t
      conv(std::istream& is, bool = true) const
      {
        if (is.peek() == '-')
          raise_invalid_value(*this, is);
        else
          return super_t::conv(is);
      }

      static constexpr bool has_lightening_weights() { return false; }

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
            o << "Nmin";
            break;
          case format::utf8:
            o << "ℕmin";
            break;
          case format::raw:
            assert(0);
            break;
          }
        return o;
      }
    };

    // Random generation.
    template <typename RandomGenerator>
    class random_weight<nmin, RandomGenerator>
      : public random_weight_base<nmin, RandomGenerator>
    {
    public:
      using super_t = random_weight_base<nmin, RandomGenerator>;
      using value_t = typename super_t::weight_t;

      using super_t::super_t;

    private:
      value_t pick_value_() const
      {
        auto dis =
          std::uniform_int_distribution<nmin::value_t>(super_t::min_, super_t::max_);
        return dis(super_t::gen_);
      }
    };

    VCSN_JOIN_SIMPLE(b, nmin);
    VCSN_JOIN_SIMPLE(nmin, nmin);
  }

  template <>
  struct is_tropical<nmin> : std::true_type
  {};
}
