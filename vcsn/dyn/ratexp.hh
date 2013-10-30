#ifndef VCSN_DYN_RATEXP_HH
# define VCSN_DYN_RATEXP_HH

# include <memory>
# include <string>

# include <vcsn/misc/export.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {

      /// An abstract ratexp.
      class LIBVCSN_API ratexp_base
      {
      public:
        /// A description of the ratexp type.
        /// \param full  whether to include the genset.
        ///              if false, same as sname.
        virtual std::string vname(bool full = true) const = 0;

        template <typename RatExpSet>
        ratexp_wrapper<RatExpSet>& as()
        {
          return dynamic_cast<ratexp_wrapper<RatExpSet>&>(*this);
        }

        template <typename RatExpSet>
        const ratexp_wrapper<RatExpSet>& as() const
        {
          return dynamic_cast<const ratexp_wrapper<RatExpSet>&>(*this);
        }
      };


      /// Aggregate a ratexp and its ratexpset.
      template <typename RatExpSet>
      class ratexp_wrapper: public ratexp_base
      {
      public:
        using ratexpset_t = RatExpSet;
        using super_type = ratexp_base;
        using ratexp_t = typename ratexpset_t::ratexp_t;
        ratexp_wrapper(const ratexp_t& ratexp,
                       const ratexpset_t& ratexpset)
          : ratexp_(ratexp)
          , ratexpset_(ratexpset)
        {}

        virtual std::string vname(bool full = true) const override
        {
          return get_ratexpset().vname(full);
        }

        const ratexp_t ratexp() const
        {
          return ratexp_;
        }

        const ratexpset_t& get_ratexpset() const
        {
          return ratexpset_;
        }

      protected:
        /// The ratexp.
        const ratexp_t ratexp_;
        /// The ratexp set.
        const ratexpset_t ratexpset_;
      };

    } // namespace detail

    using ratexp = std::shared_ptr<detail::ratexp_base>;

    template <typename RatExpSet>
    inline
    ratexp
    make_ratexp(const RatExpSet& rs,
                const typename RatExpSet::ratexp_t& ratexp)
    {
      using wrapper_t = detail::ratexp_wrapper<RatExpSet>;
      return std::make_shared<wrapper_t>(ratexp, rs);
    }

  } // namespace dyn
} // namespace vcsn

#endif // !VCSN_DYN_RATEXP_HH
