#ifndef VCSN_ALGOS_READ_WEIGHT_HH
# define VCSN_ALGOS_READ_WEIGHT_HH

# include <vcsn/dyn/fwd.hh>
# include <vcsn/dyn/weight.hh>

namespace vcsn
{

  template <typename Context>
  inline
  auto
  read_weight(const Context& ctx, const std::string& w)
    -> typename Context::weight_t
  {
    return ctx.weightset()->conv(w);
  }

  /*-------------------.
  | dyn::read_weight.  |
  `-------------------*/

  namespace dyn
  {
    namespace detail
    {
      template <typename Context>
      auto
      read_weight(const context& ctx, const std::string& s)
        -> weight
      {
        const auto& c = dynamic_cast<const Context&>(*ctx);
        auto res = ::vcsn::read_weight(c, s);
        return make_weight(*c.weightset(), res);
      }

      REGISTER_DECLARE(read_weight,
                       (const context& ctx, const std::string& s) -> weight);
    }
  }


} // namespace vcsn

#endif // !VCSN_ALGOS_READ_WEIGHT_HH
