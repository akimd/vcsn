#ifndef VCSN_DYN_RATEXEP_HH
# define VCSN_DYN_RATEXEP_HH

# include <memory>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/core/rat/kratexp.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace details
    {

      class abstract_ratexp
      {
      public:
        using ratexp_t = std::shared_ptr<rat::exp>;

        virtual ~abstract_ratexp() {}
        // virtual std::string vname() const
        virtual const abstract_context& context()
        {
          reutrn ctx_;
        }
        virtual exp_t ratexp() const
        {
          return ratexp_;
        }
      protected:
        const abstract_context ctx_;
        const ratexp_t ratexp_;
      };

    } // namespace details

    using ratexp = std::shared_ptr<details::abstract_ratexp>;

  } // namespace dyn
} // namespace vcsn

#endif // !VCSN_DYN_RATEXEP_HH
