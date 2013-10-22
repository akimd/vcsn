#ifndef VCSN_DYN_RATEXP_HH
# define VCSN_DYN_RATEXP_HH

# include <memory>
# include <vcsn/ctx/fwd.hh>
# include <vcsn/dyn/fwd.hh>
# include <vcsn/dyn/context.hh> // vname.
# include <vcsn/core/rat/ratexp.hh>
# include <vcsn/misc/export.hh>

namespace vcsn
{
  namespace dyn
  {
    namespace detail
    {

      /// Aggregate an expression and its ratexpset.
      ///
      /// FIXME: Improperly named, it is not a base class for
      /// static ratexps.
      class LIBVCSN_API abstract_ratexp
      {
      public:
        /// A description of the ratexp type.
        /// \param full  whether to include the genset.
        ///              if false, same as sname.
        virtual std::string vname(bool full = true) const = 0;

        template <typename RatExpSet>
        concrete_abstract_ratexp<RatExpSet>& as()
        {
          std::cerr << "asre: typeinfo: " << &typeid(*this) << std::endl;
          std::cerr << "asre: typeinfo: " << &typeid(concrete_abstract_ratexp<RatExpSet>&) << std::endl;
          std::cerr << "asre: typeinfo: " << typeid(*this).name() << std::endl;
          std::cerr << "asre: typeinfo: " << typeid(concrete_abstract_ratexp<RatExpSet>&).name() << std::endl;
          return dynamic_cast<concrete_abstract_ratexp<RatExpSet>&>(*this);
        }

        template <typename RatExpSet>
        const concrete_abstract_ratexp<RatExpSet>& as() const
        {
          return dynamic_cast<const concrete_abstract_ratexp<RatExpSet>&>(*this);
        }
      };


      /// Aggregate a ratexp and its ratexpset.
      ///
      /// FIXME: Improperly named, it is not a base class for
      /// static ratexps.
      template <typename RatExpSet>
      class LIBVCSN_API concrete_abstract_ratexp: public abstract_ratexp
      {
      public:
        using ratexpset_t = RatExpSet;
        using super_type = abstract_ratexp;
        using ratexp_t = typename ratexpset_t::ratexp_t;
        concrete_abstract_ratexp(const ratexp_t& ratexp,
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

    using ratexp = std::shared_ptr<detail::abstract_ratexp>;

    template <typename Context>
    inline
    ratexp
    make_ratexp(const vcsn::ratexpset<Context>& rs,
                const typename vcsn::ratexpset<Context>::ratexp_t& ratexp)
    {
      using ratexpset_t = vcsn::ratexpset<Context>;
      auto res = std::make_shared<detail::concrete_abstract_ratexp<ratexpset_t>>
        (ratexp, rs);
      std::cerr << "makere: typeinfo: " << &typeid(*res) << std::endl;
      std::cerr << "makere: typeinfo: " << &typeid(detail::concrete_abstract_ratexp<ratexpset_t>) << std::endl;
      return res;
    }

  } // namespace dyn
} // namespace vcsn

#endif // !VCSN_DYN_RATEXP_HH
