#include <sstream>
#include <vcsn/misc/cast.hh> // down_pointer_cast

namespace vcsn
{
namespace dyn
{
  namespace detail
  {
    /*---------------------.
    | abstract_ratexpset.  |
    `---------------------*/
    inline
    std::string abstract_ratexpset::format(const value_t v) const
    {
      std::ostringstream s;
      print(s, v);
      return s.str();
    }

    /*------------------------------.
    | concrete_abstract_ratexpset.  |
    `------------------------------*/

    template <typename Context>
    inline
    concrete_abstract_ratexpset<Context>::concrete_abstract_ratexpset
      (const Context& ctx)
      : super_type()
      , rs_(ratexpset_t(ctx))
    {}

#define DEFINE                                  \
    template <typename Context>                 \
    inline                                      \
    auto                                        \
    concrete_abstract_ratexpset<Context>

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

    template <typename Context>
    inline
    ratexpset
    make_ratexpset(const vcsn::ratexpset<Context>& rs)
    {
      return std::make_shared<detail::concrete_abstract_ratexpset<Context>>
      (rs.context());
    }
} // namespace dyn
} // namespace vcsn
