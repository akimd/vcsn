#include <sstream>
#include <vcsn/misc/cast.hh> // down_pointer_cast

namespace vcsn
{
namespace dyn
{
  namespace detail
  {
    /*-----------------.
    | ratexpset_base.  |
    `-----------------*/
    inline
    std::string ratexpset_base::format(const value_t v) const
    {
      std::ostringstream s;
      print(s, v);
      return s.str();
    }

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

    /// From weak to strong typing.
    DEFINE::down(const value_t& v) const
      -> std::shared_ptr<const node_t>
    {
      return down_pointer_cast<const node_t>(v);
    }

    /// From string, to typed weight.
    DEFINE::down(const std::string& w) const -> weight_t
    {
      return rs_.weightset()->conv(w);
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
      return rs_.atom(rs_.labelset()->conv(w));
    }

    DEFINE::add(value_t l, value_t r) const -> value_t
    {
      return rs_.add(down(l), down(r));
    }

    DEFINE::mul(value_t l, value_t r) const -> value_t
    {
      return rs_.mul(down(l), down(r));
    }

    DEFINE::concat(value_t l, value_t r) const -> value_t
    {
      return rs_.concat(down(l), down(r));
    }

    DEFINE::star(value_t v) const -> value_t
    {
      return rs_.star(down(v));
    }

    DEFINE::weight(const std::string& w, value_t v) const -> value_t
    {
      return rs_.weight(down(w), down(v));
    }

    DEFINE::weight(value_t v, const std::string& w) const -> value_t
    {
      return rs_.weight(down(v), down(w));
    }

    DEFINE::conv(const std::string& s) const -> value_t
    {
      return rs_.conv(s);
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
