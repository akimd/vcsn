#include <sstream>
#include <vcsn/misc/cast.hh> // down_pointer_cast
#include <vcsn/misc/stream.hh> // conv
#include <vcsn/dyn/ratexp.hh> // dyn::make_ratexp

namespace vcsn
{
namespace dyn
{
  namespace detail
  {
    /*--------------------.
    | ratexpset_wrapper.  |
    `--------------------*/

    template <typename RatExpSet>
    inline
    ratexpset_wrapper<RatExpSet>::ratexpset_wrapper
      (const ratexpset_t& rs)
      : super_type()
      , rs_(rs)
    {}

#define DEFINE                                  \
    template <typename RatExpSet>               \
    inline                                      \
    auto                                        \
    ratexpset_wrapper<RatExpSet>

    DEFINE::down(const value_t& v) const
      -> std::shared_ptr<const node_t>
    {
      return down_pointer_cast<const node_t>(v);
    }

    DEFINE::down(const std::string& w) const -> weight_t
    {
      return ::vcsn::conv(*rs_.weightset(), w);
    }

    DEFINE::make_ratexp(const value_t& v) const
      -> dyn::ratexp
    {
      return dyn::make_ratexp(rs_, down(v));
    }

    DEFINE::zero() const -> value_t
    {
      return rs_.zero();
    }

    DEFINE::one() const -> value_t
    {
      return rs_.one();
    }

    DEFINE::atom(const std::string& w) const -> value_t
    {
      return rs_.atom(::vcsn::conv(*rs_.labelset(), w));
    }

    DEFINE::add(value_t l, value_t r) const -> value_t
    {
      return rs_.add(down(l), down(r));
    }

    DEFINE::mul(value_t l, value_t r) const -> value_t
    {
      return rs_.mul(down(l), down(r));
    }

    DEFINE::intersection(value_t l, value_t r) const -> value_t
    {
      return rs_.intersection(down(l), down(r));
    }

    DEFINE::shuffle(value_t l, value_t r) const -> value_t
    {
      return rs_.shuffle(down(l), down(r));
    }

    DEFINE::ldiv(value_t l, value_t r) const -> value_t
    {
      return rs_.ldiv(down(l), down(r));
    }

    DEFINE::rdiv(value_t l, value_t r) const -> value_t
    {
      return rs_.rdiv(down(l), down(r));
    }

    DEFINE::concat(value_t l, value_t r) const -> value_t
    {
      return rs_.concat(down(l), down(r));
    }

    DEFINE::star(value_t v) const -> value_t
    {
      return rs_.star(down(v));
    }

    DEFINE::complement(value_t v) const -> value_t
    {
      return rs_.complement(down(v));
    }

    DEFINE::transposition(value_t v) const -> value_t
    {
      return rs_.transposition(down(v));
    }

    DEFINE::lmul(const std::string& w, value_t v) const -> value_t
    {
      return rs_.lmul(down(w), down(v));
    }

    DEFINE::rmul(value_t v, const std::string& w) const -> value_t
    {
      return rs_.rmul(down(v), down(w));
    }

    DEFINE::conv(std::istream& is) const -> value_t
    {
      return rs_.conv(is);
    }

    DEFINE::print(std::ostream& o, value_t v) const -> std::ostream&
    {
      return rs_.print(o, down(v));
    }

#undef DEFINE

  } // namespace detail

  template <typename RatExpSet>
  inline
  ratexpset
  make_ratexpset(const RatExpSet& rs)
  {
    using wrapper_t = detail::ratexpset_wrapper<RatExpSet>;
    return std::make_shared<wrapper_t>(rs);
  }

} // namespace dyn
} // namespace vcsn
