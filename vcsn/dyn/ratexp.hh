#ifndef VCSN_DYN_RATEXEP_HH
# define VCSN_DYN_RATEXEP_HH

# include <memory>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/core/rat/ratexp.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace details
    {

      class abstract_ratexp
      {
      public:
        using exp_t = vcsn::rat::exp_t;
        abstract_ratexp(const dyn::context& ctx, const exp_t& ratexp)
          : ctx_(ctx)
          , ratexp_(ratexp)
        {}
        ~abstract_ratexp() {}

        const dyn::context& ctx()
        {
          return ctx_;
        }
        const exp_t& ratexp() const
        {
          return ratexp_;
        }
      protected:
        const dyn::context& ctx_;
        const exp_t& ratexp_;
      };

    } // namespace details

    using ratexp = std::shared_ptr<details::abstract_ratexp>;

    inline
    ratexp
    make_ratexp(const dyn::context& ctx, const rat::exp_t& ratexp)
    {
      return std::make_shared<ratexp::element_type>(ratexp::element_type(ctx, ratexp));
    }

  } // namespace dyn
} // namespace vcsn

#endif // !VCSN_DYN_RATEXEP_HH
