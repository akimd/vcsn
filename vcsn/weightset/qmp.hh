#pragma once

#include <ostream>
#include <string>

#include <cstddef> // https://gcc.gnu.org/gcc-4.9/porting_to.html
#include <gmpxx.h>

#include <vcsn/core/join.hh>
#include <vcsn/misc/format.hh>
#include <vcsn/misc/functional.hh>
#include <vcsn/misc/raise.hh>
#include <vcsn/misc/star-status.hh>
#include <vcsn/misc/stream.hh> // eat
#include <vcsn/weightset/b.hh>
#include <vcsn/weightset/fwd.hh>
#include <vcsn/weightset/q.hh>
#include <vcsn/weightset/r.hh>
#include <vcsn/weightset/weightset.hh>
#include <vcsn/weightset/z.hh>

namespace vcsn
{
  namespace detail
  {
  class qmp_impl
  {
  public:
    using self_t = qmp;

    static symbol sname()
    {
      static auto res = symbol{"qmp"};
      return res;
    }

    /// Build from the description in \a is.
    static qmp make(std::istream& is)
    {
      eat(is, sname());
      return {};
    }

    using value_t = mpq_class;

    /// Create rational weight from num and den.
    static value_t value(int num, unsigned den)
    {
      return value_t{num, den};
    }

    static value_t zero()
    {
      // Not value_t{0, 1} to avoid the (const char* s, int base)
      // constructor.
      return value_t{mpz_class(0), 1};
    }

    static value_t one()
    {
      return value_t{1, 1};
    }

    static value_t
    min()
    {
      auto num = std::numeric_limits<int>::min();
      return value_t{num, 1};
    }

    static value_t
    max()
    {
      auto num = std::numeric_limits<int>::max();
      return value_t{num, 1};
    }

    static value_t add(const value_t& l, const value_t& r)
    {
      return l + r;
    }

    static value_t sub(const value_t& l, const value_t& r)
    {
      return l - r;
    }

    static value_t mul(const value_t& l, const value_t& r)
    {
      return l * r;
    }

    value_t
    rdivide(const value_t& l, const value_t& r) const
    {
      require(!is_zero(r), *this, ":div: division by zero");
      return l / r;
    }

    value_t
    ldivide(const value_t& l, const value_t& r) const
    {
      return rdivide(r, l);
    }

    value_t star(const value_t& v) const
    {
      if (abs(v.get_num()) < v.get_den())
        // No need to reduce: numerator and denominators are primes.
        return {v.get_den(), v.get_den() - v.get_num()};
      else
        raise_not_starrable(*this, v);
    }

    static bool is_special(const value_t&) // C++11: cannot be constexpr.
    {
      return false;
    }

    static bool is_zero(const value_t& v)
    {
      return v.get_num() == 0;
    }

    static bool is_one(const value_t& v)
    {
      // All values are normalized.
      return v.get_num() == 1 && v.get_den() == 1;
    }

    /// Three-way comparison between \a l and \a r.
    static int compare(const value_t& l, const value_t& r)
    {
      return cmp(l, r);
    }

    /// Whether \a l == \a r.
    static bool equal(const value_t& l, const value_t& r)
    {
      return l == r;
    }

    /// Whether \a  < \a r.
    static bool less(const value_t& l, const value_t& r)
    {
      return l < r;
    }

    static constexpr bool is_commutative() { return true; }
    static constexpr bool has_lightening_weights() { return true; }

    static constexpr bool show_one() { return false; }
    static constexpr star_status_t star_status() { return star_status_t::ABSVAL; }

    static value_t
    abs(const value_t& v)
    {
      return ::abs(v);
    }

    static value_t
    transpose(const value_t& v)
    {
      return v;
    }

    static size_t hash(const value_t& v)
    {
      // FIXME: be serious...
      return hash_value(to_string(qmp_impl(), v));
    }

    static value_t
    conv(self_t, const value_t& v)
    {
      return v;
    }

    static value_t
    conv(z, z::value_t v)
    {
      return {v, 1};
    }

    static value_t
    conv(b, b::value_t v)
    {
      return {v, 1};
    }

    static value_t
    conv(std::istream& i, bool = true)
    {
      value_t res;
      i >> res;
      return res;
    }

    static std::ostream&
    print(const value_t& v, std::ostream& o = std::cout,
          format fmt = {})
    {
      if (fmt == format::latex)
        {
          if (v.get_den() == 1)
            o << v.get_num();
          else
            o << "\\frac{" << v.get_num() << "}{" << v.get_den() << '}';
        }
      else
        o << v;
      return o;
    }

    std::ostream&
    print_set(std::ostream& o, format fmt = {}) const
    {
      switch (fmt.kind())
        {
        case format::latex:
          o << "\\mathbb{Q}_{\\text{mp}}";
          break;
        case format::sname:
          o << sname();
          break;
        case format::text:
          o << "Qmp";
          break;
        case format::utf8:
          o << "ℚmp";
          break;
        case format::raw:
          assert(0);
          break;
        }
      return o;
    }
  };

    /// Random generation.
    template <typename RandomGenerator>
    class random_weight<qmp, RandomGenerator>
      : public random_weight_base<qmp, RandomGenerator>
    {
    public:
      using super_t = random_weight_base<qmp, RandomGenerator>;
      using value_t = typename super_t::weight_t;

      using super_t::super_t;

    private:
      value_t pick_value_() const
      {
        auto dis_num =
          std::uniform_int_distribution<>(super_t::min_.get_num().get_ui(),
                                          super_t::max_.get_num().get_ui());
        auto dis_den =
          std::uniform_int_distribution<unsigned>
          (super_t::min_.get_den().get_ui(),
           super_t::max_.get_num().get_ui());
        auto num = dis_num(super_t::gen_);
        auto den = dis_den(super_t::gen_);
        return super_t::ws_.value(num, den);
      }
    };


    /*-------.
    | join.  |
    `-------*/

    VCSN_JOIN_SIMPLE(b, qmp);
    VCSN_JOIN_SIMPLE(z, qmp);
    VCSN_JOIN_SIMPLE(q, qmp);
    VCSN_JOIN_SIMPLE(qmp, qmp);

    // We don't want to force the inclusion of qmp.hh in r.hh, as it
    // means that qmp.hh become a hard requirement.
    VCSN_JOIN_SIMPLE(qmp, r);
  }
}
